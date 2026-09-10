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
}
