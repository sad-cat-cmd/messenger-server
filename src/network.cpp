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
        throw custom_exc_network::ExceptionCreateResponse(QString("Error: value errMsg in constructor ServerResponseError is empty"),
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
        throw custom_exc_network::ExceptionCreateResponse(QString("Error: value copyist in constructor ServerResponseGetChatInformation is empty"),
                                                          1);
    }
    if (this->statusCopyist.isEmpty()) {
        throw custom_exc_network::ExceptionCreateResponse(QString("Error: value status in constructor ServerResponseGetChatInformation is empty"),
                                                          1);
    }
    if (this->statusCopyist != network::transmitted_objects::STATUS_ONLINE_COPYIST && this->statusCopyist != network::transmitted_objects::STATUS_OFFLINE_COPYIST) {
        throw custom_exc_network::ExceptionCreateResponse(QString("Error: value status in constructor ServerResponseGetChatInformation is unvalid. Valid statuses: STATUS_ONLINE_COPYIST, STATUS_OFFLINE_COPYIST"),
                                                          1);
    }
    if (countMsg < 0) {
        throw custom_exc_network::ExceptionCreateResponse(QString("Error: value status in constructor ServerResponseGetChatInformation is less 0."),
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
        throw custom_exc_network::ExceptionCreateResponse(QString("Error: value message in constructor ServerResponseGetMsgChat is empty"),
                                                          1);
    }
    if (this->msg.size() >network::transmitted_objects::MAX_SIZE_TEXT_STRING && this->flagFile == false) {
        throw custom_exc_network::ExceptionCreateResponse(QString("Error: value message in constructor ServerResponseGetMsgChat is more MAX_SIZE_TEXT_STRING"),
                                                          1);
    }
    if (this->msg.size() >network::transmitted_objects::MAX_SIZE_TEXT_FILE_NAME && this->flagFile == true) {
        throw custom_exc_network::ExceptionCreateResponse(QString("Error: value message in constructor ServerResponseGetMsgChat is more MAX_SIZE_TEXT_FILE_NAME"),
                                                          1);
    }
    if (this->statusRead.isEmpty()) {
        throw custom_exc_network::ExceptionCreateResponse(QString("Error: value statusRead in constructor ServerResponseGetMsgChat is empty"),
                                                          1);
    }
    if (this->statusRead != network::transmitted_objects::STATUS_READED_MSG && this->statusRead != network::transmitted_objects::STATUS_UNREADED_MSG) {
        throw custom_exc_network::ExceptionCreateResponse(QString("Error: value status in constructor ServerResponseGetChatInformation is unvalid. Valid statuses: read, notread"),
                                                          1);
    }
    if (this->owner.isEmpty()) {
        throw custom_exc_network::ExceptionCreateResponse(QString("Error: value messageOwner in constructor ServerResponseGetMsgChat is empty"),
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
        throw custom_exc_network::ExceptionCreateResponse(QString("Error: value countChunk in constructor ServerResponseGetFileInfo is less 0, but flagGood = true"),
                                                  1);
    }
    if (flagGood == false && countChunk != -1) {
        throw custom_exc_network::ExceptionCreateResponse(QString("Error: value countChunk in constructor ServerResponseGetFileInfo does not equels -1, but flagGood = false"),
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









