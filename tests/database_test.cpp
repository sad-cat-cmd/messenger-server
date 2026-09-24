#include <gtest/gtest.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>
#include <QTemporaryDir>
#include <memory>
#include "database.hpp"
#include "models.hpp"

class DatabaseTest : public ::testing::Test {
protected:
    std::unique_ptr<QTemporaryDir> tempDir;
    std::unique_ptr<database::DatabaseManager> manager;
    QString dbPath;

    void SetUp() override {
        // 1. Создаём временную директорию (удалится автоматически в TearDown)
        tempDir = std::make_unique<QTemporaryDir>();
        ASSERT_TRUE(tempDir->isValid())
            << "Failed to create temp directory";

        // 2. Путь к файлу БД внутри временной директории
        dbPath = tempDir->path() + "/test_messenger.db";

        // 3. Создаём менеджер — он создаст файл
        manager = std::make_unique<database::DatabaseManager>(dbPath);
        manager->init();

        // 4. Проверяем, что файл реально создан
        ASSERT_TRUE(QFile::exists(dbPath))
            << "Database file was not created: " << dbPath.toStdString();
    }

    void TearDown() override {
        // 1. Уничтожаем менеджер (закроет соединение)
        manager.reset();

        // 2. Директория удалится автоматически при уничтожении QTemporaryDir
        tempDir.reset();
    }
};

// ==================== ТЕСТ 1: Файл создан ====================

TEST_F(DatabaseTest, DatabaseFileIsCreated) {
    EXPECT_TRUE(QFile::exists(dbPath));
    EXPECT_GT(QFile(dbPath).size(), 0)
        << "Database file is empty";
}

// ==================== ТЕСТ 2: Таблицы созданы ====================

TEST_F(DatabaseTest, TablesAreCreated) {
    QSqlDatabase db = manager->getDatabase();

    QSqlQuery query(db);
    ASSERT_TRUE(query.exec(
        "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name"
        ));

    QStringList tables;
    while (query.next()) {
        tables << query.value(0).toString();
    }

    EXPECT_TRUE(tables.contains("users"));
    EXPECT_TRUE(tables.contains("chats"));
    EXPECT_TRUE(tables.contains("messages"));
    EXPECT_TRUE(tables.contains("files"));
}

// ==================== ТЕСТ 3: Индексы созданы ====================

TEST_F(DatabaseTest, IndexesAreCreated) {
    QSqlDatabase db = manager->getDatabase();

    QSqlQuery query(db);
    ASSERT_TRUE(query.exec(
        "SELECT name FROM sqlite_master WHERE type='index'"
        ));

    QStringList indexes;
    while (query.next()) {
        indexes << query.value(0).toString();
    }

    EXPECT_TRUE(indexes.contains("idx_chats_user1"));
    EXPECT_TRUE(indexes.contains("idx_chats_user2"));
    EXPECT_TRUE(indexes.contains("idx_messages_chat"));
}

// ==================== ТЕСТ 4: AddUser и GetUserById ====================

TEST_F(DatabaseTest, AddUserAndRetrieve) {
    auto* user = manager->addUser("user-123", "testuser", "hashed_password");
    ASSERT_NE(user, nullptr);
    EXPECT_EQ(user->id.toStdString(), "user-123");
    EXPECT_EQ(user->username.toStdString(), "testuser");
    delete user;

    auto* fetched = manager->getUserById("user-123");
    ASSERT_NE(fetched, nullptr);
    EXPECT_EQ(fetched->username.toStdString(), "testuser");
    delete fetched;
}

// ==================== ТЕСТ 5: Данные сохраняются на диск ====================

TEST_F(DatabaseTest, DataPersistsAcrossReopen) {
    // 1. Добавляем пользователя
    auto* user = manager->addUser("user-persist", "persist_user", "hash");
    ASSERT_NE(user, nullptr);
    delete user;

    // 2. Закрываем менеджер
    manager.reset();

    // 3. Открываем заново
    manager = std::make_unique<database::DatabaseManager>(dbPath);
    manager->init();

    // 4. Проверяем, что пользователь на месте
    auto* fetched = manager->getUserById("user-persist");
    ASSERT_NE(fetched, nullptr) << "User not found after reopen";
    EXPECT_EQ(fetched->username.toStdString(), "persist_user");
    delete fetched;
}

// ==================== ТЕСТ 6: FOREIGN KEY работает ====================

TEST_F(DatabaseTest, ForeignKeysAreEnabled) {
    QSqlDatabase db = manager->getDatabase();

    QSqlQuery query(db);
    ASSERT_TRUE(query.exec("PRAGMA foreign_keys"));
    ASSERT_TRUE(query.next());

    EXPECT_EQ(query.value(0).toInt(), 1)
        << "Foreign keys are NOT enabled!";
}

// ==================== ТЕСТ 7 (обновлён): CASCADE удаляет чат И сообщения ====================

TEST_F(DatabaseTest, DeleteCascadeWorks) {
    // 1. Создаём пользователей
    auto* u1 = manager->addUser("u1", "user1", "hash1");
    auto* u2 = manager->addUser("u2", "user2", "hash2");
    delete u1;
    delete u2;

    // 2. Создаём чат
    auto* chat = manager->addChat("chat-1", "u1", "u2");
    delete chat;

    // 3. Добавляем сообщение
    auto* msg = manager->addMsg("msg-1", "chat-1", "u1", "u2",
                                "Hello", 1, false, 0);
    delete msg;

    // 4. Удаляем пользователя u1
    QSqlDatabase db = manager->getDatabase();
    QSqlQuery deleteUser(db);
    deleteUser.prepare("DELETE FROM users WHERE id = ?");
    deleteUser.addBindValue("u1");
    ASSERT_TRUE(deleteUser.exec());

    // 5. Проверяем: чат удалён
    QSqlQuery checkChat(db);
    checkChat.prepare("SELECT COUNT(*) FROM chats WHERE id = ?");
    checkChat.addBindValue("chat-1");
    ASSERT_TRUE(checkChat.exec());
    ASSERT_TRUE(checkChat.next());
    EXPECT_EQ(checkChat.value(0).toInt(), 0)
        << "Chat was not deleted by CASCADE";

    // 6. Проверяем: сообщение удалено (CASCADE через chat)
    QSqlQuery checkMsg(db);
    checkMsg.prepare("SELECT COUNT(*) FROM messages WHERE id = ?");
    checkMsg.addBindValue("msg-1");
    ASSERT_TRUE(checkMsg.exec());
    ASSERT_TRUE(checkMsg.next());
    EXPECT_EQ(checkMsg.value(0).toInt(), 0)
        << "Message was not deleted by CASCADE";
}
// ==================== ТЕСТ 8: AddChat с сортировкой ====================

TEST_F(DatabaseTest, AddChatSortsUserIds) {
    // ==================== ШАГ 1: Создаём пользователей ====================
    auto* uA = manager->addUser("user-a", "User A", "hash_a");
    ASSERT_NE(uA, nullptr) << "Failed to create user-a";
    delete uA;

    auto* uZ = manager->addUser("user-z", "User Z", "hash_z");
    ASSERT_NE(uZ, nullptr) << "Failed to create user-z";
    delete uZ;

    // ==================== ШАГ 2: Создаём чат ====================
    auto* chat = manager->addChat("chat-1", "user-z", "user-a");
    ASSERT_NE(chat, nullptr);

    // ==================== ШАГ 3: Проверяем сортировку ====================
    EXPECT_EQ(chat->idUser1.toStdString(), "user-a");
    EXPECT_EQ(chat->idUser2.toStdString(), "user-z");

    delete chat;
}

// ==================== ТЕСТ 9: UNIQUE constraint ====================

TEST_F(DatabaseTest, DuplicateUserThrows) {
    auto* u1 = manager->addUser("dup-id", "user1", "hash1");
    ASSERT_NE(u1, nullptr);
    delete u1;

    // Второй addUser с тем же ID должен бросить исключение
    EXPECT_THROW(
        manager->addUser("dup-id", "user2", "hash2"),
        custom_exc::database::ExceptionDateBase);
}

// ==================== ТЕСТ 10: Транзакции ====================

TEST_F(DatabaseTest, TransactionRollback) {
    QSqlDatabase db = manager->getDatabase();

    db.transaction();
    auto* u1 = manager->addUser("tx-user-1", "txuser1", "hash1");
    delete u1;

    db.rollback();

    // Пользователь не должен сохраниться
    auto* fetched = manager->getUserById("tx-user-1");
    EXPECT_EQ(fetched, nullptr) << "Rollback did not work";
    delete fetched;
}
// ==================== ТЕСТ 11: AddMsg с текстом ====================

TEST_F(DatabaseTest, AddTextMessage) {
    auto* u1 = manager->addUser("user-a", "User A", "hash_a");
    auto* u2 = manager->addUser("user-b", "User B", "hash_b");
    delete u1;
    delete u2;

    auto* chat = manager->addChat("chat-1", "user-a", "user-b");
    delete chat;

    auto* msg = manager->addMsg(
        "msg-1", "chat-1", "user-a", "user-b",
        "Hello, world!",
        1,
        false,      // is_file = 0
        0           // sizeFile = 0 (текст)
        );

    ASSERT_NE(msg, nullptr);
    EXPECT_EQ(msg->text_msg.toStdString(), "Hello, world!");
    EXPECT_EQ(msg->isFile, false);
    EXPECT_EQ(msg->sizeFile, 0);        // ← проверяем sizeFile
    EXPECT_EQ(msg->numberMsg, 1);

    delete msg;
}
// ==================== ТЕСТ 12: AddMsg с файлом ====================

TEST_F(DatabaseTest, AddFileMessage) {
    auto* u1 = manager->addUser("user-a", "User A", "hash_a");
    auto* u2 = manager->addUser("user-b", "User B", "hash_b");
    delete u1;
    delete u2;

    auto* chat = manager->addChat("chat-1", "user-a", "user-b");
    delete chat;

    const quint64 fileSize = 1024 * 1024;  // 1 MB

    auto* msg = manager->addMsg(
        "msg-file-1", "chat-1", "user-a", "user-b",
        "document.pdf",
        1,
        true,           // is_file = 1
        fileSize        // sizeFile > 0 (файл)
        );

    ASSERT_NE(msg, nullptr);
    EXPECT_EQ(msg->text_msg.toStdString(), "document.pdf");
    EXPECT_EQ(msg->isFile, true);
    EXPECT_EQ(msg->sizeFile, fileSize);    // ← проверяем sizeFile

    delete msg;
}
// ==================== ТЕСТ 13: GetAllMsgsByChatId ====================

TEST_F(DatabaseTest, GetAllMsgsByChatId) {
    auto* u1 = manager->addUser("user-a", "User A", "hash_a");
    auto* u2 = manager->addUser("user-b", "User B", "hash_b");
    delete u1;
    delete u2;

    auto* chat = manager->addChat("chat-1", "user-a", "user-b");
    delete chat;

    // Три сообщения: текст, текст, файл
    delete manager->addMsg("msg-1", "chat-1", "user-a", "user-b",
                           "First", 1, false, 0);
    delete manager->addMsg("msg-2", "chat-1", "user-b", "user-a",
                           "Second", 2, false, 0);
    delete manager->addMsg("msg-3", "chat-1", "user-a", "user-b",
                           "file.zip", 3, true, 2048);

    auto messages = manager->getAllMsgsByChatId("chat-1");

    ASSERT_EQ(messages.size(), 3);

    EXPECT_EQ(messages[0]->text_msg.toStdString(), "First");
    EXPECT_EQ(messages[0]->isFile, false);
    EXPECT_EQ(messages[0]->sizeFile, 0);

    EXPECT_EQ(messages[1]->text_msg.toStdString(), "Second");
    EXPECT_EQ(messages[1]->isFile, false);
    EXPECT_EQ(messages[1]->sizeFile, 0);

    EXPECT_EQ(messages[2]->text_msg.toStdString(), "file.zip");
    EXPECT_EQ(messages[2]->isFile, true);
    EXPECT_EQ(messages[2]->sizeFile, 2048);

    for (auto* msg : messages) delete msg;
}
// ==================== ТЕСТ 14: Сообщение слишком длинное ====================

TEST_F(DatabaseTest, MessageTooLongThrows) {
    auto* u1 = manager->addUser("user-a", "User A", "hash_a");
    auto* u2 = manager->addUser("user-b", "User B", "hash_b");
    delete u1;
    delete u2;
    auto* chat = manager->addChat("chat-1", "user-a", "user-b");
    delete chat;

    QString longText(1001, 'A');

    EXPECT_THROW(
        manager->addMsg("msg-too-long", "chat-1", "user-a", "user-b",
                        longText, 1, false, 0),
        custom_exc::database::ExceptionDateBase
        );
}
// ==================== ТЕСТ 15: Имя файла слишком длинное ====================

TEST_F(DatabaseTest, FileNameTooLongThrows) {
    auto* u1 = manager->addUser("user-a", "User A", "hash_a");
    auto* u2 = manager->addUser("user-b", "User B", "hash_b");
    delete u1;
    delete u2;
    auto* chat = manager->addChat("chat-1", "user-a", "user-b");
    delete chat;

    QString longFileName(256, 'f');

    EXPECT_THROW(
        manager->addMsg("msg-file-long", "chat-1", "user-a", "user-b",
                        longFileName, 1, true, 1024),
        custom_exc::database::ExceptionDateBase
        );
}
// ==================== ТЕСТ 16: Граничные длины ====================

TEST_F(DatabaseTest, MessageBoundaryLengths) {
    auto* u1 = manager->addUser("user-a", "User A", "hash_a");
    auto* u2 = manager->addUser("user-b", "User B", "hash_b");
    delete u1;
    delete u2;
    auto* chat = manager->addChat("chat-1", "user-a", "user-b");
    delete chat;

    // Текстовое: 1 символ
    auto* msg1 = manager->addMsg("msg-min", "chat-1", "user-a", "user-b",
                                 "A", 1, false, 0);
    EXPECT_NE(msg1, nullptr);
    delete msg1;

    // Текстовое: 1000 символов
    QString maxText(1000, 'X');
    auto* msg2 = manager->addMsg("msg-max", "chat-1", "user-a", "user-b",
                                 maxText, 2, false, 0);
    EXPECT_NE(msg2, nullptr);
    delete msg2;

    // Файл: 255 символов имени, размер 1 байт
    QString maxFile(255, 'f');
    auto* msg3 = manager->addMsg("msg-file-max", "chat-1", "user-a", "user-b",
                                 maxFile, 3, true, 1);
    EXPECT_NE(msg3, nullptr);
    delete msg3;
}
// ==================== ТЕСТ 17: Пустой текст ====================

TEST_F(DatabaseTest, EmptyTextThrows) {
    auto* u1 = manager->addUser("user-a", "User A", "hash_a");
    auto* u2 = manager->addUser("user-b", "User B", "hash_b");
    delete u1;
    delete u2;
    auto* chat = manager->addChat("chat-1", "user-a", "user-b");
    delete chat;

    EXPECT_THROW(
        manager->addMsg("msg-empty", "chat-1", "user-a", "user-b",
                        "", 1, false, 0),
        custom_exc::database::ExceptionDateBase
        );
}
// ==================== ТЕСТ 18: GetMsgsByChatIdIndexFrom ====================

TEST_F(DatabaseTest, GetMsgsByChatIdIndexFrom_Success) {
    auto* u1 = manager->addUser("user-a", "User A", "hash_a");
    auto* u2 = manager->addUser("user-b", "User B", "hash_b");
    delete u1;
    delete u2;
    auto* chat = manager->addChat("chat-1", "user-a", "user-b");
    delete chat;

    for (int i = 1; i <= 5; ++i) {
        delete manager->addMsg(
            QString("msg-%1").arg(i),
            "chat-1", "user-a", "user-b",
            QString("Message %1").arg(i),
            i, false, 0
            );
    }

    auto msgs = manager->getMsgsByChatIdIndexFrom("chat-1", 3);

    ASSERT_EQ(msgs.size(), 3);
    EXPECT_EQ(msgs[0]->numberMsg, 3);
    EXPECT_EQ(msgs[1]->numberMsg, 4);
    EXPECT_EQ(msgs[2]->numberMsg, 5);
    EXPECT_EQ(msgs[0]->text_msg.toStdString(), "Message 3");
    EXPECT_EQ(msgs[2]->text_msg.toStdString(), "Message 5");
    EXPECT_EQ(msgs[0]->sizeFile, 0);

    for (auto* msg : msgs) delete msg;
}
// ==================== ТЕСТ 19: GetMsgsByChatIdIndexFrom с 0 ====================

TEST_F(DatabaseTest, GetMsgsByChatIdIndexFrom_FromZero) {
    auto* u1 = manager->addUser("user-a", "User A", "hash_a");
    auto* u2 = manager->addUser("user-b", "User B", "hash_b");
    delete u1;
    delete u2;
    auto* chat = manager->addChat("chat-1", "user-a", "user-b");
    delete chat;

    delete manager->addMsg("msg-1", "chat-1", "user-a", "user-b", "First",  1, false, 0);
    delete manager->addMsg("msg-2", "chat-1", "user-a", "user-b", "Second", 2, false, 0);
    delete manager->addMsg("msg-3", "chat-1", "user-a", "user-b", "Third",  3, false, 0);

    auto msgs = manager->getMsgsByChatIdIndexFrom("chat-1", 0);

    ASSERT_EQ(msgs.size(), 3);
    EXPECT_EQ(msgs[0]->numberMsg, 1);
    EXPECT_EQ(msgs[1]->numberMsg, 2);
    EXPECT_EQ(msgs[2]->numberMsg, 3);

    for (auto* msg : msgs) delete msg;
}
// ==================== ТЕСТ 20: GetMsgsByChatIdIndexFrom — пустой результат ====================

TEST_F(DatabaseTest, GetMsgsByChatIdIndexFrom_EmptyResult) {
    auto* u1 = manager->addUser("user-a", "User A", "hash_a");
    auto* u2 = manager->addUser("user-b", "User B", "hash_b");
    delete u1;
    delete u2;
    auto* chat = manager->addChat("chat-1", "user-a", "user-b");
    delete chat;

    delete manager->addMsg("msg-1", "chat-1", "user-a", "user-b", "First",  1, false, 0);
    delete manager->addMsg("msg-2", "chat-1", "user-a", "user-b", "Second", 2, false, 0);

    auto msgs = manager->getMsgsByChatIdIndexFrom("chat-1", 10);

    EXPECT_EQ(msgs.size(), 0);
}
// ==================== ТЕСТ 21: GetMsgsByChatIdIndexFrom — неверный чат ====================

TEST_F(DatabaseTest, GetMsgsByChatIdIndexFrom_WrongChat) {
    auto msgs = manager->getMsgsByChatIdIndexFrom("non-existent-chat", 0);
    EXPECT_EQ(msgs.size(), 0);
}

// ==================== ТЕСТ 22: Текст с size_file > 0 отклоняется ====================

TEST_F(DatabaseTest, TextMessageWithNonZeroSizeThrows) {
    auto* u1 = manager->addUser("user-a", "User A", "hash_a");
    auto* u2 = manager->addUser("user-b", "User B", "hash_b");
    delete u1;
    delete u2;
    auto* chat = manager->addChat("chat-1", "user-a", "user-b");
    delete chat;

    // is_file = 0, но size_file = 1024 → CHECK отклоняет
    EXPECT_THROW(
        manager->addMsg("msg-bad", "chat-1", "user-a", "user-b",
                        "Text", 1, false, 1024),
        custom_exc::database::ExceptionDateBase
        );
}
// ==================== ТЕСТ 23: Файл с size_file = 0 отклоняется ====================

TEST_F(DatabaseTest, FileMessageWithZeroSizeThrows) {
    auto* u1 = manager->addUser("user-a", "User A", "hash_a");
    auto* u2 = manager->addUser("user-b", "User B", "hash_b");
    delete u1;
    delete u2;
    auto* chat = manager->addChat("chat-1", "user-a", "user-b");
    delete chat;

    // is_file = 1, но size_file = 0 → CHECK отклоняет
    EXPECT_THROW(
        manager->addMsg("msg-bad", "chat-1", "user-a", "user-b",
                        "file.zip", 1, true, 0),
        custom_exc::database::ExceptionDateBase
        );
}
// ==================== ТЕСТ 24: Границы size_file ====================

TEST_F(DatabaseTest, SizeFileBoundaryValues) {
    auto* u1 = manager->addUser("user-a", "User A", "hash_a");
    auto* u2 = manager->addUser("user-b", "User B", "hash_b");
    delete u1;
    delete u2;
    auto* chat = manager->addChat("chat-1", "user-a", "user-b");
    delete chat;

    // Минимальный файл: 1 байт
    auto* msg1 = manager->addMsg("msg-1", "chat-1", "user-a", "user-b",
                                 "tiny.txt", 1, true, 1);
    ASSERT_NE(msg1, nullptr);
    EXPECT_EQ(msg1->sizeFile, 1);
    delete msg1;

    // Большой файл: 100 MB
    quint64 bigSize = 100ULL * 1024 * 1024;
    auto* msg2 = manager->addMsg("msg-2", "chat-1", "user-a", "user-b",
                                 "big.iso", 2, true, bigSize);
    ASSERT_NE(msg2, nullptr);
    EXPECT_EQ(msg2->sizeFile, bigSize);
    delete msg2;
}
// ==================== ТЕСТ 25: getMsgById возвращает sizeFile ====================

TEST_F(DatabaseTest, GetMsgByIdPreservesSizeFile) {
    auto* u1 = manager->addUser("user-a", "User A", "hash_a");
    auto* u2 = manager->addUser("user-b", "User B", "hash_b");
    delete u1;
    delete u2;
    auto* chat = manager->addChat("chat-1", "user-a", "user-b");
    delete chat;

    const quint64 fileSize = 4096;
    delete manager->addMsg("msg-file", "chat-1", "user-a", "user-b",
                           "doc.pdf", 1, true, fileSize);

    auto* fetched = manager->getMsgById("msg-file");
    ASSERT_NE(fetched, nullptr);
    EXPECT_EQ(fetched->isFile, true);
    EXPECT_EQ(fetched->sizeFile, fileSize);
    delete fetched;
}


