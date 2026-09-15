#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <QString>

/**
 * @file exceptions.hpp
 * @brief Пользовательские исключения для сетевого модуля.
 *
 * @details Содержит определения классов исключений, используемых
 * в сетевом модуле для обработки ошибок при создании запросов
 * и ответов.
 *
 * @author Ваше Имя
 * @version 1.0
 * @date 2026-09-15
 */

/**
 * @namespace custom_exc_network
 * @brief Пользовательские исключения для сетевого модуля.
 *
 * @details Содержит исключения, возникающие при обработке сетевых
 * запросов и формировании ответов.
 */
namespace custom_exc_network {

/**
     * @class ExceptionCreateResponse
     * @brief Исключение, возникающее при ошибке создания ответа.
     *
     * @details Бросается, когда ответ не может быть сформирован
     * корректно: пустое сообщение, некорректный статус-код и т.п.
     */
class ExceptionCreateResponse {
private:
    QString msg;        ///< Текстовое описание ошибки
    int statusCode;     ///< Код ошибки (для логирования/отладки)
public:
    /**
         * @brief Конструктор исключения.
         * @param msg Текстовое описание ошибки (не должно быть пустым)
         * @param statusCode Код ошибки
         */
    ExceptionCreateResponse(const QString & msg,
                            const int statusCode);

    /**
         * @brief Возвращает текстовое описание ошибки.
         * @return QString с описанием ошибки
         */
    QString getMsg() noexcept;

    /**
         * @brief Возвращает код ошибки.
         * @return int код ошибки
         */
    int getStatusCode() noexcept;
};

/**
     * @class ExceptionCreateRequest
     * @brief Исключение, возникающее при ошибке создания запроса.
     *
     * @details Бросается, когда запрос не может быть сформирован
     * корректно: пустое сообщение, некорректный статус-код и т.п.
     *
     * @see ExceptionCreateResponse
     */
class ExceptionCreateRequest {
private:
    QString msg;        ///< Текстовое описание ошибки
    int statusCode;     ///< Код ошибки (для логирования/отладки)
public:
    /**
         * @brief Конструктор исключения.
         * @param msg Текстовое описание ошибки (не должно быть пустым)
         * @param statusCode Код ошибки
         */
    ExceptionCreateRequest(const QString & msg,
                           const int statusCode);

    /**
         * @brief Возвращает текстовое описание ошибки.
         * @return QString с описанием ошибки
         */
    QString getMsg() noexcept;

    /**
         * @brief Возвращает код ошибки.
         * @return int код ошибки
         */
    int getStatusCode() noexcept;
};

} // namespace custom_exc_network

#endif // EXCEPTIONS_H
