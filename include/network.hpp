// QT lib
#include <qthread.h>
#include <QMutex>
#include <QList>

// Linux's system network lib
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

// c_plus lib
#include <atomic>

namespace network {

    namespace transmitted_objects {
        const size_t MAX_BYTE_TEXT_MSG = 2000;
        const size_t MAX_BYTE_FILE_NAME = 100;

        constexpr size_t MAX_SIZE_BYTE_FILE = 1024 * 1024 * 64;
        constexpr size_t SIZE_BYTE_FILE_CHUNK = 1024 * 64;

        class RequestSendText {
            private:
            std::string idSender;
            std::string idRecver;
            std::string text;
        };
        class RequestSendFile {
            private:
            std::string chunk;
            std::string fileName;
            std::string fileSender;
            std::string fileRecver;
        };
        class RequestRecvText {
            private:
            std::
        };
        class RequestRecvFile;
        class Text;
        class Chat;
        class Count;
        class FileChunk;
        class ClientInfo;
        class LogINInfo;
        class StopProcess;
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
