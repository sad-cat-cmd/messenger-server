#ifndef OUT_H
#define OUT_H

#include <iostream>
#include <mutex>
#include <QString>
namespace cmd_writer {
    /**
     * @class CmdWriter
     * @brief Потокобезопасный класс для записи сообщений
     *
     * CmdWriter обеспечивает синхронизированный доступ к стандартному выводу
     * при записи сообщений из разных потоков. Использует мьютекс для
     * предотвращения перемешивания вывода.
     *
     * @note Все методы являются потокобезопасными
     * @note Методы помечены noexcept для гарантии отсутствия исключений
     *
     * @example
     * @code
     * CmdWriter writer;
     * writer.writeMsg("Server started");
     * // В другом потоке:
     * writer.writeMsg("Client connected");
     * @endcode
     */
    class CmdWriter {
    private:
        std::mutex writeMutex;  ///< Мьютекс для синхронизации доступа к выводу

    public:
        /**
         * @brief Конструктор класса CmdWriter
         *
         * Создает объект для записи сообщений. Никаких ресурсов не выделяет.
         *
         * @note noexcept гарантирует, что конструктор не выбрасывает исключений
         */
        CmdWriter() noexcept = default;

        /**
         * @brief Запись сообщения в стандартный вывод
         *
         * Выводит сообщение в std::cout с добавлением символа новой строки.
         * Операция является потокобезопасной - используется мьютекс для
         * блокировки вывода во время записи.
         *
         * @param msg Ссылка на строку с сообщением для вывода
         *
         * @note Метод помечен noexcept, гарантирует отсутствие исключений
         * @note В случае ошибки записи, сообщение может быть потеряно
         * @note Вывод происходит с добавлением std::endl для немедленной записи
         *
         * @warning Не рекомендуется использовать для высоконагруженных систем
         *          из-за возможной задержки при синхронизации
         *
         * @example
         * @code
         * CmdWriter writer;
         * writer.writeMsg(QString("Error: connection lost"));
         * // Вывод: Error: connection lost
         * @endcode
         */
        void writeMsg(const QString & msg) noexcept;
    };
}

#endif // OUT_H
