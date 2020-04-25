/*
File:  QDialogLicenses.cpp

Description: QDialogLicenses Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global.h"

// includes ////////////////////////////////////////
#include "QDialogLicenses.h"
#include <QMouseEvent>

// defines /////////////////////////////////////////

// Constructor
QDialogLicenses::QDialogLicenses(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent, f) {
    m_ui.setupUi(this);
    this->adjustSize();
    m_is_moving = false;

    init();

    connect(m_ui.buttonBox, SIGNAL(accepted()), this, SLOT(ButtonOKClicked()));
}

// other functions
void QDialogLicenses::init() {
    m_ui.TitleLabel->setVisible(false);
}

void QDialogLicenses::ButtonOKClicked() {
    this->hide();
}

void QDialogLicenses::mouseMoveEvent(QMouseEvent *event) {
    if (m_is_moving) {
        const QPointF delta = event->globalPos() - m_old_pos;
        this->move(x()+delta.x(), y()+delta.y());
        m_old_pos = event->globalPos();
    }
    event->accept();
}

void QDialogLicenses::mousePressEvent(QMouseEvent *event) {
    m_is_moving = true;
    m_old_pos = event->globalPos();

    event->accept();
}

void QDialogLicenses::mouseReleaseEvent(QMouseEvent *event) {
    m_is_moving = false;
    event->accept();
}

// eof ///////////////////////////////// class QDialogLicenses
