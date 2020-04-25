/*
File:  LoaderTexture.cpp

Description: LoaderTexture Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global/Global.h"

// includes ////////////////////////////////////////
#include "LoaderTexture.h"
#include "Scenes/SceneLoaderHelper.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#define IMAGE_INFO(fmt, ...)                {SCENE_LOADER::ParseMessage(LOADER_TEXTURE::info, "%s(): " fmt "\n", __func__, ##__VA_ARGS__);}
#define IMAGE_WARNING(fmt, ...)             {SCENE_LOADER::ParseMessage(LOADER_TEXTURE::warn, "%s(): " fmt "\n", __func__, ##__VA_ARGS__);}
#define IMAGE_ERROR(fmt, ...)               {SCENE_LOADER::ParseMessage(LOADER_TEXTURE::err, "%s(): "   fmt "\n", __func__, ##__VA_ARGS__);}

// defines /////////////////////////////////////////

namespace LOADER_TEXTURE {

    static std::string info;
    static std::string warn;
    static std::string err;

    CRAY_HANDLE load(const char* texture_name, const char* basepath) {
        info = "";
        warn = "";
        err = "";
        CRAY_HANDLE image_id = CRAY_INVALID_HANDLE;
        int32_t width = 0;
        int32_t height = 0;
        int32_t channels = 0;
        std::string path = basepath;
        stbi_uc* ptr = stbi_load(std::string(path + texture_name).c_str(), &width, &height, &channels, 3);
        if (ptr == nullptr) {
            IMAGE_WARNING("Could not load image %s", texture_name);
        }
        else {
            image_id = CR_addImage(ptr, width, height, channels, 1, false);
            if (image_id != CRAY_INVALID_HANDLE) {
                IMAGE_INFO("Loaded image %s", texture_name);
            }
            else {
                IMAGE_WARNING("Could not add image %s", texture_name);
            }
        }
        stbi_image_free(ptr);
        return image_id;
    }
}

CRAY_HANDLE loader_texture_load(const char* texture_name, const char* basepath) {
    return LOADER_TEXTURE::load(texture_name, basepath);
}

const char* loader_tex_get_info_log() {
    return LOADER_TEXTURE::info.c_str();
}

const char* loader_tex_get_warning_log() {
    return LOADER_TEXTURE::warn.c_str();
}

const char* loader_tex_get_error_log() {
    return LOADER_TEXTURE::err.c_str();
}

// eof ///////////////////////////////// class LoaderObj
