/*
File:  QStatusbarConsole.h

Description: QStatusbarConsole Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef QSTATUSBARCONSOLE_H
#define QSTATUSBARCONSOLE_H

// includes ////////////////////////////////////////
#include <QObject>
#include "Models/QLogger.h"
#include <memory>
#include <QTimer>
#include "GUI/Widgets/QStatusBarWidget.h"

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class QStatusbarConsole : public QObject
{
    Q_OBJECT
protected:
    // protected variable declarations
    QTimer                              m_timer;
    QMap<QLOGGERENTRY, QString>         m_type_color_map;
    QList<int>                          m_status_type_list;
    QStringList                         m_status_list;
    qint32                              m_message_counter;
    QStatusBarWidget                    m_statusbar_widget;
    class QToolButton*                  m_statusbar_button;
    class QLineEdit*                    m_statusbar_label;


    // protected function declarations


private:
    // private variable declarations


    // private function declarations


public:
    // Constructor
    QStatusbarConsole(void);

    // Destructor
    ~QStatusbarConsole(void) override;


    // public function declarations
    void                                init();
    void                                AddStatusMessage(const QString& message, enum QLOGGERENTRY type);

    // get functions

    // set functions

public slots:
void refreshUI();
void buttonClearClicked();
void buttonCloseClicked();
void buttonOpenClicked();
};

#endif //QSTATUSBARCONSOLE_H

// eof ///////////////////////////////// class QStatusbarConsole
