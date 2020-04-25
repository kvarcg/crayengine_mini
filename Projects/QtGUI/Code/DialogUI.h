/*
File:  DialogUI.h

Description: DialogUI Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef DIALOGSUI_H
#define DIALOGSUI_H

// includes ////////////////////////////////////////
#include <QObject>
#include <memory>
#include "MenuActions.h"
#include "GUI/Dialogs/QDialogAbout.h"
#include "GUI/Dialogs/QDialogLicenses.h"

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class DialogUI: public QObject
{
    Q_OBJECT
protected:
    friend class MenuActions;
    // protected variable declarations
    MenuActions                         m_menuActions;
    QDialogAbout                        m_dialogAbout;
    QDialogLicenses                     m_dialogLicenses;

    // protected function declarations


private:
    // private variable declarations


    // private function declarations


public:

    // public function declarations
    void                                setupLayout();

    // get functions
    MenuActions*                        getMenuActions();

    // set functions
};

#endif //DIALOGSUI_H

// eof ///////////////////////////////// class DialogUI
