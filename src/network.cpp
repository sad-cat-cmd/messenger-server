#include "network.hpp"


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








