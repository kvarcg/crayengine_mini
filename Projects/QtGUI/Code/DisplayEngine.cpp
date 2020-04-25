/*
File:  DisplayEngine.cpp

Description: DisplayEngine Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global.h"

// includes ////////////////////////////////////////
#include "QtWrapper.h"
#include "DisplayEngine.h"
#include "Scenes/Scenes.h"
#include "Scenes/SceneLoader.h"
#include "CRay/CRay.h"
#include <QImage>
#include <QDir>
#include <QMouseEvent>
#include "QMainUI.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "InputEngine.h"
#include "QInputLabel.h"
#include <QFileInfo>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include "Loader/LoaderObj.h"
#include <functional>
// defines /////////////////////////////////////////

// Constructor
DisplayEngine::DisplayEngine() {
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateImage()));
}

// Destructor
DisplayEngine::~DisplayEngine(void) {
    if (CR_started()) {
        CR_stop();
    }
    while (CR_started()) {
        std::chrono::milliseconds ms(100);
        std::this_thread::sleep_for(ms);
    }
    CR_destroy();
    m_timer.stop();
}

// other functions
void setupUI() {

    std::map<CRAY_INTEGRATOR, std::string> integrators = {
        {CR_INTEGRATOR_PT, "Path Tracing"},
        {CR_INTEGRATOR_AO, "Ambient Occlusion"}
#ifdef CRAY_ENABLE_TESTS
        ,{CR_INTEGRATOR_TEST_COLOR, "Test Colors"},
        {CR_INTEGRATOR_TEST_CAMERA, "Test Camera"},
        {CR_INTEGRATOR_TEST_NOISE, "Test Noise"},
        {CR_INTEGRATOR_TEST_UV, "Test UV"},
        {CR_INTEGRATOR_TEST_NORMAL, "Test Normals"},
        {CR_INTEGRATOR_TEST_MATERIAL, "Test Materials"}
#endif // CRAY_ENABLE_TESTS
    };

    Ui::mainWindow* mainUi = g_Window->getUI();
    for (size_t i = 0; i < integrators.size(); ++i) {
        mainUi->comboBoxIntegrator->addItem(QString());
        CRAY_INTEGRATOR en = static_cast<CRAY_INTEGRATOR>(i);
        auto it = integrators.find(en);
        mainUi->comboBoxIntegrator->setItemText(i, QCoreApplication::translate("mainWindow", it->second.c_str(), nullptr));
    }
    mainUi->comboBoxIntegrator->setCurrentIndex(0);
}

void DisplayEngine::init() {
    setupUI();
    CR_init();
    CR_printTostdout(false);
}

void loaded_scene_info() {
    std::string err = get_scene_error_log();
    if (!err.empty()) {
        g_Window->AddStatusWarningMessage(err.c_str());
    }
    std::string warn = get_scene_warning_log();
    if (!warn.empty()) {
        g_Window->AddStatusWarningMessage(warn.c_str());
    }
    std::string info = get_scene_info_log();
    if (!info.empty()) {
        g_Window->AddStatusMessage(info.c_str());
    }
}

void DisplayEngine::loadScene(const QString& filename) {
    QFileInfo info(filename);
    QString path = info.absolutePath();
    QString name = info.baseName() + info.completeSuffix();
    bool res = load_scene_xml(filename.toStdString().c_str());
    loaded_scene_info();
    if (!res) {
        g_Window->AddStatusMessage(QString("Scene %1 could not be loaded\n").arg(info.baseName()));
        clear_scene_xml();
        return;
    }
    CR_destroy();
    CR_init();
    res = generate_scene_xml();
    loaded_scene_info();
    if (!res) {
        clear_scene_xml();
        g_Window->AddStatusMessage(QString("Scene %1 could not be generated\n").arg(info.baseName()));
        return;
    }
    g_Window->AddStatusMessage(QString("Scene %1 loaded\n").arg(info.baseName()));

    get_camera_args(&m_basic_camera.eye.x, &m_basic_camera.target.x, &m_basic_camera.up.x, &m_basic_camera.aperture, &m_basic_camera.cam_near, &m_basic_camera.cam_far);
    Ui::mainWindow* mainUi = g_Window->getUI();
    mainUi->comboBoxLogLevel->setCurrentIndex(static_cast<int32_t>(CR_getMinimumLogLevel()));
    mainUi->spinBoxResX->setValue(CR_getFilmWidth());
    mainUi->spinBoxResY->setValue(CR_getFilmHeight());
    mainUi->spinBoxBlockSize->setValue(CR_getBlockSize());
    mainUi->spinBoxSPP->setValue(CR_getSamplesPerPixel());
    mainUi->spinBoxThreads->setValue(CR_getNumThreads());
    mainUi->spinBoxGamma->setValue(CR_getGamma());
    mainUi->spinBoxExposure->setValue(CR_getExposure());

    mainUi->spinBoxAOSPP->setValue(CR_getAOSamplesPerPixel());
    mainUi->spinBoxAORange->setValue(CR_getAORange());

    mainUi->spinBoxDepth->setValue(CR_getRayDepth());
    mainUi->checkBoxRussianRoulette->setChecked(CR_getRussianRoulette());

    mainUi->comboBoxIntegrator->setCurrentIndex(static_cast<int>(CR_getIntegrator()));

    clear_scene_xml();

    emit mainUi->buttonStartEngine->clicked();
}

bool DisplayEngine::ready() {
    return CR_finished();
}

bool DisplayEngine::started() {
    return CR_started();
}

void DisplayEngine::start() {
    Ui::mainWindow* mainUi = g_Window->getUI();
    m_timer.start(16);
    CR_setMinimumLogLevel(static_cast<CRAY_LOGGERENTRY>(mainUi->comboBoxLogLevel->currentIndex()));
    CR_setFilmDimensions(mainUi->spinBoxResX->value(), mainUi->spinBoxResY->value());
    CR_setBlockSize(mainUi->spinBoxBlockSize->value());
    CR_setSamplesPerPixel(mainUi->spinBoxSPP->value());
    // we need the GUI thread available
    CR_setNumThreads(mainUi->spinBoxThreads->value() - 1);
    CR_useMainThread(false);
    CR_setGamma(mainUi->spinBoxGamma->value());
    CR_setExposure(mainUi->spinBoxExposure->value());
    CR_enableRGBBuffer(true);
    CR_setLogFile(nullptr);
    CR_printTostdout(false);
    //mainUi->displayWidget->updateGamma(mainUi->spinBoxGamma->value());

    CR_setAOSamplesPerPixel(mainUi->spinBoxAOSPP->value());
    CR_setAORange(mainUi->spinBoxAORange->value());

    CR_setRayDepth(mainUi->spinBoxDepth->value());
    CR_setRussianRoulette(mainUi->checkBoxRussianRoulette->isChecked());

    CR_setIntegrator(static_cast<CRAY_INTEGRATOR>(mainUi->comboBoxIntegrator->currentIndex()));
    CR_setInteractiveRender(mainUi->checkBoxInteractive->isChecked());

    CR_debug_setTestPixelRange(mainUi->spinBoxPixelStartX->value(), mainUi->spinBoxPixelStartY->value(), mainUi->spinBoxPixelEndX->value(), mainUi->spinBoxPixelEndY->value());
    CR_debug_enableTestPixelDebug(mainUi->checkBoxPrintPixel->isChecked());

    g_Window->GetInputEngine()->Init();
    g_Window->GetInputEngine()->m_position = glm::vec3(m_basic_camera.eye.x, m_basic_camera.eye.y, m_basic_camera.eye.z);
    g_Window->GetInputEngine()->m_next_position = glm::vec3(m_basic_camera.eye.x, m_basic_camera.eye.y, m_basic_camera.eye.z);
    g_Window->GetInputEngine()->m_target = glm::vec3(m_basic_camera.target.x, m_basic_camera.target.y, m_basic_camera.target.z);
    g_Window->GetInputEngine()->m_up = glm::vec3(m_basic_camera.up.x, m_basic_camera.up.y, m_basic_camera.up.z);
    g_Window->GetInputEngine()->CalculateUVW();
    if (mainUi->checkBoxInteractive->isChecked()) {
        g_Window->getUI()->displayWidget->setMouseTracking(true);
        g_Window->getUI()->displayWidget->setFocusPolicy(Qt::StrongFocus);
        g_Window->GetInputEngine()->Start();
    }
    // use a custom scene for testing purposes
    //if (mainUi->spinBoxAOSPP->value() > 10) {
    //    buildScene();
    //}
    CR_start();
}

void DisplayEngine::buildScene() {
    // test scenes
    CR_clearScene();
    CR_setAccelerationStructure(CR_AS_BVH);
    //scene_simple_spheres();
    //scene_obj();
    //single_sphere();
    //scene_demo();
    scene_cornell();
    //scene_mis();
}

void DisplayEngine::stop() {
    if (!CR_finished()) {
        CR_stop();
    }
}

void printLogData() {
    size_t str_length = 1024;
    std::unique_ptr<char[]> msg = std::make_unique<char[]>(str_length);
    CRAY_LOGGERENTRY type = CR_LOGGER_NOTHING;
    while (CR_getLastLogMessage(msg.get(), &str_length, &type)) {
        if (str_length > 1024) {
            // if string was bigger than the max, realloc and retrieve the message again
            msg = std::make_unique<char[]>(str_length);
            CR_getLastLogMessage(msg.get(), &str_length, &type);
            msg = std::make_unique<char[]>(str_length);
        }
        switch (type) {
        case CR_LOGGER_INFO:        g_Window->AddStatusMessage(msg.get()); break;
        case CR_LOGGER_ERROR:       g_Window->AddStatusErrorMessage(msg.get()); break;
        case CR_LOGGER_WARNING:     g_Window->AddStatusWarningMessage(msg.get()); break;
        case CR_LOGGER_DEBUG:       g_Window->AddStatusDebugMessage(msg.get()); break;
        default: break;
        }
    }
}

void DisplayEngine::updateImage() {
    Ui::mainWindow* mainUi = g_Window->getUI();

    if (CR_finished()) {
        m_timer.stop();
        g_Window->GetInputEngine()->Stop();
        mainUi->displayWidget->setMouseTracking(false);
        mainUi->displayWidget->setFocusPolicy(Qt::NoFocus);
        g_Window->GetInputEngine()->Destroy();
        emit mainUi->buttonStopEngine->clicked();
    }

    if (!mainUi->checkBoxInteractive->isChecked()) {
        cr_float progress = CR_getProgressPercentage();
        mainUi->testProgressBar->setValue(static_cast<int>(progress));
    }

    uint8_t* rgb = CR_getFilmRGBDataPtr();
    if (rgb != nullptr) {
        int32_t width = CR_getFilmWidth();
        int32_t height = CR_getFilmHeight();
        int32_t num_channels = CR_getFilmChannels();

        QPixmap pixmap(QPixmap::fromImage(QImage(rgb, width, height, width * num_channels, QImage::Format::Format_RGB888)));
        mainUi->displayWidget->setPixmap(pixmap);
        mainUi->displayWidget->setMinimumSize(pixmap.size());

        printLogData();

        if (mainUi->checkBoxInteractive->isChecked() && g_Window->GetInputEngine()->m_should_update && CR_started()) {
            glm::vec3 eye = g_Window->GetInputEngine()->m_position;
            glm::vec3 target = g_Window->GetInputEngine()->m_target;
            glm::vec3 up = g_Window->GetInputEngine()->m_up;
            CR_debug_updateCameraPerspective(VEC3(eye.x, eye.y, eye.z), VEC3(target.x, target.y, target.z), VEC3(up.x, up.y, up.z), m_basic_camera.aperture, m_basic_camera.cam_near, m_basic_camera.cam_far);
        }
        g_Window->GetInputEngine()->m_should_update = false;
    }
}

void DisplayEngine::saveFile(QString& filename) {

    uint8_t* rgb = CR_getFilmRGBDataPtr();
    if (rgb != nullptr) {
        int32_t width = CR_getFilmWidth();
        int32_t height = CR_getFilmHeight();
        int32_t num_channels = CR_getFilmChannels();
        QFileInfo fileInfo(filename);
        std::string ext = fileInfo.completeSuffix().toStdString();  // ext = "tar.gz"
        std::string ext2 = fileInfo.suffix().toStdString();  // ext = "tar.gz"
        std::string base = fileInfo.completeBaseName().toStdString();  // base = "archive.tar"
        std::string base2 = fileInfo.baseName().toStdString();  // base = "archive.tar"
        std::string bundle = fileInfo.bundleName().toStdString();
        std::string path1 = fileInfo.absoluteFilePath().toStdString();
        std::string path2 = fileInfo.absolutePath().toStdString();
        if (fileInfo.completeSuffix() == "hdr") {
            float* float_ptr = CR_getFilmFloatDataPtr();
            stbi_write_hdr(filename.toStdString().c_str(), width, height, num_channels, float_ptr);
        }
        else if (fileInfo.completeSuffix() == "png") {
            stbi_write_png(filename.toStdString().c_str(), width, height, num_channels, rgb, width * num_channels);
        }
        else if (fileInfo.completeSuffix() == "jpg") {
            stbi_write_jpg(filename.toStdString().c_str(), width, height, num_channels, rgb, 95);
        }
        else if (fileInfo.completeSuffix() == "bmp") {
            stbi_write_bmp(filename.toStdString().c_str(), width, height, num_channels, rgb);
        }
        else {
            g_Window->AddStatusMessage(QString("Invalid extension ").append(fileInfo.completeSuffix()).append(". Saving to bmp\n"));
            filename = fileInfo.absolutePath().append("/").append(fileInfo.baseName()).append(".bmp");
            stbi_write_bmp(filename.toStdString().c_str(), width, height, num_channels, rgb);
        }

        g_Window->AddStatusMessage(QString("Saved image to ").append(filename).append("\n"));
    }
    else {
        g_Window->AddStatusErrorMessage(QString("There is no data to save. Operation canceled.\n"));
    }
}

void DisplayEngine::printIntersect() {
    cr_vec3 pos;
    CR_debug_intersectPixel(&pos, g_Window->getUI()->spinBoxPixelStartX->value(), g_Window->getUI()->spinBoxPixelStartY->value());
    printLogData();
}

// eof ///////////////////////////////// class DisplayEngine
