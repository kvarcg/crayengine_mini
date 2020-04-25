/*
File:  LoaderPly.cpp

Description: LoaderPly Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global/Global.h"

// includes ////////////////////////////////////////
#include "LoaderPly.h"
#include "LoaderTexture.h"
#include "Scenes/SceneLoaderHelper.h"
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"
#define TINYPLY_IMPLEMENTATION
#include "tinyply/tinyply.h"
#include <fstream>
#include <chrono>

#define PRAGMA_REMINDER __FILE__ ": Reminder: "
#define PLY_INFO(fmt, ...)                {SCENE_LOADER::ParseMessage(LOADER_PLY::info, "%s(): " fmt "\n", __func__, ##__VA_ARGS__);}
#define PLY_WARNING(fmt, ...)             {SCENE_LOADER::ParseMessage(LOADER_PLY::warn, "%s(): " fmt "\n", __func__, ##__VA_ARGS__);}
#define PLY_ERROR(fmt, ...)               {SCENE_LOADER::ParseMessage(LOADER_PLY::err, "%s(): "   fmt "\n", __func__, ##__VA_ARGS__);}

// defines /////////////////////////////////////////

namespace LOADER_PLY {
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

    static std::unique_ptr<tinyply::PlyFile> plyfile;
    static std::string basePath;
    static std::string filename;
    static std::string info;
    static std::string warn;
    static std::string err;
    std::shared_ptr<PlyData> faces;
    std::shared_ptr<PlyData> vertices;
    std::shared_ptr<PlyData> normals;
    std::shared_ptr<PlyData> texcoords;

    void uploadScene(bool double_sided, bool flip_normals, CRAY_HANDLE material_id, CRAY_HANDLE light_id) {

        if (material_id == CRAY_INVALID_HANDLE) {
            PLY_WARNING("No material provided for file %s. Using a default white diffuse material.", filename.c_str());
            material_id = CR_addMaterialDiffuseReflection(VEC3_XYZ(1.0));
        }

        // add face data
#pragma message(PRAGMA_REMINDER "Add PLY indices support")

        float* fvertices = reinterpret_cast<float*>(vertices->buffer.get());
        float* fnormals = nullptr;
        if (normals) {
            fnormals = reinterpret_cast<float*>(normals->buffer.get());
        }
        float* ftexcoords = nullptr;
        if (texcoords) {
            ftexcoords = reinterpret_cast<float*>(texcoords->buffer.get());
        }
        uint32_t* ufaces = reinterpret_cast<uint32_t*>(faces->buffer.get());
        size_t total_primitives = vertices->count / 3;
        CR_reservePrimitives(CR_getNumPrimitives() + total_primitives);
            // For each face
            for (size_t f = 0; f < faces->count * 3; f+=3) {
                CRAY_HANDLE triangle_id = CR_addPrimitiveTriangle(flip_normals, double_sided, material_id);
                if (light_id != CRAY_INVALID_HANDLE) {
                    CR_addAreaLightToPrimitive(triangle_id, light_id);
                }
                // For each vertex in the face
                for (size_t v = 0; v < 3; v++) {
                    size_t vertex_index = ufaces[f + v];
                    if (3 * vertex_index + 2 < vertices->count * 3) {
                        CR_addPrimitiveTriangleVertexPositions(triangle_id, v, VEC3(fvertices[3 * vertex_index + 0], fvertices[3 * vertex_index + 1], fvertices[3 * vertex_index + 2]));
                        if (fnormals && 3 * vertex_index + 2 < normals->count * 3) {
                            CR_addPrimitiveTriangleVertexNormals(triangle_id, v, VEC3(fnormals[3 * vertex_index + 0], fnormals[3 * vertex_index + 1], fnormals[3 * vertex_index + 2]));
                        }
                        if (ftexcoords && 2 * vertex_index + 1 < texcoords->count * 2) {
                            CR_addPrimitiveTriangleVertexTexcoords(triangle_id, v, VEC3(ftexcoords[2 * vertex_index + 0], ftexcoords[2 * vertex_index + 1], cr_float(0.0)));
                        }
                    }
                    else {
                        PLY_INFO("Wrong vertex index %d for face %zu. Skipping", vertex_index, f / 3);
                        break;
                    }
                }
            }
    }
}

bool scene_load_ply(const char* filename, const char* basepath) {

    scene_clear_ply();

    LOADER_PLY::Timer t;
    t.start();
    std::string filenameWithPath = std::string(basepath) + filename;
    std::ifstream ss(filenameWithPath, std::ios::binary);
    if (ss.fail()) {
        PLY_ERROR("Could not open file %s", filename);
        return false;
    }

    LOADER_PLY::plyfile = std::make_unique<tinyply::PlyFile>();
    if (!LOADER_PLY::plyfile->parse_header(ss)) {
        PLY_ERROR("Could not parse header of file %s", filename);
        return false;
    }

    // The header information can be used to programmatically extract properties on elements
    // known to exist in the header prior to reading the data. For brevity of this sample, properties
    // like vertex position are hard-coded:
    try {
        LOADER_PLY::vertices = LOADER_PLY::plyfile->request_properties_from_element("vertex", { "x", "y", "z" });
    }
    catch (const std::exception & e) {
        PLY_ERROR("tinyply exception: %s", e.what());
    }

    try { LOADER_PLY::normals = LOADER_PLY::plyfile->request_properties_from_element("vertex", { "nx", "ny", "nz" }); }
    catch (const std::exception & e) {
        PLY_WARNING("tinyply warning: %s", e.what());
    }

    try { LOADER_PLY::texcoords = LOADER_PLY::plyfile->request_properties_from_element("vertex", { "u", "v" }); }
    catch (const std::exception & e) {
        PLY_WARNING("tinyply warning: %s", e.what());
    }

    // Providing a list size hint (the last argument) is a 2x performance improvement. If you have
    // arbitrary ply files, it is best to leave this 0.
    try { LOADER_PLY::faces = LOADER_PLY::plyfile->request_properties_from_element("face", { "vertex_indices" }, 3); }
    catch (const std::exception & e) {
        PLY_ERROR("tinyply exception: %s", e.what());
    }

    LOADER_PLY::plyfile->read(ss);
    t.stop();
    PLY_INFO("Parsing time: %.2fms", t.getDurationDouble());
    LOADER_PLY::filename = filename;
    LOADER_PLY::basePath = basepath;

    return true;
}

bool scene_generate_ply(bool double_sided, bool flip_normals, CRAY_HANDLE material_id, CRAY_HANDLE light_id) {
    LOADER_PLY::info = "";
    LOADER_PLY::warn = "";
    LOADER_PLY::err = "";
    LOADER_PLY::uploadScene(double_sided, flip_normals, material_id, light_id);
    return true;
}

void scene_clear_ply() {
    LOADER_PLY::filename = "";
    LOADER_PLY::basePath = "";
    LOADER_PLY::info = "";
    LOADER_PLY::warn = "";
    LOADER_PLY::err = "";
    LOADER_PLY::vertices.reset();
    LOADER_PLY::normals.reset();
    LOADER_PLY::faces.reset();
    LOADER_PLY::texcoords.reset();
    LOADER_PLY::plyfile.reset();
}

const char* loader_ply_get_info_log() {
    return LOADER_PLY::info.c_str();
}

const char* loader_ply_get_warning_log() {
    return LOADER_PLY::warn.c_str();
}

const char* loader_ply_get_error_log() {
    return LOADER_PLY::err.c_str();
}

// eof ///////////////////////////////// class LoaderObj
