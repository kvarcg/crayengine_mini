/*
File:  QDialogAbout.h

Description: QDialogAbout Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef QDIALOGABOUT_H
#define QDIALOGABOUT_H

// includes ////////////////////////////////////////
#include <QDialog>
#include "../../../ui_dialogAbout.h"
#include <memory>

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class QDialogAbout : public QDialog
{
    Q_OBJECT

protected:
    // protected variable declarations
    Ui::DialogAbout                     m_ui;
    bool                                m_is_moving;
    QPointF                             m_old_pos;

    // protected function declarations
    void                                mouseMoveEvent(QMouseEvent *event) override;
    void                                mousePressEvent(QMouseEvent *event) override;
    void                                mouseReleaseEvent(QMouseEvent *event) override;


private:
    // private variable declarations


    // private function declarations


public:
    // Constructor
    QDialogAbout(QWidget *parent = nullptr, Qt::WindowFlags f = nullptr);

    // public function declarations
    void                                init();

    // get functions


    // set functions

protected slots:
    void ButtonOKClicked();
};

#endif //QDIALOGABOUT_H

// eof ///////////////////////////////// class QDialogAbout
