#include <exceptions.hpp>

custom_exc_network::ExceptionCreateResponse::ExceptionCreateResponse(const QString & msg,
                                                                     const int statusCode)
                                                                     : msg(msg),
                                                                     statusCode(statusCode)
{}
QString custom_exc_network::ExceptionCreateResponse::getMsg() noexcept {
    return this->msg;
}
int custom_exc_network::ExceptionCreateResponse::getStatusCode() noexcept {
    return this->statusCode;
}

custom_exc_network::ExceptionCreateRequest::ExceptionCreateRequest(const QString & msg,
                                                                   const int statusCode)
    : msg(msg),
    statusCode(statusCode)
{}
QString custom_exc_network::ExceptionCreateRequest::getMsg() noexcept {
    return this->msg;
}
int custom_exc_network::ExceptionCreateRequest::getStatusCode() noexcept {
    return this->statusCode;
}
