/*
File:  QtWrapper.h

Description: QtWrapper Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef QTWRAPPER_H
#define QTWRAPPER_H

// includes ////////////////////////////////////////
#include <QtWidgets/QMainWindow>
#include "../ui_QtGUI.h"
#include <memory>
#include "DialogUI.h"
#include "GUI/Widgets/QStatusbarConsole.h"
#include <QProgressBar>
#include "Models/QLogger.h"
#include "DisplayEngine.h"
#include "InputEngine.h"
#include "QMainUI.h"


// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class QtWrapper : public QMainWindow
{
    Q_OBJECT

protected:
    // protected variable declarations
    DialogUI                            m_dialogsUI;
    QStatusbarConsole                   m_statusbarModel;
    QLogger                             m_logger;
    DisplayEngine                       m_displayEngine;
    InputEngine                         m_inputEngine;
    bool                                m_initialized;
    QMainUI                             m_mainLogicUI;

    // protected function declarations


private:
    // private variable declarations
    Ui::mainWindow                      m_ui;

    // private function declarations


public:
    // Constructor
    QtWrapper(int argc, char *argv[]);

    // Destructor
    ~QtWrapper(void) override;

    // public function declarations
    void                                dragEnterEvent(QDragEnterEvent* event) override;
    void                                dropEvent(QDropEvent* event) override;
    bool                                acceptDragFiles(const QMimeData* mimedata);

    Ui::mainWindow*                     getUI();
    DialogUI*                           getDialogUI();
    void                                init();
    void                                keyPressEvent(QKeyEvent *event) override;
    void                                closeEvent(QCloseEvent *event) override;
    void                                AddStatusMessage(const QString& message);
    void                                AddStatusDebugMessage(const QString& message);
    void                                AddStatusWarningMessage(const QString& message);
    void                                AddStatusErrorMessage(const QString& message);

    void                                StartEngine()           { m_displayEngine.start(); }
    void                                StopEngine()            { m_displayEngine.stop(); }
    bool                                StoppedEngine()         { return m_displayEngine.ready(); }

    // get functions
    QStatusbarConsole*                  GetConsole()            { return &m_statusbarModel; }
    DisplayEngine*                      GetDisplayEngine()      { return &m_displayEngine; }
    InputEngine*                        GetInputEngine()        { return &m_inputEngine; }

    // set functions
    bool                                EngineStarted()         { return m_displayEngine.started(); }


public slots:

};

extern QtWrapper* g_Window;

#endif //QTWRAPPER

// eof ///////////////////////////////// class QtWrapper
