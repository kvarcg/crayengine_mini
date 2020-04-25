/*
File:  QMainUI.h

Description: QMainUI Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef QMAINTOOLBAR_H
#define QMAINTOOLBAR_H

// includes ////////////////////////////////////////
#include <QObject>
#include <QTimer>

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class QMainUI : public QObject
{
    Q_OBJECT
protected:
    // protected variable declarations
    QTimer                              m_timer;
    QString                             last_file;

    // protected function declarations


private:
    // private variable declarations


    // private function declarations


public:
    // Constructor
    QMainUI(void);

    // Destructor
    ~QMainUI(void) override;

    // public function declarations
    void                                init();
void								startLoadFileOperation(const QString& filename);

    // get functions


    // set functions

public slots:
void buttonLoadClicked();
void buttonReloadClicked();
void buttonStartEngineClicked();
void buttonStopEngineClicked();
void buttonSaveImageClicked();
void buttonPrintCameraClicked();
void stopped();
};

#endif //QMAINTOOLBAR_H

// eof ///////////////////////////////// class GLGraphicsToolbars
