/*
File:  DisplayEngine.h

Description: DisplayEngine Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef DISPLAYENGINE_H
#define DISPLAYENGINE_H

// includes ////////////////////////////////////////
#include <QObject>
#include <QTimer>

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

class DisplayEngine: public QObject
{
Q_OBJECT

protected:
// protected variable declarations
QTimer                              m_timer;

// protected function declarations
void                                buildScene();

private:
// private variable declarations


// private function declarations


public:
// Constructor
DisplayEngine(void);

// Destructor
~DisplayEngine(void);

// public function declarations
void                                init();
void                                loadScene(const QString& filename);

// get functions
bool                                started();
void                                start();
void                                stop();
bool                                ready();
void                                saveFile(QString& filename);
void                                printIntersect();

// set functions

public slots:
void updateImage();
};

#endif //DISPLAYENGINE_H

// eof ///////////////////////////////// class MenuActions
