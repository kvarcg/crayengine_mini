/*
File: CRay.h

Description: CRay Header File

Authors:
Kostas Vardis
*/

/*!
* \mainpage
* \section intro Introduction
* This is a basic CPU ray tracing engine, consisting of two basic files and with a minimum of dependencies.
* The main idea was to be able to quickly render an image through simple API calls for rapid prototyping, 
* that could be easily integrated and exploited through a larger engine. Since other, usually heavy utilities,
* responsible for image loading, logging, geometry I/O, etc. are either already present as part of larger tools or
* exist as separate projects, the provided engine is responsible only for the pure rendering part, i.e. it is 
* agnostic on how images and external models have been loaded.

* \section features Features
* List of features:
* - C/C++ API
* - Multithreaded tiled rendering
* - BVH for storing the scene primitives
* - Two integrators (for now): ambient occlusion and path tracing
* - Cameras: perspective, thinlens and orthographic
* - Primitives: spheres, rectangles, triangles
* - Multiple materials for modelling the typical matte, glass, plastic surfaces, etc (see the list of supported materials below). In more detail:
*     - Support for diffuse/specular components and smooth/rough reflection/transmission
*     - Diffuse surfaces are modelled as pure Lambertian and specular using isotropic GGX.
* - Light sources:
*     - diffuse area lights
*     - point spot/omnilights
*     - directional lights
* - Multiple importance sampling
* - Basic texturing: box and bilinear filtering
* - Support for hierarchical transformations
* - Interactive (progressive) rendering, to be integrated in a GUI for navigation
* - Internal logger mechanism as well as rendering progress functionality
*
* \section author Credits
* Kostas Vardis <br/>
* Website: https://kostasvardis.com <br/>
* Any acknowledgments/references are appreciated.
*
* \section license License
* MIT License
* 
* Copyright (c) 2020 Kostas Vardis
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#ifndef CRAY_H
#define CRAY_H

#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
#else
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#endif

/**
*  @defgroup base Base Objects and Handles
*  Representations for base engine objects
*  @{
*/
/// @brief Define to set double precision floats as default
#define DOUBLE_PRECISION
#ifdef DOUBLE_PRECISION
/// @brief floating point value
typedef double cr_float;
#else
/// @brief floating point value
typedef float cr_float;
#endif

/// @brief Struct for a 3-dimensional vector
typedef struct cr_vec3 {
    cr_float x; cr_float y; cr_float z;
} cr_vec3;

/// @brief Struct for a 4x4 matrix
typedef struct cr_mat4 {
    cr_float x0; cr_float y0; cr_float z0; cr_float w0; cr_float x1; cr_float y1; cr_float z1; cr_float w1; cr_float x2; cr_float y2; cr_float z2; cr_float w2; cr_float x3; cr_float y3; cr_float z3; cr_float w3;
} cr_mat4;

#ifdef __cplusplus
/// @brief Vec3 constructor from 3 values
#define VEC3(_x, _y, _z) cr_vec3{_x,_y,_z}
/// @brief Vec3 constructor from 1 value
#define VEC3_XYZ(_x) cr_vec3{_x,_x,_x}
/// @brief Vec3 constructor from array
#define VEC3_ARRAY(_x) cr_vec3{_x[0],_x[1],_x[2]}
/// @brief Mat4 constructor from 16 values
#define MAT4(_x0, _y0, _z0, _w0, _x1, _y1, _z1, _w1, _x2, _y2, _z2, _w2, _x3, _y3, _z3, _w3) cr_mat4{_x0, _y0, _z0, _w0, _x1, _y1, _z1, _w1, _x2, _y2, _z2, _w2, _x3, _y3, _z3, _w3}
/// @brief Mat4 constructor for identity matrix
#define MAT4_IDENTITY cr_mat4{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}
#else
/// @brief Vec3 constructor from 3 values
#define VEC3(_x, _y, _z) (cr_vec3){_x,_y,_z}
/// @brief Vec3 constructor from 1 value
#define VEC3_XYZ(_x) (cr_vec3){_x,_x,_x}
/// @brief Vec3 constructor from array
#define VEC3_ARRAY(_x) (cr_vec3){_x[0],_x[1],_x[2]}
/// @brief Mat4 constructor from 16 values
#define MAT4(_x0, _y0, _z0, _w0, _x1, _y1, _z1, _w1, _x2, _y2, _z2, _w2, _x3, _y3, _z3, _w3) (cr_mat4){_x0, _y0, _z0, _w0, _x1, _y1, _z1, _w1, _x2, _y2, _z2, _w2, _x3, _y3, _z3, _w3}
/// @brief Mat4 constructor for identity matrix
#define MAT4_IDENTITY (cr_mat4){1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}
#endif

#ifdef __cplusplus
extern "C" {
#endif

    /// @brief Opaque handle
    #define CRAY_HANDLE uint64_t
    /// @brief Opaque invalid handle
    #define CRAY_INVALID_HANDLE 0u
    /** @} */ // end of base

    /**
    *  @defgroup enums Enums
    *  Defines the enums used in the engine to define groups of elements for various operations
    *  @{
    */
    /// @brief List of supported acceleration structures
    typedef enum CRAY_ACCELERATION_STRUCTURE {
        CR_AS_ARRAY,                     ///< Array-based. No acceleration structure.
        CR_AS_BVH,                       ///< Bounding Volume Hierarchy.
        CR_AS_NONE                       ///< Unused value.
    } CRAY_ACCELERATION_STRUCTURE;

    /// @brief List of supported integrators
    typedef enum CRAY_INTEGRATOR {
        CR_INTEGRATOR_PT,               ///< Unidirectional path tracer.
        CR_INTEGRATOR_AO,               ///< Ambient Occlusion.
#ifdef CRAY_ENABLE_TESTS
        CR_INTEGRATOR_TEST_COLOR,       ///< Test integrator for colors.
        CR_INTEGRATOR_TEST_CAMERA,      ///< Test integrator for camera.
        CR_INTEGRATOR_TEST_NOISE,       ///< Test integrator for noise.
        CR_INTEGRATOR_TEST_UV,          ///< Test integrator for UVs.
        CR_INTEGRATOR_TEST_NORMAL,      ///< Test integrator for normals.
        CR_INTEGRATOR_TEST_MATERIAL,    ///< Test integrator for materials.
#endif // CRAY_ENABLE_TESTS
        CR_INTEGRATOR_NONE              ///< Unused value
    } CRAY_INTEGRATOR;

    /// @brief List of supported log messages
    typedef enum CRAY_LOGGERENTRY {
        CR_LOGGER_ERROR,                ///< Error messages: Execution will probably stop.
        CR_LOGGER_WARNING,              ///< Warning messages: Some operation was unexpected or an error was caught and handled properly.
        CR_LOGGER_INFO,                 ///< Info messages: General information. Stats, timings, etc.
        CR_LOGGER_ASSERT,               ///< Asserts: Something that shouldn't happen, happened.
        CR_LOGGER_DEBUG,                ///< Debug: Verbose info. This should only be used in extreme cases or in conjunction with \ref CR_debug_setTestPixelRange.
        CR_LOGGER_NOTHING               ///< Unused value
    } CRAY_LOGGERENTRY;

    /// @brief List of IOR for supported dielectrics, sampled at wavelength 526nm. Source: https://refractiveindex.info
    typedef enum CRAY_IOR_DIELECTRICS {
        // plastics
        CR_DIELECTRIC_Acrylic_glass,                  /// Plastics: Poly(methyl methacrylate)
        CR_DIELECTRIC_Polystyrene,                    /// Plastics: Polystyrene
        CR_DIELECTRIC_Polycarbonate,                  /// Plastics: Polycarbonate
        // crystals
        CR_DIELECTRIC_Diamond,                        /// Crystals: Diamond
        CR_DIELECTRIC_Ice,                            /// Crystals: Ice
        CR_DIELECTRIC_Sapphire,                       /// Crystals: Sapphire
        // glass
        CR_DIELECTRIC_Crown_Glass_bk7, // lenses      /// Glass: Crown Glass bk7
        CR_DIELECTRIC_Soda_lime_glass, // windows     /// Glass: Sode lime glass
        // liquids
        CR_DIELECTRIC_Water_25C,                      /// Liquids: Water at 25C
        CR_DIELECTRIC_Acetone_20C,                    /// Liquids: Acetone at 20C
        // gasses
        CR_DIELECTRIC_Air,                            /// Gasses: Air
        CR_DIELECTRIC_Carbon_dioxide,                 /// Gasses: Carbon dioxide
        // Unused
        CR_DIELECTRIC_ALL_DIELECTRICS                 /// Unused value
    } CRAY_IOR_DIELECTRICS;

    /// @brief List of supported conductors (IOR and absorption), sampled at wavelengths 645nm, 526nm, 444nm. Source: https://refractiveindex.info
    typedef enum CRAY_IOR_CONDUCTORS {
        CR_CONDUCTOR_Alluminium_Al,                  /// Alluminium
        CR_CONDUCTOR_Brass_CuZn,                     /// Brass
        CR_CONDUCTOR_Copper_Cu,                      /// Copper
        CR_CONDUCTOR_Gold_Au,                        /// Gold
        CR_CONDUCTOR_Iron_Fe,                        /// Iron
        CR_CONDUCTOR_Silver_Ag,                      /// Silver
        CR_CONDUCTOR_ALL_CONDUCTORS
    } CRAY_IOR_CONDUCTORS;

    /** @} */ // end of enums

    /**
     *  @defgroup resource Generic Resource Handling API
     * Base engine functions for rendering operations and general resource handling
     *  @{
     */
    /// @brief Initializes the rendering engine. This must be the first function call.
    void CR_init();
    /// @brief Destroys any internal allocations. This is only necessary if multiple rendering operations are to be performed.
    void CR_destroy();

    // Rendering functionality
    /// @ingroup overview
    /// @brief Starts the rendering operation: performs scene preprocessing, initializes threads and starts rendering.
    void CR_start();
    /// @ingroup overview
    /// @brief Issues a request to stop any running rendering operations.
    void CR_stop();
    /// @ingroup intro
    /// @brief Clears any internal scene data, e.g. cameras, acceleration structures, lights, materials, etc.
    /// This is needed only if a subsequent rendering operation is to be performed.
    void CR_clearScene();
    /// @brief Queries the renderer if a start command has been issued.
    /// @return Returns true if the renderer is doing work, false otherwise.
    bool CR_started();
    /// @brief Queries the renderer if rendering has finished.
    /// @return Returns true if the renderer has finished, false otherwise.
    bool CR_finished();
    /** @} */ // end of resource

    /**
     *  @defgroup settings General Settings API
     * General functions setting/retrieves internal engine state
     *  @{
     */
    /// @brief Enables/disables the use of an RGB buffer. By default, all results are written to a floating buffer, for increased precision.
    /// When an RGB buffer is enabled, the main writing is still performed on the floating buffer. The RGB buffer simply contains the converted 8-bit values after tonemapping and gamma correction.
    /// This is useful mainly for GUI operations, in order to avoid performing a whole conversion later on.
    /// @param enable A flag to enable/disable the RGB buffer (Default: false).
    void CR_enableRGBBuffer(bool enable);
    /// @brief Sets an output name for writing. This is just a utility function, e.g. for scene loaders, as the engine does not perform any file operations.
    /// The name does not contain any extension. It is the responsibility of the caller to retrieve the final framebuffer and write to whichever file format is desired (or do anything with it).
    /// @param name The output name (Default: result).
    void CR_setOutputName(const char* name);
    /// @brief Sets the image dimensions.
    /// @param width The width in pixels (Minimum: 32).
    /// @param height The height in pixels (Minimum: 32).
    void CR_setFilmDimensions(int32_t width, int32_t height);
    /// @brief Sets the number of threads to use for rendering. Ideally, this should be the number of cores in the system.
    /// The actual number of threads spawn is num_threads - 1 unless the use_main_thread flag is set to false. A value of 0 issues a serial operation.
    /// @param num_threads The number of threads to spawn (Default: 1).
    void CR_setNumThreads(int32_t num_threads);
    /// @brief Enables/disables the use of the main thread.
    /// @param use_main_thread  A flag to enforce the use of the main thread. This should be disabled for GUI integration (Default: true).
    void CR_useMainThread(bool use_main_thread);
    /// @brief Sets the number of pixels per 2D tile to use for multithreaded rendering. A value of 16 or 32 is a typical use case.
    /// @param block_size The number of tiles (Default: 16, Minimum: 16).
    void CR_setBlockSize(int32_t block_size);
    /// @brief Sets the number of ray samples per pixel.
    /// @param spp The samples per pixel (Default: 1).
    void CR_setSamplesPerPixel(int32_t spp);
    /// @brief Sets the gamma correction value. This is used only when an RGB buffer is enabled.
    /// @param gamma The samples per pixel (Default: 2.2).
    void CR_setGamma(cr_float gamma);
    /// @brief Sets the exposure value to be used during the tonemapping operation. Internally, the traditional John Hable's Uncharted 2 tonemapping operator is used.
    /// This is used only when an RGB buffer is enabled.
    /// Using an exposure value of 0.0 degrades to no tonemapping. This process is then followed by gamma correction (see \ref CR_setGamma)
    /// @param exposure The exposure value (Default: 2.0).
    void CR_setExposure(cr_float exposure);
    /// @brief Sets the acceleration structure. Simple scenes can use a simple array structure. For anything else, a BVH is the optimal option.
    /// Any subsequent calls to this function, overrides AND DELETES any previously allocated acceleration structure and internal data, e.g. primitives.
    /// This function should be called before adding any primitives.
    /// @param acceleration_structure_type The acceleration structure (Default: BVH).
    void CR_setAccelerationStructure(CRAY_ACCELERATION_STRUCTURE acceleration_structure_type);
    /// @brief Adds an integrator. Currently, Ambient Occlusion and unidirectional Path Tracing are available.
    /// A scene can be rendered multiple times with different integrators, by simply issuing calls to this function and \ref CR_start, i.e. no reloading of the scene data is required.
    /// @param integrator_type The integrator to use (Default: Path Tracing).
    void CR_setIntegrator(CRAY_INTEGRATOR integrator_type);
    /// @brief Enables/disables interactive rendering. This is useful only for integrations with a GUI.
    /// Enabling interactive rendering maintains all current settings with the following differences:
    /// - Rendering is using 1spp and the seed is restarted in every iteration. Therefore this mode is for preview purposes only.
    /// - All system cores are utilized (minus the main thread).
    /// - The RNG generator used is a classic Xorshift RNG instead of mt19937.
    /// Note that interactive rendering cannot be enabled when rendering in the main thread is also enabled (see \ref CR_setNumThreads).
    /// @param enable A flag to enable/disable interactive rendering (Default: false).
    /// @return returns true if interactive mode is enabled, false otherwise.
    bool CR_setInteractiveRender(bool enable);
    /// @brief Enforces the use of Xorshift RNG.
    /// @param enable A flag to enable/disable fast RNG (Default: false).
    void CR_useFastRNG(bool enable);

    /// @brief Check if RGB buffer is enabled.
    /// @return True if RGB buffer is enabled/false otherwise.
    bool CR_isRGBBufferEnabled();
    /// @brief Gets the output name.
    /// @return The output name.
    const char* CR_getOutputName();
    /// @brief Gets a pointer to the internal float framebuffer. This should be called when rendering has finished, e.g. to write to a file.
    /// @return A pointer to the data.
    float* CR_getFilmFloatDataPtr();
    /// @brief Gets the number of bytes for the float buffer.
    /// @return The number of bytes.
    uint64_t CR_getFilmFloatByteSize();
    /// @brief Gets a pointer to the internal RGB framebuffer. This should be called when rendering has finished, e.g. to write to a file.
    /// @return A pointer to the data.
    uint8_t* CR_getFilmRGBDataPtr();
    /// @brief Gets the number of bytes for the RGB buffer.
    /// @return The number of bytes.
    uint64_t CR_getFilmRGBByteSize();
    /// @brief Gets the width of the internal framebuffer.
    /// @return The width in pixels.
    int32_t CR_getFilmWidth();
    /// @brief Gets the height of the internal framebuffer.
    /// @return The height in pixels.
    int32_t CR_getFilmHeight();
    /// @brief Gets the number of channels of the internal framebuffer. This is currently fixed to 3.
    /// @return The number of channels.
    int32_t CR_getFilmChannels();
    /// @brief Gets the number of threads.
    /// @return The number of threads.
    int32_t CR_getNumThreads();
    /// @brief Check if main thread rendering is enabled.
    /// @return True if the main thread is used during rendering, false otherwise.
    bool CR_usesMainThread();
    /// @brief Gets the number of pixels per 2D tile.
    /// @return The number of pixels per 2D tile.
    int32_t CR_getBlockSize();
    /// @brief Gets the number of ray samples per pixel.
    /// @return The number of ray samples per pixel.
    int32_t CR_getSamplesPerPixel();
    /// @brief Gets the gamma correction value.
    /// @return The gamma correction value.
    cr_float CR_getGamma();
    /// @brief Gets the exposure value used during the tonemapping operation.
    cr_float CR_getExposure();
    /// @brief Gets the acceleration structure type used.
    /// @return The acceleration structure type.
    CRAY_ACCELERATION_STRUCTURE CR_getAccelerationStructure();
    /// @brief Gets the integrator type used.
    /// @return The integrator type.
    CRAY_INTEGRATOR CR_getIntegrator();
    /// @brief Check if interactive rendering is enabled.
    /// @return True if interactive rendering is enabled/false otherwise.
    bool CR_isInteractiveRendeEnabled();
    /// @brief Check if Fast RNG is enabled.
    /// @return True if Fast RNG is enabled/false otherwise.
    bool CR_isFastRNGEnabled();

    // Ambient Occlusion
    /// @brief Sets the number of AO rays to spawn for each sample per pixel.
    /// @param spp The AO rays per spp (Default: 1).
    void CR_setAOSamplesPerPixel(int32_t spp);
    /// @brief Sets the AO range of the rays. A value of 0 denotes infinite range.
    /// @param range The AO range (Default: 0).
    void CR_setAORange(cr_float range);
    /// @brief Gets the number of AO rays spawned for each sample per pixel.
    /// @return The AO rays per spp.
    int32_t CR_getAOSamplesPerPixel();
    /// @brief Gets the AO range of the rays.
    /// @return The AO range.
    cr_float CR_getAORange();
    // PT
    /// @brief Sets the maximum depth of the rays for the path tracer. 1 is direct lighting, 2 is 1 indirect bounce, etc.
    /// @param depth The maximum depth of rays (Default: 10).
    void CR_setRayDepth(int32_t depth);
    /// @brief Enables/disables russian roulette for path tracing. This function is also capped by the \ref CR_setRayDepth.
    /// @param enable A flag to enable/disable russian roulette (Default: true).
    void CR_setRussianRoulette(bool enable);
    /// @brief Gets the maximum depth of the rays for the path tracer.
    /// @return The maximum depth of rays.
    int32_t CR_getRayDepth();
    /// @brief Gets the russian roulette status.
    /// @return Returns true if russian roulette is enabled, false otherwise.
    bool CR_getRussianRoulette();
    /** @} */ // end of settings
  
    /**
     *  @defgroup materials Materials API
     * Functions for the creation and handling of materials.
     *  @{
     */
    // Materials
    /// @brief Returns the internal index of refraction from a list of predefined dielectrics.
    /// @param dielectric The dielectric type.
    /// @return The index of refraction.
    cr_float CR_getDielectricIOR(CRAY_IOR_DIELECTRICS dielectric);
    /// @brief Creates a diffuse reflection material, modeled as a pure lambertian surface. It can be used to model matte materials.
    /// @param diffuse_reflection_color The reflection color of the material.
    /// @return A generic handle used to pass the material to other functions.
    CRAY_HANDLE CR_addMaterialDiffuseReflection(cr_vec3 diffuse_reflection_color);
    /// @brief Creates a diffuse transmission material, modeled as a pure lambertian surface. It can be used to model translucent materials.
    /// @param diffuse_transmission_color The transmission color of the material.
    CRAY_HANDLE CR_addMaterialDiffuseTransmission(cr_vec3 diffuse_transmission_color);
    /// @brief Creates a diffuse material representing rough reflection and transmission, modeled as a pure lambertian surface.
    /// It can be used to model translucent materials.
    /// @param diffuse_reflection_color The reflection color of the material.
    /// @param diffuse_transmission_color The transmission color of the material.
    /// @param eta_t The index of refraction. This can be provided directly or through the list of predefined materials using \ref CR_getDielectricIOR.
    CRAY_HANDLE CR_addMaterialDiffuse(cr_vec3 diffuse_reflection_color, cr_vec3 diffuse_transmission_color, cr_float eta_t);
    /// @brief Returns the internal index of refraction from a list of predefined conductors
    /// @param conductor The conductor type
    /// @return The index of refraction
    cr_vec3  CR_getConductorIOR(CRAY_IOR_CONDUCTORS conductor);
    /// @brief Returns the absorption coefficients from a list of predefined conductors.
    /// @param conductor The conductor type.
    /// @return The index of refraction.
    cr_vec3  CR_getConductorAbsorption(CRAY_IOR_CONDUCTORS conductor);
    /// @brief Creates a conductor material.
    /// @param reflection_color The reflection color of the component. This can be used for artistic purposes, as the material's physical color is dictated by the other parameters.
    /// @param absorption The absorption coefficients. This can be provided directly or through the list of predefined materials using \ref CR_getConductorAbsorption.
    /// @param eta_t The index of refraction. This can be provided directly or through the list of predefined materials using \ref CR_getConductorIOR.
    /// @param roughness The roughness parameter of the specular component. 0 denotes a perfectly smooth (dirac) surface.
    /// @return A generic handle used to pass the material to other functions.
    CRAY_HANDLE CR_addMaterialConductor(cr_vec3 reflection_color, cr_vec3 absorption, cr_vec3 eta_t, cr_float roughness);
    /// @brief Creates a dielectric material representing smooth/rough reflection with an isotropic Trowbridge-Reitz(GGX) microfacet model.
    /// It can be used to model mirror materials.
    /// @param reflection_color The reflection color of the specular component.
    /// @param roughness The roughness parameter of the specular component. 0 denotes a perfectly smooth (dirac) surface.
    /// @return A generic handle used to pass the material to other functions.
    CRAY_HANDLE CR_addMaterialDielectricSpecularReflection(cr_vec3 reflection_color, cr_float roughness);
    /// @brief Creates a dielectric material representing smooth/rough reflection and transmission with an isotropic Trowbridge-Reitz(GGX) microfacet model.
    /// It can be used to model glass materials.
    /// @param reflection_color The reflection color of the specular component.
    /// @param transmission_color The transmission color of the specular component.
    /// @param eta_t The index of refraction. This can be provided directly or through the list of predefined materials using \ref CR_getDielectricIOR.
    /// @param roughness The roughness parameter of the specular component. 0 denotes a perfectly smooth (dirac) surface.
    /// @return A generic handle used to pass the material to other functions.
    CRAY_HANDLE CR_addMaterialDielectricSpecular(cr_vec3 reflection_color, cr_vec3 transmission_color, cr_float eta_t, cr_float roughness);
    /// @brief Creates a dielectric material representing smooth/rough transmission with an isotropic Trowbridge-Reitz(GGX) microfacet model.
    /// It can be used to model purely transmissive surfaces.
    /// @param transmission_color The transmission color of the specular component.
    /// @param eta_t The index of refraction. This can be provided directly or through the list of predefined materials using \ref CR_getDielectricIOR.
    /// @param roughness The roughness parameter of the specular component. 0 denotes a perfectly smooth (dirac) surface.
    /// @return A generic handle used to pass the material to other functions.
    CRAY_HANDLE CR_addMaterialDielectricSpecularTransmission(cr_vec3 transmission_color, cr_float eta_t, cr_float roughness);
    /// @brief Creates a mixed dielectric material with both diffuse and specular (both smooth/rough) reflection components.
    /// The diffuse component is modeled as a pure lambertian surface, while the specular employs an isotropic Trowbridge-Reitz(GGX) microfacet model.
    /// @param diffuse_reflection_color The reflection color of the diffuse component.
    /// @param specular_reflection_color The reflection color of the specular component.
    /// @param eta_t The index of refraction. This can be provided directly or through the list of predefined materials using \ref CR_getDielectricIOR.
    /// @param roughness The roughness parameter of the specular component. 0 denotes a perfectly smooth (dirac) surface.
    /// @return A generic handle used to pass the material to other functions.
    CRAY_HANDLE CR_addMaterialDiffuseSpecularReflection(cr_vec3 diffuse_reflection_color, cr_vec3 specular_reflection_color, cr_float eta_t, cr_float roughness);
    /** @} */ // end of materials

    /// @ingroup enums
    /// @brief List of supported texture wrapping modes
    typedef enum CRAY_TEXTURE_WRAPPING {
        CTW_CLAMP,                     ///< UVs are clamped to [0, 1].
        CTW_REPEAT                     ///< UVs are repeated, i.e. using the fractional part only.
    } CRAY_TEXTURE_WRAPPING;

    /// @ingroup enums
    /// @brief List of supported texture mapping modes
    typedef enum CRAY_TEXTURE_MAPPING {
        CTM_UV,                       ///< The primitive's uvs are used.
        CTM_PLANAR,                   ///< Planar mapping
        CTM_SPHERICAL                 ///< Spherical mapping
    } CRAY_TEXTURE_MAPPING;

    /// @ingroup enums
    /// @brief List of supported texture filtering modes
    typedef enum CRAY_TEXTURE_FILTERING {
        CTF_BOX,                      ///< Nearest neighbor, i.e. selecting the closest sample.
        CTF_TRIANGLE                  ///< Bilinear filtering, i.e. selecting the average of four.
    } CRAY_TEXTURE_FILTERING;

    /**
     *  @defgroup textures Image API
     * Functions for the generation of images, samplers and textures (an image with a sampler).
     *  @{
     */
    // Textures/images
    /// @brief Creates an image. The data is internally allocated.
    /// @param ptr The pointer to the data
    /// @param width The width of the image
    /// @param height The height of the image
    /// @param channels The number of channels in the image
    /// @param size_type The size of the data type, e.g. 1 for RGB, 4 for floats
    /// @param is_srgb If the data should be converted to linear space during storage
    /// @return A generic handle used to pass the image to other functions.
    CRAY_HANDLE CR_addImage(const uint8_t* ptr, int32_t width, int32_t height, int32_t channels, int32_t size_type, bool is_srgb);
    /// @brief Creates a checkerboard texture using two different colors.
    /// @param tex1_rgb The color of the texture for the odd texels
    /// @param tex2_rgb The color of the texture for the even texels
    /// @param frequency The repetition frequency per the uv range [0-1], e.g, a frequency of 1 shows 1 tile, a frequency of 2 shows 4. (Minimum value: 1.0)
    /// @return A generic handle used to pass the image to other functions.
    CRAY_HANDLE CR_addImageCheckerboard(cr_vec3 tex1_rgb, cr_vec3 tex2_rgb, cr_float frequency);
    /// @brief Creates a sampler.
    /// @param mapping_mode The mapping mode used. (Default: UV).
    /// @param wrap_mode The wrap mode used. (Default: CLAMP).
    /// @param filtering_mode The filtering mode used. (Default: TRIANGLE/bilinear).
    /// @return A generic handle used to pass the sampler to other functions.
    CRAY_HANDLE CR_addTextureSampler(CRAY_TEXTURE_MAPPING mapping_mode, CRAY_TEXTURE_WRAPPING wrap_mode, CRAY_TEXTURE_FILTERING filtering_mode);
    /// @brief Associates a texture with a sampler to be used for rendering.
    /// @param image_id The handle of the image.
    /// @param sampler_id The handle of the sampler.
    /// @return A generic handle used to pass the texture to other functions.
    CRAY_HANDLE CR_addTexture(CRAY_HANDLE image_id, CRAY_HANDLE sampler_id);
    /// @brief Associates a texture with a material's diffuse reflection color.
    /// @param material_id The handle of the diffuse material.
    /// @param texture_id The handle of the texture.
    void CR_addMaterialDiffuseReflectionTexture(CRAY_HANDLE material_id, CRAY_HANDLE texture_id);
    /// @brief Associates a texture with a material's diffuse transmission color.
    /// @param material_id The handle of the diffuse material.
    /// @param texture_id The handle of the texture.
    void CR_addMaterialDiffuseTransmissionTexture(CRAY_HANDLE material_id, CRAY_HANDLE texture_id);
    /// @brief Associates a texture with a material's specular reflection color.
    /// @param material_id The handle of the specular material.
    /// @param texture_id The handle of the texture.
    void CR_addMaterialSpecularReflectionTexture(CRAY_HANDLE material_id, CRAY_HANDLE texture_id);
    /// @brief Associates a texture with a material's specular transmission color.
    /// @param material_id The handle of the specular material.
    /// @param texture_id The handle of the texture.
    void CR_addMaterialSpecularTransmissionTexture(CRAY_HANDLE material_id, CRAY_HANDLE texture_id);
    /** @} */ // end of textures
    
    /**
     *  @defgroup transform Transformation API
     * Functions for hierarchical basic affine transformations
     *  @{
     */
    // Transformations
    /// @brief Pushes a transformation matrix onto the stack.
    /// @param matrix The matrix to push.
    void CR_pushMatrix(cr_mat4 matrix);
    /// @brief Pops a transformation matrix from the stack.
    void CR_popMatrix();
    /// @brief Utility function to generate a rotation matrix along an arbitrary axis.
    /// @param degrees The angle of rotation, in degrees.
    /// @param axis The axis of rotation.
    /// @return The new matrix
    cr_mat4 CR_rotate(cr_float degrees, cr_vec3 axis);
    /// @brief Utility function to generate a translation matrix.
    /// @param translate The translation vector.
    /// @return The new matrix
    cr_mat4 CR_translate(cr_vec3 translate);
    /// @brief Utility function to generate a scale matrix.
    /// @param scale The scale vector.
    /// @return The new matrix
    cr_mat4 CR_scale(cr_vec3 scale);
    /// @brief Utility function to multiply two matrices, M = M1 * M2.
    /// @param matrix1 The first matrix.
    /// @param matrix2 The second matrix.
    /// @return The new matrix
    cr_mat4 CR_mulMatrix(cr_mat4 matrix1, cr_mat4 matrix2);
    /** @} */ // end of transform

     /**
     *  @defgroup camera Cameras API
     * Functions for the generation of cameras
     *  @{
     */
    // Cameras
    /// @brief Creates a thinlens camera. This is essentially the perspective camera with a predefined lens radius and focal distance.
    /// @param position The position of the camera in world units.
    /// @param target The target of the camera in world units.
    /// @param up The up vector of the camera. This does not need to be accurate as it gets recalculated internally.
    /// @param aperture The vertical FOV of the camera.
    /// @param far The far field of the camera. This is not necessary, but is provided as an extra parameter.
    /// @param lens_radius The spherical radius of the thinlens aperture. Setting this to 0 delegates the thinlens camera to the perspective pinhole camera.
    /// @param focal_distance The focal distance of the camera, the plane where the objects are in focus. Setting this to 0 delegates the thinlens camera to the perspective pinhole camera.
    void CR_addCameraThinLens(cr_vec3 position, cr_vec3 target, cr_vec3 up, cr_float aperture, cr_float far, cr_float lens_radius, cr_float focal_distance);
    /// @brief Creates a perspective pinhole camera. This is essentially the thinlens camera without a predefined lens radius and focal distance.
    /// @param position The position of the camera in world units.
    /// @param target The target of the camera in world units.
    /// @param up The up vector of the camera. This does not need to be accurate as it gets recalculated internally.
    /// @param aperture The vertical FOV of the camera.
    /// @param near The near field of the camera. This is not necessary, but is provided as an extra parameter.
    /// @param far The far field of the camera. This is not necessary, but is provided as an extra parameter.
    void CR_addCameraPerspective(cr_vec3 position, cr_vec3 target, cr_vec3 up, cr_float aperture, cr_float near, cr_float far);
    /// @brief Creates an orthographic camera.
    /// @param position The position of the camera in world units.
    /// @param target The target of the camera in world units.
    /// @param up The up vector of the camera. This does not need to be accurate as it gets recalculated internally.
    /// @param height The height of the viewport, in world units. The width is automatically adjusted based on the aspect ration
    /// @param near The near field of the camera. This is not necessary, but is provided as an extra parameter.
    /// @param far The far field of the camera. This is not necessary, but is provided as an extra parameter.
    void CR_addCameraOrthographic(cr_vec3 position, cr_vec3 target, cr_vec3 up, cr_float height, cr_float near, cr_float far);
    /** @} */ // end of camera

    /**
     *  @defgroup light Light API
     * Functions for the generation of emitters.
     *  @{
     */
    // Lights
    /// @brief Creates a point spotlight. This is a dirac light.
    /// @param position The position of the light in world units.
    /// @param target The target of the light in world units.
    /// @param intensity The intensity of the light.
    /// @param falloffAngle The falloff angle, that dictates where the light energy starts to drop.
    /// @param maxAngle The cutoff angle, that dictates where the light energy drops to 0.
    /// @param exponent The spotlight exponent, that provides an exponential fall off as the light travels away from its perfect direction.
    void CR_addLightPointSpot(cr_vec3 position, cr_vec3 target, cr_vec3 intensity, cr_float falloffAngle, cr_float maxAngle, cr_float exponent);
    /// @brief Creates a point omnidirectional light. This is a dirac light.
    /// @param position The position of the light in world units.
    /// @param intensity The intensity of the light.
    void CR_addLightPointOmni(cr_vec3 position, cr_vec3 intensity);
    /// @brief Creates a parallel light using a direction.
    /// @param direction The direction of the light.
    /// @param intensity_rgb The intensity of the light.
    void CR_addLightDirectional(cr_vec3 direction, cr_vec3 intensity_rgb);
    /// @brief Creates a parallel light using a point and a target to generate a direction.
    /// @param position The position of the light in world units.
    /// @param target The target of the light in world units.
    /// @param intensity_rgb The intensity of the light.
    void CR_addLightDirectionalFromTo(cr_vec3 position, cr_vec3 target, cr_vec3 intensity_rgb);
    /// @brief Creates an area light, modeled as a diffuse emitter. This needs to be associated with a primitive. Currently, triangles are not yet supported.
    /// @param flux The flux of the light, in Watts.
    /// @param doublesided A flag that dictates if the light emits light from both its sides.
    /// @return A generic handle used to pass the light to other functions.
    CRAY_HANDLE CR_addLightAreaDiffuseEmitter(cr_vec3 flux, bool doublesided);
    /// @brief Associates an area light with a primitive. Note that the same light source can be shared by multiple primitives. In that case, the light's flux is scaled
    /// by each primitive's area. This way, it is easy to associate a large polygon group with one light source. Since this method is approximate, accurate rendering requires 1 light per primitive.
    /// @param primitive_id The handle of the primitive
    /// @param light_id The handle of the light
    void CR_addAreaLightToPrimitive(CRAY_HANDLE primitive_id, CRAY_HANDLE light_id);
    /** @} */ // end of light

    /**
     *  @defgroup shape Shape API
     * Functions for the generation of shapes. Both parametric and polygon group shapes are supported.
     *  @{
     */
    // Shapes/primitives
    /// @brief Reserves memory for the requested number of primitives.
    /// @param size The number of primitives.
    void CR_reservePrimitives(size_t size);
    /// @brief Reserves memory for the requested number of materials.
    /// @param size The number of materials.
    void CR_reserveMaterials(size_t size);
    /// @brief Retrieves the current number of primitives.
    /// @return The number of primitives.
    size_t CR_getNumPrimitives();
    /// @brief Retrieves the current number of materials.
    /// @return The number of materials.
    size_t CR_getNumMaterials();
    /// @brief Creates a sphere primitive.
    /// @param position The center of the sphere in world units.
    /// @param radius The radius of the sphere in world units.
    /// @param flip_normals A flag that dictates if the primitive's normals are flipped.
    /// @param double_sided A flag that dictates if the primitive is double sided.
    /// @param material_id The material id that will be used by the primitive.
    /// @return A generic handle used to pass the primitive to other functions.
    CRAY_HANDLE CR_addPrimitiveSphere(cr_vec3 position, cr_float radius, bool flip_normals, bool double_sided, CRAY_HANDLE material_id);
    /// @brief Creates an axis-aligned rectangle primitive on the XZ axis.
    /// @param min_x The minimum value in the X axis of the rectangle.
    /// @param max_x The maximum value in the X axis of the rectangle.
    /// @param min_z The minimum value in the Z axis of the rectangle.
    /// @param max_z The maximum value in the Z axis of the rectangle.
    /// @param offset The offset of the rectangle in the Y axis.
    /// @param flip_normals A flag that dictates if the primitive's normals are flipped.
    /// @param double_sided A flag that dictates if the primitive is double sided.
    /// @param material_id The material id that will be used by the primitive.
    /// @return A generic handle used to pass the primitive to other functions.
    CRAY_HANDLE CR_addPrimitiveRectangleXZ(cr_float min_x, cr_float max_x, cr_float min_z, cr_float max_z, cr_float offset, bool flip_normals, bool double_sided, CRAY_HANDLE material_id);
    /// @brief Creates an axis-aligned rectangle primitive on the XY axis.
    /// @param min_x The minimum value in the X axis of the rectangle.
    /// @param max_x The maximum value in the X axis of the rectangle.
    /// @param min_y The minimum value in the Y axis of the rectangle.
    /// @param max_y The maximum value in the Y axis of the rectangle.
    /// @param offset The offset of the rectangle in the Z axis.
    /// @param flip_normals A flag that dictates if the primitive's normals are flipped.
    /// @param double_sided A flag that dictates if the primitive is double sided.
    /// @param material_id The material id that will be used by the primitive.
    /// @return A generic handle used to pass the primitive to other functions.
    CRAY_HANDLE CR_addPrimitiveRectangleXY(cr_float min_x, cr_float max_x, cr_float min_y, cr_float max_y, cr_float offset, bool flip_normals, bool double_sided, CRAY_HANDLE material_id);
    /// @brief Creates an axis-aligned rectangle primitive on the YZ axis.
    /// @param min_y The minimum value in the Y axis of the rectangle.
    /// @param max_y The maximum value in the Y axis of the rectangle.
    /// @param min_z The minimum value in the Z axis of the rectangle.
    /// @param max_z The maximum value in the Z axis of the rectangle.
    /// @param offset The offset of the rectangle in the X axis.
    /// @param flip_normals A flag that dictates if the primitive's normals are flipped.
    /// @param double_sided A flag that dictates if the primitive is double sided.
    /// @param material_id The material id that will be used by the primitive.
    /// @return A generic handle used to pass the primitive to other functions.
    CRAY_HANDLE CR_addPrimitiveRectangleYZ(cr_float min_y, cr_float max_y, cr_float min_z, cr_float max_z, cr_float offset, bool flip_normals, bool double_sided, CRAY_HANDLE material_id);
    /// @brief Creates an empty triangle primitive.
    /// @param flip_normals A flag that dictates if the primitive's normals are flipped.
    /// @param double_sided A flag that dictates if the primitive is double sided.
    /// @param material_id The material id that will be used by the primitive.
    /// @return A generic handle used to pass the primitive to other functions.
    CRAY_HANDLE CR_addPrimitiveTriangle(bool flip_normals, bool double_sided, CRAY_HANDLE material_id);
    /// @brief Associates a group of vertices with an existing triangle.
    /// @param triangle_id The triangle id that the vertices belong to.
    /// @param index The vertex index of the triangle.
    /// @param position The vertex position in world units.
    void CR_addPrimitiveTriangleVertexPositions(CRAY_HANDLE triangle_id, int32_t index, cr_vec3 position);
    /// @brief Associates a group of normals with an existing triangle. If these are not provided, triangle plane normals are used.
    /// @param triangle_id The triangle id that the vertices belong to.
    /// @param index The vertex index of the triangle.
    /// @param normal The vertex normals.
    void CR_addPrimitiveTriangleVertexNormals(CRAY_HANDLE triangle_id, int32_t index, cr_vec3 normal);
    /// @brief Associates a group of texcoord with an existing triangle.
    /// @param triangle_id The triangle id that the vertices belong to.
    /// @param index The vertex index of the triangle.
    /// @param texcoord The vertex texcoords.
    void CR_addPrimitiveTriangleVertexTexcoords(CRAY_HANDLE triangle_id, int32_t index, cr_vec3 texcoord);
    /** @} */ // end of shape

    /**
     *  @defgroup logging Logging API
     * Functions for message logging and progress reporting
     *  @{
     */
    /// @brief Sets the minimum log level to store messages.
    /// @param type The logger type. (Default: Error. Only error messages are logged).
    void CR_setMinimumLogLevel(CRAY_LOGGERENTRY type);
    /// @brief Gets the minimum log level.
    /// @return The minimum log level.
    CRAY_LOGGERENTRY CR_getMinimumLogLevel();
    /// @brief Enables writing to an output FILE. If disabled, the messages are written to a queue and can be polled using \ref CR_getLastLogMessage.
    /// @param file A valid FILE pointer. (Default: NULL).
    void CR_setLogFile(void* file);
    /// @brief Gets the current output FILE.
    /// @return Returns the FILE pointer.
    void* CR_getLogFile();
    /// @brief Enables writing internal log messages. If disabled, the messages can be polled using \ref CR_getLastLogMessage.
    /// @param enable A flag to enable/disable writing to stdout. (Default: true).
    void CR_printTostdout(bool enable);
    /// @brief Returns the status of printing to stdout
    /// @return True if print to stdout is enabled, false otherwise.
    bool CR_isPrintTostdoutEnabled();
    /// @brief Enables of progress bar to stdout. If disabled, the progress percentage can be polled using \ref CR_getProgressPercentage.
    /// Note that stdout printing must be enabled (see \ref CR_printTostdout).
    /// @param enable A flag to enable/disable writing the rendering progress to stdout. (Default: true).
    void CR_printProgressBar(bool enable);
    /// @brief Returns the status of printing the progress bar to stdout.
    /// It is suggested to disable the progress bar when verbose logging is enabled, as it may interfere with the progress bar printing.
    /// @return True if printing the progress bar to stdout is enabled, false otherwise.
    bool CR_IsPrintProgressBarEnabled();
    /// @brief Retrieves the progress percentage of a running rendering process.
    /// @return The progress percentage in the range [0-100]
    cr_float CR_getProgressPercentage();
    /// @brief Gets the last log message stored by the rendering engine.
    /// @param msg A preallocated buffer to store the message. A size of 1024 is a good choice.
    /// @param msg_length The actual length of the msg buffer. If the stored log message is larger, nothing is stored on the buffer and this value contains the required buffer size to use in a subsequent operation.
    /// @param type The logging message type
    /// @return Returns true if a message has been successfully stored, false otherwise.
    bool CR_getLastLogMessage(char* msg, size_t* msg_length, CRAY_LOGGERENTRY* type);
    /** @} */ // end of logging

    /**
     *  @defgroup debug Debug API
     * Debugging functionality. Used internally for testing purposes
     *  @{
     */
    /// @brief Debug utility that returns the intersection position at a particular pixel.
    /// @param position Stores the intersection position.
    /// @param pixel_x The x pixel to test for intersection.
    /// @param pixel_y The y pixel to test for intersection.
    /// @return Returns true if there is an intersection, false otherwise.
    bool CR_debug_intersectPixel(cr_vec3* position, int32_t pixel_x, int32_t pixel_y);
    /// @brief Debug utility that updates camera parameters for navigation during the interactive mode. This is only useful as part of a GUI.
    /// @param position The position of the camera in world units.
    /// @param target The target of the camera in world units.
    /// @param up The up vector of the camera. This does not need to be accurate as it gets recalculated internally.
    /// @param aperture The vertical FOV of the camera.
    /// @param near The near field of the camera. This is not necessary, but is provided as an extra parameter.
    /// @param far The far field of the camera. This is not necessary, but is provided as an extra parameter.
    void CR_debug_updateCameraPerspective(cr_vec3 position, cr_vec3 target, cr_vec3 up, cr_float aperture, cr_float near, cr_float far);
    /// @brief Debug utility that logs verbose messages in debug mode for a particular pixel range [start, stop]. Very useful for cases where internal values need to be viewed. Enable/Disable with \ref CR_debug_setTestPixelRange.
    /// @param start_x The start pixel x value to start logging (Default: none).
    /// @param start_y The start pixel y value to start logging (Default: none).
    /// @param end_x The end pixel x value to stop logging (Default: none).
    /// @param end_y The end pixel y value to stop logging (Default: none).
    void CR_debug_setTestPixelRange(int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y);
    /// @brief Debug utility to enable logging of verbose messages in debug mode for a particular pixel range [start, stop]
    /// @param enable A flag indicating whether debug pixel info will be printed for the given range. (Default: false)
    void CR_debug_enableTestPixelDebug(bool enable);
    /// @brief Debug utility that returns if debug pixel printing is enabled.
    bool CR_debug_isTestPixelDebugEnabled();

    // Tests
#ifdef CRAY_ENABLE_TESTS
    /// @brief Test integrator for colors.
    void CR_testColorIntegrator();
    /// @brief Test integrator for camera uv.
    void CR_testCameraIntegrator();
    /// @brief Test integrator for noise.
    void CR_testNoiseIntegrator();
    /// @brief Test integrator for UVs.
    void CR_testUVIntegrator();
    /// @brief Test integrator for normals.
    void CR_testNormalIntegrator();
    /// @brief Test integrator for materials.
    void CR_testMaterialIntegrator();
#endif // CRAY_ENABLE_TESTS
    /** @} */ // end of logging

#ifdef __cplusplus
};
#endif

#endif //CRAY_H

// eof /////////////////////////////////
