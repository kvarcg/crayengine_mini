/*
File:  QStatusbarConsole.cpp

Description: QStatusbarConsole Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global.h"

// includes ////////////////////////////////////////
#include "QtWrapper.h"
#include "QStatusbarConsole.h"
#include "GUI/Widgets/QStatusBarWidget.h"
#include <QTimer>
#include <QFont>

// defines /////////////////////////////////////////

// Constructor
QStatusbarConsole::QStatusbarConsole() {
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(refreshUI()));

    m_message_counter = 1;
}

// Destructor
QStatusbarConsole::~QStatusbarConsole() {
    m_timer.stop();
}

// other functions
void QStatusbarConsole::init() {
    m_timer.start(100);

    Ui::mainWindow* mainUi = g_Window->getUI();
    mainUi->statusBar->addWidget(&m_statusbar_widget, 1);
    mainUi->buttonStatusConsoleClose->setEnabled(true);

    m_statusbar_button = m_statusbar_widget.getUI().buttonOpen;
    m_statusbar_label = m_statusbar_widget.getUI().messageLabel;
    mainUi->statusBar->adjustSize();
    connect(mainUi->buttonStatusConsoleClear, SIGNAL(clicked()), this, SLOT(buttonClearClicked()));
    connect(mainUi->buttonStatusConsoleClose, SIGNAL(clicked()), this, SLOT(buttonCloseClicked()));
    connect(m_statusbar_widget.getUI().buttonOpen, SIGNAL(clicked()), this, SLOT(buttonOpenClicked()));

    m_type_color_map[QLE_INFO]          = "#000000";
    m_type_color_map[QLE_EVENT]         = "#cfcfcf";
    m_type_color_map[QLE_WARNING]       = "#888800";
    m_type_color_map[QLE_ERROR]         = "#880000";
    m_type_color_map[QLE_DEBUG]         = "#006688";

    mainUi->textBrowserStatusConsole->clear();
}

void QStatusbarConsole::refreshUI() {
    Ui::mainWindow* mainUi = g_Window->getUI();
    if (mainUi->statusbarConsoleWidget->isVisible()) {
        for (int i = 0; i < m_status_list.size(); ++i) {
            QStringList strings = m_status_list[i].split(QRegExp("[\r\n]"));
            QString color = m_type_color_map[(QLOGGERENTRY)m_status_type_list[i]];
            mainUi->textBrowserStatusConsole->setTextColor(color);

            for (auto& cur_str : strings) {
                if (cur_str.isEmpty()) {
                    continue;
                }
                QString str = QString::number(m_message_counter).append(". ").append(cur_str).append("\n");
                mainUi->textBrowserStatusConsole->insertPlainText(str);
            }
            m_message_counter++;
        }
        m_status_list.clear();
        m_status_type_list.clear();
    }
}

void QStatusbarConsole::AddStatusMessage(const QString& message, QLOGGERENTRY type) {
    XQLOGGER::WriteMessage(message, type);
    uchar filter_mask = XQLOGGER::GetStatusConsoleFilter();
    QString appended = "";
    bool masked = true;
    if ((filter_mask & type) == QLE_DEBUG) {
        //appended = "Debug: ";
        masked = false;
    }
    else if ((filter_mask & type) == QLE_ERROR) {
    // appended = "Error: ";
        masked = false;
    }
    else if ((filter_mask & type) == QLE_WARNING) {
    // appended = "Warning: ";
        masked = false;
    }
    else if ((filter_mask & type) == QLE_INFO) {
        masked = false;
    }

    if (masked) {
        return;
    }

    QString appended_str(message);
    appended_str.insert(0, appended);

    m_statusbar_widget.getUI().messageLabel->setText(appended_str);

    m_status_list.push_back(appended_str);
    m_status_type_list.push_back((int)type);
}

void QStatusbarConsole::buttonClearClicked() {
    Ui::mainWindow* mainUi = g_Window->getUI();
    mainUi->textBrowserStatusConsole->clear();
    m_message_counter = 1;
    m_status_list.clear();
}

void QStatusbarConsole::buttonCloseClicked() {
    Ui::mainWindow* mainUi = g_Window->getUI();
    mainUi->statusbarConsoleWidget->hide();
    m_statusbar_widget.getUI().buttonOpen->setEnabled(true);
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/Icons/Resources/Images/basic_icons/arrow_up.png"), QSize(), QIcon::Normal, QIcon::Off);
    m_statusbar_widget.getUI().buttonOpen->setIcon(icon);
    m_statusbar_widget.getUI().buttonOpen->setChecked(false);
}

void QStatusbarConsole::buttonOpenClicked() {
    Ui::mainWindow* mainUi = g_Window->getUI();
    if (mainUi->statusbarConsoleWidget->isVisible()) {
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Icons/Resources/Images/basic_icons/arrow_up.png"), QSize(), QIcon::Normal, QIcon::Off);
        m_statusbar_widget.getUI().buttonOpen->setIcon(icon);
        m_statusbar_widget.getUI().buttonOpen->setChecked(false);
        mainUi->statusbarConsoleWidget->hide();
    }
    else {
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Icons/Resources/Images/basic_icons/arrow_down.png"), QSize(), QIcon::Normal, QIcon::Off);
        m_statusbar_widget.getUI().buttonOpen->setIcon(icon);
        m_statusbar_widget.getUI().buttonOpen->setChecked(true);
        mainUi->statusbarConsoleWidget->show();
    }
}

// eof ///////////////////////////////// class GLGraphicsToolbars
