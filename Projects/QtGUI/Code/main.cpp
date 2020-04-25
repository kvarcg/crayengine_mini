/*
File:  Main.cpp

Description: Main Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////

// includes ////////////////////////////////////////
#include "Scenes/Scenes.h"
#include "QtWrapper.h"
#include "CRay/CRay.h"
#include<QSurfaceFormat>
#include "Scenes/SceneLoader.h"

#ifdef _WIN32
#include <qt_windows.h>
#include <windows.h>
#endif

void benchmark();
int main(int argc, char *argv[]) {
    //benchmark();
    //return 0;
    QSurfaceFormat fmt = QSurfaceFormat::defaultFormat();
    fmt.setMajorVersion(4);
    fmt.setMinorVersion(0);
    fmt.setProfile(QSurfaceFormat::CoreProfile);
    fmt.setRedBufferSize(8);
    fmt.setGreenBufferSize(8);
    fmt.setBlueBufferSize(8);
    fmt.setDepthBufferSize(32);
    fmt.setRenderableType(QSurfaceFormat::OpenGL);
    QSurfaceFormat::setDefaultFormat(fmt);
    QApplication app(argc, argv);
    QPixmap pix = QPixmap(":/icon");
    QIcon icon = QIcon(pix);
    app.setWindowIcon(icon);

    g_Window = new QtWrapper(argc, argv);
    g_Window->init();
    g_Window->show();
    int res = app.exec();
    delete g_Window;
    return res;
}

void benchmark() {

    CR_init();
    CR_setMinimumLogLevel(CR_LOGGER_INFO);
    CR_setBlockSize(16);
    CR_setNumThreads(12);
    CR_useMainThread(true);

    bool res = load_scene_xml("C:/Work/Projects/github/rayengine/data/test_ball.scene");
    res = generate_scene_xml();

    CR_setAOSamplesPerPixel(50);
    CR_setAORange(10.0);

    CR_setRayDepth(5);
    CR_setRussianRoulette(true);
    CR_setFilmDimensions(1024, 1024);
    CR_setSamplesPerPixel(100);

    CR_start();
}
