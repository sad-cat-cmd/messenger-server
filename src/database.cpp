#include <database.hpp>

void database::DatabaseManager::enableForeignKeys() {
    QString msgErr;
    QSqlQuery query(db_);
    if (!query.exec("PRAGMA foreign_keys = ON")) {
        msgErr = "DatabaseManager::enableForeignKeys(). Failed to create tables:" + query.lastError().text();
        throw custom_exc::database::ExceptionDateBase(msgErr,
                                                      2);
    }
}

void database::DatabaseManager::createTables() {
    QString msgErr;
    const QStringList tables = {
        sqllite_requests::CREATE_USERS_TABLE,
        sqllite_requests::CREATE_CHATS_TABLE,
        sqllite_requests::CREATE_MSGS_TABLE,
        sqllite_requests::CREATE_FILES_TABLE
    };

    for (const QString& sql : tables) {
        QSqlQuery query(db_);
        if (!query.exec(sql)) {
            msgErr = "DatabaseManager::createTables(). Failed to create table:" + query.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr,
                                                          3);
        }
    }
}
void database::DatabaseManager::createIndexes()
{
    QString msgErr;
    const QStringList indexes = {
        sqllite_requests::CREATE_INDEX_CHAT_ID_USER_1,
        sqllite_requests::CREATE_INDEX_CHAT_ID_USER_2,
        sqllite_requests::CREATE_INDEX_MESSAGES_ID_PARENT_CHAT,
        sqllite_requests::CREATE_INDEX_MESSAGES_CREATED_AT,
        sqllite_requests::CREATE_INDEX_MESSAGES_FILES_ID_PARENT_MSG
    };

    for (const QString& sql : indexes) {
        QSqlQuery query(db_);
        if (!query.exec(sql)) {
            msgErr = "DatabaseManager::createIndexes(). Failed to create index:" + query.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr,
                                                          4);
        }
    }
}
database::DatabaseManager::DatabaseManager(const QString & dbPath)
{
    this->connectionName = "DatabaseManager_" +
                           QUuid::createUuid().toString(QUuid::WithoutBraces);
    db_ = QSqlDatabase::addDatabase("QSQLITE" , this->connectionName);
    db_.setDatabaseName(dbPath);
}
database::DatabaseManager::~DatabaseManager() {
    if (db_.isOpen()) {
        db_.close();
    }
    db_ = QSqlDatabase();
    QSqlDatabase::removeDatabase(this->connectionName);
}
QString database::DatabaseManager::getConnectionName()
{
    return this->getConnectionName();
}
QSqlDatabase& database::DatabaseManager::getDatabase()
{
    return this->db_;
}
void database::DatabaseManager::init()
{
    QString msgErr;
    if (!db_.open()) {
        msgErr = "DatabaseManager::init(). Failed to create index:" + db_.lastError().text();
        throw custom_exc::database::ExceptionDateBase(msgErr,
                                                      1);
    }
    try {
        this->enableForeignKeys();
        this->createTables();
        this->createIndexes();
    }
    catch (custom_exc::database::ExceptionDateBase & exc) {
        throw exc;
    }
}

models::User* database::DatabaseManager::addUser(const QString& userId,
                                                 const QString& username,
                                                 const QString& password)
{
    QString msgErr;
    {
        QSqlQuery selectQuery(db_);
        selectQuery.prepare(sqllite_requests::SELECT_USER_BY_ID);
        selectQuery.addBindValue(userId);

        if (!selectQuery.exec()) {
            msgErr = "DatabaseManager::addUser(). SELECT_USER_BY_ID failed: "
                     + selectQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 5);
        }

        if (selectQuery.next()) {
            msgErr = "DatabaseManager::addUser(). This userName is not avaible: "
                     + selectQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 5);
        }
    }

    {
        QSqlQuery insertQuery(db_);
        insertQuery.prepare(sqllite_requests::INSERT_USER);
        insertQuery.addBindValue(userId);
        insertQuery.addBindValue(username);
        insertQuery.addBindValue(password);

        if (!insertQuery.exec()) {
            msgErr = "DatabaseManager::addUser(). INSERT_USER failed: "
                     + insertQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 5);
        }
    }
    try {
        return this->getUserById(userId);
    }
    catch (custom_exc::database::ExceptionDateBase & exc) {
        throw exc;
    }
}
models::User * database::DatabaseManager::getUserById(const QString & userId)
{
    QString msgErr;
    {
        QSqlQuery selectQuery(db_);
        selectQuery.prepare(sqllite_requests::SELECT_USER_BY_ID);
        selectQuery.addBindValue(userId);

        if (!selectQuery.exec()) {
            msgErr = "DatabaseManager::getUserById(). SELECT_USER_BY_ID failed: "
                     + selectQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 6);
        }

        if (selectQuery.next()) {
            models::User* pUser = new models::User;
            pUser->id           = selectQuery.value("id").toString();
            pUser->username     = selectQuery.value("username").toString();
            pUser->password     = selectQuery.value("password").toString();
            pUser->registration = selectQuery.value("registration").toDateTime();
            pUser->lastLogIn    = selectQuery.value("last_log_in").toDateTime();
            return pUser;
        }
    }
    return nullptr;
}
models::User * database::DatabaseManager::getUserByUsername(const QString & username)
{
    QString msgErr;
    {
        QSqlQuery selectQuery(db_);
        selectQuery.prepare(sqllite_requests::SELECT_USER_BY_USERNAME);
        selectQuery.addBindValue(username);

        if (!selectQuery.exec()) {
            msgErr = "DatabaseManager::getUserByUsername(). SELECT_USER_BY_USERNAME failed: "
                     + selectQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 5);
        }

        if (selectQuery.next()) {
            models::User* pUser = new models::User;
            pUser->id           = selectQuery.value("id").toString();
            pUser->username     = selectQuery.value("username").toString();
            pUser->password     = selectQuery.value("password").toString();
            pUser->registration = selectQuery.value("registration").toDateTime();
            pUser->lastLogIn    = selectQuery.value("last_log_in").toDateTime();
            return pUser;
        }
    }
    return nullptr;
}
models::Chat* database::DatabaseManager::addChat(const QString& idChat,
                                       const QString& idUser1,
                                       const QString& idUser2)
{
    // Сортируем ID: CHECK (id_user_1 < id_user_2)
    QString u1 = idUser1;
    QString u2 = idUser2;
    if (u1 > u2) {
        std::swap(u1, u2);
    }

    QString msgErr;
    {
        QSqlQuery insertQuery(db_);
        insertQuery.prepare(sqllite_requests::INSERT_CHAT);
        insertQuery.addBindValue(idChat);
        insertQuery.addBindValue(u1);   // ← отсортированный
        insertQuery.addBindValue(u2);   // ← отсортированный

        if (!insertQuery.exec()) {
            msgErr = "DatabaseManager::addChat(). INSERT_CHAT failed: "
                     + insertQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 7);
        }
    }

    return this->getChatById(idChat);
}

models::Chat * database::DatabaseManager::getChatById (const QString & idChat)
{
    QString msgErr;
    {
        QSqlQuery selectQuery(db_);
        selectQuery.prepare(sqllite_requests::SELECT_CHAT_BY_ID);
        selectQuery.addBindValue(idChat);

        if (!selectQuery.exec()) {
            msgErr = "DatabaseManager::getChatById(). SELECT_CHAT_BY_ID failed: "
                     + selectQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 8);
        }

        if (selectQuery.next()) {
            models::Chat* pChat = new models::Chat;
            pChat->id = selectQuery.value("id").toString();
            pChat->idUser1 = selectQuery.value("id_user_1").toString();
            pChat->idUser2 = selectQuery.value("id_user_2").toString();
            pChat->countMsg = selectQuery.value("count_msg").toString().toLongLong();
            return pChat;
        }
    }
    return nullptr;
}
QList<models::Chat *> database::DatabaseManager::getAllChatsByUserId (const QString & userId)
{
    QString msgErr;
    QList<models::Chat *> chats = QList<models::Chat *>();
    {
        QSqlQuery selectQuery(db_);
        selectQuery.prepare(sqllite_requests::SELECT_ALL_CHATS_BY_USER_ID);
        selectQuery.addBindValue(userId);
        if (!selectQuery.exec()) {
            msgErr = "DatabaseManager::getAllChatsByUserId(). SELECT_ALL_CHATS_BY_USER_ID failed: "
                     + selectQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 9);
        }
        while (selectQuery.next()) {
            models::Chat* pChat = new models::Chat;
            pChat->id = selectQuery.value("id").toString();
            pChat->idUser1 = selectQuery.value("id_user_1").toString();
            pChat->idUser2 = selectQuery.value("id_user_2").toString();
            pChat->countMsg = selectQuery.value("count_msg").toString().toLongLong();
            chats.push_back(pChat);
        }
    }
    return chats;
}
models::Chat * database::DatabaseManager::setCountMsgInChatByChatId (const QString & idChat,
                                                                     const quint64 newCount)
{
    QString msgErr;
    {
        QSqlQuery updateQuery(db_);
        updateQuery.prepare(sqllite_requests::UPDATE_CHAT_COUNT_MSG);
        updateQuery.addBindValue(newCount);
        updateQuery.addBindValue(idChat);

        if (!updateQuery.exec()) {
            msgErr = "DatabaseManager::setCountMsgInChatByChatId(). UPDATE_CHAT_COUNT_MSG failed: "
                     + updateQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 11);
        }
        if (updateQuery.numRowsAffected() == 0) {
            throw custom_exc::database::ExceptionDateBase(
                "DatabaseManager::setCountMsgInChatByChatId(). No rows inserted", 11);
        }
    }
    try {
        return this->getChatById(idChat);
    }
    catch (custom_exc::database::ExceptionDateBase & exc) {
        throw exc;
    }
}
models::Message * database::DatabaseManager::addMsg (const QString & idMsg,
                                                     const QString & idChat,
                                                     const QString & idOwner,
                                                     const QString & idReceiver,
                                                     const QString & textMsg,
                                                     const quint64 numMsg,
                                                     const bool flagFile,
                                                     const quint64 sizeFile)
{
    QString msgErr;
    {
        QSqlQuery insertQuery(db_);
        insertQuery.prepare(sqllite_requests::INSERT_MSG);
        insertQuery.addBindValue(idMsg);
        insertQuery.addBindValue(idChat);
        insertQuery.addBindValue(idOwner);
        insertQuery.addBindValue(idReceiver);
        insertQuery.addBindValue(textMsg);
        insertQuery.addBindValue(numMsg);
        insertQuery.addBindValue(flagFile ? 1 : 0);
        insertQuery.addBindValue(sizeFile);
        if (!insertQuery.exec()) {
            msgErr = "DatabaseManager::addMsg(). INSERT_MSG failed: "
                     + insertQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 8);
        }
        if (insertQuery.numRowsAffected() == 0) {
            throw custom_exc::database::ExceptionDateBase(
                "DatabaseManager::addMsg(). No rows inserted", 8);
        }
    }
    try {
        return this->getMsgById(idMsg);
    }
    catch (custom_exc::database::ExceptionDateBase & exc) {
        throw exc;
    }
}
models::Message * database::DatabaseManager::getMsgById (const QString idMsg)
{
    QString msgErr;
    {
        QSqlQuery selectQuery(db_);
        selectQuery.prepare(sqllite_requests::SELECT_MSG_BY_ID);
        selectQuery.addBindValue(idMsg);

        if (!selectQuery.exec()) {
            msgErr = "DatabaseManager::getMsgById(). SELECT_MSG_BY_ID failed: "
                     + selectQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 9);
        }

        if (selectQuery.next()) {
            models::Message * pMsg = new models::Message;
            pMsg->id           = selectQuery.value("id").toString();
            pMsg->idParentChat     = selectQuery.value("id_parent_chat").toString();
            pMsg->createdAt     = selectQuery.value("created_at").toDateTime();
            pMsg->owner = selectQuery.value("owner").toString();
            pMsg->receiver    = selectQuery.value("receiver").toString();
            pMsg->text_msg = selectQuery.value("text_msg").toString();
            pMsg->numberMsg = selectQuery.value("number_msg").toString().toLongLong();
            pMsg->isReaded = selectQuery.value("is_readed").toInt();
            pMsg->isFile = selectQuery.value("is_file").toInt();
            pMsg->sizeFile = selectQuery.value("size_file").toLongLong();
            return pMsg;
        }
    }
    return nullptr;
}
QList<models::Message *> database::DatabaseManager::getAllMsgsByChatId (const QString & chatId)
{
    QString msgErr;
    QList<models::Message *> msgs = QList<models::Message *>();
    {
        QSqlQuery selectQuery(db_);
        selectQuery.prepare(sqllite_requests::SELECT_ALL_MESSAGES_BY_CHAT_ID);
        selectQuery.addBindValue(chatId);
        if (!selectQuery.exec()) {
            msgErr = "DatabaseManager::getAllMsgsByChatId(). SELECT_ALL_MESSAGES_BY_CHAT_ID failed: "
                     + selectQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 12);
        }
        while (selectQuery.next()) {
            models::Message * pMsg = new models::Message;
            pMsg->id           = selectQuery.value("id").toString();
            pMsg->idParentChat     = selectQuery.value("id_parent_chat").toString();
            pMsg->createdAt     = selectQuery.value("created_at").toDateTime();
            pMsg->owner = selectQuery.value("owner").toString();
            pMsg->receiver    = selectQuery.value("receiver").toString();
            pMsg->text_msg = selectQuery.value("text_msg").toString();
            pMsg->numberMsg = selectQuery.value("number_msg").toString().toLongLong();
            pMsg->isReaded = selectQuery.value("is_readed").toString().toInt();
            pMsg->isFile = selectQuery.value("is_file").toString().toInt();
            pMsg->sizeFile = selectQuery.value("size_file").toLongLong();
            msgs.push_back(pMsg);
        }
    }
    return msgs;
}
QList<models::Message *> database::DatabaseManager::getMsgsByChatIdIndexFrom (const QString & chatId,
                                                                              const quint64 beginIndex)
{
    QString msgErr;
    QList<models::Message *> msgs = QList<models::Message *>();
    {
        QSqlQuery selectQuery(db_);
        selectQuery.prepare(sqllite_requests::SELECT_MSGS_BY_ID_CHAT_AND_INDEX_FROM);
        selectQuery.addBindValue(chatId);
        selectQuery.addBindValue(beginIndex);
        if (!selectQuery.exec()) {
            msgErr = "DatabaseManager::getMsgsByChatIdIndexFrom(). SELECT_MSGS_BY_ID_CHAT_AND_INDEX_FROM failed: "
                     + selectQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 15);
        }
        while (selectQuery.next()) {
            models::Message * pMsg = new models::Message;
            pMsg->id           = selectQuery.value("id").toString();
            pMsg->idParentChat     = selectQuery.value("id_parent_chat").toString();
            pMsg->createdAt     = selectQuery.value("created_at").toDateTime();
            pMsg->owner = selectQuery.value("owner").toString();
            pMsg->receiver    = selectQuery.value("receiver").toString();
            pMsg->text_msg = selectQuery.value("text_msg").toString();
            pMsg->numberMsg = selectQuery.value("number_msg").toString().toLongLong();
            pMsg->isReaded = selectQuery.value("is_readed").toString().toInt();
            pMsg->isFile = selectQuery.value("is_file").toString().toInt();
            pMsg->sizeFile = selectQuery.value("size_file").toLongLong();
            msgs.push_back(pMsg);
        }
    }
    return msgs;
}
models::Message * database::DatabaseManager::setMsgIsReaded (const QString &idMsg)
{
    QString msgErr;
    {
        QSqlQuery updateQuery(db_);
        updateQuery.prepare(sqllite_requests::UPDATE_MSG_IS_READED);
        updateQuery.addBindValue(idMsg);

        if (!updateQuery.exec()) {
            msgErr = "DatabaseManager::setMsgIsReaded(). UPDATE_MSG_IS_READED failed: "
                     + updateQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 10);
        }
        if (updateQuery.numRowsAffected() == 0) {
            throw custom_exc::database::ExceptionDateBase(
                "DatabaseManager::setMsgIsReaded(). No rows inserted", 10);
        }
    }
    try {
        return this->getMsgById(idMsg);
    }
    catch (custom_exc::database::ExceptionDateBase & exc) {
        throw exc;
    }
}

models::File * database::DatabaseManager::addMsgFile (const QString & idFile,
                                                      const QString & localPath,
                                                      const QString & idMsg,
                                                      const QString & idChat,
                                                      const QString & idOwner,
                                                      const QString & idReceiver,
                                                      const QString & textMsg,
                                                      const quint64 numMsg,
                                                      const quint64 sizeFile)
{
    QString msgErr;

    {
        try {
            this->addMsg(idMsg,
                         idChat,
                         idOwner,
                         idReceiver,
                         textMsg,
                         numMsg,
                         true,
                         sizeFile);
        }
        catch (custom_exc::database::ExceptionDateBase & exc) {
            throw exc;
        }
        QSqlQuery insertQuery(db_);
        insertQuery.prepare(sqllite_requests::INSERT_FILE);
        insertQuery.addBindValue(idFile);
        insertQuery.addBindValue(idMsg);
        insertQuery.addBindValue(localPath);
        if (!insertQuery.exec()) {
            msgErr = "DatabaseManager::addMsgFile(). INSERT_MSG failed: "
                     + insertQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 13);
        }
        if (insertQuery.numRowsAffected() == 0) {
            throw custom_exc::database::ExceptionDateBase(
                "atabaseManager::addMsgFile(). No rows inserted", 13);
        }
    }
    try {
        return this->getFileByMsgId(idMsg);
    }
    catch (custom_exc::database::ExceptionDateBase & exc) {
        throw exc;
    }
}

models::File * database::DatabaseManager::getFileByMsgId (const QString &idMsg)
{

    QString msgErr;
    {
        QSqlQuery selectQuery(db_);
        selectQuery.prepare(sqllite_requests::SELECT_FILE_BY_MSG_ID);
        selectQuery.addBindValue(idMsg);
        if (!selectQuery.exec()) {
            msgErr = "DatabaseManager::getFileByMsgId(). SELECT_FILE_BY_MSG_ID failed: "
                     + selectQuery.lastError().text();
            throw custom_exc::database::ExceptionDateBase(msgErr, 14);
        }
        if (selectQuery.next()) {

            models::File* pFile = new models::File;
            pFile->id = selectQuery.value("id").toString();
            pFile->idParentMsg = selectQuery.value("id_parent_msg").toString();
            pFile->localPathFile = selectQuery.value("local_path_file").toString();
            return pFile;
        }
    }
    return nullptr;
}
