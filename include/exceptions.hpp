#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <QString>

/**
 * @file exceptions.hpp
 * @brief Пользовательские исключения для сетевого модуля.
 *
 * @details Содержит иерархию классов исключений, используемых
 * в сетевом модуле для обработки ошибок при создании запросов
 * и ответов.
 *
 * @author sad-cat-cmd
 * @version 1.0
 * @date 2026-09-15
 */

/**
 * @namespace custom_exc
 * @brief Базовое пространство имён для пользовательских исключений.
 *
 * @details Содержит базовый класс исключения и вложенное пространство
 * имён для сетевых исключений
 */
namespace custom_exc {
    /**
     * @class BaseException
     * @brief Базовый класс для всех пользовательских исключений.
     *
     * @details Хранит текстовое описание ошибки и числовой код статуса.
     * Является родительским классом для всех исключений проекта.
     */
    class BaseException {
    private:
        QString msg;          ///< Текстовое описание ошибки
        qint64 code;    ///< Код ошибки (для логирования/отладки)
    public:
        /**
         * @brief Конструктор базового исключения.
         * @param messange Текстовое описание ошибки
         * @param code Код ошибки
         */
        BaseException(const QString& msg, const quint64 code);

        /**
         * @brief Возвращает текстовое описание ошибки.
         * @return QString с описанием ошибки
         */
        QString getMsg() noexcept;

        /**
         * @brief Возвращает код ошибки.
         * @return qint64 код ошибки
         */
        qint64 getStatusCode() noexcept;
    };
    /**
     * @namespace custom_exc::database
     * @brief Исключения для работы с базой данных
     *
     * @details Содержит исключения, возникающие при обработке операций в базе данных
     */
    namespace database {
        /**
         * @class ExceptionDateBase
         * @brief Исключение, возникающее при работе с БД.
         * @see ExceptionCreateRequest
         */
        class ExceptionDateBase : public BaseException {
        private:
        public:
            /**
             * @brief Конструктор исключения.
             * @param msg Текстовое описание ошибки (не должно быть пустым)
             * @param code Код ошибки
             */
            ExceptionDateBase(const QString & msg,
                              const quint64 code);
        };

        /**
         * @class ExceptionFile
         * @brief Исключение, возникающее при работе с файлами.
         * @see ExceptionCreateRequest
         */
        class ExceptionFile : public BaseException {
            /**
             * @brief Конструктор исключения.
             * @param msg Текстовое описание ошибки (не должно быть пустым)
             * @param code Код ошибки
             */
            ExceptionFile(const QString & msg,
                              const quint64 code);
        };
    } // namespace database

    /**
     * @namespace custom_exc::network
     * @brief Исключения для сетевого модуля.
     *
     * @details Содержит исключения, возникающие при обработке сетевых
     * запросов и формировании ответов.
     */
    namespace network {
        /**
         * @class ExceptionCreateResponse
         * @brief Исключение, возникающее при ошибке создания ответа.
         *
         * @details Бросается, когда ответ не может быть сформирован
         * корректно: пустое сообщение, некорректный статус-код и т.п.
         *
         * @see ExceptionCreateRequest
         */
        class ExceptionCreateResponse : public custom_exc::BaseException {
        private:
        public:
            /**
             * @brief Конструктор исключения.
             * @param msg Текстовое описание ошибки (не должно быть пустым)
             * @param code Код ошибки
             */
            ExceptionCreateResponse(const QString& msg, const qint64 code);
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
        class ExceptionCreateRequest : public custom_exc::BaseException {
        private:
        public:
            /**
             * @brief Конструктор исключения.
             * @param msg Текстовое описание ошибки (не должно быть пустым)
             * @param code Код ошибки
             */
            ExceptionCreateRequest(const QString& msg, const qint64 code);
        };

        /**
         * @class ExceptionServer
         * @brief Исключение, возникающее при ошибках работы с сервером.
         *
         * @see ExceptionCreateRequest
         */
        class ExceptionServer : public custom_exc::BaseException{
        private:
        public:
            /**
             * @brief Конструктор исключения.
             * @param msg Текстовое описание ошибки (не должно быть пустым)
             * @param code Код ошибки
             */
            ExceptionServer(const QString& msg, const qint64 code);
        };
    } // namespace network

} // namespace custom_exc

#endif // EXCEPTIONS_H
