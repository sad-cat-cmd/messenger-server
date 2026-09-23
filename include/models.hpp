#ifndef MODELS_HPP
#define MODELS_HPP

#include <QString>
#include <QDateTime>

namespace models {

/**
 * @struct User
 * @brief Модель пользователя.
 */
struct User {
    QString id;             ///< Уникальный идентификатор
    QString username;       ///< Логин
    QString password;       ///< Хеш пароля
    QDateTime registration; ///< Дата регистрации
    QDateTime lastLogIn;    ///< Дата последнего входа
};

/**
 * @struct Chat
 * @brief Модель чата.
 */
struct Chat {
    QString id;         ///< Уникальный идентификатор
    QString idUser1;    ///< ID первого участника
    QString idUser2;    ///< ID второго участника
    qint64 countMsg;    ///< Количество сообщений
};

/**
 * @struct Message
 * @brief Модель сообщения.
 */
struct Message {
    QString id;             ///< Уникальный идентификатор
    QString idParentChat;   ///< ID чата
    QDateTime createdAt;    ///< Дата создания
    QString owner;          ///< ID отправителя
    QString receiver;       ///< ID получателя
    QString text_msg;       ///< текст сообщения
    qint64 numberMsg;       ///< Порядковый номер
    bool isReaded;          ///< Прочитано?
    bool isFile;            ///< Это файл?
};

/**
 * @struct File
 * @brief Модель файла.
 */
struct File {
    QString id;             ///< Уникальный идентификатор
    QString idParentMsg;    ///< ID сообщения
    QString localPathFile;  ///< Локальный путь
};

} // namespace models

#endif // MODELS_HPP
