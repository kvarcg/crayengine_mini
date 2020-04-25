/*
File:  QMainUI.cpp

Description: QMainUI Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global.h"

// includes ////////////////////////////////////////
#include "CRay/CRay.h"
#include "QtWrapper.h"
#include "QMainUI.h"
#include "GlobalLib.h"
#include <QFileDialog>

// defines /////////////////////////////////////////

// Constructor
QMainUI::QMainUI() {
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(stopped()));
}

// Destructor
QMainUI::~QMainUI() {
    m_timer.stop();
}

// other functions
void QMainUI::init() {
    Ui::mainWindow* mainUi = g_Window->getUI();
    mainUi->buttonStartEngine->setEnabled(true);
    mainUi->buttonStopEngine->setEnabled(true);

    connect(mainUi->buttonStartEngine, SIGNAL(clicked()), this, SLOT(buttonStartEngineClicked()));
    connect(mainUi->buttonStopEngine, SIGNAL(clicked()), this, SLOT(buttonStopEngineClicked()));
    connect(mainUi->buttonSaveImage, SIGNAL(clicked()), this, SLOT(buttonSaveImageClicked()));
    connect(mainUi->buttonLoad, SIGNAL(clicked()), this, SLOT(buttonLoadClicked()));
    connect(mainUi->buttonReload, SIGNAL(clicked()), this, SLOT(buttonReloadClicked()));
    connect(mainUi->buttonPrintCamera, SIGNAL(clicked()), this, SLOT(buttonPrintCameraClicked()));

    mainUi->buttonStartEngine->setEnabled(true);
    mainUi->buttonStopEngine->setEnabled(false);
    mainUi->buttonStartEngine->setChecked(false);
    mainUi->buttonStartEngine->setVisible(true);
    mainUi->buttonStopEngine->setVisible(true);
    mainUi->buttonReload->setEnabled(false);
}

void QMainUI::buttonPrintCameraClicked() {
    g_Window->GetInputEngine()->PrintCamera();
}

void QMainUI::buttonLoadClicked() {

    if (g_Window->EngineStarted())
        return;

    QString selfilter = tr("Scene file (*.scene)");
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    QStringList fileNames = dialog.getOpenFileNames(
        g_Window,
        QString("Select project"),
        g_globalLib->GetCurrentPath(),
        selfilter,
        &selfilter);

    if (fileNames.size() == 0) {
        return;
    }
    startLoadFileOperation(fileNames[0]);
    QFileInfo info(fileNames[0]);
    g_globalLib->SetCurrentPath(info.absolutePath());
}

void QMainUI::startLoadFileOperation(const QString& filename) {
    last_file = filename;
    g_Window->GetDisplayEngine()->loadScene(last_file);
    Ui::mainWindow* mainUi = g_Window->getUI();
    mainUi->buttonReload->setEnabled(true);
}

void QMainUI::buttonReloadClicked() {

    if (g_Window->EngineStarted())
        return;

    if (last_file.isEmpty()) {
        return;
    }

    g_Window->GetDisplayEngine()->loadScene(last_file);
}


void QMainUI::buttonStartEngineClicked() {
    Ui::mainWindow* mainUi = g_Window->getUI();
    if (!mainUi->buttonStartEngine->isChecked())
        mainUi->buttonStartEngine->setChecked(true);

    if (g_Window->EngineStarted())
        return;

    g_Window->StartEngine();
    mainUi->buttonLoad->setEnabled(false);
    mainUi->buttonReload->setEnabled(false);
    mainUi->buttonStartEngine->setEnabled(false);
    mainUi->buttonStopEngine->setEnabled(true);
    mainUi->WidgetAO->setEnabled(false);
    mainUi->widgetGeneral->setEnabled(false);
    mainUi->widgetPT->setEnabled(false);
    mainUi->checkBoxInteractive->setEnabled(false);
    mainUi->comboBoxIntegrator->setEnabled(false);
    mainUi->buttonSaveImage->setEnabled(false);

    if (!mainUi->checkBoxInteractive->isChecked()) {
        mainUi->testProgressBar->setValue(0);
        mainUi->testProgressBar->setVisible(true);
    }
}

void QMainUI::stopped() {
    if (g_Window->StoppedEngine()) {
        Ui::mainWindow* mainUi = g_Window->getUI();
        mainUi->buttonStopEngine->setEnabled(false);
        mainUi->buttonStartEngine->setEnabled(true);
        mainUi->WidgetAO->setEnabled(true);
        mainUi->widgetGeneral->setEnabled(true);
        mainUi->widgetPT->setEnabled(true);
        //mainUi->testProgressBar->setVisible(false);
        mainUi->checkBoxInteractive->setEnabled(true);
        mainUi->comboBoxIntegrator->setEnabled(true);
        mainUi->buttonSaveImage->setEnabled(true);
        mainUi->buttonLoad->setEnabled(true);
        mainUi->buttonReload->setEnabled(!last_file.isEmpty());
        m_timer.stop();
    }
}

void QMainUI::buttonStopEngineClicked() {
    Ui::mainWindow* mainUi = g_Window->getUI();
    mainUi->buttonStopEngine->setEnabled(false);
    g_Window->StopEngine();
    m_timer.start(33);
}

void QMainUI::buttonSaveImageClicked() {
    QString selfilter = tr("BMP file (*.bmp);;PNG file (*.png);;JPG file (*.jpg);;HDR file (*.hdr)");
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    QString fileName = QFileDialog::getSaveFileName(
        g_Window,
        QString("Save File"),
        g_globalLib->GetCurrentPath(),
        selfilter);

    if (fileName.length() == 0)
        return;

    QFile file(fileName);
    QFileInfo fileinfo(file);
    g_Window->GetDisplayEngine()->saveFile(fileName);
}

// eof ///////////////////////////////// class GLGraphicsToolbars
