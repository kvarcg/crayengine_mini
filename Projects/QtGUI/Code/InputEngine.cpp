/*
File:  InputEngine.cpp

Description: InputEngine Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global.h"

// includes ////////////////////////////////////////
#include "QtWrapper.h"
#include "InputEngine.h"
#include <QMouseEvent>
#include "glm/gtc/epsilon.hpp"
#include <sstream>
#include <chrono>
#include <iomanip>

#define KEYPRESSED(key)             (m_keyPressedMap[key])
#define KEYDOWN(key)                (m_keyMap[key])

// defines /////////////////////////////////////////

static auto high_perf_timer = std::chrono::high_resolution_clock::now();
struct Timer {
private:
    decltype(high_perf_timer) timer_start;
    decltype(high_perf_timer) timer_end;
    std::chrono::duration<double, std::milli> time_span = { std::chrono::milliseconds{0} };
public:
    void start() {
        timer_start = std::chrono::high_resolution_clock::now();
    }

    void stop() {
        timer_end = std::chrono::high_resolution_clock::now();
        time_span = timer_end - timer_start;
    }

    void restart() {
        stop();
        start();
    }

    double getDurationDouble() const {
        return time_span.count();
    }
} mouse_motion_timer;

// Constructor
// Constructor
InputEngine::InputEngine() :
    m_initialized(false) {
    connect(&m_update_timer, SIGNAL(timeout()), this, SLOT(Update()));
}

// Destructor
InputEngine::~InputEngine() {
    m_update_timer.stop();
}

// other functions
void InputEngine::Start() {
    m_update_timer.start(16);
    mouse_motion_timer.start();
}

void InputEngine::Stop() {
    m_update_timer.stop();
    mouse_motion_timer.stop();
}

void InputEngine::Init() {
    m_mouse_button = -1;
    m_prev_x = 0;
    m_prev_y = 0;
    m_passive_x = 0;
    m_passive_y = 0;
    m_passive_prev_x = 0;
    m_passive_prev_y = 0;

    memset(m_mouse_keyboard_device.special_keys, 0, m_mouse_keyboard_device.num_special_keys * sizeof(bool));
    memset(m_mouse_keyboard_device.axes, 0, m_mouse_keyboard_device.num_axes * sizeof(float));
    memset(m_mouse_keyboard_device.buttons, 0, m_mouse_keyboard_device.num_buttons * sizeof(InputButton));

    m_initialized = false;
    m_should_update = false;

    m_position = glm::vec3(0);
    m_next_position = glm::vec3(0);
    m_target = glm::vec3(0);
    m_direction = glm::vec3(0);
    m_right = glm::vec3(0);
    m_up = glm::vec3(0);
    m_linear_speed = 2.0f;
    m_angular_speed = 0.5f;
    m_upright = false;
    m_prev_time_ms = 0;

    m_elapsed_timer.start();

    m_initialized = true;
}

void InputEngine::Destroy() {
    m_keyMap.clear();
    m_keyPressedMap.clear();
    m_initialized = false;
}

void InputEngine::KeyboardPressed(QKeyEvent* event) {
    if (!m_initialized) return;

    //g_Window->AddStatusDebugMessage("KeyboardPressed\n");
    //g_Window->AddStatusDebugMessage(QString("Key event: %1\n").arg(event->key()));
    m_keyMap[event->key()] = true; // Set the state of the current key to pressed
    m_keyPressedMap[event->key()] = true; // Set the state of the current key to pressed
}

void InputEngine::KeyboardUp(QKeyEvent* event) {
    if (!m_initialized) return;

    //g_Window->AddStatusDebugMessage("KeyboardUp\n");
    m_keyMap[event->key()] = false; // Set the state of the current key to up
    m_keyPressedMap[event->key()] = false; // Set the state of the current key to up
}

void InputEngine::CheckSpecialKeys(QMouseEvent* event) {
    if (!m_initialized) return;

    // check for a combination of user clicks
    Qt::KeyboardModifiers mod_key = event->modifiers();
    m_mouse_keyboard_device.special_keys[XISK_CTRL] = (mod_key & Qt::ControlModifier) ? true : false;
    m_mouse_keyboard_device.special_keys[XISK_ALT] = (mod_key & Qt::AltModifier) ? true : false;
    m_mouse_keyboard_device.special_keys[XISK_SHIFT] = (mod_key & Qt::ShiftModifier) ? true : false;
}

void InputEngine::CheckSpecialKeys(QInputEvent* event) {
    if (!m_initialized) return;

    // check for a combination of user clicks
    Qt::KeyboardModifiers mod_key = event->modifiers();
    m_mouse_keyboard_device.special_keys[XISK_CTRL] = (mod_key & Qt::ControlModifier) ? true : false;
    m_mouse_keyboard_device.special_keys[XISK_ALT] = (mod_key & Qt::AltModifier) ? true : false;
    m_mouse_keyboard_device.special_keys[XISK_SHIFT] = (mod_key & Qt::ShiftModifier) ? true : false;
}

void InputEngine::KeyPressedOperations() {
    if (!m_initialized) return;

    // check for everything else
    if (KEYPRESSED(Qt::Key_M)) {
        m_should_update = true;
    }
}

void InputEngine::KeyDownOperations() {
    if (!m_initialized) return;

    if (KEYDOWN(Qt::Key_Up)) {
        m_mouse_keyboard_device.axes[XIA_AXIS_2] = 1.0f;
    }
    if (KEYDOWN(Qt::Key_Down)) {
        m_mouse_keyboard_device.axes[XIA_AXIS_2] = -1.0f;
    }
    if (KEYDOWN(Qt::Key_Left)) {
        m_mouse_keyboard_device.axes[XIA_AXIS_0] = -1.0f;
    }
    if (KEYDOWN(Qt::Key_Right)) {
        m_mouse_keyboard_device.axes[XIA_AXIS_0] = 1.0f;
    }
    if (KEYDOWN(Qt::Key_PageUp)) {
        m_mouse_keyboard_device.axes[XIA_AXIS_1] = 1.0f;
    }
    if (KEYDOWN(Qt::Key_PageDown)) {
        m_mouse_keyboard_device.axes[XIA_AXIS_1] = -1.0f;
    }
    if (KEYDOWN('w') || KEYDOWN('W')) {
        m_mouse_keyboard_device.axes[XIA_AXIS_2] = 1.0f;
    }
    if (KEYDOWN('s') || KEYDOWN('S')) {
        m_mouse_keyboard_device.axes[XIA_AXIS_2] = -1.0f;
    }
    if (KEYDOWN('a') || KEYDOWN('A')) {
        m_mouse_keyboard_device.axes[XIA_AXIS_0] = -1.0f;
    }
    if (KEYDOWN('d') || KEYDOWN('D')) {
        m_mouse_keyboard_device.axes[XIA_AXIS_0] = 1.0f;
    }
}

void InputEngine::SceneMouse(QMouseEvent* event, bool isDown, bool isDoubleClick) {
    int x = event->pos().x();
    int y = event->pos().y();
    //g_Window->AddStatusDebugMessage(QString("Click: %1, %2\n").arg(x).arg(y));
    if (isDoubleClick) {
        return;
    }

    if (!m_initialized) return;
    //m_engine_wrapper->Redraw();

    m_prev_x = x;
    m_prev_y = y;

    m_mouse_button = event->button();

    //g_Window->AddStatusDebugMessage(QString("Mouse ").append(m_mouse_button).append(isDown?"down\n":"up\n"));

    if (m_mouse_button == Qt::LeftButton) {
        m_mouse_keyboard_device.buttons[XIB_LEFT_BUTTON].value = isDown;
        m_mouse_keyboard_device.buttons[XIB_LEFT_BUTTON].screen_x = x;
        m_mouse_keyboard_device.buttons[XIB_LEFT_BUTTON].screen_y = y;
    }
    else if (m_mouse_button == Qt::RightButton) {
        m_mouse_keyboard_device.buttons[XIB_RIGHT_BUTTON].value = isDown;
        m_mouse_keyboard_device.buttons[XIB_RIGHT_BUTTON].screen_x = x;
        m_mouse_keyboard_device.buttons[XIB_RIGHT_BUTTON].screen_y = y;
    }
    else if (m_mouse_button == Qt::MiddleButton) {
        m_mouse_keyboard_device.buttons[XIB_MIDDLE_BUTTON].value = isDown;
        m_mouse_keyboard_device.buttons[XIB_MIDDLE_BUTTON].screen_x = x;
        m_mouse_keyboard_device.buttons[XIB_MIDDLE_BUTTON].screen_y = y;
    }
}

// Method to handle the mouse motion
double mouse_motion_prev_ms = 0.0;
void InputEngine::SceneMouseMotion(QMouseEvent* event) {
    if (!m_initialized) return;

    //g_Window->AddStatusDebugMessage("Entered Mouse Motion\n");
//mouse_motion_timer.restart();
    double delta_time_ms = mouse_motion_timer.getDurationDouble();

    Ui::mainWindow* mainUi = g_Window->getUI();
    float system_metrics_x = mainUi->displayWidget->size().width();
    float system_metrics_y = mainUi->displayWidget->size().height();

    if (delta_time_ms == 0.0) return;
    int x = event->pos().x();
    int y = event->pos().y();
    long delta_x = x - m_prev_x;
    long delta_y = y - m_prev_y;

    if (m_mouse_button == Qt::LeftButton && m_mouse_keyboard_device.buttons[XIB_LEFT_BUTTON].value == true) {
        float rot_x = (4.0f * (delta_y) / system_metrics_y) / (float)(delta_time_ms * 0.001f);
        float rot_y = (4.0f * (delta_x) / system_metrics_x) / (float)(delta_time_ms * 0.001f);

        m_mouse_keyboard_device.axes[XIA_AXIS_3] = rot_x;
        m_mouse_keyboard_device.axes[XIA_AXIS_4] = rot_y;

        m_mouse_keyboard_device.buttons[XIB_LEFT_BUTTON].screen_x = x;
        m_mouse_keyboard_device.buttons[XIB_LEFT_BUTTON].screen_y = y;

        //std::stringstream ss;
        //ss << delta_time_ms << ", " << delta_x << ", " << delta_y << ", " << rot_x << ", " << rot_y << std::endl;
        //g_Window->AddStatusDebugMessage(ss.str().c_str());
    }
    m_prev_x = x;
    m_prev_y = y;
}

void InputEngine::Update() {
    mouse_motion_timer.restart();
    double cur_time_ms = m_elapsed_timer.elapsed();
    m_delta_time_sec = (cur_time_ms - m_prev_time_ms) * 0.001f;
    //g_Window->AddStatusDebugMessage(QString("Time: ").append(std::to_string(m_delta_time_sec).c_str()).append("\n"));
    m_prev_time_ms = cur_time_ms;
    if (!m_initialized) return;
    KeyPressedOperations();
    KeyDownOperations();
    UpdateEvents();
    KeyAndMouseReset();
}

void InputEngine::KeyAndMouseReset() {
    if (!m_initialized) return;

    // reset the axes
    memset(m_mouse_keyboard_device.axes, 0, m_mouse_keyboard_device.num_axes * sizeof(float));

    // reset keys to not pressed
    QMap<int, bool>::iterator map_iter;
    for (map_iter = m_keyPressedMap.begin(); map_iter != m_keyPressedMap.end(); ++map_iter)
        map_iter.value() = false;
}

void InputEngine::LostFocus() {
    if (!m_initialized) return;

    KeyAndMouseReset();
    // reset keys to not pressed
    QMap<int, bool>::iterator map_iter;
    for (map_iter = m_keyMap.begin(); map_iter != m_keyMap.end(); ++map_iter)
        map_iter.value() = false;
}

void InputEngine::UpdateEvents() {

    // motion
    glm::vec3 move = glm::vec3(0);
    glm::vec2 rot = glm::vec2(0);
    float _epsilon = std::numeric_limits<float>::epsilon();

    move.x = m_mouse_keyboard_device.axes[XIA_AXIS_0];
    move.y = m_mouse_keyboard_device.axes[XIA_AXIS_1];
    move.z = m_mouse_keyboard_device.axes[XIA_AXIS_2];
    rot.x = m_mouse_keyboard_device.axes[XIA_AXIS_3];
    rot.y = m_mouse_keyboard_device.axes[XIA_AXIS_4];

    /*g_Window->AddStatusDebugMessage(QString("Axes: ").append(std::to_string(move.x).c_str()).append(", ").
        append(std::to_string(move.y).c_str()).append(", ").
        append(std::to_string(move.z).c_str()).append(", ").
        append(std::to_string(rot.x).c_str()).append(", ").
        append(std::to_string(rot.y).c_str()).append("\n"));
        */
    glm::bvec3 move_status = glm::epsilonNotEqual(move, glm::vec3(0.0f), _epsilon);
    glm::bvec2 rot_status = glm::epsilonNotEqual(rot, glm::vec2(0.0f), _epsilon);

    if (!glm::any(move_status) && !glm::any(rot_status)) {
        return;
    }

    m_should_update = true;
    if (move_status.x)
        Strafe(move.x);
    if (move_status.y)
        Updown(move.y);
    if (move_status.z)
        Move(move.z);
    if (rot_status.x)
        Tilt(-rot.x);
    if (rot_status.y)
        Turn(rot.y);
    AdjustPosition();
}

void InputEngine::CalculateUVW() {
    m_direction = m_target - m_position;
    m_direction = glm::normalize(m_direction);
    m_right = glm::cross(m_direction, m_up);
    m_right = glm::normalize(m_right);
    m_up = glm::cross(m_right, m_direction);
    m_up = glm::normalize(m_up);
}

void InputEngine::AdjustPosition() {
    glm::vec3 offset = m_next_position - m_position;
    float dist_to_go = glm::length(offset);
    if (dist_to_go < 0.00001)
        return;
    m_position = m_next_position;

    m_target += offset;
    CalculateUVW();
}

void InputEngine::Move(float mult) {
    glm::vec3 offset;
    if (!m_upright)
        offset = m_direction * (float)(mult * m_linear_speed * m_delta_time_sec);
    else
    {
        glm::vec3 planar_dir = m_direction;
        planar_dir.y = 0.0f;
        planar_dir = glm::normalize(planar_dir);
        offset = planar_dir * (float)(mult * m_linear_speed * m_delta_time_sec);
    }

    m_next_position += offset;
}

void InputEngine::Strafe(float mult) {
    glm::vec3 offset = m_right * (float)(mult * m_linear_speed * m_delta_time_sec);
    m_next_position += offset;
}

void InputEngine::Updown(float mult) {
    glm::vec3 offset = m_up * (float)(mult * m_linear_speed * m_delta_time_sec);
    m_next_position += offset;
}

void InputEngine::Turn(float mult) {
    float len = glm::length(m_target - m_position);
    m_direction = m_direction + m_right * (float)(mult * m_angular_speed * m_delta_time_sec);
    m_direction = glm::normalize(m_direction);
    m_right = glm::cross(m_direction, m_up);
    m_right.y = 0;
    m_right = glm::normalize(m_right);
    m_up = glm::cross(m_right, m_direction);
    m_up = glm::normalize(m_up);
    m_target = m_position + m_direction * len;
}

void InputEngine::Tilt(float mult) {
    float len = glm::length(m_target - m_position);
    m_direction = m_direction + m_up * (float)(m_angular_speed * mult * m_delta_time_sec);
    m_direction = glm::normalize(m_direction);
    m_up = glm::cross(m_right, m_direction);
    m_up = glm::normalize(m_up);
    m_right = glm::cross(m_direction, m_up);
    m_right.y = 0;
    m_right = glm::normalize(m_right);
    m_up = glm::cross(m_right, m_direction);
    m_up = glm::normalize(m_up);
    m_target = m_position + m_direction * len;
}

void InputEngine::PrintCamera() {
    glm::vec3 eye = g_Window->GetInputEngine()->m_position;
    glm::vec3 target = g_Window->GetInputEngine()->m_target;
    glm::vec3 up = g_Window->GetInputEngine()->m_up;

    std::stringstream ss;
    ss << std::setprecision(2) << std::fixed
        << "Eye: " << eye.x << ", " << eye.y << ", " << eye.z
        << ", Target: " << target.x << ", " << target.y << ", " << target.z
        << ", Up: " << up.x << ", " << up.y << ", " << up.z
        << std::endl;

    g_Window->AddStatusDebugMessage(QString(ss.str().c_str()));

}

// eof ///////////////////////////////// class DisplayEngine
