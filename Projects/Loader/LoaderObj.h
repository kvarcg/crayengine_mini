/*
File:  LoaderObj.h

Description: LoaderObj Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef LOADEROBJ_H
#define LOADEROBJ_H

// includes ////////////////////////////////////////
#include "CRay/CRay.h"
#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#endif

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

// protected function declarations

#ifdef __cplusplus
extern "C" {
#endif
    bool scene_load_obj(const char* filename, const char* basepath, bool triangulate);
    bool scene_generate_obj(bool double_sided, bool flip_normals, CRAY_HANDLE material_id);
    void scene_clear_obj();
    const char* loader_obj_get_info_log();
    const char* loader_obj_get_warning_log();
    const char* loader_obj_get_error_log();

#ifdef __cplusplus
};
#endif
#endif //LOADEROBJ_H

// eof ///////////////////////////////// class LoaderObj
