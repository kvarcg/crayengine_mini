/*
File:  GlobalLib.cpp

Description: GlobalLib Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global.h"

// includes ////////////////////////////////////////
#include "GlobalLib.h"
#include <ctime>
#include <QDir>
#include <cmath>
#include <iomanip>
#include <sstream>

// defines /////////////////////////////////////////
GlobalLib* g_globalLib;

// Constructor
GlobalLib::GlobalLib() {
    m_general_preferences_model.init();
}

// Destructor
GlobalLib::~GlobalLib() {

}

// other functions
const char* GlobalLib::GetVersion() {
    return "0.01a";
}

QString GlobalLib::convertIntToSizeString(qint64 size) {
    double byte = sizeof(char);
    double KB = 1024 * byte;
    double MB = 1024 * KB;
    double GB = 1024 * MB;

    QString res;
    if (size > GB) {
        double s = size / GB;
        res = QString::number(s, 'f', 4).append(" GB");
    }
    else if (size > MB) {
        double s = size / MB;
        res = QString::number(s, 'f', 4).append(" MB");
    }
    else if (size > KB) {
        double s = size / KB;
        res = QString::number(s, 'f', 4).append(" KB");
    }
    else
    {
        double s = size / byte;
        res = QString::number(s, 'f', 4).append(" bytes");
    }
    return res;
}

QString GlobalLib::convertIntToFormattedString(float size) {
    double thousand = 1000;
    double million = 1000 * thousand;
    double billion = 1000 * million;

    double s = size;

    QString res;
    if (s > billion) {
        s /= billion;
        res = QString::number(s, 'f', 4);
        res.append("B");
    }
    else if (s > million) {
        s /= million;
        res = QString::number(s, 'f', 4);
        res.append("M");
    }
    else if (s > thousand) {
        s /= thousand;
        res = QString::number(s, 'f', 4);
        res.append("K");
    }
    else
    {
        double ff = std::floor(s);
        double remf = s - ff;
        if (remf < 0.00001)
            res = QString::number(s, 'f', 6);
        else if (remf < 0.0001)
            res = QString::number(s, 'f', 5);
        else if (remf < 0.001)
            res = QString::number(s, 'f', 4);
        else if (remf < 0.01)
            res = QString::number(s, 'f', 3);
        else
            res = QString::number(s, 'f', 2);
    }
    return res;
}

QString& GlobalLib::GetCurrentPath() {
    return m_general_preferences_model.GetPreferences().m_default_folder;
}

void GlobalLib::SetCurrentPath(const QString& path) {
    m_general_preferences_model.GetPreferences().m_default_folder = path;
    m_general_preferences_model.Save();
}

GeneralPreferencesModel& GlobalLib::GetPreferencesModel() {
    return m_general_preferences_model;
}

void GlobalLib::GetCurrentDateTime(QString& timestr) {
    std::stringstream m_time_sstr;
    std::time_t now = std::time(nullptr);
    std::tm* tm = std::localtime(&now);
    m_time_sstr << std::setfill('0') << std::setw(2) << tm->tm_mday << "/";
    m_time_sstr << std::setfill('0') << std::setw(2) << tm->tm_mon << "/";
    m_time_sstr << tm->tm_year + 1900 << " ";
    m_time_sstr << std::setfill('0') << std::setw(2) << tm->tm_hour << ":";
    m_time_sstr << std::setfill('0') << std::setw(2) << tm->tm_min << ":";
    m_time_sstr << std::setfill('0') << std::setw(2) << tm->tm_sec << " ";
    timestr = m_time_sstr.str().c_str();
}

// eof ///////////////////////////////// class GlobalLib
