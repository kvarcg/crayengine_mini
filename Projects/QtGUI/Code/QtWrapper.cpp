/*
File:  QtWrapper.cpp

Description: QtWrapper Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global.h"

// includes ////////////////////////////////////////
#include "QtWrapper.h"
#include "CRay/CRay.h"
#include "DialogUI.h"
#include "MenuActions.h"
#include "GUI/Widgets/QStatusBarWidget.h"
#include "GUI/Widgets/QStatusbarConsole.h"
#include "QMainUI.h"
#include "Models/QLogger.h"
#include <thread>
#include <QKeyEvent>
#include <QMimeData>
#include <QDir>
#include "DisplayEngine.h"
#include "GlobalLib.h"
#include <qdebug.h>
#include <QImageReader>
#include <QImage>
#include <memory>
#include <sstream>

// defines /////////////////////////////////////////
QtWrapper* g_Window = nullptr;

// Constructor
QtWrapper::QtWrapper(int , char *[])
    : QMainWindow(nullptr) {
    m_initialized = false;

    m_ui.setupUi(this);

    if (!QDir("Config").exists()) {
        QDir().mkdir("Config");
    }
    if (!QDir("Logs").exists()) {
        QDir().mkdir("Logs");
    }

    XQLOGGER::AttachLogger(&m_logger);
    unsigned char log_entry = 0;
    log_entry |= QLE_INFO;
    log_entry |= QLE_WARNING;
    log_entry |= QLE_ERROR;
    log_entry |= QLE_DEBUG;

    unsigned char log_location = 0;
    log_location |= QLL_FILE;
    log_location |= QLL_CONSOLE;

    XQLOGGER::SetLogLocation(log_location);
    XQLOGGER::SetLogConsoleFilter(log_entry);
    XQLOGGER::SetLogFileFilter(log_entry);
    XQLOGGER::SetStatusConsoleFilter(log_entry);

    QFont f = QApplication::font();
    f.setStyleStrategy(QFont::PreferAntialias);
    QApplication::setFont(f);

    m_ui.testProgressBar->setValue(0);
    m_ui.testProgressBar->setVisible(true);

    m_ui.statusbarConsoleWidget->hide();

    qApp->installEventFilter(this);
}

// Destructor
QtWrapper::~QtWrapper() {
    m_inputEngine.Destroy();
    delete g_globalLib;
}

// other functions
void QtWrapper::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
#ifndef NDEBUG
        m_dialogsUI.getMenuActions()->actionFileExitClicked();
#endif
    }
}

void QtWrapper::closeEvent(QCloseEvent *) {
    m_dialogsUI.getMenuActions()->actionFileExitClicked();
}

Ui::mainWindow* QtWrapper::getUI() {
    return &m_ui;
}

DialogUI* QtWrapper::getDialogUI() {
    return &m_dialogsUI;
}

#include <random>
#include <chrono>

// includes ////////////////////////////////////////
void QtWrapper::init() {
    std::thread::id this_id = std::this_thread::get_id();
    std::stringstream ss;
    ss << this_id;
    QString mystring(ss.str().c_str());
    qDebug() << "Main thread id " << mystring;

    g_globalLib = new GlobalLib();
    m_dialogsUI.setupLayout();
    m_displayEngine.init();
    m_mainLogicUI.init();
    m_statusbarModel.init();

    //randomtest();
    m_initialized = true;
}

void QtWrapper::AddStatusWarningMessage(const QString& message) {
    m_statusbarModel.AddStatusMessage(message, QLE_WARNING);
}

void QtWrapper::AddStatusErrorMessage(const QString& message) {
    m_statusbarModel.AddStatusMessage(message, QLE_ERROR);
}

void QtWrapper::AddStatusDebugMessage(const QString& message) {
    m_statusbarModel.AddStatusMessage(message, QLE_DEBUG);
}

void QtWrapper::AddStatusMessage(const QString& message) {
    m_statusbarModel.AddStatusMessage(message, QLE_INFO);
}

bool QtWrapper::acceptDragFiles(const QMimeData* mimeData) {
    if (mimeData->hasUrls()) {
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();

        if (urlList.size() != 1) {
            return false;
        }

        QFile file(urlList.at(0).toLocalFile());
        QFileInfo info(file);
        if (!file.exists()) {
            g_Window->AddStatusWarningMessage(QString("File ").append(info.fileName()).append(" cannot be found."));
            return false;
        }

        QString suffix = info.suffix().toLower();
        if (!suffix.compare("scene")) {
            return true;
        }
    }

    return false;
}

void QtWrapper::dragEnterEvent(QDragEnterEvent* event) {
    const QMimeData* mimeData = event->mimeData();
    if (!acceptDragFiles(mimeData))
 	    return;

    // if some actions should not be usable, like move, this code must be adopted
    event->acceptProposedAction();
}

void QtWrapper::dropEvent(QDropEvent* event) {
    const QMimeData* mimeData = event->mimeData();
    if (!acceptDragFiles(mimeData))
        return;

    QList<QUrl> urlList = mimeData->urls();
    QFile file(urlList.at(0).toLocalFile());
    QFileInfo info(file);
    m_mainLogicUI.startLoadFileOperation(info.absoluteFilePath());
    event->acceptProposedAction();
}

// eof ///////////////////////////////// class QtWrapper
