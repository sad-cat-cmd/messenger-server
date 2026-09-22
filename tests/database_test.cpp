#include <gtest/gtest.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDir>
#include <QTemporaryDir>
#include <memory>
#include "database.hpp"
#include "models.hpp"

class DatabaseFileTest : public ::testing::Test {
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

TEST_F(DatabaseFileTest, DatabaseFileIsCreated) {
    EXPECT_TRUE(QFile::exists(dbPath));
    EXPECT_GT(QFile(dbPath).size(), 0)
        << "Database file is empty";
}

// ==================== ТЕСТ 2: Таблицы созданы ====================

TEST_F(DatabaseFileTest, TablesAreCreated) {
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

TEST_F(DatabaseFileTest, IndexesAreCreated) {
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

TEST_F(DatabaseFileTest, AddUserAndRetrieve) {
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

TEST_F(DatabaseFileTest, DataPersistsAcrossReopen) {
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

TEST_F(DatabaseFileTest, ForeignKeysAreEnabled) {
    QSqlDatabase db = manager->getDatabase();

    QSqlQuery query(db);
    ASSERT_TRUE(query.exec("PRAGMA foreign_keys"));
    ASSERT_TRUE(query.next());

    EXPECT_EQ(query.value(0).toInt(), 1)
        << "Foreign keys are NOT enabled!";
}

// ==================== ТЕСТ 7: ON DELETE CASCADE работает ====================

TEST_F(DatabaseFileTest, DeleteCascadeWorks) {
    // 1. Создаём двух пользователей
    auto* u1 = manager->addUser("u1", "user1", "hash1");
    auto* u2 = manager->addUser("u2", "user2", "hash2");
    ASSERT_NE(u1, nullptr);
    ASSERT_NE(u2, nullptr);
    delete u1;
    delete u2;

    // 2. Создаём чат
    auto* chat = manager->addChat("chat-1", "u1", "u2");
    ASSERT_NE(chat, nullptr);
    delete chat;

    // 3. Удаляем одного пользователя
    QSqlDatabase db = manager->getDatabase();
    QSqlQuery deleteUser(db);
    deleteUser.prepare("DELETE FROM users WHERE id = ?");
    deleteUser.addBindValue("u1");
    ASSERT_TRUE(deleteUser.exec());

    // 4. Проверяем, что чат удалился каскадно
    QSqlQuery checkChat(db);
    checkChat.prepare("SELECT COUNT(*) FROM chats WHERE id = ?");
    checkChat.addBindValue("chat-1");
    ASSERT_TRUE(checkChat.exec());
    ASSERT_TRUE(checkChat.next());

    EXPECT_EQ(checkChat.value(0).toInt(), 0)
        << "ON DELETE CASCADE did not work!";
}

// ==================== ТЕСТ 8: AddChat с сортировкой ====================

TEST_F(DatabaseFileTest, AddChatSortsUserIds) {
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

TEST_F(DatabaseFileTest, DuplicateUserThrows) {
    auto* u1 = manager->addUser("dup-id", "user1", "hash1");
    ASSERT_NE(u1, nullptr);
    delete u1;

    // Второй addUser с тем же ID должен бросить исключение
    EXPECT_THROW(
        manager->addUser("dup-id", "user2", "hash2"),
        custom_exc::database::ExceptionDateBase);
}

// ==================== ТЕСТ 10: Транзакции ====================

TEST_F(DatabaseFileTest, TransactionRollback) {
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
