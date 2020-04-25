/*
File:  SceneLoaderHelper.cpp

Description: SceneLoaderHelper Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global/Global.h"

// includes ////////////////////////////////////////
#include "Scenes/SceneLoaderHelper.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <cstdarg>
#include <cctype>
#include <cstring>


// defines /////////////////////////////////////////

namespace SCENE_LOADER {
    std::string scene_info;
    std::string scene_warn;
    std::string scene_err;

    void ParseMessage(std::string& str, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
#ifdef _WIN32
        int len = _vscprintf(fmt, args) + 1;
#else
        va_list argcopy;
        va_copy(argcopy, args);
        int len = vsnprintf(nullptr, 0, fmt, argcopy) + 1;
        va_end(argcopy);
#endif
        if (len <= 1) { va_end(args); return; }
        std::unique_ptr<char[]> text = std::make_unique<char[]>(len);
        vsnprintf(text.get(), len, fmt, args);
        va_end(args);
        str.append(text.get());
    }

    bool ParseVec2(glm::vec2& vec, const char* text) {
        if (!text) {
            return false;
        }

        if (sscanf(text, "%f%*[ ,\t]%f", &(vec.x), &(vec.y)) < 2) {
            SCENE_WARNING("Incorrect parsing of msg: %s", text);
            return false;
        }
        else
            return true;
    }

    bool ParseVec3(glm::vec3& vec, const char* text) {
        if (!text) {
            return false;
        }

        if (sscanf(text, "%f%*[ ,\t]%f%*[ ,\t]%f", &(vec.x), &(vec.y), &(vec.z)) < 3) {
            SCENE_WARNING("Incorrect parsing of msg: %s", text);
            return false;
        }
        else
            return true;
    }

    bool ParseIVec2(glm::ivec2& vec, const char* text) {
        if (!text) {
            return false;
        }

        if (sscanf(text, "%d%*[ ,\t]%d", &(vec.x), &(vec.y)) < 2) {
            SCENE_WARNING("Incorrect parsing of msg: %s", text);
            return false;
        }

        return true;
    }

    bool ParseIVec3(glm::ivec3& vec, const char* text) {
        if (!text) {
            return false;
        }
        if (sscanf(text, "%d%*[ ,\t]%d%*[ ,\t]%d", &(vec.x), &(vec.y), &(vec.z)) < 3) {
            SCENE_WARNING("Incorrect parsing of msg: %s", text);
            return false;
        }

        return true;
    }

    bool ParseVec4(glm::vec4& vec, const char* text) {
        if (!text) {
            return false;
        }
        if (sscanf(text, "%f%*[ ,\t]%f%*[ ,\t]%f%*[ ,\t]%f", &(vec.x), &(vec.y), &(vec.z), &(vec.w)) < 4) {
            SCENE_WARNING("Incorrect parsing of msg: %s", text);
            return false;
        }
        else
            return true;
    }

    bool ParseString(std::string& str, const char* text) {
        if (!text) {
            return false;
        }
        str = text;
        return true;
    }

    bool ParseMat4(glm::mat4x4& mat, const char* text) {
        if (!text) {
            return false;
        }
        if (sscanf(text,
            "%f%*[ ,\t]%f%*[ ,\t]%f%*[ ,\t]%f"
            "%*[ ,\t]%f%*[ ,\t]%f%*[ ,\t]%f%*[ ,\t]%f"
            "%*[ ,\t]%f%*[ ,\t]%f%*[ ,\t]%f%*[ ,\t]%f"
            "%*[ ,\t]%f%*[ ,\t]%f%*[ ,\t]%f%*[ ,\t]%f",
            &mat[0][0], &mat[0][1], &mat[0][2], &mat[0][3],
            &mat[1][0], &mat[1][1], &mat[1][2], &mat[1][3],
            &mat[2][0], &mat[2][1], &mat[2][2], &mat[2][3],
            &mat[3][0], &mat[3][1], &mat[3][2], &mat[3][3]) < 16) {
            SCENE_WARNING("Incorrect parsing of msg: %s", text);
            return false;
        }
        else
            return true;
    }

    bool ParseDouble(double& val, const char* text) {
        if (!text) {
            return false;
        }
        if (sscanf(text, "%lf", &(val)) < 1) {
            SCENE_WARNING("Incorrect parsing of msg: %s", text);
            return false;
        }
        else
            return true;
    }

    bool ParseFloat(float& val, const char* text) {
        if (!text) {
            return false;
        }
        if (sscanf(text, "%f", &(val)) < 1) {
            SCENE_WARNING("Incorrect parsing of msg: %s", text);
            return false;
        }
        else
            return true;
    }

    bool ParseShort(short& val, const char* text) {
        if (!text) {
            return false;
        }
        if (sscanf(text, "%hd", &(val)) < 1) {
            SCENE_WARNING("Incorrect parsing of msg: %s", text);
            return false;
        }
        else
            return true;
    }

    bool ParseUShort(unsigned short& val, const char* text) {
        if (!text) {
            return false;
        }
        if (sscanf(text, "%hd", &(val)) < 1) {
            SCENE_WARNING("Incorrect parsing of msg: %s", text);
            return false;
        }
        else
            return true;
    }

    bool ParseInteger(int32_t& val, const char* text) {
        if (!text) {
            return false;
        }
        if (sscanf(text, "%d", &(val)) < 1) {
            SCENE_WARNING("Incorrect parsing of msg: %s", text);
            return false;
        }
        else
            return true;
    }

    bool ParseUInteger(uint32_t& val, const char* text) {
        if (!text) {
            return false;
        }
        if (sscanf(text, "%u", &(val)) < 1) {
            SCENE_WARNING("Incorrect parsing of msg: %s", text);
            return false;
        }
        else
            return true;
    }

    bool ParseIVec(std::vector <int32_t>& vec, const std::string& text) {
        std::string  temp, str = text;
        int     tempi = 0;
        size_t  pos;

        // does the string have a space a comma or a tab in it?
        // store the position of the delimiter
        while ((pos = str.find_first_of(" ,\t", 0)) != std::string::npos) {
            temp = str.substr(0, pos); // get the token
            std::stringstream ss(temp);
            ss >> tempi;                // check if the token is an integer
            if (ss.fail()) {
                SCENE_WARNING("Incorrect parsing of msg: %s", text.c_str());
                return false;
            }
            vec.push_back(tempi);      // and put it into the array
            str.erase(0, pos + 1);     // erase it from the source
        }

        std::stringstream ss(str);
        ss >> tempi;
        if (ss.fail()) {
            SCENE_WARNING("Incorrect parsing of msg: %s", text.c_str());
            return false;
        }
        vec.push_back(tempi);          // the last token is all alone

        return true;
    }

    bool ParseBoolean(bool& val, const char* text) {
        if (!text) {
            return false;
        }
        if (CompareStrings(text, "on") || CompareStrings(text, "true") || CompareStrings(text, "1") || CompareStrings(text, "yes")) {
            val = true;
        } else if (CompareStrings(text, "off") || CompareStrings(text, "false") || CompareStrings(text, "0") || CompareStrings(text, "no")) {
            val = false;
        }
        return true;
    }

    bool ParseBooleanVector(std::vector <bool> & vec, const std::string & text) {
        std::string  temp, str = text;
        size_t  pos;
        bool res = false;
        // does the string have a space a comma or a tab in it?
        // store the position of the delimiter
        while ((pos = str.find_first_of(" ,\t", 0)) != std::string::npos) {
            if (pos > 0)     // avoid double delimiters
            {
                temp = str.substr(0, pos);
                res = false;
                ParseBoolean(res, temp.c_str());
                vec.push_back(res);    // get the token and put it into the array
            }
            str.erase(0, pos + 1);     // erase it from the source
        }

        res = false;
        ParseBoolean(res, str.c_str());
        vec.push_back(res);            // the last token is all alone

        return true;
    }

    bool ParseStringsVector(std::vector<std::string> & vec, const std::string & text) {
        std::string  temp, str = text;
        size_t  pos;

        // does the string have a space a comma or a tab in it?
        // store the position of the delimiter
        while ((pos = str.find_first_of(" ,\t", 0)) != std::string::npos) {
            if (pos > 0)                                // avoid double delimiters
                vec.push_back(str.substr(0, pos));    // get the token and put it into the array
            str.erase(0, pos + 1);     // erase it from the source
        }

        vec.push_back(str);            // the last token is all alone

        return true;
    }

    const char* SkipParameterName(const char* buf) {
        const char* first = buf, * found;
        size_t next;
        while (first == (char*)'\n' || first == (char*)' ' || first == (char*)'\t') {
            first++;
        }
        next = strcspn(first, " \t\n");
        found = first + next;
        return found;
    }

    bool CompareStrings(const char* str1, const char* str2, bool case_sensitive) {
        if (!str1 || !str2) {
            return false;
        }
        std::string s1 = str1;
        std::string s2 = str2;
        if (case_sensitive) {
            return s1.compare(s2) == 0;
        }
        else {
            std::transform(s1.begin(), s1.end(), s1.begin(), [](unsigned char c) { return std::tolower(c); });
            std::transform(s2.begin(), s2.end(), s2.begin(), [](unsigned char c) { return std::tolower(c); });
            return s1.compare(s2) == 0;
        }
    }

    static std::unordered_map<int32_t, const char*> s_message_map;
    void SceneRegisterNodeValues() {
        // generic node values
        s_message_map[MSG_NODE_NAME] = "name";

        // directory values
        s_message_map[MSG_DIRECTORY] = "path";
        s_message_map[MSG_FILENAME] = "file";

        // world values
        s_message_map[MSG_NUM_THREADS] = "num_threads";
        s_message_map[MSG_NUM_THREADS_AUTO] = "auto";
        s_message_map[MSG_BLOCK_SIZE] = "block_size";
        s_message_map[MSG_LOG_LEVEL] = "log_level";
        s_message_map[MSG_LOG_LEVEL_ERROR] = "error";
        s_message_map[MSG_LOG_LEVEL_WARNING] = "warning";
        s_message_map[MSG_LOG_LEVEL_INFO] = "info";
        s_message_map[MSG_LOG_LEVEL_ASSERT] = "assert";
        s_message_map[MSG_LOG_LEVEL_DEBUG] = "debug";
        s_message_map[MSG_OUTPUT_NAME] = "output_name";

        // integrator values
        s_message_map[MSG_INTEGRATOR_TYPE] = "type";
        s_message_map[MSG_INTEGRATOR_TYPE_AO] = "ambient_occlusion";
        s_message_map[MSG_INTEGRATOR_TYPE_PT] = "path_tracing";
        s_message_map[MSG_INTEGRATOR_TYPE_VPT] = "volume_path_tracing";
        s_message_map[MSG_INTEGRATOR_SPP] = "spp";
        s_message_map[MSG_INTEGRATOR_AO_SAMPLES] = "ao_samples";
        s_message_map[MSG_INTEGRATOR_AO_RANGE] = "ao_range";
        s_message_map[MSG_INTEGRATOR_PT_DEPTH] = "pt_depth";
        s_message_map[MSG_INTEGRATOR_PT_RR] = "pt_russian_roulette";

        // film values
        s_message_map[MSG_FILM_DIM] = "dimensions";
        s_message_map[MSG_FILM_FILTER] = "filter";
        s_message_map[MSG_RGB_GAMMA] = "rgb_gamma";
        s_message_map[MSG_RGB_EXPOSURE] = "rgb_exposure";

        // camera values
        s_message_map[MSG_CAMERA_TYPE] = "type";
        s_message_map[MSG_CAMERA_TYPE_ORTHO] = "orthographic";
        s_message_map[MSG_CAMERA_TYPE_PERSPECTIVE] = "perspective";
        s_message_map[MSG_CAMERA_TYPE_THINLENS] = "thinlens";
        s_message_map[MSG_CAMERA_ORTHO_HEIGHT] = "ortho_height";
        s_message_map[MSG_CAMERA_APERTURE] = "aperture";
        s_message_map[MSG_CAMERA_FOCAL_DISTANCE] = "focal_distance";
        s_message_map[MSG_CAMERA_LENS_RADIUS] = "lens_radius";
        s_message_map[MSG_CAMERA_NEAR_FIELD] = "near";
        s_message_map[MSG_CAMERA_FAR_FIELD] = "far";

        // user values
        s_message_map[MSG_USER_POSITION] = "position";
        s_message_map[MSG_USER_TARGET] = "target";
        s_message_map[MSG_USER_UP] = "up";

        // texture values
        s_message_map[MSG_TEXTURE_TYPE] = "type";
        s_message_map[MSG_TEXTURE_TYPE_FILE] = "file";
        s_message_map[MSG_TEXTURE_TYPE_CHECKER] = "checker";
        s_message_map[MSG_TEXTURE_CHECKERBOARD_RGB1] = "rgb1";
        s_message_map[MSG_TEXTURE_CHECKERBOARD_RGB2] = "rgb2";
        s_message_map[MSG_TEXTURE_CHECKERBOARD_FREQUENCY] = "frequency";
        s_message_map[MSG_TEXTURE_MAP] = "map";
        s_message_map[MSG_TEXTURE_MAP_UV] = "uv";
        s_message_map[MSG_TEXTURE_MAP_SPHERICAL] = "spherical";
        s_message_map[MSG_TEXTURE_MAP_PLANAR] = "planar";
        s_message_map[MSG_TEXTURE_WRAP] = "wrap";
        s_message_map[MSG_TEXTURE_WRAP_CLAMP] = "clamp";
        s_message_map[MSG_TEXTURE_WRAP_REPEAT] = "repeat";
        s_message_map[MSG_TEXTURE_FILTER] = "filter";
        s_message_map[MSG_TEXTURE_FILTER_BOX] = "box";
        s_message_map[MSG_TEXTURE_FILTER_TRIANGLE] = "triangle";

        // material values
        s_message_map[MSG_MATERIAL_SPECULAR_ROUGHNESS] = "roughness";
        s_message_map[MSG_MATERIAL_SPECULAR_REFLECTION] = "specular_reflection";
        s_message_map[MSG_MATERIAL_SPECULAR_TRANSMISSION] = "specular_transmission";
        s_message_map[MSG_MATERIAL_SPECULAR_ETAT] = "eta_t";
        s_message_map[MSG_MATERIAL_SPECULAR_CONDUCTOR_ABSORPTION] = "absorption_k";
        s_message_map[MSG_MATERIAL_PREDEFINED] = "predefined";
        s_message_map[MSG_MATERIAL_DIFFUSE_REFLECTION] = "diffuse_reflection";
        s_message_map[MSG_MATERIAL_DIFFUSE_TRANSMISSION] = "diffuse_transmission";
        s_message_map[MSG_MATERIAL_DIFFUSE_REFLECTION_TEXTURE] = "diffuse_reflection_texture";
        s_message_map[MSG_MATERIAL_SPECULAR_REFLECTION_TEXTURE] = "specular_transmission_texture";
        s_message_map[MSG_MATERIAL_DIFFUSE_TRANSMISSION_TEXTURE] = "diffuse_transmission_texture";
        s_message_map[MSG_MATERIAL_SPECULAR_TRANSMISSION_TEXTURE] = "specular_transmission_texture";
        s_message_map[MSG_MATERIAL_DIELECTRIC_ACRYLIC_GLASS] = "Acrylic_glass";
        s_message_map[MSG_MATERIAL_DIELECTRIC_POLYSTYRENE] = "Polystyrene";
        s_message_map[MSG_MATERIAL_DIELECTRIC_POLYCARBONATE] = "Polycarbonate";
        s_message_map[MSG_MATERIAL_DIELECTRIC_DIAMOND] = "Diamond";
        s_message_map[MSG_MATERIAL_DIELECTRIC_ICE] = "Ice";
        s_message_map[MSG_MATERIAL_DIELECTRIC_SAPPHIRE] = "Sapphire";
        s_message_map[MSG_MATERIAL_DIELECTRIC_CROWN_GLASS_BK7] = "Crown_Glass_bk7";
        s_message_map[MSG_MATERIAL_DIELECTRIC_SODA_LIME_GLASS] = "Soda_lime_glass";
        s_message_map[MSG_MATERIAL_DIELECTRIC_WATER_25C] = "Water_25C";
        s_message_map[MSG_MATERIAL_DIELECTRIC_ACETONE_20C] = "Acetone_20C";
        s_message_map[MSG_MATERIAL_DIELECTRIC_AIR] = "Air";
        s_message_map[MSG_MATERIAL_DIELECTRIC_CARBON_DIOXIDE] = "Carbon_dioxide";
        s_message_map[MSG_MATERIAL_METAL_ALUMINIUM] = "Alluminium";
        s_message_map[MSG_MATERIAL_METAL_BRASS] = "Brass";
        s_message_map[MSG_MATERIAL_METAL_COPPER] = "Copper";
        s_message_map[MSG_MATERIAL_METAL_GOLD] = "Gold";
        s_message_map[MSG_MATERIAL_METAL_IRON] = "Iron";
        s_message_map[MSG_MATERIAL_METAL_SILVER] = "Silver";

        // geometry values
        s_message_map[MSG_GEOMETRY_MATERIAL] = "material";
        s_message_map[MSG_GEOMETRY_DOUBLE_SIDED] = "double_sided";
        s_message_map[MSG_GEOMETRY_FLIP_NORMALS] = "flip_normals";
        s_message_map[MSG_GEOMETRY_SPHERE_CENTER] = "center";
        s_message_map[MSG_GEOMETRY_SPHERE_RADIUS] = "radius";
        s_message_map[MSG_GEOMETRY_RECTANGLE_TYPE] = "type";
        s_message_map[MSG_GEOMETRY_RECTANGLE_TYPE_XY] = "xy";
        s_message_map[MSG_GEOMETRY_RECTANGLE_TYPE_XZ] = "xz";
        s_message_map[MSG_GEOMETRY_RECTANGLE_TYPE_YZ] = "yz";
        s_message_map[MSG_GEOMETRY_RECTANGLE_DIM_MIN] = "min";
        s_message_map[MSG_GEOMETRY_RECTANGLE_DIM_MAX] = "max";

        // transform values
        s_message_map[MSG_TRANSFORM_FULL] = "matrix";
        s_message_map[MSG_TRANSFORM_SCALE] = "scale";
        s_message_map[MSG_TRANSFORM_ROTATION] = "rotation";
        s_message_map[MSG_TRANSFORM_TRANSLATION] = "translation";

        // light values
        s_message_map[MSG_LIGHT_SHADOWS] = "has_shadow";
        s_message_map[MSG_LIGHT_FLUX] = "flux";
        s_message_map[MSG_LIGHT_GEOMETRY] = "primitive";
        s_message_map[MSG_LIGHT_TYPE] = "type";

        s_message_map[MSG_LIGHT_POINT_DIRECTION] = "direction";
        s_message_map[MSG_LIGHT_POINT_POSITION] = "position";
        s_message_map[MSG_LIGHT_POINT_TARGET] = "target";
        s_message_map[MSG_LIGHT_SPOTLIGHT_CONE_APERTURE] = "aperture";
        s_message_map[MSG_LIGHT_SPOTLIGHT_FALLOFF] = "falloff";
        s_message_map[MSG_LIGHT_SPOTLIGHT_MAXFALLOFF] = "maxfalloff";
        s_message_map[MSG_LIGHT_SPOTLIGHT_EXPONENT] = "exponent";
    }

    const char* GetMessageFromMap(NODE_VALUES n) {
        if (s_message_map.empty()) {
            SceneRegisterNodeValues();
        }
        const auto iter = s_message_map.find(n);
        if (iter != s_message_map.end())
            return s_message_map[n];
        else
        {
            SCENE_ERROR("%s", "Error mapping XML message/attribute to enum map")
                return "Undefined Enum";
        }
    }
}

// eof ///////////////////////////////// CRayScenes
