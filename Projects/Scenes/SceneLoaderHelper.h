/*
File:  SceneLoaderHelper.h

Description: SceneLoaderHelper Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef SCENELOADERHELPER_H
#define SCENELOADERHELPER_H

#include <cstddef>
#include "glm/glm.hpp"
#include <string>
#include <vector>

#define SCENE_INFO(fmt, ...)                {SCENE_LOADER::ParseMessage(SCENE_LOADER::scene_info, "Info: %s(): " fmt "\n", __func__, ##__VA_ARGS__);}
#define SCENE_WARNING(fmt, ...)             {SCENE_LOADER::ParseMessage(SCENE_LOADER::scene_warn, "Warning: %s(): " fmt "\n", __func__, ##__VA_ARGS__);}
#define SCENE_ERROR(fmt, ...)               {SCENE_LOADER::ParseMessage(SCENE_LOADER::scene_err, "Error: %s(): "   fmt "\n", __func__, ##__VA_ARGS__);}
namespace SCENE_LOADER {
    extern std::string scene_info;
    extern std::string scene_warn;
    extern std::string scene_err;
    void        ParseMessage(std::string& str, const char* fmt, ...);
    bool        ParseVec2(glm::vec2& vec, const char* text);
    bool        ParseVec3(glm::vec3& vec, const char* text);
    bool        ParseIVec2(glm::ivec2& vec, const char* text);
    bool        ParseIVec3(glm::ivec3& vec, const char* text);
    bool        ParseVec4(glm::vec4& vec, const char* text);
    bool        ParseString(std::string& str, const char* text);
    bool        ParseMat4(glm::mat4x4& mat, const char* text);
    bool        ParseDouble(double& val, const char* text);
    bool        ParseFloat(float& val, const char* text);
    bool        ParseShort(short& val, const char* text);
    bool        ParseUShort(unsigned short& val, const char* text);
    bool        ParseInteger(int32_t& val, const char* text);
    bool        ParseUInteger(uint32_t& val, const char* text);
    bool        ParseIVec(std::vector<int32_t>& vec, const std::string& text);
    bool        ParseBoolean(bool& val, const char* text);
    bool        ParseBooleanVector(std::vector<bool>& vec, const std::string& text);
    bool        ParseStringsVector(std::vector<std::string>& vec, const std::string& text);
    bool        CompareStrings(const char* str1, const char* str2, bool case_sensitive = false);
    const char* SkipParameterName(const char* buf);

    enum NODE_VALUES
    {
        // generic node values
        MSG_NODE_NAME,

        // directory values
        MSG_DIRECTORY,
        MSG_FILENAME,

        // world values
        MSG_NUM_THREADS,
        MSG_NUM_THREADS_AUTO,
        MSG_BLOCK_SIZE,
        MSG_LOG_LEVEL,
        MSG_LOG_LEVEL_ERROR,
        MSG_LOG_LEVEL_WARNING,
        MSG_LOG_LEVEL_INFO,
        MSG_LOG_LEVEL_ASSERT,
        MSG_LOG_LEVEL_DEBUG,
        MSG_OUTPUT_NAME,

        // integrator values
        MSG_INTEGRATOR_TYPE,
        MSG_INTEGRATOR_TYPE_AO,
        MSG_INTEGRATOR_TYPE_PT,
        MSG_INTEGRATOR_TYPE_VPT,
        MSG_INTEGRATOR_SPP,
        MSG_INTEGRATOR_AO_SAMPLES,
        MSG_INTEGRATOR_AO_RANGE,
        MSG_INTEGRATOR_PT_DEPTH,
        MSG_INTEGRATOR_PT_RR,

        // film values
        MSG_FILM_DIM,
        MSG_FILM_FILTER,
        MSG_USE_RGB,
        MSG_RGB_GAMMA,
        MSG_RGB_EXPOSURE,

        // camera values
        MSG_CAMERA_TYPE,
        MSG_CAMERA_TYPE_ORTHO,
        MSG_CAMERA_TYPE_PERSPECTIVE,
        MSG_CAMERA_TYPE_THINLENS,
        MSG_CAMERA_APERTURE,
        MSG_CAMERA_ORTHO_HEIGHT,
        MSG_CAMERA_FOCAL_DISTANCE,
        MSG_CAMERA_LENS_RADIUS,
        MSG_CAMERA_NEAR_FIELD,
        MSG_CAMERA_FAR_FIELD,

        // user values
        MSG_USER_POSITION,
        MSG_USER_TARGET,
        MSG_USER_UP,

        // texture values
        MSG_TEXTURE_TYPE,
        MSG_TEXTURE_TYPE_FILE,
        MSG_TEXTURE_TYPE_CHECKER,
        MSG_TEXTURE_CHECKERBOARD_RGB1,
        MSG_TEXTURE_CHECKERBOARD_RGB2,
        MSG_TEXTURE_CHECKERBOARD_FREQUENCY,
        MSG_TEXTURE_MAP,
        MSG_TEXTURE_MAP_UV,
        MSG_TEXTURE_MAP_SPHERICAL,
        MSG_TEXTURE_MAP_PLANAR,
        MSG_TEXTURE_WRAP,
        MSG_TEXTURE_WRAP_CLAMP,
        MSG_TEXTURE_WRAP_REPEAT,
        MSG_TEXTURE_FILTER,
        MSG_TEXTURE_FILTER_BOX,
        MSG_TEXTURE_FILTER_TRIANGLE,

        // material values
        MSG_MATERIAL_SPECULAR_ROUGHNESS,
        MSG_MATERIAL_SPECULAR_REFLECTION,
        MSG_MATERIAL_SPECULAR_TRANSMISSION,
        MSG_MATERIAL_SPECULAR_ETAT,
        MSG_MATERIAL_SPECULAR_CONDUCTOR_ABSORPTION,
        MSG_MATERIAL_PREDEFINED,
        MSG_MATERIAL_DIFFUSE_REFLECTION,
        MSG_MATERIAL_DIFFUSE_TRANSMISSION,
        MSG_MATERIAL_SPECULAR_REFLECTION_TEXTURE,
        MSG_MATERIAL_SPECULAR_TRANSMISSION_TEXTURE,
        MSG_MATERIAL_DIFFUSE_REFLECTION_TEXTURE,
        MSG_MATERIAL_DIFFUSE_TRANSMISSION_TEXTURE,

        MSG_MATERIAL_DIELECTRIC_ACRYLIC_GLASS,
        MSG_MATERIAL_DIELECTRIC_POLYSTYRENE,
        MSG_MATERIAL_DIELECTRIC_POLYCARBONATE,
        MSG_MATERIAL_DIELECTRIC_DIAMOND,
        MSG_MATERIAL_DIELECTRIC_ICE,
        MSG_MATERIAL_DIELECTRIC_SAPPHIRE,
        MSG_MATERIAL_DIELECTRIC_CROWN_GLASS_BK7,
        MSG_MATERIAL_DIELECTRIC_SODA_LIME_GLASS,
        MSG_MATERIAL_DIELECTRIC_WATER_25C,
        MSG_MATERIAL_DIELECTRIC_ACETONE_20C,
        MSG_MATERIAL_DIELECTRIC_AIR,
        MSG_MATERIAL_DIELECTRIC_CARBON_DIOXIDE,
        MSG_MATERIAL_METAL_ALUMINIUM,
        MSG_MATERIAL_METAL_BRASS,
        MSG_MATERIAL_METAL_COPPER,
        MSG_MATERIAL_METAL_GOLD,
        MSG_MATERIAL_METAL_IRON,
        MSG_MATERIAL_METAL_SILVER,

        // geometry values
        MSG_GEOMETRY_MATERIAL,
        MSG_GEOMETRY_DOUBLE_SIDED,
        MSG_GEOMETRY_FLIP_NORMALS,
        MSG_GEOMETRY_SPHERE_CENTER,
        MSG_GEOMETRY_SPHERE_RADIUS,
        MSG_GEOMETRY_RECTANGLE_TYPE,
        MSG_GEOMETRY_RECTANGLE_TYPE_XY,
        MSG_GEOMETRY_RECTANGLE_TYPE_XZ,
        MSG_GEOMETRY_RECTANGLE_TYPE_YZ,
        MSG_GEOMETRY_RECTANGLE_DIM_MIN,
        MSG_GEOMETRY_RECTANGLE_DIM_MAX,

        // transform values
        MSG_TRANSFORM_FULL,
        MSG_TRANSFORM_SCALE,
        MSG_TRANSFORM_ROTATION,
        MSG_TRANSFORM_TRANSLATION,

        // light values
        MSG_LIGHT_SHADOWS,
        MSG_LIGHT_FLUX,
        MSG_LIGHT_GEOMETRY,
        MSG_LIGHT_TYPE,

        MSG_LIGHT_POINT_DIRECTION,
        MSG_LIGHT_POINT_POSITION,
        MSG_LIGHT_POINT_TARGET,
        MSG_LIGHT_SPOTLIGHT_CONE_APERTURE,
        MSG_LIGHT_SPOTLIGHT_FALLOFF,
        MSG_LIGHT_SPOTLIGHT_MAXFALLOFF,
        MSG_LIGHT_SPOTLIGHT_EXPONENT
    };
    const char* GetMessageFromMap(NODE_VALUES n);
}

#endif //SCENELOADERHELPER_H

// eof /////////////////////////////////
