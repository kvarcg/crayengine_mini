/*
File:  QDialogAbout.cpp

Description: QDialogAbout Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global.h"

// includes ////////////////////////////////////////
#include "QDialogAbout.h"
#include <QMouseEvent>

// defines /////////////////////////////////////////


// Constructor
QDialogAbout::QDialogAbout(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f) {
    m_ui.setupUi(this);
    this->adjustSize();
    m_is_moving = false;

    init();

    connect(m_ui.buttonBox, SIGNAL(accepted()), this, SLOT(ButtonOKClicked()));
}

// other functions
void QDialogAbout::init() {
    m_ui.TitleLabel->setVisible(false);
}

void QDialogAbout::ButtonOKClicked() {
    this->hide();
}

void QDialogAbout::mouseMoveEvent(QMouseEvent *event) {
    if (m_is_moving) {
        const QPointF delta = event->globalPos() - m_old_pos;
        this->move(x()+delta.x(), y()+delta.y());
        m_old_pos = event->globalPos();
    }
    event->accept();
}

void QDialogAbout::mousePressEvent(QMouseEvent *event) {
    m_is_moving = true;
    m_old_pos = event->globalPos();

    event->accept();
}

void QDialogAbout::mouseReleaseEvent(QMouseEvent *event) {
    m_is_moving = false;
    event->accept();
}

// eof ///////////////////////////////// class QDialogAbout
