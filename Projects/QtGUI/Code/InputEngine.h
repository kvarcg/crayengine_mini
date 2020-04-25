/*
File:  InputEngine.h

Description: InputEngine Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef INPUTENGINE_H
#define INPUTENGINE_H

// includes ////////////////////////////////////////
#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include "QInputLabel.h"
#define GLM_FORCE_SILENT_WARNINGS // disable some GLM warnings
#include "glm/glm.hpp"

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////


enum XE_INPUT_SPECIAL_KEYS
{
    XISK_CTRL,              ///< Ctrl Key
    XISK_ALT,               ///< Alt Key
    XISK_SHIFT,             ///< Shift Key
    XISK_ALL,               ///< Default (none)
};

enum XE_INPUT_BUTTON
{
    XIB_LEFT_BUTTON,        ///< Left Mouse Button
    XIB_RIGHT_BUTTON,       ///< Right Mouse Button
    XIB_MIDDLE_BUTTON,      ///< Middle Mouse Button (Wheel)
    XIB_ALL                 ///< Default (none)
};

enum XE_INPUT_AXIS
{
    XIA_AXIS_0,             ///< Axis 0 (e.g., Left/Right)
    XIA_AXIS_1,             ///< Axis 1 (e.g., Up/Down)
    XIA_AXIS_2,             ///< Axis 2 (e.g., Front/Back)
    XIA_AXIS_3,             ///< Axis 3
    XIA_AXIS_4              ///< Axis 4
};

struct InputButton
{
    bool value = { false };
    int screen_x = { 0 };
    int screen_y = { 0 };
};

class InputDevice
{
public:
    int             id;
    float           axes[5];
    InputButton     buttons[5];
    bool            special_keys[3];
    int             num_buttons;
    int             num_axes;
    int             num_special_keys;

    InputDevice() :
        id(0), num_buttons(5), num_axes(5), num_special_keys(3) { }
};

class InputEngine: public QObject
{
    Q_OBJECT
protected:
    // protected variable declarations
    QElapsedTimer     m_elapsed_timer;

    // protected function declarations


private:
    // private variable declarations


    // private function declarations
    void                                KeyPressedOperations(void);
    void                                KeyDownOperations(void);


public:
    // Constructor
    InputEngine(void);

    // Destructor
    ~InputEngine(void);

    // public variable declarations
    QTimer                              m_update_timer;
    InputDevice                         m_mouse_keyboard_device;

    QMap<int, bool>                     m_keyMap;
    QMap<int, bool>                     m_keyPressedMap;
    int                                 m_mouse_button;
    int                                 m_prev_x;
    int                                 m_prev_y;
    int                                 m_passive_x;
    int                                 m_passive_y;
    int                                 m_passive_prev_x;
    int                                 m_passive_prev_y;
    bool                                m_initialized;
    bool                                m_should_update;

    glm::vec3                           m_position;
    glm::vec3                           m_next_position;
    glm::vec3                           m_target;
    glm::vec3                           m_direction;
    glm::vec3                           m_right;
    glm::vec3                           m_up;
    float                               m_linear_speed;
    float                               m_angular_speed;
    bool                                m_upright;
    double                              m_prev_time_ms;
    double                              m_delta_time_sec;

    // public function declarations
    void                                Start();
    void                                Stop();
    void                                Init(void);
    void                                UpdateEvents(void);
    void                                Destroy(void);
    void                                KeyboardPressed(QKeyEvent *event);
    void                                KeyboardUp(QKeyEvent *event);
    void                                CheckSpecialKeys(QInputEvent *event);
    void                                CheckSpecialKeys(QMouseEvent *event);

    // Method to handle the mouse motion
    void                                SceneMouse(QMouseEvent *event, bool isDown, bool isDoubleClick);
    void                                SceneMouseMotion(QMouseEvent *event);
    void                                KeyAndMouseReset(void);
    void                                LostFocus(void);
    void                                CalculateUVW(void);


    void                                AdjustPosition(void);
    void                                Move(float mult);
    void                                Updown(float mult);
    void                                Strafe(float mult);
    void                                Turn(float mult);
    void                                Tilt(float mult);

    void                                PrintCamera();

    // get functions


    // set functions

public slots:
    void                                Update(void);

};
#endif //INPUTENGINE_H

// eof ///////////////////////////////// class InputEngine
