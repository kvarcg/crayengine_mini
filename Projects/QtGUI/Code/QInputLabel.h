/*
File:  QInputLabel.h

Description: QInputLabel Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef QINPUTLABEL_H
#define QINPUTLABEL_H

// includes ////////////////////////////////////////
#include <QKeyEvent>
#include <QMouseEvent>
#include <QLabel>

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class QInputLabel : public QLabel {
Q_OBJECT
public:
explicit QInputLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
~QInputLabel() override;

protected:
void                                mouseDoubleClickEvent(QMouseEvent* event) override;
void                                mousePressEvent(QMouseEvent* event) override;
void                                mouseMoveEvent(QMouseEvent* event) override;
void                                mouseReleaseEvent(QMouseEvent* event) override;
void                                keyPressEvent(QKeyEvent *event) override;
void                                keyReleaseEvent(QKeyEvent *event) override;
};



#endif //QINPUTLABEL_H

// eof ///////////////////////////////// class Input


