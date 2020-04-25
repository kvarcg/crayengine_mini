/*
File:  MenuActions.cpp

Description: MenuActions Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////

// includes ////////////////////////////////////////
#include "QtWrapper.h"
#include "MenuActions.h"
#include "DialogUI.h"
#include "GUI/Dialogs/QDialogAbout.h"
#include "GUI/Dialogs/QDialogLicenses.h"
#include <QFileDialog>


// defines /////////////////////////////////////////


// Constructor
MenuActions::MenuActions() {

}

// Destructor
MenuActions::~MenuActions() {

}

// other functions
void MenuActions::init() {
    Ui::mainWindow* mainUi = g_Window->getUI();

    // File
    connect(mainUi->actionExit, SIGNAL(triggered()), this, SLOT(actionFileExitClicked()));

    // help
    connect(mainUi->actionAbout, SIGNAL(triggered()), this, SLOT(actionHelpAboutClicked()));
    connect(mainUi->actionLicenses, SIGNAL(triggered()), this, SLOT(actionHelpLicensesClicked()));
}

void MenuActions::actionFileExitClicked() {
    g_Window->close();
}

void MenuActions::actionHelpAboutClicked() {
    DialogUI* dialogUi = g_Window->getDialogUI();
    dialogUi->m_dialogAbout.setVisible(true);
}

void MenuActions::actionHelpLicensesClicked() {
    DialogUI* dialogUi = g_Window->getDialogUI();
    dialogUi->m_dialogLicenses.setVisible(true);
}

// eof ///////////////////////////////// class MenuActions
