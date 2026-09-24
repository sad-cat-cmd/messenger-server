#include <gtest/gtest.h>
#include <QTemporaryDir>
#include <QFile>
#include <memory>
#include <database.hpp>
#include <models.hpp>

// ==================== FIXTURE ====================

class FileManagerTest : public ::testing::Test {
protected:
    std::unique_ptr<QTemporaryDir> tempDir;
    QString filePath;

    void SetUp() override {
        tempDir = std::make_unique<QTemporaryDir>();
        ASSERT_TRUE(tempDir->isValid())
            << "Failed to create temp directory";
        filePath = tempDir->path() + "/test_file.bin";
    }

    void TearDown() override {
        tempDir.reset();
    }

    // Вспомогательный метод: создать файл с данными
    void createFileWithData(const QByteArray& data) {
        QFile file(filePath);
        ASSERT_TRUE(file.open(QIODevice::WriteOnly));
        file.write(data);
        file.close();
    }

    // Вспомогательный метод: освободить чанки
    void freeChunks(QList<models::FileChunk*>& chunks) {
        for (auto* chunk : chunks) {
            delete chunk;
        }
        chunks.clear();
    }
};

// ==================== ТЕСТ 1: Конструктор ====================

TEST_F(FileManagerTest, Constructor_EmptyPath_Throws) {
    EXPECT_THROW(
        database::FileManager(""),
        custom_exc::database::ExceptionDateBase
        );
}

TEST_F(FileManagerTest, Constructor_ValidPath_NoThrow) {
    EXPECT_NO_THROW((database::FileManager(filePath)));
}

// ==================== ТЕСТ 2: Чтение маленького файла ====================

TEST_F(FileManagerTest, GetChunksFromFile_SmallFile) {
    // 1. Создаём файл 100 байт
    QByteArray data("Hello, world!");
    createFileWithData(data);

    // 2. Читаем чанками по 64 КБ (файл меньше чанка → 1 чанк)
    database::FileManager manager(filePath);
    QList<models::FileChunk*> chunks = manager.getChunksFromFile(64 * 1024);

    // 3. Проверяем
    ASSERT_EQ(chunks.size(), 1);
    EXPECT_EQ(chunks[0]->numberChunk, 0);
    EXPECT_EQ(chunks[0]->data, data);

    freeChunks(chunks);
}

// ==================== ТЕСТ 3: Чтение файла с несколькими чанками ====================

TEST_F(FileManagerTest, GetChunksFromFile_MultipleChunks) {
    // 1. Создаём файл 100 байт
    QByteArray data(100, 'X');
    for (int i = 0; i < data.size(); ++i) {
        data[i] = static_cast<char>(i % 256);
    }
    createFileWithData(data);

    // 2. Читаем чанками по 40 байт → 3 чанка (40, 40, 20)
    database::FileManager manager(filePath);
    QList<models::FileChunk*> chunks = manager.getChunksFromFile(40);

    // 3. Проверяем
    ASSERT_EQ(chunks.size(), 3);

    EXPECT_EQ(chunks[0]->numberChunk, 0);
    EXPECT_EQ(chunks[0]->data.size(), 40);
    EXPECT_EQ(chunks[0]->data, data.mid(0, 40));

    EXPECT_EQ(chunks[1]->numberChunk, 1);
    EXPECT_EQ(chunks[1]->data.size(), 40);
    EXPECT_EQ(chunks[1]->data, data.mid(40, 40));

    EXPECT_EQ(chunks[2]->numberChunk, 2);
    EXPECT_EQ(chunks[2]->data.size(), 20);
    EXPECT_EQ(chunks[2]->data, data.mid(80, 20));

    freeChunks(chunks);
}

// ==================== ТЕСТ 4: Чтение пустого файла ====================

TEST_F(FileManagerTest, GetChunksFromFile_EmptyFile) {
    // 1. Создаём пустой файл
    createFileWithData(QByteArray());

    // 2. Читаем
    database::FileManager manager(filePath);
    QList<models::FileChunk*> chunks = manager.getChunksFromFile(64 * 1024);

    // 3. Пустой файл → пустой список
    EXPECT_EQ(chunks.size(), 0);
}

// ==================== ТЕСТ 5: Чтение с sizeChunk <= 0 ====================

TEST_F(FileManagerTest, GetChunksFromFile_InvalidSizeChunk_Throws) {
    createFileWithData("test");
    database::FileManager manager(filePath);

    EXPECT_THROW(manager.getChunksFromFile(0),
                 custom_exc::database::ExceptionDateBase);
    EXPECT_THROW(manager.getChunksFromFile(-1),
                 custom_exc::database::ExceptionDateBase);
}

// ==================== ТЕСТ 6: Чтение несуществующего файла ====================

TEST_F(FileManagerTest, GetChunksFromFile_NonExistentFile_Throws) {
    database::FileManager manager("/non/existent/file.bin");

    EXPECT_THROW(manager.getChunksFromFile(64 * 1024),
                 custom_exc::database::ExceptionDateBase);
}

// ==================== ТЕСТ 7: Запись одного чанка ====================

TEST_F(FileManagerTest, SaveFileFromFileChunks_SingleChunk) {
    // 1. Создаём чанк
    auto* chunk = new models::FileChunk;
    chunk->numberChunk = 0;
    chunk->data = "Hello, world!";

    QList<models::FileChunk*> chunks;
    chunks.append(chunk);

    // 2. Сохраняем
    database::FileManager manager(filePath);
    EXPECT_NO_THROW(manager.saveFileFromFileChunks(chunks));

    // 3. Проверяем, что файл создан
    QFile file(filePath);
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray readData = file.readAll();
    file.close();

    EXPECT_EQ(readData, QByteArray("Hello, world!"));

    freeChunks(chunks);
}

// ==================== ТЕСТ 8: Запись нескольких чанков ====================

TEST_F(FileManagerTest, SaveFileFromFileChunks_MultipleChunks) {
    // 1. Создаём 3 чанка
    QList<models::FileChunk*> chunks;

    auto* chunk1 = new models::FileChunk;
    chunk1->numberChunk = 0;
    chunk1->data = "First";

    auto* chunk2 = new models::FileChunk;
    chunk2->numberChunk = 1;
    chunk2->data = "Second";

    auto* chunk3 = new models::FileChunk;
    chunk3->numberChunk = 2;
    chunk3->data = "Third";

    chunks << chunk1 << chunk2 << chunk3;

    // 2. Сохраняем
    database::FileManager manager(filePath);
    manager.saveFileFromFileChunks(chunks);

    // 3. Проверяем
    QFile file(filePath);
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray readData = file.readAll();
    file.close();

    EXPECT_EQ(readData, QByteArray("FirstSecondThird"));

    freeChunks(chunks);
}

// ==================== ТЕСТ 9: Запись чанков в неправильном порядке ====================

TEST_F(FileManagerTest, SaveFileFromFileChunks_OutOfOrder_Sorted) {
    // 1. Создаём чанки в неправильном порядке
    QList<models::FileChunk*> chunks;

    auto* chunk1 = new models::FileChunk;
    chunk1->numberChunk = 2;
    chunk1->data = "Third";

    auto* chunk2 = new models::FileChunk;
    chunk2->numberChunk = 0;
    chunk2->data = "First";

    auto* chunk3 = new models::FileChunk;
    chunk3->numberChunk = 1;
    chunk3->data = "Second";

    chunks << chunk1 << chunk2 << chunk3;

    // 2. Сохраняем — метод должен отсортировать
    database::FileManager manager(filePath);
    EXPECT_NO_THROW(manager.saveFileFromFileChunks(chunks));

    // 3. Проверяем, что данные в правильном порядке
    QFile file(filePath);
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray readData = file.readAll();
    file.close();

    EXPECT_EQ(readData, QByteArray("FirstSecondThird"));

    freeChunks(chunks);
}

// ==================== ТЕСТ 10: Запись пропущенных чанков ====================

TEST_F(FileManagerTest, SaveFileFromFileChunks_MissingChunk_Throws) {
    // 1. Создаём чанки с пропуском (0, 1, 3 — пропущен 2)
    QList<models::FileChunk*> chunks;

    auto* chunk0 = new models::FileChunk;
    chunk0->numberChunk = 0;
    chunk0->data = "First";

    auto* chunk1 = new models::FileChunk;
    chunk1->numberChunk = 1;
    chunk1->data = "Second";

    auto* chunk3 = new models::FileChunk;
    chunk3->numberChunk = 3;  // ← пропущен 2
    chunk3->data = "Fourth";

    chunks << chunk0 << chunk1 << chunk3;

    // 2. Сохраняем — должно бросить исключение
    database::FileManager manager(filePath);
    EXPECT_THROW(manager.saveFileFromFileChunks(chunks),
                 custom_exc::database::ExceptionDateBase);

    freeChunks(chunks);
}

// ==================== ТЕСТ 11: Запись пустого списка ====================

TEST_F(FileManagerTest, SaveFileFromFileChunks_EmptyList_Throws) {
    QList<models::FileChunk*> chunks;

    database::FileManager manager(filePath);
    EXPECT_THROW(manager.saveFileFromFileChunks(chunks),
                 custom_exc::database::ExceptionDateBase);
}

// ==================== ТЕСТ 12: Запись null-чанка ====================

TEST_F(FileManagerTest, SaveFileFromFileChunks_NullChunk_Throws) {
    QList<models::FileChunk*> chunks;
    chunks.append(nullptr);

    database::FileManager manager(filePath);
    EXPECT_THROW(manager.saveFileFromFileChunks(chunks),
                 custom_exc::database::ExceptionDateBase);

    // nullptr удалять не нужно
    chunks.clear();
}

// ==================== ТЕСТ 13: Полный цикл чтение → запись ====================

TEST_F(FileManagerTest, FullCycle_ReadWriteRead) {
    // 1. Создаём исходный файл 1000 байт
    QByteArray originalData(1000, 'A');
    for (int i = 0; i < originalData.size(); ++i) {
        originalData[i] = static_cast<char>(i % 256);
    }
    createFileWithData(originalData);

    // 2. Читаем чанками по 100 байт
    database::FileManager manager(filePath);
    QList<models::FileChunk*> chunks = manager.getChunksFromFile(100);

    ASSERT_EQ(chunks.size(), 10);

    // 3. Создаём новый файл и записываем туда
    QString newFilePath = tempDir->path() + "/restored.bin";
    database::FileManager newManager(newFilePath);
    newManager.saveFileFromFileChunks(chunks);

    // 4. Читаем новый файл
    QFile file(newFilePath);
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray restoredData = file.readAll();
    file.close();

    // 5. Проверяем, что данные совпадают
    EXPECT_EQ(restoredData, originalData);

    freeChunks(chunks);
}

// ==================== ТЕСТ 14: Полный цикл с большим файлом ====================

TEST_F(FileManagerTest, FullCycle_LargeFile) {
    // 1. Данные 1 МБ
    QByteArray originalData(1024 * 1024, 'Z');
    for (int i = 0; i < originalData.size(); ++i) {
        originalData[i] = static_cast<char>(i % 256);
    }
    createFileWithData(originalData);

    // 2. Читаем чанками по 64 КБ
    database::FileManager manager(filePath);
    QList<models::FileChunk*> chunks = manager.getChunksFromFile(64 * 1024);

    ASSERT_EQ(chunks.size(), 16);  // 1 МБ / 64 КБ = 16

    // 3. Записываем в новый файл
    QString newFilePath = tempDir->path() + "/restored_large.bin";
    database::FileManager newManager(newFilePath);
    newManager.saveFileFromFileChunks(chunks);

    // 4. Читаем и сравниваем
    QFile file(newFilePath);
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray restoredData = file.readAll();
    file.close();

    EXPECT_EQ(restoredData, originalData);

    freeChunks(chunks);
}

// ==================== ТЕСТ 15: Размер чанка = 1 байт ====================

TEST_F(FileManagerTest, GetChunksFromFile_ChunkSizeOne) {
    // 1. Файл 5 байт
    QByteArray data("Hello");
    createFileWithData(data);

    // 2. Читаем по 1 байту
    database::FileManager manager(filePath);
    QList<models::FileChunk*> chunks = manager.getChunksFromFile(1);

    // 3. Должно быть 5 чанков
    ASSERT_EQ(chunks.size(), 5);

    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(chunks[i]->numberChunk, static_cast<quint64>(i));
        EXPECT_EQ(chunks[i]->data.size(), 1);
        EXPECT_EQ(chunks[i]->data[0], data[i]);
    }

    freeChunks(chunks);
}
