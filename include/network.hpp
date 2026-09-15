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
        class ClientRequestGetAllChatId {
        private:
            QString requestsCode;
            const qsizetype numOperation = 1;
        public:
            ClientRequestGetAllChatId();
            QString getRequstCode() noexcept;
            qsizetype getNumOperation() noexcept;
        };

        QDataStream &operator<< (QDataStream & out,
                                 const network::transmitted_objects::ClientRequestGetAllChatId & request);
        QDataStream &operator>> (QDataStream & in,
                                 network::transmitted_objects::ClientRequestGetAllChatId &request);

        class ClientRequestGetChatInformation {
        private:
            std::string requestsCode;
            qsizetype numOperation = 2;
            QString id;
        public:
            ClientRequestGetChatInformation(const std::string code);
            int getNumOperation = 3;
        };
        QDataStream &operator<< (QDataStream & out,
                                 const network::transmitted_objects::ClientRequestGetChatInformation & request);
        QDataStream &operator>> (QDataStream & in,
                                 network::transmitted_objects::ClientRequestGetChatInformation &request);

        class ClientRequestGetMsgChat {
        private:
            std::string requestsCode;
            int numOperation = 3;
            int numMsg;
        public:
            ClientRequestGetMsgChat(const std::string requestsCode,
                                    const int numMsg);
            int getNumMsg();
            int getNumOperation();
        };

        QDataStream &operator<< (QDataStream & out,
                                 const ClientRequestGetMsgChat & request);
        QDataStream &operator>> (QDataStream & in,
                                 ClientRequestGetMsgChat &request);


        class ClientRequestGetFile {
        private:
            std::string requestsCode;
            std::string fileName;
            int numOperation = 4;
        public:
            ClientRequestGetFile(const std::string code,
                                 const std::string fileName);
            std::string getRequestsCode();
            std::string getFileName();
            int getNumOperation();
        };

        QDataStream &operator<< (QDataStream & out,
                                 const ClientRequestGetFile & request);
        QDataStream &operator>> (QDataStream & in,
                                 ClientRequestGetFile &request);

        class ClientRequestEndTransactionClient {
        private:
            std::string requestsCode;
            int numOperation = 5;
        public:
            ClientRequestEndTransactionClient(const std::string & code);
            std::string getRequestsCode();
            int getNumOperation();
        };
        QDataStream &operator<< (QDataStream & out,
                                 const ClientRequestEndTransactionClient & request);
        QDataStream &operator>> (QDataStream & in,
                                 ClientRequestEndTransactionClient &request);
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
