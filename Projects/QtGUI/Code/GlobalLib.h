/*
File:  GlobalLib.h

Description: GlobalLib Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef GLOBALLIB_H
#define GLOBALLIB_H

// includes ////////////////////////////////////////
#include <QString>
#include <Models/GeneralPreferencesModel.h>

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class GlobalLib
{

protected:
    // protected variable declarations
    GeneralPreferencesModel	            m_general_preferences_model;

    // protected function declarations

private:
    // private variable declarations


    // private function declarations


public:
    // Constructor
    GlobalLib(void);

    // Destructor
    ~GlobalLib(void);

    // public function declarations

    // get functions
    GeneralPreferencesModel&            GetPreferencesModel();
    const char* GetVersion();
    QString                             convertIntToSizeString(qint64 size);
    QString                             convertIntToFormattedString(float size);
    QString&                            GetCurrentPath();
    void                                GetCurrentDateTime(QString& timestr);

    // set functions
    void                                SetCurrentPath(const QString& path);
};

extern GlobalLib* g_globalLib;

#endif //GLOBALLIB_H

// eof ///////////////////////////////// class GlobalLib
