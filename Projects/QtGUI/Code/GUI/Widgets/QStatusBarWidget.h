/*
File:  QStatusBarWidget.h

Description: QStatusBarWidget Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef QSTATUSBARWIDGET_H
#define QSTATUSBARWIDGET_H

// includes ////////////////////////////////////////
#include <QWidget>
#include "../../../ui_statusBarWidget.h"
#include <memory>


// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class QStatusBarWidget : public QWidget
{
    Q_OBJECT
protected:
    // protected variable declarations
    Ui::statusbarWidget                 m_ui;

    // protected function declarations


private:
    // private variable declarations


    // private function declarations


public:
    // Constructor
    QStatusBarWidget(QWidget *parent = nullptr, Qt::WindowFlags f = nullptr);

    // public function declarations
    void                                init();

    // get functions
    Ui::statusbarWidget&                getUI();


    // set functions


};

#endif //QSTATUSBARWIDGET_H

// eof ///////////////////////////////// class QStatusBarWidget
