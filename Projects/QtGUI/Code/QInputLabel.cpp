/*
File:  QInputLabel.cpp

Description: QInputLabel Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// includes ////////////////////////////////////////
#include "QtWrapper.h"
#include "QInputLabel.h"

// defines /////////////////////////////////////////


QInputLabel::QInputLabel(QWidget* parent, Qt::WindowFlags f)
    : QLabel(parent, f) {

    this->setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    this->setScaledContents(false);
    this->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);
}

QInputLabel::~QInputLabel() {}

void QInputLabel::mouseDoubleClickEvent(QMouseEvent* event) {
    //g_Window->AddStatusDebugMessage("Mouse Press Accepted\n");

    g_Window->GetInputEngine()->SceneMouse(event, true, true);
    g_Window->getUI()->spinBoxPixelStartX->setValue(event->pos().x());
    g_Window->getUI()->spinBoxPixelStartY->setValue(event->pos().y());
    g_Window->getUI()->spinBoxPixelEndX->setValue(event->pos().x());
    g_Window->getUI()->spinBoxPixelEndY->setValue(event->pos().y());
    g_Window->GetDisplayEngine()->printIntersect();
    event->accept();
}

void QInputLabel::mousePressEvent(QMouseEvent* event) {
    //g_Window->AddStatusDebugMessage("Mouse Press Accepted\n");

    g_Window->GetInputEngine()->SceneMouse(event, true, false);
    event->accept();
}

void QInputLabel::mouseReleaseEvent(QMouseEvent* event) {
    //g_Window->AddStatusDebugMessage("Mouse Release Accepted\n");

    g_Window->GetInputEngine()->SceneMouse(event, false, false);
    event->accept();
}

void QInputLabel::mouseMoveEvent(QMouseEvent* event) {

    g_Window->GetInputEngine()->SceneMouseMotion(event);
    event->accept();
}

void QInputLabel::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        QApplication::closeAllWindows();
    }

    g_Window->GetInputEngine()->CheckSpecialKeys(event);
    g_Window->GetInputEngine()->KeyboardPressed(event);
    event->accept();
}

void QInputLabel::keyReleaseEvent(QKeyEvent *event) {
    g_Window->GetInputEngine()->CheckSpecialKeys(event);
    g_Window->GetInputEngine()->KeyboardUp(event);
    event->accept();
}

// eof ///////////////////////////////// class Input
