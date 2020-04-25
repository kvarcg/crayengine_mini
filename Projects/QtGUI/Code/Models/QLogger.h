/*
File:  QLogger.h

Description: QLogger Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef QLOGGER_H
#define QLOGGER_H

// includes ////////////////////////////////////////
#include <QObject>
#include <memory>

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////
enum QLOGGERENTRY : unsigned char
{
    QLE_NOTHING = 0x00,
    QLE_INFO = 0x01,
    QLE_EVENT = 0x02,
    QLE_WARNING = 0x04,
    QLE_ERROR = 0x08,
    QLE_DEBUG = 0x10
};

enum QLOGGERLOCATION : unsigned char
{
    QLL_NOWHERE = 0x00,
    QLL_FILE = 0x01,
    QLL_CONSOLE = 0x02
};

class BaseLoggerImpl {
public:
    virtual ~BaseLoggerImpl();
    virtual void Lock() = 0;
    virtual void Unlock() = 0;
    virtual void GetLogCurrentTime() = 0;
    virtual void OpenForWriting(const char* name) = 0;
    virtual void CloseForWriting() = 0;
    virtual void PreparePrint(const char* text) = 0;
    virtual void AppendFileMessage(const char* text) = 0;
    virtual void AppendConsoleMessage(const char* text) = 0;
};

class QLogger: public QObject
{
    Q_OBJECT
protected:
    // protected variable declarations
    std::unique_ptr<BaseLoggerImpl>     m_impl;
    unsigned char                       m_file_filter_mask;
    unsigned char                       m_console_filter_mask;
    unsigned char                       m_status_console_filter_mask;
    unsigned char                       m_location_mask;

    // protected function declarations


private:
    // private variable declarations


    // private function declarations


public:
    // Constructor
    QLogger(void);

    // public function declarations
    bool                                PrintMessage(char* text, bool valid_file, bool valid_console);

    void                                OpenForWriting(const char* name);
    void                                CloseForWriting(void);

    void                                Clear(void);

    // get functions
    void SetLogConsoleFilter(unsigned char filter_mask)     { m_console_filter_mask = filter_mask; }
    void SetLogFileFilter(unsigned char filter_mask)        { m_file_filter_mask = filter_mask; }
    void SetStatusConsoleFilter(unsigned char filter_mask)  { m_status_console_filter_mask = filter_mask; }
    void SetLogLocation(unsigned char location_mask)        { m_location_mask = location_mask; }

    // set functions
    unsigned char GetLogConsoleFilter(void)                 { return m_console_filter_mask; }
    unsigned char GetLogFileFilter(void)                    { return m_file_filter_mask; }
    unsigned char GetStatusConsoleFilter(void)              { return m_status_console_filter_mask; }
    unsigned char GetLogLocation(void)                      { return m_location_mask; }
    bool IsWriteToFileEnabled()                             { return (m_location_mask & QLL_FILE) > 0; }
    bool IsWriteToConsoleEnabled()                          { return (m_location_mask & QLL_CONSOLE) > 0; }
    bool IsConsoleFilterTypeEnabled(unsigned char type)     { return (m_console_filter_mask & type) > 0; }
    bool IsFileFilterTypeEnabled(unsigned char type)        { return (m_file_filter_mask & type) > 0; }
    bool                                WriteMessage(const QString& text, QLOGGERENTRY type);

};

namespace XQLOGGER
{
    void                    AttachLogger(QLogger* logger);
    void                    OpenForWriting(const char* name);
    void                    CloseForWriting(void);
    void                    DetachLogger(void);
    void                    SetLogConsoleFilter(unsigned char filter_mask);
    void                    SetLogFileFilter(unsigned char filter_mask);
    void                    SetStatusConsoleFilter(unsigned char filter_mask);
    void                    SetLogLocation(unsigned char location_mask);
    unsigned char           GetStatusConsoleFilter(void);
    unsigned char           GetLogConsoleFilter(void);
    unsigned char           GetLogFileFilter(void);
    unsigned char           GetLogLocation(void);
    void                    WriteMessage(const QString& text, QLOGGERENTRY type);
} // namespace XLOGGER


#endif //QLOGGER_H

// eof ///////////////////////////////// class Logger
