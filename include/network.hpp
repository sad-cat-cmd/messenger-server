// qt specific lib
#include <qthread.h>
#include <QMutex>
#include <QList>
#include <QDataStream>
#include <QUuid>

// Linux's system network lib
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

// c_plus lib
#include <atomic>

// my header
#include <exceptions.hpp>

namespace network {

    namespace transmitted_objects {
        const QString STATUS_READED_MSG = "read";
        const QString STATUS_UNREADED_MSG = "nonread";
        const QString STATUS_ONLINE_COPYIST = "online";
        const QString STATUS_OFFLINE_COPYIST = "offline";
        const qsizetype MAX_SIZE_TEXT_STRING = 1000;
        const qsizetype MAX_SIZE_TEXT_FILE_NAME = 255;
        constexpr size_t MAX_SIZE_BYTE_FILE = 1024 * 1024 * 64;
        constexpr size_t SIZE_BYTE_FILE_CHUNK = 1024 * 64;
        const qint64 NUM_OPERATION_GET_ALL_CHAT_ID = 1;
        const qint64 NUM_OPERATION_GET_CHAT_INFO = 2;
        const qint64 NUM_OPERATION_GET_MSG_CHAT = 3;
        const qint64 NUM_OPERATION_GET_CHAT_FILE = 4;
        const qint64 NUM_OPERATION_GET_CHAT_FILE_CHUNK = 5;
        const qint64 NUM_OPERATION_END_TRANSACTION = 6;
        const qint64 NUM_OPERATION_LOG_OUT = 7;
        const qint64 NUM_OPERATION_LOG_IN = 8;

        QString generateRequestsCode() noexcept;

        class ServerRequestInitTransactionClient {
        private:
            QString requestsCode;
        public:
            ServerRequestInitTransactionClient() noexcept;
            QString getCode() noexcept;
            friend QDataStream& operator<<(QDataStream& out,
                                           const ServerRequestInitTransactionClient& request) noexcept;
            friend QDataStream& operator>>(QDataStream& in,
                                           ServerRequestInitTransactionClient& request) noexcept;
        };
        QDataStream &operator<< (QDataStream & out,
                                const ServerRequestInitTransactionClient &request) noexcept;
        QDataStream &operator>> (QDataStream & in,
                                 network::transmitted_objects::ServerRequestInitTransactionClient &request) noexcept;

        class ServerResponseError{
        private:
            QString errorMsg;
            int status;
        public:
            ServerResponseError(const QString & errMsg,
                                const int status);
            QString getErrMsg() noexcept;
            int getStatus() noexcept;
            friend QDataStream &operator<< (QDataStream & out,
                                            const network::transmitted_objects::ServerResponseError & request) noexcept;
            friend QDataStream &operator>> (QDataStream & in,
                                            network::transmitted_objects::ServerResponseError &request) noexcept;
        };
        QDataStream &operator<< (QDataStream & out,
                                 const network::transmitted_objects::ServerResponseError & request) noexcept;
        QDataStream &operator>> (QDataStream & in,
                                 network::transmitted_objects::ServerResponseError &request) noexcept;

        class ServerResponseGetChatInformation {
        private:
            QString nameCopyist;
            QString statusCopyist;
            quint64 countMsg;
        public:
            ServerResponseGetChatInformation(const QString & copyist,
                                             const QString & status,
                                             const size_t countMsg);
            QString getNameCopyist() noexcept;
            QString getStatusCopyist() noexcept;
            quint64 getCountMsg() noexcept;
            friend QDataStream &operator<< (QDataStream & out,
                                    const ServerResponseGetChatInformation & response) noexcept;
            friend QDataStream &operator>> (QDataStream & in,
                                    ServerResponseGetChatInformation &response) noexcept;
        };
        QDataStream &operator<< (QDataStream & out,
                                const network::transmitted_objects::ServerResponseGetChatInformation & response) noexcept;
        QDataStream &operator>> (QDataStream & in,
                                network::transmitted_objects::ServerResponseGetChatInformation &response) noexcept;

        class ServerResponseGetMsgChat {
        private:
            QString msg;
            QString statusRead;
            QString owner;
            bool flagFile;
        public:
            ServerResponseGetMsgChat(const QString & message,
                                     const QString & status,
                                     const QString & messageOwner,
                                     const bool fileFlag);
            QString getMsg() noexcept;
            QString getStatus() noexcept;
            QString getOwner() noexcept;
            bool getFlagFile() noexcept;
            friend QDataStream &operator<< (QDataStream & out,
                                            const ServerResponseGetMsgChat & response) noexcept;
            friend QDataStream &operator>> (QDataStream & in,
                                            ServerResponseGetMsgChat &response) noexcept;
        };
        QDataStream &operator<< (QDataStream & out,
                                 const network::transmitted_objects::ServerResponseGetMsgChat & response) noexcept;
        QDataStream &operator>> (QDataStream & in,
                                 network::transmitted_objects::ServerResponseGetMsgChat &response) noexcept;

        class ServerResponseGetFileInfo {
        private:
            bool flagGood;
            qsizetype countChunk;
        public:
            ServerResponseGetFileInfo(const bool flagGood,
                                      const qsizetype countChunk);
            bool getFlagGood() noexcept;
            qsizetype getCountChunk() noexcept;
            friend QDataStream &operator<< (QDataStream & out,
                                            const network::transmitted_objects::ServerResponseGetFileInfo & response) noexcept;
            friend QDataStream &operator>> (QDataStream & in,
                                            network::transmitted_objects::ServerResponseGetFileInfo &response) noexcept;
        };
        QDataStream &operator<< (QDataStream & out,
                                 const network::transmitted_objects::ServerResponseGetFileInfo & response) noexcept;
        QDataStream &operator>> (QDataStream & in,
                                 network::transmitted_objects::ServerResponseGetFileInfo &response) noexcept;

        // додумать и дописать класс для отправки чанков
        class ServerResponseGetFileChunk{};
        //
        // додумать и дописать класс для отправки списка идентефикаторов чатов клиента
        class ServerResponseGetAllChatId{};
        //
        // додумать и дописать класс для отправки информации о залогиненом пользователе
        class ServerResponseLogIN{};
        //
        class BaseClientRequest {
        protected:
            QString requestsCode;
            qint64 numOperation;
        public:
            BaseClientRequest(const QString & code,
                              qint64 numOperation);
            qint64 getNumOperation() noexcept;
            QString getRequestCode() noexcept;
        };

        class ClientRequestGetAllChatId : public BaseClientRequest{
        private:
        public:
            ClientRequestGetAllChatId(const QString& code);
            friend QDataStream &operator<< (QDataStream & out,
                                            const network::transmitted_objects::ClientRequestGetAllChatId & request) noexcept;
            friend QDataStream &operator>> (QDataStream & in,
                                            network::transmitted_objects::ClientRequestGetAllChatId &request) noexcept;
        };
        QDataStream &operator<< (QDataStream & out,
                                 const network::transmitted_objects::ClientRequestGetAllChatId & request) noexcept;
        QDataStream &operator>> (QDataStream & in,
                                 network::transmitted_objects::ClientRequestGetAllChatId &request) noexcept;

        class ClientRequestGetChatInformation : public BaseClientRequest{
        private:
            QString id;
        public:
            ClientRequestGetChatInformation(const QString & code,
                                            const QString & idChat);
            QString getIdChat() noexcept;
            friend QDataStream &operator<< (QDataStream & out,
                                    const network::transmitted_objects::ClientRequestGetChatInformation & request) noexcept;
            friend QDataStream &operator>> (QDataStream & in,
                                    network::transmitted_objects::ClientRequestGetChatInformation &request) noexcept;
        };
        QDataStream &operator<< (QDataStream & out,
                                 const network::transmitted_objects::ClientRequestGetChatInformation & request) noexcept;
        QDataStream &operator>> (QDataStream & in,
                                 network::transmitted_objects::ClientRequestGetChatInformation &request) noexcept;

        class ClientRequestGetMsgChat : public BaseClientRequest{
        private:
            qint64 numMsg;
        public:
            ClientRequestGetMsgChat(const QString & code,
                                    const int numberMsg);
            qint64 getNumMsg() noexcept;
            friend QDataStream &operator<< (QDataStream & out,
                                            const ClientRequestGetMsgChat & request) noexcept;
            friend QDataStream &operator>> (QDataStream & in,
                                            ClientRequestGetMsgChat &request) noexcept;
        };
        QDataStream &operator<< (QDataStream & out,
                                 const ClientRequestGetMsgChat & request) noexcept;
        QDataStream &operator>> (QDataStream & in,
                                 ClientRequestGetMsgChat &request) noexcept;


        class ClientRequestGetFile : public BaseClientRequest{
        private:
            QString fileName;
        public:
            ClientRequestGetFile(const QString code,
                                 const QString fileName);
            QString getFileName() noexcept;
            friend QDataStream &operator<< (QDataStream & out,
                                            const ClientRequestGetFile & request) noexcept;
            friend QDataStream &operator>> (QDataStream & in,
                                            ClientRequestGetFile &request) noexcept;
        };
        QDataStream &operator<< (QDataStream & out,
                                 const ClientRequestGetFile & request) noexcept;
        QDataStream &operator>> (QDataStream & in,
                                 ClientRequestGetFile &request) noexcept;

        class ClientRequestEndTransactionClient : public BaseClientRequest{
        private:
        public:
            ClientRequestEndTransactionClient(const QString & code);
            friend QDataStream &operator<< (QDataStream & out,
                                            const ClientRequestEndTransactionClient & request) noexcept;
            friend QDataStream &operator>> (QDataStream & in,
                                            ClientRequestEndTransactionClient &request) noexcept;
        };
        QDataStream &operator<< (QDataStream & out,
                                 const ClientRequestEndTransactionClient & request) noexcept;
        QDataStream &operator>> (QDataStream & in,
                                 ClientRequestEndTransactionClient &request) noexcept;

        class ClientRequestLogIn : public BaseClientRequest {
        private:
            QString username;
            QString password;
        public:
            ClientRequestLogIn(const QString & code,
                               const QString & username,
                               const QString & password);
            QString getUsername() noexcept;
            QString getPassword() noexcept;
            friend QDataStream &operator<< (QDataStream & out,
                                            const ClientRequestLogIn & request) noexcept;
            friend QDataStream &operator>> (QDataStream & in,
                                            ClientRequestLogIn &request) noexcept;
        };
        QDataStream &operator<< (QDataStream & out,
                                 const ClientRequestLogIn & request) noexcept;
        QDataStream &operator>> (QDataStream & in,
                                 ClientRequestLogIn &request) noexcept;

        class ClientRequestLogOut : public BaseClientRequest {
        private:
        public:
            ClientRequestLogOut(const QString & code);
            friend QDataStream &operator<< (QDataStream & out,
                                            const ClientRequestLogOut & request) noexcept;
            friend QDataStream &operator>> (QDataStream & in,
                                            ClientRequestLogOut &request) noexcept;
        };
        QDataStream &operator<< (QDataStream & out,
                                 const ClientRequestLogOut & request) noexcept;
        QDataStream &operator>> (QDataStream & in,
                                 ClientRequestLogOut &request) noexcept;
    }

    class Socket {
        private:
        std::atomic<int> numberSocket;

        public:
        QMutex socketMutex;
        Socket(int numSocket);
        ~Socket();
        int getSocket() noexcept;
        bool isZero() noexcept;
        bool isOne() noexcept;
        bool isMinusOne() noexcept;
        bool isLessZero() noexcept;
        bool isClosed() noexcept;
        void closeSocket() noexcept;
    };

    class IClient {
        protected:
        QString name;
        QString id;
        bool status;
    };
    class ServerThreadAccept : public QThread {
        private:
        network::Socket & lserverSocket;
        ServerThreadAccept(network::Socket & serverSocket);
    };
}
