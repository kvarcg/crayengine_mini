/*
File:  QStatusBarWidget.cpp

Description: QStatusBarWidget Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global.h"

// includes ////////////////////////////////////////
#include "QStatusBarWidget.h"

// defines /////////////////////////////////////////

// Constructor
QStatusBarWidget::QStatusBarWidget(QWidget *parent, Qt::WindowFlags f)
: QWidget(parent, f) {
    m_ui.setupUi(this);
    this->adjustSize();

    init();
}

// other functions
void QStatusBarWidget::init() {

}

Ui::statusbarWidget& QStatusBarWidget::getUI() {
    return m_ui;
}

// eof ///////////////////////////////// class QStatusBarWidget
