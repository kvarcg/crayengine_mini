/*
File:  LoaderTexture.h

Description: LoaderTexture Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef LOADERTEXTURE_H
#define LOADERTEXTURE_H

// includes ////////////////////////////////////////
#include "CRay/CRay.h"

// defines /////////////////////////////////////////


// forward declarations ////////////////////////////


// class declarations //////////////////////////////

// protected function declarations

#ifdef __cplusplus
extern "C" {
#endif
    CRAY_HANDLE loader_texture_load(const char* filename, const char* basepath);
    const char* loader_tex_get_info_log();
    const char* loader_tex_get_warning_log();
    const char* loader_tex_get_error_log();

#ifdef __cplusplus
};
#endif
#endif //LOADERTEXTURE_H

// eof ///////////////////////////////// class LoaderTexture
