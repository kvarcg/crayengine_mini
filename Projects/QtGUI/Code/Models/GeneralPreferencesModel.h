/*
File:  GeneralPreferencesModel.h

Description: GeneralPreferencesModel Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef GENERALPREFERENCESMODEL_H
#define GENERALPREFERENCESMODEL_H

// includes ////////////////////////////////////////
#include <QObject>
#include <QString>
#include <QDir>

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

struct GENERAL_PREFERENCES {
    QString								m_default_folder;
    GENERAL_PREFERENCES() : m_default_folder(QDir::currentPath()) { ; }
};

class GeneralPreferencesModel : public QObject
{
    Q_OBJECT
public:

protected:
    // protected variable declarations

    GENERAL_PREFERENCES                 m_general_preferences;
    QString                             m_file_str;

    // protected function declarations
    void								buildFile(QString& filestr);
    void								readFile();

private:
    // private variable declarations


    // private function declarations


public:

    // Constructor
    GeneralPreferencesModel(void);

    // Destructor
    ~GeneralPreferencesModel(void);

    // public function declarations
    void                                init();
    bool								Save();

    // get functions
    GENERAL_PREFERENCES&                GetPreferences();

    // set functions

public slots:
};

#endif //GENERALPREFERENCESMODEL_H

// eof ///////////////////////////////// class GeneralPreferencesModel
