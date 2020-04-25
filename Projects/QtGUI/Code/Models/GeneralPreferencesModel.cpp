/*
File:  GeneralPreferencesModel.cpp

Description: GeneralPreferencesModel Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global.h"

// includes ////////////////////////////////////////
#include "QtWrapper.h"
#include "GeneralPreferencesModel.h"
#include <regex>
#include <exception>
#include <QFile>
#include <QTextStream>

// defines /////////////////////////////////////////


// Constructor
GeneralPreferencesModel::GeneralPreferencesModel():
m_file_str("Config/config.txt") {

}

// Destructor
GeneralPreferencesModel::~GeneralPreferencesModel() {

}

// other functions
void GeneralPreferencesModel::init() {
    readFile();
}

void GeneralPreferencesModel::buildFile(QString& filestr) {
    filestr.append("DATA_DIR ");
    filestr.append(m_general_preferences.m_default_folder);
    filestr.append("\n");
}

void GeneralPreferencesModel::readFile() {
    QFile file(m_file_str);
    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly)) {
            QString filestr = "";
            buildFile(filestr);
            QTextStream writeToFile(&file);
            writeToFile << filestr;
            file.close();
        }
        else {
            QString msg = QString("Failed to create preferences config file. Using default parameters");
            g_Window->AddStatusErrorMessage(msg);
        }
    }
    else {
        bool read_folder = false;
        if (file.open(QIODevice::ReadOnly)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();
                if (line.contains("DATA_DIR", Qt::CaseInsensitive)) {
                    line = line.simplified();
                    line.remove("DATA_DIR ");
                    bool ok = false;
                    QString f = line;
                    ok = QDir(f).exists();
                    if (ok) {
                        m_general_preferences.m_default_folder = f;
                        read_folder = true;
                    }
                }
            }
            file.close();
        }

        bool resave = false;
        if (!read_folder) {
            QString msg = QString("Failed to read or locate the ").append("DATA_DIR").toLower().append(" value in preferences config file. Using ").append(m_general_preferences.m_default_folder);
            g_Window->AddStatusWarningMessage(msg);
            resave = true;
        }

        if (resave) {
            if (file.open(QIODevice::WriteOnly)) {
                QString filestr = "";
                buildFile(filestr);
                QTextStream writeToFile(&file);
                writeToFile << filestr;
                file.close();
            }
            else {
                QString msg = QString("Failed to create preferences config file. Using default parameters");
                g_Window->AddStatusErrorMessage(msg);
            }
        }
    }
}

bool GeneralPreferencesModel::Save() {
    QString filestr = "";
    buildFile(filestr);
    QFile file(m_file_str);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream writeToFile(&file);
        writeToFile << filestr;
        file.close();
    }
    else {
        QString msg = QString("Failed to save preferences config file.");
        return false;
    }
    return true;
}

GENERAL_PREFERENCES& GeneralPreferencesModel::GetPreferences() {
    return m_general_preferences;
}

// eof ///////////////////////////////// class dialogUI
