/*
File:  MenuActions.h

Description: MenuActions Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef GLMENUACTIONS_H
#define GLMENUACTIONS_H

// includes ////////////////////////////////////////
#include <QObject>

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class MenuActions: public QObject
{
    Q_OBJECT

protected:
    // protected variable declarations

    // protected function declarations


private:
    // private variable declarations


    // private function declarations


public:
    // Constructor
    MenuActions(void);

    // Destructor
    ~MenuActions(void);

    // public function declarations
    void                                init();

    // get functions


    // set functions

    public slots:
        void actionFileExitClicked();
        void actionHelpAboutClicked();
        void actionHelpLicensesClicked();
};

#endif //GLMENUACTIONS_H

// eof ///////////////////////////////// class MenuActions
