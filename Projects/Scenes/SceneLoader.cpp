/*
File:  SceneLoader.cpp

Description: SceneLoader Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global/Global.h"

// includes ////////////////////////////////////////
#include "Scenes/SceneLoaderHelper.h"
#include "Scenes/SceneLoader.h"
#include "Loader/LoaderTexture.h"
#include "Loader/LoaderObj.h"
#include "Loader/LoaderPly.h"
#include "tinyxml/tinyxml2.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <map>
#include <cmath>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#define Stringize( L )     #L
#define MakeString( M, L ) M(L)
#define Line MakeString( Stringize, __LINE__ )
#define PRAGMA_REMINDER __FILE__ "(" Line ") : Reminder: "

#define XMLGETENUMVALUE(enumvalue)  pXMLElement->Attribute(GetMessageFromMap(enumvalue))
#define XMLGETNAME(element) element->Value()

#if defined(_WIN32)
#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif
#else
#endif

// defines /////////////////////////////////////////

namespace SCENE_LOADER {
    static std::string scenePath = "";
    static std::vector<std::string> sceneDirectoryRelativePathList;
    bool FileExists(const std::string& name) {
        return std::ifstream(name.c_str()).good();
    }

    bool GetFile(const std::string& filename, std::string& fileNameWithPath) {
        fileNameWithPath = std::string("");
        for (auto& directory : sceneDirectoryRelativePathList) {
            std::string currentFilePath = scenePath + directory + filename;
            if (FileExists(currentFilePath)) {
                fileNameWithPath = currentFilePath;
                return true;
            }
        }
        return false;
    }

    void validatePathSlashes(std::string& str) {
        std::replace(str.begin(), str.end(), '\\', '/');
    }

    void validatePathEnd(std::string& str) {
        if (str.substr(str.length() - 1).compare("/") != 0)
            str = str.append("/");
    }

    bool getFilePath(const std::string & fileName, std::string & path) {
        std::string token = "";
        size_t start_index_back = fileName.find_last_of('\\');
        size_t start_index_forw = fileName.find_last_of('/');
        if (start_index_back == std::string::npos && start_index_forw == std::string::npos) {
            path = "";
            return false;
        }

        if (start_index_back == std::string::npos) {
            start_index_back = 0;
        }
        if (start_index_forw == std::string::npos) {
            start_index_forw = 0;
        }

        path = fileName.substr(0, start_index_back > start_index_forw ? start_index_back : start_index_forw);
        validatePathSlashes(path);
        validatePathEnd(path);
        return true;
    }

    bool getFileName(const std::string & filePath, std::string & fileName) {
        std::string token = "";
        size_t start_index = filePath.find_last_of('/');
        if (start_index == std::string::npos) {
            start_index = filePath.find_last_of('\\');
            if (start_index == std::string::npos) return false;
        }

        fileName = filePath.substr(start_index + 1, filePath.length() - start_index);
        return true;
    }

    void validateParameterList(const std::vector<std::string> & parameter_list, const tinyxml2::XMLElement* node) {
        for (const tinyxml2::XMLAttribute* attr = node->FirstAttribute(); attr != nullptr; attr = attr->Next()) {
            bool found = false;
            for (auto& str : parameter_list) {
                if (CompareStrings(str.c_str(), attr->Name())) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                SCENE_WARNING("Unused XML parameter %s", attr->Name());
            }
        }
    }

    enum class PRIORITIES : int {
        SETTINGS,
        TEXTURES,
        MATERIALS,
        LIGHTS,
        PRIMITIVES,
        ALL
    };

    struct Node {
        static PRIORITIES current_priority;
        static std::map<std::string, int32_t> unnamed_node_map;
        std::vector<std::string> parameter_list;
        PRIORITIES m_priority = { PRIORITIES::SETTINGS };
        CRAY_HANDLE nm_handle = { CRAY_INVALID_HANDLE };
        std::string name = { "Undefined" };
        Node() = default;
        virtual ~Node() = default;
        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;
        Node(Node&&) = delete;
        Node& operator = (Node&&) = delete;

        void ParseName(const tinyxml2::XMLElement* pXMLElement, const char* alt_name) {
            if (!ParseString(name, XMLGETENUMVALUE(MSG_NODE_NAME))) {
                name = std::string(alt_name) + std::to_string(unnamed_node_map[alt_name]++);
            }

            parameter_list.push_back(GetMessageFromMap(MSG_NODE_NAME));
        }

        virtual void Parse(const tinyxml2::XMLElement*) {}
        virtual void Generate() {}
    };
    PRIORITIES Node::current_priority = { PRIORITIES::SETTINGS };
    std::map<std::string, int32_t> Node::unnamed_node_map;

    std::unique_ptr<Node> BuildNode(const char* name);

    struct GroupNode : public Node {
        std::vector<std::unique_ptr<Node>> children;
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            const tinyxml2::XMLElement* cur = pXMLElement->FirstChildElement();
            while (cur != nullptr) {
                const char* node_name = XMLGETNAME(cur);
                std::unique_ptr<Node> node = BuildNode(node_name);
                if (node) {
                    children.push_back(std::move(node));
                    children.back()->Parse(cur);
                }
                cur = cur->NextSiblingElement();
            }
            Node::Parse(pXMLElement);
        }

        void Generate() override {
            for (auto& node : children) {
                node->Generate();
                if (!scene_err.empty()) {
                    break;
                }
            }
        }
    };

    struct SceneNode : public GroupNode {
        std::map<std::string, CRAY_HANDLE> images;
        std::map<std::string, struct TextureNode*> textures;
        std::map<std::string, struct MaterialNode*> materials;
        std::map<std::string, std::pair<std::string, CRAY_HANDLE>> area_lights;

        void clear() {
            images.clear();
            textures.clear();
            materials.clear();
            area_lights.clear();
            children.clear();
            unnamed_node_map.clear();
        }

        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            clear();
            Node::ParseName(pXMLElement, "RootNode");
            GroupNode::Parse(pXMLElement);
        }

        void Generate() override {
            CR_setAccelerationStructure(CR_AS_BVH);
            while (current_priority < PRIORITIES::ALL) {
                GroupNode::Generate();
                current_priority = static_cast<PRIORITIES>(std::underlying_type<PRIORITIES>::type(current_priority) + 1);
                if (!scene_err.empty()) {
                    break;
                }
            }
            current_priority = PRIORITIES::SETTINGS;

            // check for any remaining light primitive mappings
            for (const auto& iter : area_lights) {
                SCENE_ERROR("Could not find primitive %s for area light source %s", iter.second.first.c_str(), iter.first.c_str());
            }
        }
    };
    static SceneNode scene;

    struct TransformNode : public GroupNode {
        glm::mat4x4 mat = { glm::mat4x4(1) };
        bool has_mat = { false };
        glm::mat4x4 mat_translate = { glm::mat4x4(1) };
        glm::mat4x4 mat_scale = { glm::mat4x4(1) };
        glm::mat4x4 mat_rotate = { glm::mat4x4(1) };
        glm::vec4 rotation = { glm::vec4(0) };
        glm::vec3 translation = { glm::vec3(0) };
        glm::vec3 scale = { glm::vec3(1) };

        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::ParseName(pXMLElement, "TransformNode");
            if (ParseVec4(rotation, XMLGETENUMVALUE(MSG_TRANSFORM_ROTATION))) {
                if (glm::all(glm::lessThanEqual(glm::vec3(rotation.y, rotation.z, rotation.w), glm::vec3(std::numeric_limits<float>::epsilon())))) {
                    SCENE_ERROR("Invalid rotation axis %f %f %f for node %s", rotation.y, rotation.z, rotation.w, this->name.c_str());
                }
                mat_rotate = glm::rotate(glm::radians(rotation.x), glm::vec3(rotation.y, rotation.z, rotation.w));
            }
            if (ParseVec3(translation, XMLGETENUMVALUE(MSG_TRANSFORM_TRANSLATION))) {
                mat_translate = glm::translate(translation);
            }
            if (ParseVec3(scale, XMLGETENUMVALUE(MSG_TRANSFORM_SCALE))) {
                mat_scale = glm::scale(scale);
            }
            has_mat = ParseMat4(mat, XMLGETENUMVALUE(MSG_TRANSFORM_FULL));

            GroupNode::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_TRANSFORM_ROTATION));
            parameter_list.push_back(GetMessageFromMap(MSG_TRANSFORM_TRANSLATION));
            parameter_list.push_back(GetMessageFromMap(MSG_TRANSFORM_SCALE));
            parameter_list.push_back(GetMessageFromMap(MSG_TRANSFORM_FULL));
            validateParameterList(parameter_list, pXMLElement);
        }

        void Generate() override {
            if (!has_mat) {
                glm::mat4x4 glm_mat = mat_translate * mat_scale * mat_rotate;
                CR_pushMatrix(MAT4(glm_mat[0][0], glm_mat[0][1], glm_mat[0][2], glm_mat[0][3], glm_mat[1][0], glm_mat[1][1], glm_mat[1][2], glm_mat[1][3], glm_mat[2][0], glm_mat[2][1], glm_mat[2][2], glm_mat[2][3], glm_mat[3][0], glm_mat[3][1], glm_mat[3][2], glm_mat[3][3]));;
            }
            else {
                CR_pushMatrix(MAT4(mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0], mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1], mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2], mat[3][3]));;
            }
            GroupNode::Generate();
            CR_popMatrix();
        }
    };

    struct CameraNode* stored_camera = nullptr;
    struct CameraNode : public Node {
        std::string type = { "Undefined" };
        glm::vec3 eye = { glm::vec3(0) };
        glm::vec3 target = { glm::vec3(0,0,-1) };
        glm::vec3 up = { glm::vec3(0,1,0) };
        float ortho_height = { 1.0f };
        float aperture = { 45.0f };
        float cam_near = { 1.0f };
        float cam_far = { std::numeric_limits<float>::max() };
        float focal_distance = { 0.0f };
        float lens_radius = { 0.0f };
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::ParseName(pXMLElement, "CameraNode");
            ParseVec3(eye, XMLGETENUMVALUE(MSG_USER_POSITION));
            ParseVec3(target, XMLGETENUMVALUE(MSG_USER_TARGET));
            ParseVec3(up, XMLGETENUMVALUE(MSG_USER_UP));
            ParseString(type, XMLGETENUMVALUE(MSG_CAMERA_TYPE));
            ParseFloat(ortho_height, XMLGETENUMVALUE(MSG_CAMERA_ORTHO_HEIGHT));
            ParseFloat(aperture, XMLGETENUMVALUE(MSG_CAMERA_APERTURE));
            ParseFloat(lens_radius, XMLGETENUMVALUE(MSG_CAMERA_LENS_RADIUS));
            ParseFloat(focal_distance, XMLGETENUMVALUE(MSG_CAMERA_FOCAL_DISTANCE));
            ParseFloat(cam_near, XMLGETENUMVALUE(MSG_CAMERA_NEAR_FIELD));
            ParseFloat(cam_far, XMLGETENUMVALUE(MSG_CAMERA_FAR_FIELD));
            Node::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_USER_POSITION));
            parameter_list.push_back(GetMessageFromMap(MSG_USER_TARGET));
            parameter_list.push_back(GetMessageFromMap(MSG_USER_UP));
            parameter_list.push_back(GetMessageFromMap(MSG_CAMERA_TYPE));
            parameter_list.push_back(GetMessageFromMap(MSG_CAMERA_ORTHO_HEIGHT));
            parameter_list.push_back(GetMessageFromMap(MSG_CAMERA_APERTURE));
            parameter_list.push_back(GetMessageFromMap(MSG_CAMERA_LENS_RADIUS));
            parameter_list.push_back(GetMessageFromMap(MSG_CAMERA_FOCAL_DISTANCE));
            parameter_list.push_back(GetMessageFromMap(MSG_CAMERA_NEAR_FIELD));
            parameter_list.push_back(GetMessageFromMap(MSG_CAMERA_FAR_FIELD));
            validateParameterList(parameter_list, pXMLElement);
        }

        void Generate() override {
            if (m_priority != current_priority) {
                return;
            }
            stored_camera = this;
            if (CompareStrings(type.c_str(), GetMessageFromMap(MSG_CAMERA_TYPE_ORTHO))) {
                CR_addCameraOrthographic(VEC3_ARRAY(eye), VEC3_ARRAY(target), VEC3_ARRAY(up), ortho_height, cam_near, cam_far);
            } else if (CompareStrings(type.c_str(), GetMessageFromMap(MSG_CAMERA_TYPE_PERSPECTIVE))) {
                CR_addCameraPerspective(VEC3_ARRAY(eye), VEC3_ARRAY(target), VEC3_ARRAY(up), aperture, cam_near, cam_far);
            } else if (CompareStrings(type.c_str(), GetMessageFromMap(MSG_CAMERA_TYPE_THINLENS))) {
                CR_addCameraThinLens(VEC3_ARRAY(eye), VEC3_ARRAY(target), VEC3_ARRAY(up), aperture, cam_far, lens_radius, focal_distance);
            } else {
                SCENE_ERROR("Unknown camera type %s.", type.c_str());
            }
            Node::Generate();
        }
    };

    struct TextureNode : public Node {
        TextureNode() : Node() { m_priority = PRIORITIES::TEXTURES; }
        std::string filename = { "Undefined" };
        bool has_filename = false;

        std::string type = { "Undefined" };
        glm::vec3 checker1 = { glm::vec3(0) };
        glm::vec3 checker2 = { glm::vec3(0) };
        float checker_frequency = { 0.0f };
        CRAY_TEXTURE_MAPPING map = { CTM_UV };
        CRAY_TEXTURE_FILTERING filter = { CTF_BOX };
        CRAY_TEXTURE_WRAPPING wrap = { CTW_CLAMP };

        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::ParseName(pXMLElement, "TextureNode");

            if (ParseString(type, XMLGETENUMVALUE(MSG_TEXTURE_TYPE))) {
                if (CompareStrings(type.c_str(), GetMessageFromMap(MSG_TEXTURE_TYPE_FILE))) {
                    has_filename = ParseString(filename, XMLGETENUMVALUE(MSG_FILENAME));
                    if (!has_filename) {
                        SCENE_ERROR("No texture filename provided");
                    }
                }
                else if (CompareStrings(type.c_str(), GetMessageFromMap(MSG_TEXTURE_TYPE_CHECKER))) {
                    ParseVec3(checker1, XMLGETENUMVALUE(MSG_TEXTURE_CHECKERBOARD_RGB1));
                    ParseVec3(checker2, XMLGETENUMVALUE(MSG_TEXTURE_CHECKERBOARD_RGB2));
                    ParseFloat(checker_frequency, XMLGETENUMVALUE(MSG_TEXTURE_CHECKERBOARD_FREQUENCY));
                }
                else {
                    SCENE_ERROR("Unknown texture type %s", type.c_str());
                }
            }
            std::string type_str;
            if (ParseString(type_str, XMLGETENUMVALUE(MSG_TEXTURE_MAP))) {
                if (CompareStrings(type_str.c_str(), GetMessageFromMap(MSG_TEXTURE_MAP_UV))) { map = CTM_UV; }
                else if (CompareStrings(type_str.c_str(), GetMessageFromMap(MSG_TEXTURE_MAP_PLANAR))) { map = CTM_PLANAR; }
                else if (CompareStrings(type_str.c_str(), GetMessageFromMap(MSG_TEXTURE_MAP_SPHERICAL))) { map = CTM_SPHERICAL; }
                else {
                    SCENE_ERROR("Unknown texture map type %s", type_str.c_str());
                }
            }
            if (ParseString(type_str, XMLGETENUMVALUE(MSG_TEXTURE_WRAP))) {
                if (CompareStrings(type_str.c_str(), GetMessageFromMap(MSG_TEXTURE_WRAP_CLAMP))) { wrap = CTW_CLAMP; }
                else if (CompareStrings(type_str.c_str(), GetMessageFromMap(MSG_TEXTURE_WRAP_REPEAT))) { wrap = CTW_REPEAT; }
                else {
                    SCENE_ERROR("Unknown texture wrap type %s", type_str.c_str());
                }
            }
            if (ParseString(type_str, XMLGETENUMVALUE(MSG_TEXTURE_FILTER))) {
                if (CompareStrings(type_str.c_str(), GetMessageFromMap(MSG_TEXTURE_FILTER_BOX))) { filter = CTF_BOX; }
                else if (CompareStrings(type_str.c_str(), GetMessageFromMap(MSG_TEXTURE_FILTER_TRIANGLE))) { filter = CTF_TRIANGLE; }
                else {
                    SCENE_ERROR("Unknown texture wrap type %s", type_str.c_str());
                }
            }
            Node::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_FILENAME));
            parameter_list.push_back(GetMessageFromMap(MSG_TEXTURE_TYPE));
            parameter_list.push_back(GetMessageFromMap(MSG_TEXTURE_CHECKERBOARD_RGB1));
            parameter_list.push_back(GetMessageFromMap(MSG_TEXTURE_CHECKERBOARD_RGB2));
            parameter_list.push_back(GetMessageFromMap(MSG_TEXTURE_CHECKERBOARD_FREQUENCY));
            parameter_list.push_back(GetMessageFromMap(MSG_TEXTURE_MAP));
            parameter_list.push_back(GetMessageFromMap(MSG_TEXTURE_WRAP));
            parameter_list.push_back(GetMessageFromMap(MSG_TEXTURE_FILTER));
            validateParameterList(parameter_list, pXMLElement);
        }

        void Generate() override {
            if (m_priority != current_priority) {
                return;
            }
            CRAY_HANDLE image_id = CRAY_INVALID_HANDLE;
            if (CompareStrings(type.c_str(), GetMessageFromMap(MSG_TEXTURE_TYPE_FILE))) {
                auto iter = scene.images.find(filename);
                if (iter != scene.images.end()) {
                    image_id = iter->second;
                } else {
                    std::string fullFilename = "";
                    std::string filePath = "";
                    if (!GetFile(filename, fullFilename) || !getFilePath(fullFilename, filePath)) {
                        SCENE_ERROR("Texture %s could not be found in the path list. Maybe a <directory> element is missing?", filename.c_str())
                    }
                    else {
                        image_id = loader_texture_load(filename.c_str(), filePath.c_str());
                        std::string info = loader_tex_get_info_log();
                        std::string warn = loader_tex_get_warning_log();
                        std::string err = loader_tex_get_error_log();
                        if (!info.empty()) {
                            info = info.substr(0, info.size() - 1);
                            SCENE_INFO("%s", info.c_str());
                        }
                        if (!warn.empty()) {
                            warn = warn.substr(0, warn.size() - 1);
                            SCENE_WARNING("%s", warn.c_str());
                        }
                        if (!err.empty()) {
                            err = err.substr(0, err.size() - 1);
                            SCENE_ERROR("%s", err.c_str());
                        }
                    }
                }
            }
            else if (CompareStrings(type.c_str(), GetMessageFromMap(MSG_TEXTURE_TYPE_CHECKER))) {
                image_id = CR_addImageCheckerboard(VEC3(checker1.x, checker1.y, checker1.z), VEC3(checker2.x, checker2.y, checker2.z), checker_frequency);
            }

            scene.images[filename] = image_id;
            CRAY_HANDLE sampler_id = CR_addTextureSampler(map, wrap, filter);
            nm_handle = CR_addTexture(image_id, sampler_id);

            if (nm_handle != CRAY_INVALID_HANDLE) {
                scene.textures[this->name] = this;
            }

            Node::Generate();
        }
    };

    struct MaterialNode : public Node {
        MaterialNode() : Node() { m_priority = PRIORITIES::MATERIALS; }
    };

    struct MaterialDielectricNode : public MaterialNode {
        glm::vec3 diffuse_reflection_color = { glm::vec3(0) };
        bool has_diffuse_reflection = { false };
        glm::vec3 diffuse_transmission_color = { glm::vec3(0) };
        bool has_diffuse_transmission = { false };
        glm::vec3 specular_reflection_color = { glm::vec3(0) };
        bool has_specular_reflection = { false };
        glm::vec3 specular_transmission_color = { glm::vec3(0) };
        bool has_specular_transmission = { false };
        std::string diffuse_reflection_texture = { "Undefined" };
        bool has_diffuse_reflection_texture = { false };
        std::string specular_reflection_texture = { "Undefined" };
        bool has_specular_reflection_texture = { false };
        std::string diffuse_transmission_texture = { "Undefined" };
        bool has_diffuse_transmission_texture = { false };
        std::string specular_transmission_texture = { "Undefined" };
        bool has_specular_transmission_texture = { false };
        float roughness = { 0.0f };
        float eta_t = { 0.0f };

        CRAY_IOR_DIELECTRICS dielectric = CR_DIELECTRIC_ALL_DIELECTRICS;
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::ParseName(pXMLElement, "MaterialDielectricNode");
            has_diffuse_reflection_texture = ParseString(diffuse_reflection_texture, XMLGETENUMVALUE(MSG_MATERIAL_DIFFUSE_REFLECTION_TEXTURE));
            has_specular_reflection_texture = ParseString(specular_reflection_texture, XMLGETENUMVALUE(MSG_MATERIAL_SPECULAR_REFLECTION_TEXTURE));
            has_diffuse_transmission_texture = ParseString(diffuse_reflection_texture, XMLGETENUMVALUE(MSG_MATERIAL_DIFFUSE_TRANSMISSION_TEXTURE));
            has_specular_transmission_texture = ParseString(specular_reflection_texture, XMLGETENUMVALUE(MSG_MATERIAL_SPECULAR_TRANSMISSION_TEXTURE));
            has_specular_reflection = ParseVec3(specular_reflection_color, XMLGETENUMVALUE(MSG_MATERIAL_SPECULAR_REFLECTION));
            has_specular_transmission = ParseVec3(specular_transmission_color, XMLGETENUMVALUE(MSG_MATERIAL_SPECULAR_TRANSMISSION));
            has_diffuse_reflection = ParseVec3(diffuse_reflection_color, XMLGETENUMVALUE(MSG_MATERIAL_DIFFUSE_REFLECTION));
            has_diffuse_transmission = ParseVec3(diffuse_transmission_color, XMLGETENUMVALUE(MSG_MATERIAL_DIFFUSE_TRANSMISSION));
            ParseFloat(eta_t, XMLGETENUMVALUE(MSG_MATERIAL_SPECULAR_ETAT));
            ParseFloat(roughness, XMLGETENUMVALUE(MSG_MATERIAL_SPECULAR_ROUGHNESS));
            std::string predefined_name = { "Undefined" };
            if (ParseString(predefined_name, XMLGETENUMVALUE(MSG_MATERIAL_PREDEFINED))) {
                if (CompareStrings(predefined_name.c_str(),      GetMessageFromMap(MSG_MATERIAL_DIELECTRIC_ACRYLIC_GLASS))) { dielectric = CR_DIELECTRIC_Acrylic_glass; }
                else if (CompareStrings(predefined_name.c_str(), GetMessageFromMap(MSG_MATERIAL_DIELECTRIC_POLYSTYRENE))) { dielectric = CR_DIELECTRIC_Polystyrene; }
                else if (CompareStrings(predefined_name.c_str(), GetMessageFromMap(MSG_MATERIAL_DIELECTRIC_POLYCARBONATE))) { dielectric = CR_DIELECTRIC_Polycarbonate; }
                else if (CompareStrings(predefined_name.c_str(), GetMessageFromMap(MSG_MATERIAL_DIELECTRIC_DIAMOND))) { dielectric = CR_DIELECTRIC_Diamond; }
                else if (CompareStrings(predefined_name.c_str(), GetMessageFromMap(MSG_MATERIAL_DIELECTRIC_ICE))) { dielectric = CR_DIELECTRIC_Ice; }
                else if (CompareStrings(predefined_name.c_str(), GetMessageFromMap(MSG_MATERIAL_DIELECTRIC_SAPPHIRE))) { dielectric = CR_DIELECTRIC_Sapphire; }
                else if (CompareStrings(predefined_name.c_str(), GetMessageFromMap(MSG_MATERIAL_DIELECTRIC_CROWN_GLASS_BK7))) { dielectric = CR_DIELECTRIC_Crown_Glass_bk7; }
                else if (CompareStrings(predefined_name.c_str(), GetMessageFromMap(MSG_MATERIAL_DIELECTRIC_SODA_LIME_GLASS))) { dielectric = CR_DIELECTRIC_Soda_lime_glass; }
                else if (CompareStrings(predefined_name.c_str(), GetMessageFromMap(MSG_MATERIAL_DIELECTRIC_WATER_25C))) { dielectric = CR_DIELECTRIC_Water_25C; }
                else if (CompareStrings(predefined_name.c_str(), GetMessageFromMap(MSG_MATERIAL_DIELECTRIC_ACETONE_20C))) { dielectric = CR_DIELECTRIC_Acetone_20C; }
                else if (CompareStrings(predefined_name.c_str(), GetMessageFromMap(MSG_MATERIAL_DIELECTRIC_AIR))) { dielectric = CR_DIELECTRIC_Air; }
                else if (CompareStrings(predefined_name.c_str(), GetMessageFromMap(MSG_MATERIAL_DIELECTRIC_CARBON_DIOXIDE))) { dielectric = CR_DIELECTRIC_Carbon_dioxide; }
                else { SCENE_ERROR("Undefined dielectric %s", predefined_name.c_str()); }
                eta_t = dielectric != CR_DIELECTRIC_ALL_DIELECTRICS ? CR_getDielectricIOR(dielectric) : eta_t;
            }
            Node::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_SPECULAR_REFLECTION));
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_SPECULAR_TRANSMISSION));
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_SPECULAR_ETAT));
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_SPECULAR_ROUGHNESS));
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_PREDEFINED));
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_DIFFUSE_REFLECTION));
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_DIFFUSE_TRANSMISSION));
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_DIFFUSE_REFLECTION_TEXTURE));
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_SPECULAR_REFLECTION_TEXTURE));
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_DIFFUSE_TRANSMISSION_TEXTURE));
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_SPECULAR_TRANSMISSION_TEXTURE));
            validateParameterList(parameter_list, pXMLElement);
        }

        void Generate() override {
            if (m_priority != current_priority) {
                return;
            }

            if (has_diffuse_reflection && has_specular_reflection) {
                nm_handle = CR_addMaterialDiffuseSpecularReflection(VEC3_ARRAY(diffuse_reflection_color), VEC3_ARRAY(specular_reflection_color), eta_t, roughness);
            }
            else if (has_diffuse_reflection && has_diffuse_transmission) {
                nm_handle = CR_addMaterialDiffuse(VEC3_ARRAY(diffuse_reflection_color), VEC3_ARRAY(diffuse_transmission_color), eta_t);
            }
            else if (has_diffuse_reflection && !has_diffuse_transmission) {
                nm_handle = CR_addMaterialDiffuseReflection(VEC3_ARRAY(diffuse_reflection_color));
            }
            else if (!has_diffuse_reflection && has_diffuse_transmission) {
                nm_handle = CR_addMaterialDiffuseTransmission(VEC3_ARRAY(diffuse_transmission_color));
            }
            else if (has_specular_reflection && has_specular_transmission) {
                nm_handle = CR_addMaterialDielectricSpecular(VEC3_ARRAY(specular_reflection_color), VEC3_ARRAY(specular_transmission_color), eta_t, roughness);
            }
            else if (has_specular_reflection && !has_specular_transmission) {
                nm_handle = CR_addMaterialDielectricSpecularReflection(VEC3_ARRAY(specular_reflection_color), roughness);
            }
            else if (!has_specular_reflection && has_specular_transmission) {
                nm_handle = CR_addMaterialDielectricSpecularTransmission(VEC3_ARRAY(specular_transmission_color), eta_t, roughness);
            }
            else {
                SCENE_ERROR("Invalid properties to generate material %s", name.c_str());
                return;
            }

            auto diffuse_reflection_texture_iter = scene.textures.find(diffuse_reflection_texture);
            if (has_diffuse_reflection_texture){
                if (diffuse_reflection_texture_iter == scene.textures.end()) {
                    SCENE_ERROR("Unknown diffuse reflection texture element %s", diffuse_reflection_texture.c_str());
                } else {
                    CR_addMaterialDiffuseReflectionTexture(nm_handle, diffuse_reflection_texture_iter->second->nm_handle);
                }
            }
            auto specular_reflection_texture_iter = scene.textures.find(specular_reflection_texture);
            if (has_specular_reflection_texture) {
                if (specular_reflection_texture_iter == scene.textures.end()) {
                    SCENE_ERROR("Unknown specular reflection texture element %s", specular_reflection_texture.c_str());
                } else {
                    CR_addMaterialSpecularReflectionTexture(nm_handle, specular_reflection_texture_iter->second->nm_handle);
                }
            }
            auto diffuse_transmission_texture_iter = scene.textures.find(diffuse_transmission_texture);
            if (has_diffuse_transmission_texture) {
                if (diffuse_transmission_texture_iter == scene.textures.end()) {
                    SCENE_ERROR("Unknown diffuse transmission texture element %s", diffuse_transmission_texture.c_str());
                }
                else {
                    CR_addMaterialDiffuseReflectionTexture(nm_handle, diffuse_transmission_texture_iter->second->nm_handle);
                }
            }
            auto specular_transmission_texture_iter = scene.textures.find(specular_transmission_texture);
            if (has_specular_transmission_texture) {
                if (specular_transmission_texture_iter == scene.textures.end()) {
                    SCENE_ERROR("Unknown specular transmission texture element %s", specular_transmission_texture.c_str());
                }
                else {
                    CR_addMaterialSpecularReflectionTexture(nm_handle, specular_transmission_texture_iter->second->nm_handle);
                }
            }

            if (nm_handle != CRAY_INVALID_HANDLE) {
                scene.materials[this->name] = this;
            }
            Node::Generate();
        }
    };

    struct MaterialMetalNode : public MaterialNode {
        bool has_specular_reflection = { false };
        glm::vec3 specular_reflection_color = { glm::vec3(0) };
        float roughness = { 0.0f };
        CRAY_IOR_CONDUCTORS conductor = CR_CONDUCTOR_ALL_CONDUCTORS;
        glm::vec3 eta_t = { glm::vec3(0) };
        glm::vec3 absorption = { glm::vec3(0) };
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::ParseName(pXMLElement, "MaterialMetalNode");
            has_specular_reflection = ParseVec3(specular_reflection_color, XMLGETENUMVALUE(MSG_MATERIAL_SPECULAR_REFLECTION));
            ParseFloat(roughness, XMLGETENUMVALUE(MSG_MATERIAL_SPECULAR_ROUGHNESS));
            ParseVec3(absorption, XMLGETENUMVALUE(MSG_MATERIAL_SPECULAR_CONDUCTOR_ABSORPTION));
            ParseVec3(eta_t, XMLGETENUMVALUE(MSG_MATERIAL_SPECULAR_ETAT));
            std::string predefined_name = { "Undefined" };
            if (ParseString(predefined_name, XMLGETENUMVALUE(MSG_MATERIAL_PREDEFINED))) {
                if (CompareStrings(predefined_name.c_str(),         GetMessageFromMap(MSG_MATERIAL_METAL_ALUMINIUM))) { conductor = CR_CONDUCTOR_Alluminium_Al; }
                else if (CompareStrings(predefined_name.c_str(),    GetMessageFromMap(MSG_MATERIAL_METAL_BRASS) )) { conductor = CR_CONDUCTOR_Brass_CuZn; }
                else if (CompareStrings(predefined_name.c_str(),    GetMessageFromMap(MSG_MATERIAL_METAL_COPPER))) { conductor = CR_CONDUCTOR_Copper_Cu; }
                else if (CompareStrings(predefined_name.c_str(),    GetMessageFromMap(MSG_MATERIAL_METAL_GOLD)  )) { conductor = CR_CONDUCTOR_Gold_Au; }
                else if (CompareStrings(predefined_name.c_str(),    GetMessageFromMap(MSG_MATERIAL_METAL_IRON)  )) { conductor = CR_CONDUCTOR_Iron_Fe; }
                else if (CompareStrings(predefined_name.c_str(),    GetMessageFromMap(MSG_MATERIAL_METAL_SILVER))) { conductor = CR_CONDUCTOR_Silver_Ag; }
                else { SCENE_ERROR("Undefined conductor %s", predefined_name.c_str()); }
                if (conductor != CR_CONDUCTOR_ALL_CONDUCTORS) {
                    cr_vec3 e = CR_getConductorIOR(conductor);
                    cr_vec3 k = CR_getConductorAbsorption(conductor);
                    eta_t = glm::vec3(e.x, e.y, e.z);
                    absorption = glm::vec3(k.x, k.y, k.z);
                }
            }
            Node::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_SPECULAR_REFLECTION));
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_SPECULAR_ETAT));
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_SPECULAR_ROUGHNESS));
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_SPECULAR_CONDUCTOR_ABSORPTION));
            parameter_list.push_back(GetMessageFromMap(MSG_MATERIAL_PREDEFINED));
            validateParameterList(parameter_list, pXMLElement);
        }

        void Generate() override {
            if (m_priority != current_priority) {
                return;
            }
            nm_handle = CR_addMaterialConductor(VEC3_ARRAY(specular_reflection_color), VEC3_ARRAY(absorption), VEC3_ARRAY(eta_t), roughness);
            scene.materials[this->name] = this;
            Node::Generate();
        }
    };

    struct GeometryNode : public Node {
        std::string material = { "Undefined" };
        bool has_material = { false };
        bool flip_normals = { false };
        bool double_sided = { false };
        GeometryNode() : Node() { m_priority = PRIORITIES::PRIMITIVES; }
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::Parse(pXMLElement);
            ParseBoolean(double_sided, XMLGETENUMVALUE(MSG_GEOMETRY_DOUBLE_SIDED));
            ParseBoolean(flip_normals, XMLGETENUMVALUE(MSG_GEOMETRY_FLIP_NORMALS));
            has_material = ParseString(material, XMLGETENUMVALUE(MSG_GEOMETRY_MATERIAL));
            parameter_list.push_back(GetMessageFromMap(MSG_GEOMETRY_DOUBLE_SIDED));
            parameter_list.push_back(GetMessageFromMap(MSG_GEOMETRY_FLIP_NORMALS));
            parameter_list.push_back(GetMessageFromMap(MSG_GEOMETRY_MATERIAL));
        }

        void AttachToAreaLight() {
            for (auto& iter : scene.area_lights) {
                if (SCENE_LOADER::CompareStrings(name.c_str(), iter.second.first.c_str())) {
                    CR_addAreaLightToPrimitive(nm_handle, iter.second.second);
                    scene.area_lights.erase(iter.first);
                    break;
                }
            }
        }
    };

    struct GeometryPolygonNode : public GeometryNode {
        std::string filename = { "Undefined" };
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::ParseName(pXMLElement, "GeometryPolygonNode");
            if (!ParseString(filename, XMLGETENUMVALUE(MSG_FILENAME))) {
                SCENE_ERROR("No geometry filename provided");
            }
            GeometryNode::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_FILENAME));
            validateParameterList(parameter_list, pXMLElement);
        }

        void Generate() override {
            if (m_priority != current_priority) {
                return;
            }

            CRAY_HANDLE material_id = CRAY_INVALID_HANDLE;
            if (has_material) {
                auto iter = scene.materials.find(this->material);
                if (iter != scene.materials.end() && iter->second->nm_handle != CRAY_INVALID_HANDLE) {
                    material_id = iter->second->nm_handle;
                } else {
                    SCENE_ERROR("Could not find material %s for primitive %s", this->material.c_str(), this->name.c_str());
                    return;
                }
            }

            std::string fullFilename = "";
            std::string filePath = "";
            if (!GetFile(filename, fullFilename) || !getFilePath(fullFilename, filePath)) {
                SCENE_ERROR("File %s could not be found in the path list. Maybe a <directory> element is missing?", filename.c_str())
            }
            else {
                std::string info;
                std::string warn;
                std::string err;
                std::string ext = "";
                std::string text_name = "";
                size_t ext_pos = fullFilename.find_last_of(".");
                if (ext_pos != std::string::npos) {
                    ext = fullFilename.substr(ext_pos + 1);
                }
                enum POLYGON_TYPE {
                    PT_OBJ,
                    PT_PLY
                } ptype;
                if (SCENE_LOADER::CompareStrings(ext.c_str(), "obj")) {
                    ptype = PT_OBJ;
                    text_name = "Obj";
                }
                else if (SCENE_LOADER::CompareStrings(ext.c_str(), "ply")) {
                    ptype = PT_PLY;
                    text_name = "Ply";
                }
                else {
                    SCENE_ERROR("File %s not a supported format (obj or ply)", filename.c_str())
                        return;
                }

                if (ptype == PT_OBJ) {
                    if (!scene_load_obj(fullFilename.c_str(), filePath.c_str(), true)) {
                        SCENE_ERROR("Could not load file %s. Maybe a <directory> element is missing?", filename.c_str());
                    }
                    info = loader_obj_get_info_log();
                    warn = loader_obj_get_warning_log();
                    err = loader_obj_get_error_log();
                    if (err.empty()) {
                        scene_generate_obj(double_sided, flip_normals, material_id);
                        info = info + loader_obj_get_info_log();
                        warn = warn + loader_obj_get_warning_log();
                        err = err + loader_obj_get_error_log();
                    }
                }
                else if (ptype == PT_PLY) {
                    if (!scene_load_ply(filename.c_str(), filePath.c_str())) {
                        SCENE_ERROR("Could not load file %s. Maybe a <directory> element is missing?", filename.c_str());
                    }
                    info = loader_ply_get_info_log();
                    warn = loader_ply_get_warning_log();
                    err = loader_ply_get_error_log();
                    if (err.empty()) {

                        CRAY_HANDLE light_id = CRAY_INVALID_HANDLE;
                        for (auto& iter : scene.area_lights) {
                            if (SCENE_LOADER::CompareStrings(name.c_str(), iter.second.first.c_str())) {
                                light_id = iter.second.second;
                                scene.area_lights.erase(iter.first);
                                break;
                            }
                        }
                        scene_generate_ply(double_sided, flip_normals, material_id, light_id);
                        info = info + loader_ply_get_info_log();
                        warn = warn + loader_ply_get_warning_log();
                        err = err + loader_ply_get_error_log();
                    }
                }

                if (!info.empty()) {
                    info = info.substr(0, info.size() - 1);
                    SCENE_INFO("%s Info: %s", text_name.c_str(), info.c_str());
                }
                if (!warn.empty()) {
                    warn = warn.substr(0, warn.size() - 1);
                    SCENE_WARNING("%s Warnings: %s", text_name.c_str(), warn.c_str());
                }
                if (!err.empty()) {
                    err = err.substr(0, err.size() - 1);
                    SCENE_ERROR("%s Errors: %s", text_name.c_str(), err.c_str());
                }
                else {
                    SCENE_INFO("Successfully loaded %s", filename.c_str());
                }
                if (ptype == PT_OBJ) {
                    scene_clear_obj();
                }
                else if (ptype == PT_PLY) {
                    scene_clear_ply();
                }
            }

            Node::Generate();
        }
    };

    struct GeometrySphereNode : public GeometryNode {
        glm::vec3 center = { glm::vec3(0) };
        float radius = { 0.0f };
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::ParseName(pXMLElement, "GeometrySphereNode");
            ParseFloat(radius, XMLGETENUMVALUE(MSG_GEOMETRY_SPHERE_RADIUS));
            ParseVec3(center, XMLGETENUMVALUE(MSG_GEOMETRY_SPHERE_CENTER));
            GeometryNode::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_GEOMETRY_SPHERE_RADIUS));
            parameter_list.push_back(GetMessageFromMap(MSG_GEOMETRY_SPHERE_CENTER));
            validateParameterList(parameter_list, pXMLElement);
        }

        void Generate() override {
            if (m_priority != current_priority) {
                return;
            }
            auto iter = scene.materials.find(this->material);
            if (iter != scene.materials.end() && iter->second->nm_handle != CRAY_INVALID_HANDLE) {
                nm_handle = CR_addPrimitiveSphere(VEC3_ARRAY(center), radius, flip_normals, double_sided, iter->second->nm_handle);
                AttachToAreaLight();
            }
            else {
                SCENE_ERROR("Could not find material %s for primitive %s", this->material.c_str(), this->name.c_str());
            }
            Node::Generate();
        }
    };

    struct GeometryRectangleNode : public GeometryNode {
        enum TYPE {
            XY, YZ, XZ, NONE
        } type = { NONE };
        glm::vec2 rec_min = { glm::vec2(0) };
        glm::vec2 rec_max = { glm::vec2(0) };
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::ParseName(pXMLElement, "GeometryRectangleNode");
            std::string type_str = "Undefined";
            if (ParseString(type_str, XMLGETENUMVALUE(MSG_GEOMETRY_RECTANGLE_TYPE))) {
                if (CompareStrings(type_str.c_str(), GetMessageFromMap(MSG_GEOMETRY_RECTANGLE_TYPE_XZ))) { type = XZ; }
                else if (CompareStrings(type_str.c_str(), GetMessageFromMap(MSG_GEOMETRY_RECTANGLE_TYPE_XY))) { type = XY; }
                else if (CompareStrings(type_str.c_str(), GetMessageFromMap(MSG_GEOMETRY_RECTANGLE_TYPE_YZ))) { type = YZ; }
                else {
                    SCENE_ERROR("Unknown rectangle type %s", this->name.c_str());
                }
            }
            ParseVec2(rec_min, XMLGETENUMVALUE(MSG_GEOMETRY_RECTANGLE_DIM_MIN));
            ParseVec2(rec_max, XMLGETENUMVALUE(MSG_GEOMETRY_RECTANGLE_DIM_MAX));

            GeometryNode::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_GEOMETRY_RECTANGLE_TYPE));
            parameter_list.push_back(GetMessageFromMap(MSG_GEOMETRY_RECTANGLE_DIM_MIN));
            parameter_list.push_back(GetMessageFromMap(MSG_GEOMETRY_RECTANGLE_DIM_MAX));
            validateParameterList(parameter_list, pXMLElement);
        }

        void Generate() override {
            if (m_priority != current_priority) {
                return;
            }
            auto iter = scene.materials.find(this->material);
            if (iter != scene.materials.end() && iter->second->nm_handle != CRAY_INVALID_HANDLE) {
                if (type == XY) {
                    nm_handle = CR_addPrimitiveRectangleXY(rec_min.x, rec_max.x, rec_min.y, rec_max.y, 0.0, flip_normals, double_sided, iter->second->nm_handle);
                    AttachToAreaLight();
                }
                else if (type == XZ) {
                    nm_handle = CR_addPrimitiveRectangleXZ(rec_min.x, rec_max.x, rec_min.y, rec_max.y, 0.0, flip_normals, double_sided, iter->second->nm_handle);
                    AttachToAreaLight();
                }
                else if (type == YZ) {
                    nm_handle = CR_addPrimitiveRectangleYZ(rec_min.x, rec_max.x, rec_min.y, rec_max.y, 0.0, flip_normals, double_sided, iter->second->nm_handle);
                    AttachToAreaLight();
                }
                else {
                    SCENE_ERROR("Unknown rectangle type %s", this->name.c_str());
                    return;
                }
            }
            else {
                SCENE_ERROR("Could not find material %s for primitive %s", this->material.c_str(), this->name.c_str());
                return;
            }

            Node::Generate();
        }
    };

    struct LightNode : public Node {
        LightNode() : Node() { m_priority = PRIORITIES::LIGHTS; }
        bool shadows = { true };
        glm::vec3 flux = { glm::vec3(0) };
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            ParseBoolean(shadows, XMLGETENUMVALUE(MSG_LIGHT_SHADOWS));
            ParseVec3(flux, XMLGETENUMVALUE(MSG_LIGHT_FLUX));
            Node::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_LIGHT_SHADOWS));
            parameter_list.push_back(GetMessageFromMap(MSG_LIGHT_FLUX));
        }
    };

    struct AreaLightNode : public LightNode {
        std::string geom_name = { "Undefined" };
        bool double_sided = { false };
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::ParseName(pXMLElement, "AreaLightNode");
            ParseString(geom_name, XMLGETENUMVALUE(MSG_LIGHT_GEOMETRY));
            ParseBoolean(double_sided, XMLGETENUMVALUE(MSG_GEOMETRY_DOUBLE_SIDED));
            LightNode::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_LIGHT_GEOMETRY));
            parameter_list.push_back(GetMessageFromMap(MSG_GEOMETRY_DOUBLE_SIDED));
            validateParameterList(parameter_list, pXMLElement);
        }

        void Generate() override {
            if (m_priority != current_priority) {
                return;
            }
            nm_handle = CR_addLightAreaDiffuseEmitter(VEC3_ARRAY(flux), double_sided);
            scene.area_lights[this->name] = { geom_name, nm_handle };
            Node::Generate();
        }
    };

    struct OmniLightNode : public LightNode {
        glm::vec3 position = { glm::vec3(0) };
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::ParseName(pXMLElement, "OmniLightNode");
            ParseVec3(position, XMLGETENUMVALUE(MSG_LIGHT_POINT_POSITION));
            LightNode::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_LIGHT_POINT_POSITION));
            validateParameterList(parameter_list, pXMLElement);
        }

        void Generate() override {
            if (m_priority != current_priority) {
                return;
            }
            CR_addLightPointOmni(VEC3_ARRAY(position), VEC3_ARRAY(flux));
            Node::Generate();
        }
    };

    struct DirectionalLightNode : public LightNode {
        glm::vec3 position = { glm::vec3(0) };
        glm::vec3 target = { glm::vec3(0) };
        glm::vec3 direction = { glm::vec3(0) };
        bool has_direction = false;
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::ParseName(pXMLElement, "DirectionalLightNode");
            ParseVec3(position, XMLGETENUMVALUE(MSG_LIGHT_POINT_POSITION));
            ParseVec3(target, XMLGETENUMVALUE(MSG_LIGHT_POINT_TARGET));
            has_direction = ParseVec3(direction, XMLGETENUMVALUE(MSG_LIGHT_POINT_DIRECTION));
            LightNode::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_LIGHT_POINT_POSITION));
            parameter_list.push_back(GetMessageFromMap(MSG_LIGHT_POINT_TARGET));
            parameter_list.push_back(GetMessageFromMap(MSG_LIGHT_POINT_DIRECTION));
            validateParameterList(parameter_list, pXMLElement);
        }

        void Generate() override {
            if (m_priority != current_priority) {
                return;
            }
            if (has_direction) {
                CR_addLightDirectional(VEC3_ARRAY(direction), VEC3_ARRAY(flux));
            }
            else {
                CR_addLightDirectionalFromTo(VEC3_ARRAY(position), VEC3_ARRAY(target), VEC3_ARRAY(flux));
            }
            Node::Generate();
        }
    };

    struct SpotLightNode : public LightNode {
        glm::vec3 position = { glm::vec3(0) };
        glm::vec3 target = { glm::vec3(0,0,1) };
        float falloffAngle = { 90.0f };
        float maxAngle = { 90.0f };
        float exponent = { 1.0f };
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::ParseName(pXMLElement, "SpotLightNode");
            ParseVec3(position, XMLGETENUMVALUE(MSG_LIGHT_POINT_POSITION));
            ParseVec3(target, XMLGETENUMVALUE(MSG_LIGHT_POINT_TARGET));
            ParseFloat(falloffAngle, XMLGETENUMVALUE(MSG_LIGHT_SPOTLIGHT_FALLOFF));
            ParseFloat(maxAngle, XMLGETENUMVALUE(MSG_LIGHT_SPOTLIGHT_MAXFALLOFF));
            ParseFloat(exponent, XMLGETENUMVALUE(MSG_LIGHT_SPOTLIGHT_EXPONENT));
            LightNode::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_LIGHT_POINT_POSITION));
            parameter_list.push_back(GetMessageFromMap(MSG_LIGHT_POINT_TARGET));
            parameter_list.push_back(GetMessageFromMap(MSG_LIGHT_SPOTLIGHT_FALLOFF));
            parameter_list.push_back(GetMessageFromMap(MSG_LIGHT_SPOTLIGHT_MAXFALLOFF));
            parameter_list.push_back(GetMessageFromMap(MSG_LIGHT_SPOTLIGHT_EXPONENT));
            validateParameterList(parameter_list, pXMLElement);
        }

        void Generate() override {
            if (m_priority != current_priority) {
                return;
            }
            CR_addLightPointSpot(VEC3_ARRAY(position), VEC3_ARRAY(target), VEC3_ARRAY(flux), falloffAngle, maxAngle, exponent);
            Node::Generate();
        }
    };

    struct FilmNode : public Node {
        glm::ivec2 dim = { glm::vec2(32) };
        std::string filter = "Undefined";
        float rgb_exposure = { 2.0f };
        float rgb_gamma = { 2.2f };
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::ParseName(pXMLElement, "FilmNode");
            ParseIVec2(dim, XMLGETENUMVALUE(MSG_FILM_DIM));
            ParseString(filter, XMLGETENUMVALUE(MSG_FILM_FILTER));
#pragma message(PRAGMA_REMINDER "Implement Film reconstruction filter")
            ParseFloat(rgb_gamma, XMLGETENUMVALUE(MSG_RGB_GAMMA));
            ParseFloat(rgb_exposure, XMLGETENUMVALUE(MSG_RGB_EXPOSURE));
            Node::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_FILM_DIM));
            parameter_list.push_back(GetMessageFromMap(MSG_FILM_FILTER));
            parameter_list.push_back(GetMessageFromMap(MSG_RGB_GAMMA));
            parameter_list.push_back(GetMessageFromMap(MSG_RGB_EXPOSURE));
            validateParameterList(parameter_list, pXMLElement);
        }

        void Generate() override {
            if (m_priority != current_priority) {
                return;
            }
            CR_setFilmDimensions(dim.x, dim.y);
            CR_setGamma(rgb_gamma);
            CR_setExposure(rgb_exposure);
            Node::Generate();
        }
    };

    struct IntegratorNode : public Node {
        CRAY_INTEGRATOR integrator = { CR_INTEGRATOR_NONE };
        int spp = { 1 };
        int ao_spp = { 1 };
        float ao_range = { 0.0f };
        int pt_ray_depth = { 1 };
        bool pt_russian_roulette = { true };
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::ParseName(pXMLElement, "IntegratorNode");
            std::string integrator_name = "Undefined";
            if (ParseString(integrator_name, XMLGETENUMVALUE(MSG_INTEGRATOR_TYPE))) {
                if (CompareStrings(integrator_name.c_str(), GetMessageFromMap(MSG_INTEGRATOR_TYPE_AO))) {
                    integrator = CR_INTEGRATOR_AO;
                } else if (CompareStrings(integrator_name.c_str(), GetMessageFromMap(MSG_INTEGRATOR_TYPE_PT))) {
                    integrator = CR_INTEGRATOR_PT;
                }
                else {
                    SCENE_ERROR("Unknown integrator %s", integrator_name.c_str());
                }
            }
            else {
                SCENE_ERROR("No integrator specified");
            }
            ParseInteger(spp, XMLGETENUMVALUE(MSG_INTEGRATOR_SPP));
            ParseInteger(ao_spp, XMLGETENUMVALUE(MSG_INTEGRATOR_AO_SAMPLES));
            ParseFloat(ao_range, XMLGETENUMVALUE(MSG_INTEGRATOR_AO_RANGE));
            ParseInteger(pt_ray_depth, XMLGETENUMVALUE(MSG_INTEGRATOR_PT_DEPTH));
            ParseBoolean(pt_russian_roulette, XMLGETENUMVALUE(MSG_INTEGRATOR_PT_RR));
            Node::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_INTEGRATOR_TYPE));
            parameter_list.push_back(GetMessageFromMap(MSG_INTEGRATOR_SPP));
            parameter_list.push_back(GetMessageFromMap(MSG_INTEGRATOR_AO_SAMPLES));
            parameter_list.push_back(GetMessageFromMap(MSG_INTEGRATOR_AO_RANGE));
            parameter_list.push_back(GetMessageFromMap(MSG_INTEGRATOR_PT_DEPTH));
            parameter_list.push_back(GetMessageFromMap(MSG_INTEGRATOR_PT_RR));
            validateParameterList(parameter_list, pXMLElement);
        }

        void Generate() override {
            if (m_priority != current_priority) {
                return;
            }
            CR_setIntegrator(integrator);
            CR_setSamplesPerPixel(spp);
            if (integrator == CR_INTEGRATOR_AO) {
                CR_setAOSamplesPerPixel(ao_spp);
                CR_setAORange(ao_range);
            }
            else if (integrator == CR_INTEGRATOR_PT) {
                CR_setRayDepth(pt_ray_depth);
                CR_setRussianRoulette(pt_russian_roulette);
            }
            Node::Generate();
        }
    };

    struct DirectoryNode : Node {
        std::string directory = { "" };
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            directory = XMLGETENUMVALUE(MSG_DIRECTORY);
            if (directory.empty()) {
                SCENE_ERROR("Empty directory provided.");
            }
            Node::Parse(pXMLElement);
            parameter_list.push_back(GetMessageFromMap(MSG_DIRECTORY));
            validateParameterList(parameter_list, pXMLElement);
        }
        void Generate() override {
            if (m_priority != current_priority) {
                return;
            }

            auto pathListIter = std::find(sceneDirectoryRelativePathList.begin(), sceneDirectoryRelativePathList.end(), directory);
            if (pathListIter == sceneDirectoryRelativePathList.end()) {
                validatePathSlashes(directory);
                validatePathEnd(directory);
                sceneDirectoryRelativePathList.push_back(directory);
            }

            Node::Generate();
        }
    };
    struct SettingsNode : Node {
        int num_threads = { 0 };
        int block_size = { 32 };
        CRAY_LOGGERENTRY log_level = { CR_LOGGER_ERROR };
        std::string output_name = { "result" };
        void Parse(const tinyxml2::XMLElement* pXMLElement) override {
            Node::ParseName(pXMLElement, "SettingsNode");
            const char* threads = XMLGETENUMVALUE(MSG_NUM_THREADS);
            if (CompareStrings(threads, GetMessageFromMap(MSG_NUM_THREADS_AUTO))) {
                num_threads = std::thread::hardware_concurrency();
            }
            else {
                ParseInteger(num_threads, threads);
            }

            std::string log_level_str;
            if (ParseString(log_level_str, XMLGETENUMVALUE(MSG_LOG_LEVEL))) {
                if (CompareStrings(log_level_str.c_str(), GetMessageFromMap(MSG_LOG_LEVEL_ERROR))) {
                    log_level = CR_LOGGER_ERROR;
                }
                else if (CompareStrings(log_level_str.c_str(), GetMessageFromMap(MSG_LOG_LEVEL_WARNING))) {
                    log_level = CR_LOGGER_WARNING;
                }
                else if (CompareStrings(log_level_str.c_str(), GetMessageFromMap(MSG_LOG_LEVEL_INFO))) {
                    log_level = CR_LOGGER_INFO;
                }
                else if (CompareStrings(log_level_str.c_str(), GetMessageFromMap(MSG_LOG_LEVEL_ASSERT))) {
                    log_level = CR_LOGGER_ASSERT;
                }
                else if (CompareStrings(log_level_str.c_str(), GetMessageFromMap(MSG_LOG_LEVEL_DEBUG))) {
                    log_level = CR_LOGGER_DEBUG;
                }
                else {
                    SCENE_ERROR("Wrong log level argumeng %s", log_level_str.c_str());
                }
            }
            ParseInteger(block_size, XMLGETENUMVALUE(MSG_BLOCK_SIZE));
            ParseString(output_name, XMLGETENUMVALUE(MSG_OUTPUT_NAME));
            Node::Parse(pXMLElement);

            parameter_list.push_back(GetMessageFromMap(MSG_NUM_THREADS));
            parameter_list.push_back(GetMessageFromMap(MSG_LOG_LEVEL));
            parameter_list.push_back(GetMessageFromMap(MSG_BLOCK_SIZE));
            parameter_list.push_back(GetMessageFromMap(MSG_OUTPUT_NAME));
            validateParameterList(parameter_list, pXMLElement);
        }

        void Generate() override {
            if (m_priority != current_priority) {
                return;
            }
            CR_setNumThreads(num_threads);
            CR_useMainThread(true);
            CR_setBlockSize(block_size);
            CR_setMinimumLogLevel(log_level);
            CR_setOutputName(output_name.c_str());

            Node::Generate();
        }
    };

    std::unique_ptr<Node> BuildNode(const char* name) {
#define DECLARE_NODE(STRING,TYPE) if (CompareStrings(name,STRING)) {std::unique_ptr<TYPE> t = std::make_unique<TYPE>(); return std::move(t);}
        DECLARE_NODE("integrator", IntegratorNode);
        DECLARE_NODE("directory", DirectoryNode);
        DECLARE_NODE("film", FilmNode);
        DECLARE_NODE("settings", SettingsNode);
        DECLARE_NODE("camera", CameraNode);
        DECLARE_NODE("transform", TransformNode);
        DECLARE_NODE("spotlight", SpotLightNode);
        DECLARE_NODE("omnilight", OmniLightNode);
        DECLARE_NODE("directionallight", DirectionalLightNode);
        DECLARE_NODE("arealight", AreaLightNode);
        DECLARE_NODE("dielectric", MaterialDielectricNode);
        DECLARE_NODE("metal", MaterialMetalNode);
        DECLARE_NODE("texture", TextureNode);
        DECLARE_NODE("sphere", GeometrySphereNode);
        DECLARE_NODE("rectangle", GeometryRectangleNode);
        DECLARE_NODE("polygon_group", GeometryPolygonNode);
        SCENE_ERROR("Unknown node %s", name);
        return nullptr;
    }
}

bool generate_scene_xml() {
    SCENE_LOADER::scene_info.clear();
    SCENE_LOADER::scene_warn.clear();
    SCENE_LOADER::scene_err.clear();
    SCENE_LOADER::scene.Generate();
    return SCENE_LOADER::scene_err.empty();
}

void clear_scene_xml() {
    SCENE_LOADER::stored_camera = nullptr;
    SCENE_LOADER::scenePath.clear();
    SCENE_LOADER::sceneDirectoryRelativePathList.clear();
    SCENE_LOADER::scene_info.clear();
    SCENE_LOADER::scene_warn.clear();
    SCENE_LOADER::scene_err.clear();
    SCENE_LOADER::scene.clear();
}

const char* get_scene_info_log() {
    return SCENE_LOADER::scene_info.c_str();
}

const char* get_scene_warning_log() {
    return SCENE_LOADER::scene_warn.c_str();
}

const char* get_scene_error_log() {
    return SCENE_LOADER::scene_err.c_str();
}

void get_camera_args(cr_float* position, cr_float* target, cr_float* up, cr_float* aperture, cr_float* cam_near, cr_float* cam_far) {
    if (SCENE_LOADER::stored_camera == nullptr) {
        return;
    }
    position[0] = SCENE_LOADER::stored_camera->eye[0];
    position[1] = SCENE_LOADER::stored_camera->eye[1];
    position[2] = SCENE_LOADER::stored_camera->eye[2];
    target[0] = SCENE_LOADER::stored_camera->target[0];
    target[1] = SCENE_LOADER::stored_camera->target[1];
    target[2] = SCENE_LOADER::stored_camera->target[2];
    up[0] = SCENE_LOADER::stored_camera->up[0];
    up[1] = SCENE_LOADER::stored_camera->up[1];
    up[2] = SCENE_LOADER::stored_camera->up[2];
    *aperture = SCENE_LOADER::stored_camera->aperture;
    *cam_near = SCENE_LOADER::stored_camera->cam_near;
    *cam_far = SCENE_LOADER::stored_camera->cam_far;
}

bool load_scene_xml(const char* filename) {
    clear_scene_xml();
    if (filename == nullptr) {
        SCENE_ERROR("%s", "No filename string is provided.")
        return false;
    }

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError xml_error = doc.LoadFile(filename);

    if (xml_error != tinyxml2::XML_SUCCESS) {
        const char* errorstr = doc.ErrorStr();
        SCENE_ERROR("Cannot load xml scene file %s with Message: %s", filename, errorstr);
        return false;
    }

    tinyxml2::XMLElement* xml_root = doc.RootElement();
    if (xml_root == nullptr) {
        SCENE_ERROR("%s", "No valid XML root node found for scene description.")
            doc.Clear();
            return false;
    }

    const char* val = XMLGETNAME(xml_root);
    if (val == nullptr) {
        SCENE_ERROR("%s", "No valid XML root node found for scene description.")
            doc.Clear();
        return false;
    }

    if (strcmp(val, "root") == 0 || strcmp(val, "world") == 0) {
        std::locale::global(std::locale("C"));
        SCENE_LOADER::scene.Parse(xml_root);
        doc.Clear();
    }

    std::string path = "";
    if (SCENE_LOADER::getFilePath(filename, path)) {
        SCENE_LOADER::scenePath = path;
    }
    else {
        SCENE_INFO("Could not extract path from %s. File parsing may not work properly.", filename);
    }

    return SCENE_LOADER::scene_err.empty();
}

// eof ///////////////////////////////// CRayScenes
