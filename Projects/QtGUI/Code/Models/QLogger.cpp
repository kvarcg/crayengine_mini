/*
File:  QLogger.cpp

Description: QLogger Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#ifdef _WIN32
#include <windows.h>
#endif
#include "Global.h"

// includes ////////////////////////////////////////
#include "QLogger.h"
#include <ctime>
#include <iostream>
#include <iomanip>
#include <thread>
#include <string>
#include <sstream>
#include <mutex>
#include <fstream>
#include <sstream>
#include <memory>
#include <QDir>

// defines /////////////////////////////////////////

namespace
{
    QLogger* m_logger;
}

class LoggerImpl final : public BaseLoggerImpl {
private:
    std::ofstream                       m_ofstream;
    std::string                         m_time_str;
    std::string                         m_name;
    std::mutex                          m_message_mutex;
    friend class Logger;
public:

    LoggerImpl() = default;
    ~LoggerImpl() override;
    LoggerImpl(const LoggerImpl&) = default;
    LoggerImpl& operator=(const LoggerImpl&) = default;

    std::mutex& getMutex() { return m_message_mutex; }

    void GetLogCurrentTime() override {
        std::stringstream m_time_sstr;
        std::time_t now = std::time(nullptr);
        std::tm* tm = std::localtime(&now);
        m_time_sstr << std::setfill('0') << std::setw(2) << tm->tm_mday << "/";
        m_time_sstr << std::setfill('0') << std::setw(2) << tm->tm_mon << "/";
        m_time_sstr << tm->tm_year + 1900 << " ";
        m_time_sstr << std::setfill('0') << std::setw(2) << tm->tm_hour << ":";
        m_time_sstr << std::setfill('0') << std::setw(2) << tm->tm_min << ":";
        m_time_sstr << std::setfill('0') << std::setw(2) << tm->tm_sec << " ";
        m_time_str = m_time_sstr.str().c_str();
    }

    void OpenForWriting(const char* name) override {
        if (!m_ofstream.is_open()) {
            m_ofstream.open(name);
            m_name = name;
        }
    }

    void CloseForWriting() override {
        if (m_ofstream.is_open()) {
            m_ofstream.close();
        }
    }

    void PreparePrint(const char* text) override {
        GetLogCurrentTime();
#ifdef WINMSVC
        OutputDebugStringA(LPCSTR(m_time_str.c_str()));
        OutputDebugStringA(LPCSTR(text));
#endif
    }
    void AppendFileMessage(const char* text) override {
        if (m_ofstream.is_open()) {
            m_ofstream << m_time_str.c_str() << text;
            CloseForWriting();
            m_ofstream.open(m_name.c_str(), std::ofstream::app);
        }
    }

    void AppendConsoleMessage(const char* text) override {
        std::cout << text << std::flush;
    }

    void Lock() override {
        m_message_mutex.lock();
    }

    void Unlock() override {
        m_message_mutex.unlock();
    }
};

BaseLoggerImpl::~BaseLoggerImpl() {}
LoggerImpl::~LoggerImpl() {}

// Constructor
QLogger::QLogger() {
    m_impl = std::make_unique<LoggerImpl>();
    uchar log_entry = 0;
    m_logger = nullptr;

    log_entry |= QLE_INFO;
    log_entry |= QLE_WARNING;
    log_entry |= QLE_ERROR;
    log_entry |= QLE_DEBUG;

    SetLogConsoleFilter(QLE_NOTHING);
    SetLogFileFilter(log_entry);
    SetStatusConsoleFilter(log_entry);

    OpenForWriting("Logs/QtGUI_Log.txt");

    uchar location_entry = QLL_FILE;

    SetLogLocation(location_entry);
}

// other functions
void QLogger::OpenForWriting(const char* name) {
    m_impl->OpenForWriting(name);
}

void QLogger::Clear() {

}

void QLogger::CloseForWriting() {
    m_impl->CloseForWriting();
}

bool QLogger::WriteMessage(const QString& text, QLOGGERENTRY type) {
    if (!m_logger) return true;

    m_impl->Lock();

    if (!m_logger->IsWriteToFileEnabled() && !m_logger->IsWriteToConsoleEnabled()) return true;

    bool valid_file = m_logger->IsFileFilterTypeEnabled(type);
    bool valid_console = m_logger->IsConsoleFilterTypeEnabled(type);
    if (!valid_console && !valid_file) return true;

    std::string text_str = text.toStdString();
    char* textc = const_cast<char*>(text_str.c_str());
    m_logger->PrintMessage(textc, valid_file, valid_console);

    m_impl->Unlock();
    return true;
}

bool QLogger::PrintMessage(char* text, bool valid_file, bool valid_console) {
    QString message = text;

    if (m_location_mask != QLL_NOWHERE) {
        m_impl->PreparePrint(message.toStdString().c_str());
    }

    if (m_location_mask & QLL_FILE && valid_file) {
        m_impl->AppendFileMessage(message.toStdString().c_str());
    }

    if (m_location_mask & QLL_CONSOLE && valid_console) {
        m_impl->AppendConsoleMessage(message.toStdString().c_str());
    }

    return true;
}

namespace XQLOGGER
{
    void AttachLogger(QLogger* logger) {
        m_logger = logger;
    }

    void OpenForWriting(const char* name) {
        m_logger->OpenForWriting(name);
    }

    void CloseForWriting(void) {
        m_logger->CloseForWriting();
    }

    void DetachLogger(void) {
        m_logger->CloseForWriting();
    }

    void SetLogConsoleFilter(unsigned char filter_mask) {
        m_logger->SetLogConsoleFilter(filter_mask);
    }

    void SetLogFileFilter(unsigned char filter_mask) {
        m_logger->SetLogFileFilter(filter_mask);
    }

    void SetStatusConsoleFilter(unsigned char filter_mask) {
        m_logger->SetStatusConsoleFilter(filter_mask);
    }

    void SetLogLocation(unsigned char location_mask) {
        m_logger->SetLogLocation(location_mask);
    }

    unsigned char GetStatusConsoleFilter() {
        return m_logger->GetStatusConsoleFilter();
    }

    unsigned char GetLogConsoleFilter() {
        return m_logger->GetLogConsoleFilter();
    }

    unsigned char GetLogFileFilter() {
        return m_logger->GetLogFileFilter();
    }

    unsigned char GetLogLocation() {
        return m_logger->GetLogLocation();
    }

    void WriteMessage(const QString& text, QLOGGERENTRY type) {
        m_logger->WriteMessage(text, type);
    }

} // namespace XLOGGER


// eof ///////////////////////////////// class Logger
