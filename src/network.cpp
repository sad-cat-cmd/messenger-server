#include "network.hpp"


QString network::transmitted_objects::generateRequestsCode() noexcept{
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}

network::transmitted_objects::ServerRequestInitTransactionClient::ServerRequestInitTransactionClient() noexcept
{
    requestsCode = network::transmitted_objects::generateRequestsCode();
}
QString network::transmitted_objects::ServerRequestInitTransactionClient::getCode() noexcept
{
    return this->requestsCode;
}
QDataStream & network::transmitted_objects::operator<<(QDataStream & out,
                                                       const network::transmitted_objects::ServerRequestInitTransactionClient & request) noexcept
{
    out << request.requestsCode;
    return out;
}
QDataStream & network::transmitted_objects::operator>> (QDataStream & in,
                                                        network::transmitted_objects::ServerRequestInitTransactionClient &request) noexcept
{
    in >> request.requestsCode;
    return in;
}

network::transmitted_objects::ServerResponseError::ServerResponseError(const QString & errMsg,
                                                                       const int status)
                                                                       :errorMsg(errMsg),
                                                                       status(status)
{
    if (this->errorMsg.isEmpty()) {
        throw custom_exc::network::ExceptionCreateResponse(QString("Error: value errMsg in constructor ServerResponseError is empty"),
                                                           1);
    }
}

QString network::transmitted_objects::ServerResponseError::getErrMsg() noexcept {
    return this->errorMsg;
}
int network::transmitted_objects::ServerResponseError::getStatus() noexcept {
    return this->status;
}
QDataStream & network::transmitted_objects::operator<< (QDataStream & out,
                                                        const network::transmitted_objects::ServerResponseError & request) noexcept
{
    out << request.errorMsg << request.status;
    return out;
}
QDataStream & network::transmitted_objects::operator>> (QDataStream & in,
                                                        network::transmitted_objects::ServerResponseError &request) noexcept
{
    in >> request.errorMsg >> request.status;
    return in;
}
network::transmitted_objects::ServerResponseGetChatInformation::ServerResponseGetChatInformation(const QString & copyist,
                                                                                                 const QString & status,
                                                                                                 const size_t countMsg)
                                                                                                : nameCopyist(copyist),
                                                                                                  statusCopyist(status),
                                                                                                  countMsg(countMsg)
{
    if (this->nameCopyist.isEmpty()) {
        throw custom_exc::network::ExceptionCreateResponse(QString("Error: value copyist in constructor ServerResponseGetChatInformation is empty"),
                                                          1);
    }
    if (this->statusCopyist.isEmpty()) {
        throw custom_exc::network::ExceptionCreateResponse(QString("Error: value status in constructor ServerResponseGetChatInformation is empty"),
                                                          1);
    }
    if (this->statusCopyist != network::transmitted_objects::STATUS_ONLINE_COPYIST && this->statusCopyist != network::transmitted_objects::STATUS_OFFLINE_COPYIST) {
        throw custom_exc::network::ExceptionCreateResponse(QString("Error: value status in constructor ServerResponseGetChatInformation is unvalid. Valid statuses: STATUS_ONLINE_COPYIST, STATUS_OFFLINE_COPYIST"),
                                                          1);
    }
    if (countMsg < 0) {
        throw custom_exc::network::ExceptionCreateResponse(QString("Error: value status in constructor ServerResponseGetChatInformation is less 0."),
                                                          1);
    }
}
QString network::transmitted_objects::ServerResponseGetChatInformation::getNameCopyist() noexcept
{
    return this->nameCopyist;
}
QString network::transmitted_objects::ServerResponseGetChatInformation::getStatusCopyist() noexcept
{
    return this->statusCopyist;
}
quint64 network::transmitted_objects::ServerResponseGetChatInformation::getCountMsg() noexcept
{
    return this->countMsg;
}
QDataStream & network::transmitted_objects::operator<< (QDataStream & out,
                                                        const network::transmitted_objects::ServerResponseGetChatInformation & response) noexcept
{
    out << response.nameCopyist << response.statusCopyist << response.countMsg;
    return out;
}
QDataStream & network::transmitted_objects::operator>> (QDataStream & in,
                                                        network::transmitted_objects::ServerResponseGetChatInformation &response) noexcept
{
    in >> response.nameCopyist >> response.statusCopyist >> response.countMsg;
    return in;
}

network::transmitted_objects::ServerResponseGetMsgChat::ServerResponseGetMsgChat(const QString & message,
                                                                                 const QString & status,
                                                                                 const QString & messageOwner,
                                                                                 const bool flagFile)
                                                                                 : msg(message),
                                                                                   statusRead(status),
                                                                                   owner(messageOwner),
                                                                                   flagFile(flagFile)
{
    if (this->msg.isEmpty()) {
        throw custom_exc::network::ExceptionCreateResponse(QString("Error: value message in constructor ServerResponseGetMsgChat is empty"),
                                                          1);
    }
    if (this->msg.size() >network::transmitted_objects::MAX_SIZE_TEXT_STRING && this->flagFile == false) {
        throw custom_exc::network::ExceptionCreateResponse(QString("Error: value message in constructor ServerResponseGetMsgChat is more MAX_SIZE_TEXT_STRING"),
                                                          1);
    }
    if (this->msg.size() >network::transmitted_objects::MAX_SIZE_TEXT_FILE_NAME && this->flagFile == true) {
        throw custom_exc::network::ExceptionCreateResponse(QString("Error: value message in constructor ServerResponseGetMsgChat is more MAX_SIZE_TEXT_FILE_NAME"),
                                                          1);
    }
    if (this->statusRead.isEmpty()) {
        throw custom_exc::network::ExceptionCreateResponse(QString("Error: value statusRead in constructor ServerResponseGetMsgChat is empty"),
                                                          1);
    }
    if (this->statusRead != network::transmitted_objects::STATUS_READED_MSG && this->statusRead != network::transmitted_objects::STATUS_UNREADED_MSG) {
        throw custom_exc::network::ExceptionCreateResponse(QString("Error: value status in constructor ServerResponseGetChatInformation is unvalid. Valid statuses: read, notread"),
                                                          1);
    }
    if (this->owner.isEmpty()) {
        throw custom_exc::network::ExceptionCreateResponse(QString("Error: value messageOwner in constructor ServerResponseGetMsgChat is empty"),
                                                          1);
    }
}
QDataStream & network::transmitted_objects::operator<< (QDataStream & out,
                                                        const network::transmitted_objects::ServerResponseGetMsgChat & response) noexcept
{
    out << response.msg << response.statusRead << response.owner << response.flagFile;
    return out;
}
QDataStream & network::transmitted_objects::operator>> (QDataStream & in,
                                                        network::transmitted_objects::ServerResponseGetMsgChat &response) noexcept{
    in >> response.msg >> response.statusRead >> response.owner >> response.flagFile;
    return in;
}

network::transmitted_objects::ServerResponseGetFileInfo::ServerResponseGetFileInfo(const bool flagGood,
                                                                                   const qsizetype countChunk)
    : flagGood(flagGood),
      countChunk(countChunk)
{
    if (flagGood == true && countChunk < 0) {
        throw custom_exc::network::ExceptionCreateResponse(QString("Error: value countChunk in constructor ServerResponseGetFileInfo is less 0, but flagGood = true"),
                                                  1);
    }
    if (flagGood == false && countChunk != -1) {
        throw custom_exc::network::ExceptionCreateResponse(QString("Error: value countChunk in constructor ServerResponseGetFileInfo does not equels -1, but flagGood = false"),
                                                  1);
    }
}

bool network::transmitted_objects::ServerResponseGetFileInfo::getFlagGood() noexcept
{
    return this->flagGood;
}
qsizetype network::transmitted_objects::ServerResponseGetFileInfo::getCountChunk() noexcept
{
    return this->countChunk;
}
QDataStream & network::transmitted_objects::operator<< (QDataStream & out,
                                                        const network::transmitted_objects::ServerResponseGetFileInfo & response) noexcept
{
    out << response.flagGood << response.countChunk;
    return out;
}
QDataStream & network::transmitted_objects::operator>> (QDataStream & in,
                                                        network::transmitted_objects::ServerResponseGetFileInfo &response) noexcept
{
    in >> response.flagGood >> response.countChunk;
    return in;
}

network::transmitted_objects::BaseClientRequest::BaseClientRequest(const QString &code,
                                                                  qint64 numOperation)
                                                                  : requestsCode(code),
                                                                    numOperation(numOperation)
{
    if (this->requestsCode.isEmpty()) {
        throw custom_exc::network::ExceptionCreateRequest(QString("Error: value code in constructor BaseRequest is empty"),
                                                          1);
    }
    if (this->numOperation <= 0) {
        throw custom_exc::network::ExceptionCreateRequest(QString("Error: value code in constructor BaseRequest is less or equeul 0"),
                                                          1);
    }
}
qint64 network::transmitted_objects::BaseClientRequest::getNumOperation() noexcept
{
    return this->numOperation;
}
QString network::transmitted_objects::BaseClientRequest::getRequestCode() noexcept
{
    return this->requestsCode;
}

network::transmitted_objects::ClientRequestGetAllChatId::ClientRequestGetAllChatId(const QString& code)
                                                                                   try : network::transmitted_objects::BaseClientRequest(code,network::transmitted_objects::NUM_OPERATION_GET_ALL_CHAT_ID)
{}
catch(custom_exc::network::ExceptionCreateResponse & exc)
{
    throw custom_exc::network::ExceptionCreateRequest(QString("Error: value code in constructor ClientRequestGetAllChatId is empty"),
                                                      1);
}
QDataStream & network::transmitted_objects::operator<< (QDataStream & out,
                                                        const network::transmitted_objects::ClientRequestGetAllChatId & request) noexcept
{
    out << request.requestsCode << request.numOperation;
    return out;
}
QDataStream & network::transmitted_objects::operator>> (QDataStream & in,
                                                        network::transmitted_objects::ClientRequestGetAllChatId &request) noexcept
{
    in >> request.requestsCode >> request.numOperation;
    return in;
}

network::transmitted_objects::ClientRequestGetChatInformation::ClientRequestGetChatInformation(const QString & code,
                                                                                               const QString & idChat)
                                                                                               try : network::transmitted_objects::BaseClientRequest(code, network::transmitted_objects::NUM_OPERATION_GET_CHAT_INFO),
                                                                                                     id(idChat)
{
    if (this->id.isEmpty()) {
        throw custom_exc::network::ExceptionCreateRequest(QString("Error: value idChat in constructor ClientRequestGetChatInformation is empty"),
                                                                  1);
    }
}
catch (custom_exc::network::ExceptionCreateRequest & exc) {
    throw custom_exc::network::ExceptionCreateRequest(QString("Error: value code in constructor ClientRequestGetChatInformation is empty"),
                                                             1);
}
QString network::transmitted_objects::ClientRequestGetChatInformation::getIdChat() noexcept
{
    return this->id;
}
QDataStream & network::transmitted_objects::operator<< (QDataStream & out,
                                                        const network::transmitted_objects::ClientRequestGetChatInformation & request) noexcept
{
    out << request.requestsCode << request.numOperation << request.id;
    return out;
}
QDataStream & network::transmitted_objects::operator>> (QDataStream & in,
                                                        network::transmitted_objects::ClientRequestGetChatInformation &request) noexcept
{
    in >> request.requestsCode >> request.numOperation >> request.id;
    return in;
}

network::transmitted_objects::ClientRequestGetMsgChat::ClientRequestGetMsgChat(const QString & code,
                                                                               const int numberMsg)
                                                                               try : network::transmitted_objects::BaseClientRequest(code, network::transmitted_objects::NUM_OPERATION_GET_MSG_CHAT),
                                                                                     numMsg(numberMsg)
{
    if (this->numMsg <= 0) {
        throw custom_exc::network::ExceptionCreateRequest(QString("Error: value numberMsg in constructor ClientRequestGetMsgChat is less or equil 0"),
                                                          1);
    }
}
catch (custom_exc::network::ExceptionCreateRequest & exc) {
    throw custom_exc::network::ExceptionCreateRequest(QString("Error: value code in constructor ClientRequestGetMsgChat is empty"),
                                                      1);
}
qint64 network::transmitted_objects::ClientRequestGetMsgChat::getNumMsg() noexcept
{
    return this->numMsg;
}
QDataStream & network::transmitted_objects::operator<< (QDataStream & out,
                                                        const ClientRequestGetMsgChat & request) noexcept
{
    out << request.requestsCode << request.numOperation << request.numMsg;
    return out;
}
QDataStream & network::transmitted_objects::operator>> (QDataStream & in,
                                                        ClientRequestGetMsgChat &request) noexcept
{
    in >> request.requestsCode >> request.numOperation >> request.numMsg;
    return in;
}

network::transmitted_objects::ClientRequestGetFile::ClientRequestGetFile(const QString code,
                                                                         const QString fileName)
                                                                         try : network::transmitted_objects::BaseClientRequest(code, network::transmitted_objects::NUM_OPERATION_GET_CHAT_FILE),
                                                                               fileName(fileName)
{
    if (this->fileName.isEmpty()) {
        throw custom_exc::network::ExceptionCreateRequest(QString("Error: value fileName in constructor ClientRequestGetFile is empty"),
                                                          1);
    }
}
catch (custom_exc::network::ExceptionCreateRequest & exc) {
    throw custom_exc::network::ExceptionCreateRequest(QString("Error: value code in constructor ClientRequestGetFile is empty"),
                                                      1);
}
QString network::transmitted_objects::ClientRequestGetFile::getFileName() noexcept
{
    return this->fileName;
}
QDataStream & network::transmitted_objects::operator<< (QDataStream & out,
                                                        const ClientRequestGetFile & request) noexcept
{
    out << request.requestsCode << request.numOperation << request.fileName;
    return out;
}
QDataStream &network::transmitted_objects::operator>> (QDataStream & in,
                                                       ClientRequestGetFile &request) noexcept
{
    in >> request.requestsCode >> request.numOperation >> request.fileName;
    return in;
}

network::transmitted_objects::ClientRequestEndTransactionClient::ClientRequestEndTransactionClient(const QString& code)
                                                                                                   try : network::transmitted_objects::BaseClientRequest(code,network::transmitted_objects::NUM_OPERATION_END_TRANSACTION)
{}
catch(custom_exc::network::ExceptionCreateResponse & exc)
{
    throw custom_exc::network::ExceptionCreateRequest(QString("Error: value code in constructor ClientRequestEndTransactionClient is empty"),
                                                      1);
}
QDataStream & network::transmitted_objects::operator<< (QDataStream & out,
                                                        const ClientRequestEndTransactionClient & request) noexcept
{
    out << request.requestsCode << request.numOperation;
    return out;
}
QDataStream & network::transmitted_objects::operator>> (QDataStream & in,
                                                        ClientRequestEndTransactionClient &request) noexcept
{
    in >> request.requestsCode >> request.numOperation;
    return in;
}

network::transmitted_objects::ClientRequestLogIn::ClientRequestLogIn(const QString & code,
                                                                     const QString & username,
                                                                     const QString & password)
                                                                     try : network::transmitted_objects::BaseClientRequest(code,network::transmitted_objects::NUM_OPERATION_LOG_IN),
                                                                           username(username),
                                                                           password(password)
{
    if (this->username.isEmpty()){
        throw custom_exc::network::ExceptionCreateRequest(QString("Error: value username in constructor ClientRequestLogIn is empty"),
                                                         1);
    }
    if (this->password.isEmpty()) {
        throw custom_exc::network::ExceptionCreateRequest(QString("Error: value password in constructor ClientRequestLogIn is empty"),
                                                         1);
    }
}
catch(custom_exc::network::ExceptionCreateResponse & exc)
{
    throw custom_exc::network::ExceptionCreateRequest(QString("Error: value code in constructor ClientRequestLogIn is empty"),
                                                     1);
}

QDataStream & network::transmitted_objects::operator<< (QDataStream & out,
                                                        const ClientRequestLogIn & request) noexcept
{
    out << request.requestsCode <<request.numOperation << request.username << request.password;
    return out;
}
QDataStream & network::transmitted_objects::operator>> (QDataStream & in,
                                                        ClientRequestLogIn &request) noexcept
{
    in >> request.requestsCode >> request.numOperation >> request.username >> request.password;
    return in;
}

network::transmitted_objects::ClientRequestLogOut::ClientRequestLogOut(const QString& code)
                                                                       try : network::transmitted_objects::BaseClientRequest(code,network::transmitted_objects::NUM_OPERATION_LOG_OUT)
{}
catch(custom_exc::network::ExceptionCreateResponse & exc)
{
    throw custom_exc::network::ExceptionCreateRequest(QString("Error: value code in constructor ClientRequestLogOut is empty"),
                                                     1);
}
QDataStream & network::transmitted_objects::operator<< (QDataStream & out,
                                                      const ClientRequestLogOut & request) noexcept
{
    out << request.requestsCode << request.numOperation;
    return out;
}
QDataStream & network::transmitted_objects::operator>> (QDataStream & in,
                                                      ClientRequestLogOut &request) noexcept
{
    in >> request.requestsCode >> request.numOperation;
    return in;
}

network::Socket::Socket(int numSocket) : numberSocket(numSocket)
{}
network::Socket::~Socket() {}
int network::Socket::getSocket() noexcept {
    return this->numberSocket.load(std::memory_order_acquire);
}
bool network::Socket::isZero() noexcept{
    if (this->numberSocket.load(std::memory_order_acquire) == 0) {
        return true;
    }
    return false;
}
bool network::Socket::isOne() noexcept{
    if (this->numberSocket.load(std::memory_order_acquire) == 1) {
        return true;
    }
    return false;
}
bool network::Socket::isLessZero() noexcept {
    if (this->numberSocket.load(std::memory_order_acquire) < 0) {
        return true;
    }
    return false;
}
bool network::Socket::isMinusOne() noexcept {
    if (this->numberSocket.load(std::memory_order_acquire) == -1) {
        return true;
    }
    return false;
}
bool network::Socket::isClosed() noexcept {
    return this->isMinusOne();
}
void network::Socket::closeSocket() noexcept {
    QMutexLocker locker(&this->socketMutex);
    if (this->numberSocket > 0) {
        close(this->numberSocket);
        this->numberSocket = -1;
    }
}









