/*
File:  LoaderObj.cpp

Description: LoaderObj Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global/Global.h"

// includes ////////////////////////////////////////
#include "LoaderObj.h"
#include "LoaderTexture.h"
#include "Scenes/SceneLoaderHelper.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader/tiny_obj_loader.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <chrono>

#define PRAGMA_REMINDER __FILE__ ": Reminder: "
#define OBJ_INFO(fmt, ...)                {SCENE_LOADER::ParseMessage(LOADER_OBJ::info, "%s(): " fmt "\n", __func__, ##__VA_ARGS__);}
#define OBJ_WARNING(fmt, ...)             {SCENE_LOADER::ParseMessage(LOADER_OBJ::warn, "%s(): " fmt "\n", __func__, ##__VA_ARGS__);}
#define OBJ_ERROR(fmt, ...)               {SCENE_LOADER::ParseMessage(LOADER_OBJ::err, "%s(): "   fmt "\n", __func__, ##__VA_ARGS__);}

// defines /////////////////////////////////////////

namespace LOADER_OBJ {
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

        double getDurationDouble() const {
            return time_span.count();
        }

        uint64_t getDurationUint() const {
            return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(timer_end - timer_start).count());
        }
    };

    static std::string basePath;
    static std::string info;
    static std::string warn;
    static std::string err;
    static tinyobj::attrib_t attrib;
    static std::vector<tinyobj::shape_t> shapes;
    static std::vector<tinyobj::material_t> materials;

    cr_float minV(cr_float x, cr_float minVal) {
        return x < minVal ? x : minVal;
    }

    cr_float maxV(cr_float x, cr_float maxVal) {
        return x > maxVal ? x : maxVal;
    }

    cr_float clamp(cr_float x, cr_float minVal, cr_float maxVal) {
        return minV(maxV(x, minVal), maxVal);
    }

    cr_float mix(cr_float start, cr_float end, cr_float a) {
        return start * (1.0f - a) + end * a;
    }

    bool isEqual(cr_float val, cr_float val2) {
        return fabs(val - val2) < std::numeric_limits<cr_float>::epsilon();
    }

    bool isEqual(const cr_vec3 & val, const cr_vec3 & val2) {
        return isEqual(val.x, val2.x) && isEqual(val.y, val2.y) && isEqual(val.z, val2.z);
    }

    bool loadImage(std::map<std::string, CRAY_HANDLE>& image_maps, const std::string& image_name) {
        CRAY_HANDLE image_id = CRAY_INVALID_HANDLE;
        if (image_name.empty()) {
            return false;
        }
        auto iter = image_maps.find(image_name);
        if (iter == image_maps.end()) {
            image_id = loader_texture_load(image_name.c_str(), basePath.c_str());
            info += loader_tex_get_info_log();
            warn += loader_tex_get_warning_log();
            err += loader_tex_get_error_log();
            if (image_id == CRAY_INVALID_HANDLE) {
                return false;
            }
            image_maps[image_name] = image_id;
        }
        return true;
    }

    void uploadScene(bool double_sided, bool flip_normals, CRAY_HANDLE material_id) {
        struct NMMaterial {
            CRAY_HANDLE mat_id = { CRAY_INVALID_HANDLE };
            CRAY_HANDLE light_id = { CRAY_INVALID_HANDLE };
        };
        bool has_material = material_id != CRAY_INVALID_HANDLE;
        std::map<size_t, NMMaterial> material_maps;
        // add material data
        if (has_material) {
            NMMaterial nmmat;
            nmmat.mat_id = material_id;
            nmmat.light_id = CRAY_INVALID_HANDLE;
            material_maps[0] = nmmat;
        } else {
            std::map<std::string, CRAY_HANDLE> image_maps;
            CRAY_HANDLE default_sampler_id = CR_addTextureSampler(CTM_UV, CTW_REPEAT, CTF_TRIANGLE);
            CRAY_HANDLE null_material_id = CR_addMaterialDiffuseReflection(VEC3_XYZ(0.0));
            CR_reserveMaterials(CR_getNumMaterials() + materials.size() + 1);
            for (size_t i = 0; i < materials.size(); i++) {
                const tinyobj::material_t& material = materials[i];
                cr_vec3 diffuse = VEC3_ARRAY(material.diffuse);
                cr_vec3 specular = VEC3_ARRAY(material.specular);
                cr_vec3 transmittance = VEC3_ARRAY(material.transmittance);
                bool has_diffuse = !isEqual(diffuse, VEC3_XYZ(0.0));
                bool has_specular = !isEqual(specular, VEC3_XYZ(0.0));
                bool is_metal = material.metallic > 0;
                cr_float phong_shininess = clamp(1.0 - material.shininess / 128.0, 0.0, 1.0);
                phong_shininess = mix(0.0, 0.5, phong_shininess);
                cr_float roughness = isEqual(material.roughness, 0.0) ? phong_shininess : material.roughness;
                cr_vec3 eta_t = VEC3_XYZ(material.ior);
                cr_vec3 absoprtion_k = VEC3_XYZ(0);
                bool is_transmissive = !isEqual(transmittance, VEC3_XYZ(1.0));
                if (is_metal) {
                    if (material.ior == 1.0) {
                        eta_t = CR_getConductorIOR(CR_CONDUCTOR_Alluminium_Al);
                        absoprtion_k = CR_getConductorAbsorption(CR_CONDUCTOR_Alluminium_Al);
                    }
                    else {
                        eta_t = VEC3_XYZ(material.ior);
                        absoprtion_k = VEC3_XYZ(1.0);
                    }
                }
                else if (material.ior == 1.0) {
                    eta_t.x = CR_getDielectricIOR(CR_DIELECTRIC_Polystyrene);
                }

                CRAY_HANDLE mat_id = 0u;
                cr_vec3 emission = VEC3_ARRAY(material.emission);
                bool has_emission = !isEqual(emission, VEC3_XYZ(0.0));
                if (has_emission) {
                    mat_id = null_material_id;
                }
                else {
                    if (is_metal) {
                        mat_id = CR_addMaterialConductor(specular, absoprtion_k, eta_t, roughness);
                    }
                    else if (has_diffuse && has_specular) {
                        mat_id = CR_addMaterialDiffuseSpecularReflection(diffuse, specular, eta_t.x, roughness);
                    }
                    else if (has_diffuse && !has_specular) {
                        if (!is_transmissive) {
                            mat_id = CR_addMaterialDiffuseReflection(diffuse);
                        }
                        else {
                            mat_id = CR_addMaterialDiffuse(diffuse, transmittance, eta_t.x);
                        }
                    }
                    else if (!has_diffuse && has_specular) {
                        if (!is_transmissive) {
                            mat_id = CR_addMaterialDielectricSpecularReflection(specular, roughness);
                        }
                        else {
                            mat_id = CR_addMaterialDielectricSpecular(specular, transmittance, eta_t.x, roughness);
                        }
                    }
                    else {
                        OBJ_INFO("Could not generate material for %s. Using default diffuse 0.5", material.name.c_str());
                    }

                    if (has_diffuse && loadImage(image_maps, material.diffuse_texname) != CRAY_INVALID_HANDLE) {
                        CRAY_HANDLE image_id = image_maps[material.diffuse_texname];
                        CRAY_HANDLE texture_id = CR_addTexture(image_id, default_sampler_id);
                        CR_addMaterialDiffuseReflectionTexture(mat_id, texture_id);
                        if (is_transmissive) {
                            CR_addMaterialDiffuseTransmissionTexture(mat_id, texture_id);
                        }
                    }
                    if (has_specular && loadImage(image_maps, material.specular_texname) != CRAY_INVALID_HANDLE) {
                        CRAY_HANDLE image_id = image_maps[material.specular_texname];
                        CRAY_HANDLE texture_id = CR_addTexture(image_id, default_sampler_id);
                        CR_addMaterialSpecularReflectionTexture(mat_id, texture_id);
                        if (is_transmissive) {
                            CR_addMaterialSpecularTransmissionTexture(mat_id, texture_id);
                        }
                    }
                }
                NMMaterial nmmat;
                nmmat.mat_id = mat_id;
                nmmat.light_id = CRAY_INVALID_HANDLE;
                material_maps[i] = nmmat;
            }
        }

        // add face data
        size_t total_primitives = 0;
        for (const auto& shape : shapes) {
            total_primitives += shape.mesh.num_face_vertices.size();
        }
        CR_reservePrimitives(CR_getNumPrimitives() + total_primitives);
        for (const auto& shape : shapes) {
            assert(shape.mesh.num_face_vertices.size() == shape.mesh.material_ids.size());
            assert(shape.mesh.num_face_vertices.size() == shape.mesh.smoothing_group_ids.size());
            size_t index_offset = 0;

#pragma message(PRAGMA_REMINDER "Add OBJ indices support")
            // For each face
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                size_t fnum = shape.mesh.num_face_vertices[f];

                assert(fnum == 3);
                if (fnum != 3) {
                    OBJ_INFO("Face %zu does not contains 3 vertices (%zu). Skipping", f, fnum);
                    continue;
                }
                NMMaterial* nmmaterial = nullptr;
                bool is_emissive = false;
                if (!has_material) {
                    int mat = shape.mesh.material_ids[f];
                    nmmaterial = &material_maps[mat];
                    cr_vec3 emission = VEC3_ARRAY(materials[mat].emission);
                    is_emissive = !isEqual(emission, VEC3_XYZ(0.0));
                    if (is_emissive && nmmaterial->light_id == CRAY_INVALID_HANDLE) {
                        nmmaterial->light_id = CR_addLightAreaDiffuseEmitter(emission, double_sided);
                    }
                }
                else {
                    nmmaterial = &material_maps[0];
                }
                CRAY_HANDLE triangle_id = 0;
                // For each vertex in the face
                for (size_t v = 0; v < fnum; v++) {
                    if (v == 0) {
                        if (is_emissive) {
                            triangle_id = CR_addPrimitiveTriangle(flip_normals, double_sided, nmmaterial->mat_id);
                            CR_addAreaLightToPrimitive(triangle_id, nmmaterial->light_id);
                        }
                        else {
                            triangle_id = CR_addPrimitiveTriangle(flip_normals, double_sided, nmmaterial->mat_id);
                        }
                    }
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
                    if (idx.vertex_index >= 0) {
                        CR_addPrimitiveTriangleVertexPositions(triangle_id, v, VEC3(attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1], attrib.vertices[3 * idx.vertex_index + 2]));
                        if (idx.normal_index >= 0) {
                            CR_addPrimitiveTriangleVertexNormals(triangle_id, v, VEC3(attrib.normals[3 * idx.normal_index + 0], attrib.normals[3 * idx.normal_index + 1], attrib.normals[3 * idx.normal_index + 2]));
                        }
                        if (idx.texcoord_index >= 0) {
                            CR_addPrimitiveTriangleVertexTexcoords(triangle_id, v, VEC3(attrib.texcoords[2 * idx.texcoord_index + 0], attrib.texcoords[2 * idx.texcoord_index + 1], cr_float(0.0)));
                        }
                    }
                    else {
                        OBJ_INFO("Wrong vertex index %d for face %zu. Skipping", idx.vertex_index, f);
                        break;
                    }
                }
                index_offset += fnum;
            }
        }
    }
}

bool scene_load_obj(const char* filename, const char* basepath, bool triangulate) {

    scene_clear_obj();

    LOADER_OBJ::Timer t;
    t.start();
    bool ret = tinyobj::LoadObj(&LOADER_OBJ::attrib, &LOADER_OBJ::shapes, &LOADER_OBJ::materials,
        &LOADER_OBJ::warn, &LOADER_OBJ::err, filename, basepath, triangulate);
    t.stop();
    if (ret) {
        OBJ_INFO("Parsing time: %.2fms", t.getDurationDouble());
    }

    LOADER_OBJ::basePath = basepath;
    return ret;
}

bool scene_generate_obj(bool double_sided, bool flip_normals, CRAY_HANDLE material_id) {
    LOADER_OBJ::info = "";
    LOADER_OBJ::warn = "";
    LOADER_OBJ::err = "";
    LOADER_OBJ::uploadScene(double_sided, flip_normals, material_id);
    return true;
}

void scene_clear_obj() {
    LOADER_OBJ::basePath = "";
    LOADER_OBJ::info = "";
    LOADER_OBJ::warn = "";
    LOADER_OBJ::err = "";
    LOADER_OBJ::attrib.vertices.clear();
    LOADER_OBJ::attrib.normals.clear();
    LOADER_OBJ::attrib.texcoords.clear();
    LOADER_OBJ::attrib.colors.clear();
    LOADER_OBJ::shapes.clear();
    LOADER_OBJ::materials.clear();
}

const char* loader_obj_get_info_log() {
    return LOADER_OBJ::info.c_str();
}

const char* loader_obj_get_warning_log() {
    return LOADER_OBJ::warn.c_str();
}

const char* loader_obj_get_error_log() {
    return LOADER_OBJ::err.c_str();
}

// eof ///////////////////////////////// class LoaderObj
