/*
File:  DialogUI.cpp

Description: DialogUI Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global.h"

// includes ////////////////////////////////////////
#include "DialogUI.h"
#include "QMainUI.h"
#include "QtWrapper.h"

// defines /////////////////////////////////////////

void DialogUI::setupLayout() {
    m_menuActions.init();
}

MenuActions* DialogUI::getMenuActions() {
    return &m_menuActions;
}

// eof ///////////////////////////////// class DialogUI
