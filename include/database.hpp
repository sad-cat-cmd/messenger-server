#ifndef DATABASE_H
#define DATABASE_H

#include <models.hpp>
#include <exceptions.hpp>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QList>

namespace database {
/**
         * @namespace sqllite_requests
         * @brief SQL-запросы для работы с базой данных SQLite.
         *
         * @details Содержит все SQL-запросы, используемые в приложении,
         * в виде константных строк. Запросы разделены на категории:
         *
         * - **CREATE** — создание таблиц и индексов
         * - **INSERT** — вставка новых записей
         * - **SELECT** — выборка данных
         * - **UPDATE** — обновление записей
         *
         * @note Все INSERT/SELECT/UPDATE запросы используют
         * позиционные placeholder'ы (`?`) для защиты от SQL-инъекций
         * через QSqlQuery::prepare() и QSqlQuery::addBindValue().
         *
         * @warning Для работы FOREIGN KEY и ON DELETE CASCADE
         * необходимо выполнить `PRAGMA foreign_keys = ON` при открытии БД.
         */
    namespace sqllite_requests {

        // ==================== CREATE TABLES ====================

        /**
                     * @brief SQL-запрос для создания таблицы пользователей.
                     *
                     * @details Таблица содержит:
                     * - `id` — уникальный идентификатор (TEXT, PRIMARY KEY)
                     * - `username` — логин (UNIQUE, NOT NULL)
                     * - `password` — хеш пароля (NOT NULL)
                     * - `registration` — дата регистрации (DEFAULT CURRENT_TIMESTAMP)
                     * - `last_log_in` — дата последнего входа (DEFAULT CURRENT_TIMESTAMP)
                     */
        const QString CREATE_USERS_TABLE = R"(
                        CREATE TABLE IF NOT EXISTS users (
                            id TEXT PRIMARY KEY NOT NULL,
                            username TEXT NOT NULL UNIQUE,
                            password TEXT NOT NULL,
                            registration DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
                            last_log_in DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
                        )
                    )";

        /**
                     * @brief SQL-запрос для создания таблицы чатов.
                     *
                     * @details Таблица содержит:
                     * - `id` — уникальный идентификатор чата (TEXT, PRIMARY KEY)
                     * - `id_user_1` — ID первого участника (FK → users.id)
                     * - `id_user_2` — ID второго участника (FK → users.id)
                     * - `count_msg` — количество сообщений в чате (DEFAULT 0)
                     *
                     * @note Ограничения:
                     * - `UNIQUE(id_user_1, id_user_2)` — защита от дубликатов чатов
                     * - `CHECK(id_user_1 < id_user_2)` — порядок участников (сортировка)
                     * - `ON DELETE CASCADE` — при удалении пользователя его чаты удаляются
                     *
                     * @warning Для работы FK нужно `PRAGMA foreign_keys = ON`.
                     */
        const QString CREATE_CHATS_TABLE = R"(
                        CREATE TABLE IF NOT EXISTS chats (
                            id TEXT PRIMARY KEY NOT NULL,
                            id_user_1 TEXT NOT NULL,
                            id_user_2 TEXT NOT NULL,
                            count_msg INTEGER NOT NULL DEFAULT 0,
                            UNIQUE (id_user_1, id_user_2),
                            CHECK (id_user_1 < id_user_2),
                            CHECK (id_user_1 != id_user_2),
                            FOREIGN KEY (id_user_1) REFERENCES users(id) ON DELETE CASCADE,
                            FOREIGN KEY (id_user_2) REFERENCES users(id) ON DELETE CASCADE
                        )
                    )";

        /**
                     * @brief SQL-запрос для создания таблицы сообщений.
                     *
                     * @details Таблица содержит:
                     * - `id` — уникальный идентификатор сообщения (TEXT, PRIMARY KEY)
                     * - `id_parent_chat` — ID чата, к которому относится сообщение (FK → chats.id)
                     * - `created_at` — дата создания (DEFAULT CURRENT_TIMESTAMP)
                     * - `owner` — ID отправителя (FK → users.id)
                     * - `receiver` — ID получателя (FK → users.id)
                     * - `number_msg` — порядковый номер сообщения в чате
                     * - `is_readed` — флаг прочтения (0 — не прочитано, 1 — прочитано)
                     * - `is_file` — флаг файла (0 — текст, 1 — файл)
                     *
                     * @note Ограничения:
                     * - `CHECK(is_file IN (0,1))` — только 0 или 1
                     * - `CHECK(is_readed IN (0,1))` — только 0 или 1
                     * - `ON DELETE CASCADE` — при удалении чата/пользователя сообщения удаляются
                     */
        const QString CREATE_MSGS_TABLE = R"(
                        CREATE TABLE IF NOT EXISTS messages (
                            id TEXT PRIMARY KEY NOT NULL,
                            id_parent_chat TEXT NOT NULL,
                            created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
                            owner TEXT NOT NULL,
                            receiver TEXT NOT NULL,
                            number_msg INTEGER NOT NULL DEFAULT 0,
                            is_readed INTEGER NOT NULL DEFAULT 0,
                            is_file INTEGER NOT NULL DEFAULT 0,
                            CHECK (is_file IN (1, 0)),
                            CHECK (is_readed IN (0, 1)),
                            FOREIGN KEY (id_parent_chat) REFERENCES chats(id) ON DELETE CASCADE,
                            FOREIGN KEY (owner) REFERENCES users(id) ON DELETE CASCADE,
                            FOREIGN KEY (receiver) REFERENCES users(id) ON DELETE CASCADE
                        )
                    )";

        /**
                     * @brief SQL-запрос для создания таблицы файлов.
                     *
                     * @details Таблица содержит:
                     * - `id` — уникальный идентификатор файла (TEXT, PRIMARY KEY)
                     * - `id_parent_msg` — ID сообщения, к которому прикреплён файл (FK → messages.id)
                     * - `local_path_file` — локальный путь к файлу
                     *
                     * @note `ON DELETE CASCADE` — при удалении сообщения файл удаляется.
                     */
        const QString CREATE_FILES_TABLE = R"(
                        CREATE TABLE IF NOT EXISTS files(
                            id TEXT PRIMARY KEY NOT NULL,
                            id_parent_msg TEXT NOT NULL,
                            local_path_file TEXT NOT NULL,
                            FOREIGN KEY (id_parent_msg) REFERENCES messages(id) ON DELETE CASCADE
                        )
                    )";

        // ==================== CREATE INDEXES ====================

        /** @brief Индекс по `chats.id_user_1` для ускорения поиска чатов пользователя. */
        const QString CREATE_INDEX_CHAT_ID_USER_1 =
            "CREATE INDEX IF NOT EXISTS idx_chats_user1 ON chats(id_user_1)";

        /** @brief Индекс по `chats.id_user_2` для ускорения поиска чатов пользователя. */
        const QString CREATE_INDEX_CHAT_ID_USER_2 =
            "CREATE INDEX IF NOT EXISTS idx_chats_user2 ON chats(id_user_2)";

        /** @brief Индекс по `messages.id_parent_chat` для выборки сообщений чата. */
        const QString CREATE_INDEX_MESSAGES_ID_PARENT_CHAT =
            "CREATE INDEX IF NOT EXISTS idx_messages_chat ON messages(id_parent_chat)";

        /** @brief Индекс по `messages.created_at` для сортировки сообщений по времени. */
        const QString CREATE_INDEX_MESSAGES_CREATED_AT =
            "CREATE INDEX IF NOT EXISTS idx_messages_created ON messages(created_at)";

        /** @brief Индекс по `files.id_parent_msg` для выборки файлов сообщения. */
        const QString CREATE_INDEX_MESSAGES_FILES_ID_PARENT_MSG =
            "CREATE INDEX IF NOT EXISTS idx_files_message ON files(id_parent_msg)";

        // ==================== INSERT ====================

        /**
                     * @brief Вставка нового пользователя.
                     * @details Параметры (по порядку): id, username, password.
                     * @note registration и last_log_in заполняются автоматически (DEFAULT).
                     */
        const QString INSERT_USER =
            "INSERT INTO users(id, username, password) VALUES (?, ?, ?)";

        /**
                     * @brief Вставка нового чата.
                     * @details Параметры (по порядку): id, id_user_1, id_user_2.
                     * @note count_msg заполняется автоматически (DEFAULT 0).
                     * @warning id_user_1 должен быть меньше id_user_2 (см. CHECK в таблице).
                     */
        const QString INSERT_CHAT =
            "INSERT INTO chats(id, id_user_1, id_user_2) VALUES (?, ?, ?)";

        /**
                     * @brief Вставка нового сообщения.
                     * @details Параметры (по порядку):
                     * id, id_parent_chat, owner, receiver, number_msg, is_readed, is_file.
                     * @note created_at заполняется автоматически (DEFAULT CURRENT_TIMESTAMP).
                     */
        const QString INSERT_MSG =
            "INSERT INTO messages(id, id_parent_chat, owner, receiver, number_msg, is_file) "
            "VALUES (?, ?, ?, ?, ?, ?)";

        /**
                     * @brief Вставка нового файла.
                     * @details Параметры (по порядку): id, id_parent_msg, local_path_file.
                     */
        const QString INSERT_FILE =
            "INSERT INTO files(id, id_parent_msg, local_path_file) VALUES (?, ?, ?)";

        // ==================== SELECT ====================

        /**
                     * @brief Выборка всех чатов пользователя.
                     * @details Параметры (по порядку): id_user_1, id_user_2 (обычно один и тот же ID).
                     * @return Все чаты, где пользователь является участником.
                     */
        const QString SELECT_ALL_CHATS_BY_USER_ID =
            "SELECT * FROM chats WHERE id_user_1 = ? or id_user_2 = ?";

        /**
                     * @brief Выборка всех сообщений чата.
                     * @details Параметры: id_parent_chat.
                     * @return Сообщения, отсортированные по number_msg.
                     */
        const QString SELECT_ALL_MESSAGES_BY_CHAT_ID =
            "SELECT * FROM messages WHERE id_parent_chat = ? ORDER BY number_msg";

        /**
                     * @brief Выборка файла по ID сообщения.
                     * @details Параметры: id_parent_msg.
                     */
        const QString SELECT_FILE_BY_MSG_ID =
            "SELECT * FROM files WHERE id_parent_msg = ?";

        /**
                     * @brief Выборка пользователя по ID.
                     * @details Параметры: id.
                     */
        const QString SELECT_USER_BY_ID =
            "SELECT * FROM users WHERE id = ?";

        /**
                     * @brief Выборка пользователя по username.
                     * @details Параметры: username.
                     */
        const QString SELECT_USER_BY_USERNAME =
            "SELECT * FROM users WHERE username = ?";

        /**
                     * @brief Выборка чата по ID.
                     * @details Параметры: id.
                     */
        const QString SELECT_CHAT_BY_ID =
            "SELECT * FROM chats WHERE id = ?";

        const QString SELECT_MSG_BY_ID =
            "SELECT * FROM messages WHERE id = ?";
        // ==================== UPDATE ====================

        /**
                     * @brief Обновление счётчика сообщений в чате.
                     * @details Параметры (по порядку): count_msg, id.
                     * @warning Лучше использовать триггеры для автоматического обновления.
                     */
        const QString UPDATE_CHAT_COUNT_MSG =
            "UPDATE chats SET count_msg = ? WHERE id = ?";

        /**
                     * @brief Отметка сообщения как прочитанного.
                     * @details Параметры: id.
                     */
        const QString UPDATE_MSG_IS_READED =
            "UPDATE messages SET is_readed = 1 WHERE id = ?";

    } // namespace sqllite_requests

    class DatabaseManager {
    private:
        QSqlDatabase db_;
        void createTables();
        void createIndexes();
        void enableForeignKeys();
    public:
        DatabaseManager(const QString & dbPath);
        void init();
        ~DatabaseManager();
        models::User * addUser(const QString & userId,
                               const QString & username,
                               const QString & password);

        models::User * getUserById(const QString & userId);
        models::User * getUserByUsername(const QString & username);

        models::Chat * addChat (const QString & idChat,
                                const QString & idUser1,
                                const QString & idUser2);
        models::Chat * getChatById (const QString & idChat);
        QList<models::Chat *> getAllChatsByUserId (const QString & userId);
        models::Chat * setCountMsgInChatByChatId (const QString & idChat,
                                                  const quint64 newCount);

        models::Message * addMsg (const QString &idMsg,
                                  const QString idChat,
                                  const QString idOwner,
                                  const QString idReceiver,
                                  const quint64 numMsg,
                                  const bool flagFile);
        models::Message * getMsgById (const QString idMsg);
        models::Message * setMsgIsReaded (const QString &idMsg);
        QList<models::Message *> getAllMsgsByChatId (const QString & chatId);

        models::File * addMsgFile (const QString & idFile,
                                   const QString & localPath,
                                   const QString & idMsg,
                                   const QString & idChat,
                                   const QString & idOwner,
                                   const QString & idReceiver,
                                   const quint64 numMsg);

        models::File * getFileByMsgId (const QString &idMsg);
    };
}


#endif // DATABASE_H
