/*
File:  SceneLoader.h

Description: SceneLoader Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef SCENELOADER_H
#define SCENELOADER_H

#include "CRay/CRay.h"
#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
    void        get_camera_args(cr_float* position, cr_float* target, cr_float* up, cr_float* aperture, cr_float* near, cr_float* far);
    bool        load_scene_xml(const char* filename);
    bool        generate_scene_xml();
    void        clear_scene_xml();
    const char* get_scene_info_log();
    const char* get_scene_warning_log();
    const char* get_scene_error_log();

#ifdef __cplusplus
};
#endif
#endif //SCENELOADER_H

// eof /////////////////////////////////
