#include <out.hpp>

void cmd_writer::CmdWriter::writeMsg(const QString & msg) noexcept {
    std::unique_lock <std::mutex> lock(writeMutex);
    std::cout << msg.toStdString() << std::endl;
}
