#include <exceptions.hpp>

custom_exc::BaseException::BaseException(const QString &messange,
                                         const quint64 code)
                                         : msg(messange),
                                           statusCode(code)
{}
QString custom_exc::BaseException::getMsg() noexcept
{
    return this->msg;
}
qint64 custom_exc::BaseException::getStatusCode() noexcept
{
    return this->statusCode;
}

custom_exc::network::ExceptionCreateResponse::ExceptionCreateResponse(const QString & msg,
                                                                      const qint64 statusCode)
                                                                      : custom_exc::BaseException(msg, statusCode)
{}

custom_exc::network::ExceptionCreateRequest::ExceptionCreateRequest(const QString & msg,
                                                                    const qint64 statusCode)
                                                                    : custom_exc::BaseException(msg, statusCode)
{}
