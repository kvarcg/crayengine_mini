/*
File:  Scenes.cpp

Description: Scenes Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

// global includes    //////////////////////////////
#include "Global/Global.h"

// includes ////////////////////////////////////////
#include "Scenes/Scenes.h"
#include "Scenes/SceneLoader.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <cmath>
#include "Loader/LoaderObj.h"

// defines /////////////////////////////////////////

BasicCamera m_basic_camera;

void create_basic_camera() {
    m_basic_camera.eye = VEC3(0.0, 0.0, 0.0);
    m_basic_camera.target = VEC3(0.0, 0.0, -1.0);
    m_basic_camera.up = VEC3(0.0, 1.0, 0.0);
    m_basic_camera.aperture = 35.0;
    m_basic_camera.cam_near = 1.0;
    m_basic_camera.cam_far = 100.0;
}

void scene_obj() {
    std::string path = std::string(SOURCEPATH) + "data/generic/";
    std::string filename = "cube.obj";
    std::string info;
    std::string warn;
    std::string err;
    if (!scene_load_obj((path + filename).c_str(), path.c_str(), true)) {
        printf("Could not load obj %s\n", filename.c_str());
    }
    else {
        printf("Successfully loaded %s\n", filename.c_str());
    }
    info = loader_obj_get_info_log();
    warn = loader_obj_get_warning_log();
    err = loader_obj_get_error_log();

    if (!info.empty()) {
        printf("Info: %s", info.c_str());
    }
    if (!warn.empty()) {
        printf("Warnings: %s", warn.c_str());
    }
    if (!err.empty()) {
        printf("Errors: %s", err.c_str());
    }
}

void scene_simple_spheres() {
    m_basic_camera.eye = { 0.0, 5.6, 10.0 };
    m_basic_camera.target = { 0.0, 1.0, -1.5 };
    m_basic_camera.up = { 0.0, 1.0, 0.0 };
    m_basic_camera.aperture = { 35.0 };
    m_basic_camera.cam_near = { 1.0 };
    m_basic_camera.cam_far = { 100.0 };
    CR_addCameraPerspective(m_basic_camera.eye, m_basic_camera.target, m_basic_camera.up, m_basic_camera.aperture, m_basic_camera.cam_near, m_basic_camera.cam_far);
    //CR_addLightPointSpot(VEC3(-10.0, 5.0, -10.0), VEC3( 5.0, 0.0, -10.0), VEC3(0.0, 300.0, 300.0), 30.0, 60.0, 1.0);
    //CR_addLightPointSpot(VEC3(10.0, 5.0, -10.0),  VEC3(-5.0, 0.0, -10.0), VEC3(300.0, 0.0, 0.0), 30.0, 60.0, 1.0);
    //CR_addLightDirectionalFromTo(VEC3(-10.0, 5.0, -10.0), VEC3(5.0, 0.0, -10.0), VEC3(0.3, 0.0, 0.0));
    //CR_addLightPointOmni(VEC3(-10.0, 5.0, -10.0), VEC3(0.0, 0.0, -10.0), VEC3(300.0, 300.0, 300.0));
    //CR_addLightPointOmni(VEC3(10.0, 5.0, -10.0), VEC3(0.0, 0.0, -10.0), VEC3(300.0, 300.0, 300.0));
    //CR_addLightPointOmni(VEC3(-5.0, 5.0, -10.0), VEC3(300.0, 00.0, 00.0));
    CR_addLightPointOmni(VEC3(-10.0, 10.0, -5.0), VEC3(300, 300, 300));
    //CR_addLightPointOmni(VEC3(10.0, 10.0, -5.0), VEC3(300, 300, 300));
    //CRAY_HANDLE light_id = CR_addLightAreaDiffuseEmitter(VEC3(0.0, 00.0, 1000.0));
    //CRAY_HANDLE sphere_id = CR_addPrimitiveSphere(VEC3(10.0, 10.0, -5), 1.0, CR_addMaterialDiffuseReflection(VEC3_XYZ(1.0)));
    //CR_addAreaLightToPrimitive(sphere_id, light_id);

    cr_float height = 0.0;
    cr_float depth = -22.0;
    cr_float step = 2.0;
    cr_float radius = 0.5;

    // lambert
    depth += step;
    CRAY_HANDLE material_diffuse_reflection_black_id = CR_addMaterialDiffuseReflection(VEC3(0.0, 0.0, 0.0));
    CRAY_HANDLE material_diffuse_reflection_orange_id = CR_addMaterialDiffuseReflection(VEC3(0.8, 0.3, 0.3));
    CRAY_HANDLE material_diffuse_reflection_white_id = CR_addMaterialDiffuseReflection(VEC3(1.0, 1.0, 1.0));
    CR_addPrimitiveSphere(VEC3(-1.5, height, depth), radius, false, false, material_diffuse_reflection_black_id);
    CR_addPrimitiveSphere(VEC3(0.0, height, depth), radius, false, false, material_diffuse_reflection_orange_id);
    CR_addPrimitiveSphere(VEC3(1.5, height, depth), radius, false, false, material_diffuse_reflection_white_id);

    // lambert transmission
    depth += step;
    CRAY_HANDLE material_diffuse_tranmission_black_id = CR_addMaterialDiffuseTransmission(VEC3(0.0, 0.0, 0.0));
    CRAY_HANDLE material_diffuse_tranmission_orange_id = CR_addMaterialDiffuseTransmission(VEC3(0.8, 0.3, 0.3));
    CRAY_HANDLE material_diffuse_tranmission_white_id = CR_addMaterialDiffuseTransmission(VEC3(1.0, 1.0, 1.0));
    CR_addPrimitiveSphere(VEC3(-1.5, height, depth), radius, false, false, material_diffuse_tranmission_black_id);
    CR_addPrimitiveSphere(VEC3(0.0, height, depth), radius, false, false, material_diffuse_tranmission_orange_id);
    CR_addPrimitiveSphere(VEC3(1.5, height, depth), radius, false, false, material_diffuse_tranmission_white_id);

    // lambert diffuse/transmission
    depth += step;
    CRAY_HANDLE material_diffuse_black_id = CR_addMaterialDiffuse(VEC3(0.0, 0.0, 0.0), VEC3(0.0, 0.0, 0.0), 1.05);
    CRAY_HANDLE material_diffuse_orange_id = CR_addMaterialDiffuse(VEC3(0.2, 0.2, 0.2), VEC3(0.8, 0.3, 0.3), 1.05);
    CRAY_HANDLE material_diffuse_white_id = CR_addMaterialDiffuse(VEC3(0.2, 0.2, 0.2), VEC3(1.0, 1.0, 1.0), 1.3);
    CR_addPrimitiveSphere(VEC3(-1.5, height, depth), radius, false, false, material_diffuse_black_id);
    CR_addPrimitiveSphere(VEC3(0.0, height, depth), radius, false, false, material_diffuse_orange_id);
    CR_addPrimitiveSphere(VEC3(1.5, height, depth), radius, false, false, material_diffuse_white_id);

    // diffuse/specular
    depth += step;
    CRAY_HANDLE material_plastic_black_id = CR_addMaterialDiffuseSpecularReflection(VEC3(0.8, 0.3, 0.3), VEC3(0.0, 0.0, 0.0), 0.001, CR_getDielectricIOR(CR_DIELECTRIC_Polystyrene));
    CRAY_HANDLE material_plastic_orange_id = CR_addMaterialDiffuseSpecularReflection(VEC3(0.8, 0.3, 0.3), VEC3(0.8, 0.3, 0.3), 0.001, CR_getDielectricIOR(CR_DIELECTRIC_Polystyrene));
    CRAY_HANDLE material_plastic_white_id = CR_addMaterialDiffuseSpecularReflection(VEC3(0.8, 0.3, 0.3), VEC3(1.0, 1.0, 1.0), 0.001, CR_getDielectricIOR(CR_DIELECTRIC_Polystyrene));
    CR_addPrimitiveSphere(VEC3(-1.5, height, depth), radius, false, false, material_plastic_black_id);
    CR_addPrimitiveSphere(VEC3(0.0, height, depth), radius, false, false, material_plastic_orange_id);
    CR_addPrimitiveSphere(VEC3(1.5, height, depth), radius, false, false, material_plastic_white_id);

    // dielectrics mirrors
    depth += step;
    CRAY_HANDLE material_mirror_id = CR_addMaterialDielectricSpecularReflection(VEC3(1.0, 1.0, 1.0), 0.0);
    CRAY_HANDLE material_mirror_rough_id = CR_addMaterialDielectricSpecularReflection(VEC3(1.0, 1.0, 1.0), 0.001);
    CRAY_HANDLE material_mirror_rough2_id = CR_addMaterialDielectricSpecularReflection(VEC3(1.0, 1.0, 1.0), 0.01);
    CR_addPrimitiveSphere(VEC3(-1.5, height, depth), radius, false, false, material_mirror_id);
    CR_addPrimitiveSphere(VEC3(0.0, height, depth), radius, false, false, material_mirror_rough_id);
    CR_addPrimitiveSphere(VEC3(1.5, height, depth), radius, false, false, material_mirror_rough2_id);

    // dielectrics transmission
    depth += step;
    CRAY_HANDLE material_specular_transmission_id = CR_addMaterialDielectricSpecularTransmission(VEC3(1.0, 1.0, 1.0), CR_getDielectricIOR(CR_DIELECTRIC_Crown_Glass_bk7), 0.0);
    CRAY_HANDLE material_specular_transmission_rough_id = CR_addMaterialDielectricSpecularTransmission(VEC3(1.0, 1.0, 1.0), CR_getDielectricIOR(CR_DIELECTRIC_Crown_Glass_bk7), 0.001);
    CRAY_HANDLE material_specular_transmission_rough2_id = CR_addMaterialDielectricSpecularTransmission(VEC3(1.0, 1.0, 1.0), CR_getDielectricIOR(CR_DIELECTRIC_Crown_Glass_bk7), 0.01);
    CR_addPrimitiveSphere(VEC3(-1.5, height, depth), radius, false, false, material_specular_transmission_id);
    CR_addPrimitiveSphere(VEC3(0.0, height, depth), radius, false, false, material_specular_transmission_rough_id);
    CR_addPrimitiveSphere(VEC3(1.5, height, depth), radius, false, false, material_specular_transmission_rough2_id);

    // dielectrics glass
    depth += step;
    CRAY_HANDLE material_glass_id = CR_addMaterialDielectricSpecular(VEC3(1.0, 1.0, 1.0), VEC3(1.0, 1.0, 1.0), CR_getDielectricIOR(CR_DIELECTRIC_Crown_Glass_bk7), 0.0);
    CRAY_HANDLE material_glass_rough_id = CR_addMaterialDielectricSpecular(VEC3(1.0, 1.0, 1.0), VEC3(1.0, 1.0, 1.0), CR_getDielectricIOR(CR_DIELECTRIC_Crown_Glass_bk7), 0.01);
    CRAY_HANDLE material_glass_rough2_id = CR_addMaterialDielectricSpecular(VEC3(1.0, 1.0, 1.0), VEC3(1.0, 1.0, 1.0), CR_getDielectricIOR(CR_DIELECTRIC_Crown_Glass_bk7), 0.1);
    CR_addPrimitiveSphere(VEC3(-1.5, height, depth), radius, false, false, material_glass_id);
    CR_addPrimitiveSphere(VEC3(0.0, height, depth), radius, false, false, material_glass_rough_id);
    CR_addPrimitiveSphere(VEC3(1.5, height, depth), radius, false, false, material_glass_rough2_id);

    // simple metals specular
    depth += step;
    CRAY_HANDLE material_black_specular_id = CR_addMaterialConductor(VEC3(0.0, 0.0, 0.0), VEC3(1.0, 1.0, 1.0), VEC3(1.0, 1.0, 1.0), 0.001);
    CRAY_HANDLE material_gray_specular_id = CR_addMaterialConductor(VEC3(0.5, 0.5, 0.5), VEC3(1.0, 1.0, 1.0), VEC3(1.0, 1.0, 1.0), 0.001);
    CRAY_HANDLE material_white_specular_id = CR_addMaterialConductor(VEC3(1.0, 1.0, 1.0), VEC3(1.0, 1.0, 1.0), VEC3(1.0, 1.0, 1.0), 0.001);
    CR_addPrimitiveSphere(VEC3(-1.5, height, depth), radius, false, false, material_black_specular_id);
    CR_addPrimitiveSphere(VEC3(0.0, height, depth), radius, false, false, material_gray_specular_id);
    CR_addPrimitiveSphere(VEC3(1.5, height, depth), radius, false, false, material_white_specular_id);

    // metals, eta
    depth += step;
    CRAY_HANDLE material_metal_eta_id = CR_addMaterialConductor(VEC3(1.0, 1.0, 1.0), VEC3(1.0, 1.0, 1.0), VEC3(0.1, 0.1, 0.1), 0.01);
    CRAY_HANDLE material_metal2_eta_id = CR_addMaterialConductor(VEC3(1.0, 1.0, 1.0), VEC3(1.0, 1.0, 1.0), VEC3(0.5, 0.5, 0.5), 0.01);
    CRAY_HANDLE material_metal3_eta_id = CR_addMaterialConductor(VEC3(1.0, 1.0, 1.0), VEC3(1.0, 1.0, 1.0), VEC3(1.0, 1.0, 1.0), 0.01);
    CR_addPrimitiveSphere(VEC3(-1.5, height, depth), radius, false, false, material_metal_eta_id);
    CR_addPrimitiveSphere(VEC3(0.0, height, depth), radius, false, false, material_metal2_eta_id);
    CR_addPrimitiveSphere(VEC3(1.5, height, depth), radius, false, false, material_metal3_eta_id);

    // metals, absorption
    depth += step;
    CRAY_HANDLE material_metal_absorption_id = CR_addMaterialConductor(VEC3(1.0, 1.0, 1.0), VEC3(0.2, 0.2, 0.2), VEC3(1.0, 1.0, 1.0), 0.01);
    CRAY_HANDLE material_metal_absorption2_id = CR_addMaterialConductor(VEC3(1.0, 1.0, 1.0), VEC3(0.5, 0.5, 0.5), VEC3(1.0, 1.0, 1.0), 0.01);
    CRAY_HANDLE material_metal_absorption3_id = CR_addMaterialConductor(VEC3(1.0, 1.0, 1.0), VEC3(1.0, 1.0, 1.0), VEC3(1.0, 1.0, 1.0), 0.01);
    CR_addPrimitiveSphere(VEC3(-1.5, height, depth), radius, false, false, material_metal_absorption_id);
    CR_addPrimitiveSphere(VEC3(0.0, height, depth), radius, false, false, material_metal_absorption2_id);
    CR_addPrimitiveSphere(VEC3(1.5, height, depth), radius, false, false, material_metal_absorption3_id);

    // metals
    depth += step;
    cr_float start = -CR_CONDUCTOR_ALL_CONDUCTORS * radius;
    cr_float end = -start;
    cr_float x_step = (end - start) / (CR_CONDUCTOR_ALL_CONDUCTORS - 1);
    for (int i = 0; i < CR_CONDUCTOR_ALL_CONDUCTORS; ++i) {
        CRAY_HANDLE material_metal_id = CR_addMaterialConductor(VEC3(1.0, 1.0, 1.0), CR_getConductorAbsorption(CRAY_IOR_CONDUCTORS(i)), CR_getConductorIOR(CRAY_IOR_CONDUCTORS(i)), 0.01);
        cr_float pos_x = start + x_step * i;
        CR_addPrimitiveSphere(VEC3(pos_x, height, depth), radius, false, false, material_metal_id);
    }

    // ground
    CRAY_HANDLE material_ground_id = CR_addMaterialDiffuseReflection(VEC3(0.8, 0.8, 0.0));
    CRAY_HANDLE image_id = CR_addImageCheckerboard(VEC3(0.3, 0.5, 0.1), VEC3(1.0, 1.0, 1.0), 100.0);
    CRAY_HANDLE sampler_id = CR_addTextureSampler(CTM_UV, CTW_CLAMP, CTF_BOX);
    CRAY_HANDLE texture_id = CR_addTexture(image_id, sampler_id);
    CR_addMaterialDiffuseReflectionTexture(material_ground_id, texture_id);
    CR_addPrimitiveRectangleXZ(-50, 50, -50, 50, -0.5, true, false, material_ground_id);
}

void scene_cornell() {
    m_basic_camera.eye = { 0.0, 0.0, 4.1 };
    m_basic_camera.target = { 0.0, 0.0, -1.0 };
    m_basic_camera.up = { 0.0, 1.0, 0.0 };
    m_basic_camera.aperture = { 35.0 };
    m_basic_camera.cam_near = { 1.0 };
    m_basic_camera.cam_far = { 100.0 };
    CR_addCameraPerspective(m_basic_camera.eye, m_basic_camera.target, m_basic_camera.up, m_basic_camera.aperture, m_basic_camera.cam_near, m_basic_camera.cam_far);
    CRAY_HANDLE light_id = CR_addLightAreaDiffuseEmitter(VEC3(17.0, 12.0, 4.0), true);
    //CRAY_HANDLE primitive_id = CR_addPrimitiveSphere(VEC3(0.0, 0.75, 0.0), 0.2, false, CR_addMaterialDiffuseReflection(VEC3_XYZ(0.0)));
    CRAY_HANDLE primitive_id = CR_addPrimitiveRectangleXZ(-0.2, 0.2, -0.2, 0.2, 0.999, false, false, CR_addMaterialDiffuseReflection(VEC3_XYZ(0.0)));
    CR_addAreaLightToPrimitive(primitive_id, light_id);

    CRAY_HANDLE material_mirror_id = CR_addMaterialDielectricSpecularReflection(VEC3_XYZ(1.0), 0.0);
    CR_addPrimitiveSphere(VEC3(-0.75, -0.875, 0.0), 0.125, false, false, material_mirror_id);
    CRAY_HANDLE material_metal_id = CR_addMaterialConductor(VEC3_XYZ(1.0), CR_getConductorAbsorption(CR_CONDUCTOR_Copper_Cu), CR_getConductorIOR(CR_CONDUCTOR_Copper_Cu), 0.0);
    CR_addPrimitiveSphere(VEC3(-0.25, -0.875, 0.0), 0.125, false, false, material_metal_id);
    CRAY_HANDLE material_diffuse_transmission_blue_id = CR_addMaterialDiffuseTransmission(VEC3(0.1, 0.1, 0.8));
    CR_addPrimitiveSphere(VEC3(0.25, -0.875, 0.0), 0.125, false, false, material_diffuse_transmission_blue_id);
    CRAY_HANDLE material_diffuse_orange_id = CR_addMaterialDiffuseReflection(VEC3(0.8, 0.8, 0.3));
    CR_addPrimitiveSphere(VEC3(0.75, -0.875, 0.0), 0.125, false, false, material_diffuse_orange_id);

    // build box
    CRAY_HANDLE material_red_id = CR_addMaterialDiffuseReflection(VEC3(0.63, 0.065, 0.05));
    CRAY_HANDLE material_white_id = CR_addMaterialDiffuseReflection(VEC3(0.725, 0.71, 0.68));
    CRAY_HANDLE material_green_id = CR_addMaterialDiffuseReflection(VEC3(0.14, 0.45, 0.091));
    CR_addPrimitiveRectangleXZ(-1.0, 1.0, -1.0, 1.0, 1.0, false, false, material_white_id);  // top
    CR_addPrimitiveRectangleXZ(-1.0, 1.0, -1.0, 1.0, -1.0, true, false, material_white_id);   // bottom
    CR_addPrimitiveRectangleXY(-1.0, 1.0, -1.0, 1.0, -1.0, false, false, material_white_id);  // back
    CR_addPrimitiveRectangleYZ(-1.0, 1.0, -1.0, 1.0, -1.0, true, false, material_red_id);     // left
    CR_addPrimitiveRectangleYZ(-1.0, 1.0, -1.0, 1.0, 1.0, false, false, material_green_id);  // right
}

void scene_mis() {
    m_basic_camera.eye = { 0, 3.5, 28.2792 };
    m_basic_camera.target = { 0, 3.5, 0 };
    m_basic_camera.up = { 0.0, 1.0, 0.0 };
    m_basic_camera.aperture = { 35.0 };
    m_basic_camera.cam_near = { 1.0 };
    m_basic_camera.cam_far = { 100.0 };
    CR_addCameraPerspective(m_basic_camera.eye, m_basic_camera.target, m_basic_camera.up, m_basic_camera.aperture, m_basic_camera.cam_near, m_basic_camera.cam_far);
    //CRAY_HANDLE material_mirror_id = CR_addMaterialDielectricSpecularReflection(VEC3_XYZ(1.0), 0.0);
    // CR_addPrimitiveSphere(VEC3(-0.75, -0.875, 0.0), 0.125, false, false, material_mirror_id);
    //CRAY_HANDLE material_metal_id = CR_addMaterialConductor(VEC3_XYZ(1.0), CR_getConductorAbsorption(Copper_Cu), CR_getConductorIOR(Copper_Cu), 0.0);
    //CR_addPrimitiveSphere(VEC3(-0.25, -0.875, 0.0), 0.125, false, false, material_metal_id);
    //CRAY_HANDLE material_diffuse_transmission_blue_id = CR_addMaterialDiffuseTransmission(VEC3(0.1, 0.1, 0.8), 1.05);
    //CR_addPrimitiveSphere(VEC3(0.25, -0.875, 0.0), 0.125, false, false, material_diffuse_transmission_blue_id);
    //CRAY_HANDLE material_diffuse_gray_id = CR_addMaterialDiffuseReflection(VEC3(0.1, 0.1, 0.1));
    CRAY_HANDLE material_diffuse_red_id = CR_addMaterialDiffuseReflection(VEC3(0.8, 0.0, 0.0));
    //CR_addPrimitiveSphere(VEC3(0.75, -0.875, 0.0), 0.125, false, false, material_diffuse_orange_id);

    //CRAY_HANDLE material_metal_rough_id = CR_addMaterialConductor(VEC3_XYZ(1.0), VEC3(3.912949, 2.452848, 2.142188), VEC3(0.200438, 0.924033, 1.102212), 0.03);
    // CRAY_HANDLE material_metal_glossy_id = CR_addMaterialConductor(VEC3_XYZ(1.0), VEC3(3.912949, 2.452848, 2.142188), VEC3(0.200438, 0.924033, 1.102212), 0.01);
    // CRAY_HANDLE material_metal_very_glossy_id = CR_addMaterialConductor(VEC3_XYZ(1.0), VEC3(3.912949, 2.452848, 2.142188), VEC3(0.200438, 0.924033, 1.102212), 0.001);
    // CRAY_HANDLE material_metal_smooth_id = CR_addMaterialConductor(VEC3_XYZ(1.0), VEC3(3.912949, 2.452848, 2.142188), VEC3(0.200438, 0.924033, 1.102212), 0.0);
    //CRAY_HANDLE material_metal_smooth_id = CR_addMaterialDielectricSpecularReflection(VEC3_XYZ(1.0), 0.0);
    // background
    CR_pushMatrix(CR_translate(VEC3(0.0, -9.9, -80))); CR_pushMatrix(CR_scale(VEC3(19.8, 19.8, 47.52))); CR_pushMatrix(CR_rotate(-90, VEC3(1, 0, 0)));
    //CR_addPrimitiveRectangleXY(min_x, max_x, min_z, max_z, 0.0, false, material_diffuse_red_id);  // back
    CR_popMatrix(); CR_popMatrix(); CR_popMatrix();

    cr_float min_x = -1.0;
    cr_float max_x = 1.0;
    cr_float min_z = -1.0;
    cr_float max_z = 1.0;
    cr_float angle = 0.0;
    CR_pushMatrix(CR_translate(VEC3(0.0, 4.09801, 0.264069))); CR_pushMatrix(CR_scale(VEC3(8, 0.3, 2.1))); CR_pushMatrix(CR_rotate(angle - 39.8801, VEC3(1, 0, 0)));
    CR_addPrimitiveRectangleXY(min_x, max_x, min_z, max_z, 0.0, true, false, material_diffuse_red_id);  // back
    CR_popMatrix(); CR_popMatrix(); CR_popMatrix();
    CR_pushMatrix(CR_translate(VEC3(0.0, 2.71702, 3.06163))); CR_pushMatrix(CR_scale(VEC3(8, 0.3, 2.1))); CR_pushMatrix(CR_rotate(angle - 22.2154, VEC3(1, 0, 0)));
    CR_addPrimitiveRectangleXY(min_x, max_x, min_z, max_z, 0.0, true, false, material_diffuse_red_id);  // back
    CR_popMatrix(); CR_popMatrix(); CR_popMatrix();
    CR_pushMatrix(CR_translate(VEC3(0.0, 1.81891, 7.09981))); CR_pushMatrix(CR_scale(VEC3(8, 0.3, 2.1))); CR_pushMatrix(CR_rotate(angle - 10.6515, VEC3(1, 0, 0)));
    CR_addPrimitiveRectangleXY(min_x, max_x, min_z, max_z, 0.0, true, false, material_diffuse_red_id);  // back
    CR_popMatrix(); CR_popMatrix(); CR_popMatrix();
    CR_pushMatrix(CR_translate(VEC3(0.0, 1.23376, 10.6769))); CR_pushMatrix(CR_scale(VEC3(8, 0.3, 2.1))); CR_pushMatrix(CR_rotate(angle - 7.00104, VEC3(1, 0, 0)));
    CR_addPrimitiveRectangleXY(min_x, max_x, min_z, max_z, 0.0, true, false, material_diffuse_red_id);  // back
    CR_popMatrix(); CR_popMatrix(); CR_popMatrix();

    cr_float unit_flux = 300;
    CR_addAreaLightToPrimitive(CR_addPrimitiveSphere(VEC3(-2.8, 6.5, 0.0), 0.01, false, false, CR_addMaterialDiffuseReflection(VEC3_XYZ(0.0))), CR_addLightAreaDiffuseEmitter(VEC3_XYZ(unit_flux), true));
    CR_addAreaLightToPrimitive(CR_addPrimitiveSphere(VEC3(0.0, 6.5, 0.0), 0.05, false, false, CR_addMaterialDiffuseReflection(VEC3_XYZ(0.0))), CR_addLightAreaDiffuseEmitter(VEC3_XYZ(unit_flux), true));
    CR_addAreaLightToPrimitive(CR_addPrimitiveSphere(VEC3(2.7, 6.5, 0.0), 0.50, false, false, CR_addMaterialDiffuseReflection(VEC3_XYZ(0.0))), CR_addLightAreaDiffuseEmitter(VEC3_XYZ(unit_flux), true));
    CR_addAreaLightToPrimitive(CR_addPrimitiveSphere(VEC3(5.4, 6.5, 0.0), 1.00, false, false, CR_addMaterialDiffuseReflection(VEC3_XYZ(0.0))), CR_addLightAreaDiffuseEmitter(VEC3_XYZ(unit_flux), true));
}

void scene_many_elements() {

    m_basic_camera.eye = { 0.0, 0.0, 50.0 };
    m_basic_camera.target = { 0.0, 0.0, -1.0 };
    m_basic_camera.up = { 0.0, 1.0, 0.0 };
    m_basic_camera.aperture = { 35.0 };
    m_basic_camera.cam_near = { 1.0 };
    m_basic_camera.cam_far = { 100.0 };
    CR_addCameraPerspective(m_basic_camera.eye, m_basic_camera.target, m_basic_camera.up, m_basic_camera.aperture, m_basic_camera.cam_near, m_basic_camera.cam_far);
    CR_addLightPointSpot(VEC3(0.0, 0.0, 100.0), VEC3(0.0, 0.0, 0.0), VEC3(1000.0, 1000.0, 1000.0), 30.0, 60.0, 1.0);

    CRAY_HANDLE material_green_id = CR_addMaterialDiffuseReflection(VEC3(0.0, 1.0, 0.0));

    int objects = 50000;
    cr_float radius = 0.5;
    cr_float objects_per_dim = std::floor(std::pow(objects, 1 / 3.0));
    cr_float start = -(objects_per_dim)*radius;
    cr_float end = -start;
    cr_float step = (end - start) / (objects_per_dim - 1);
    std::stringstream ss;
    for (cr_float i = start; i <= end; i += step) {
        for (cr_float j = start; j <= end; j += step) {
            for (cr_float k = start; k <= end; k += step) {
                CR_addPrimitiveSphere(VEC3(i, j, k + start), radius, false, false, material_green_id);
                ss << i << " " << j << " " << k << std::endl;
            }
        }
    }
}

void single_sphere() {
    m_basic_camera.eye = { 0.0, 0.0, 5.0 };
    m_basic_camera.target = { 0.0, -0.3, -1.0 };
    m_basic_camera.up = { 0.0, 1.0, 0.0 };
    m_basic_camera.aperture = { 35.0 };
    m_basic_camera.cam_near = { 0.01 };
    m_basic_camera.cam_far = { 100.0 };
    CR_addCameraPerspective(m_basic_camera.eye, m_basic_camera.target, m_basic_camera.up, m_basic_camera.aperture, m_basic_camera.cam_near, m_basic_camera.cam_far);
    CR_addLightPointSpot(VEC3(0.0, 10.0, 10.0), VEC3(0.0, 0.0, 0.0), VEC3(1000.0, 1000.0, 1000.0), 60.0, 120.0, 1.0);

    CRAY_HANDLE material_red_id = CR_addMaterialDiffuseReflection(VEC3(0.1, 0.1, 1.0));
    CR_addPrimitiveSphere(VEC3(0.0, 0.0, -150.5), 100, false, false, material_red_id);
    CRAY_HANDLE material_diffuse_orange_id = CR_addMaterialDiffuseTransmission(VEC3(0.2, 0.2, 0.2));
    CR_addPrimitiveSphere(VEC3(0.0, 0.0, -1.5), 0.5, false, false, material_diffuse_orange_id);

    // ground
    CRAY_HANDLE material_ground_id = CR_addMaterialDiffuseReflection(VEC3(0.8, 0.8, 0.3));
    CR_addPrimitiveSphere(VEC3(0, -100.5, -10.0), 100.0, false, false, material_ground_id);
}

void single_triangle() {
    m_basic_camera.eye = { 0.0, 0.0, 0.0 };
    m_basic_camera.target = { 0.0, 0.0, -1.0 };
    m_basic_camera.up = { 0.0, 1.0, 0.0 };
    m_basic_camera.aperture = { 45.0 };
    m_basic_camera.cam_near = { 0.01 };
    m_basic_camera.cam_far = { 100.0 };
    CR_addCameraPerspective(m_basic_camera.eye, m_basic_camera.target, m_basic_camera.up, m_basic_camera.aperture, m_basic_camera.cam_near, m_basic_camera.cam_far);
    CR_addLightPointSpot(VEC3(0.0, 0.0, 10.0), VEC3(0.0, 0.0, 0.0), VEC3(100.0, 100.0, 100.0), 30.0, 60.0, 1.0);
    CRAY_HANDLE material_gray_id = CR_addMaterialDiffuseReflection(VEC3(1.0, 0.5, 0.5));
    CRAY_HANDLE triangle_id = CR_addPrimitiveTriangle(false, false, material_gray_id);
    CR_addPrimitiveTriangleVertexPositions(triangle_id, 0, VEC3(-1.0, -0.5, -2.5));
    CR_addPrimitiveTriangleVertexPositions(triangle_id, 1, VEC3(1.0, -0.5, -2.5));
    CR_addPrimitiveTriangleVertexPositions(triangle_id, 2, VEC3(0.0, 1.0, -3.5));
}

void scene_demo() {
    m_basic_camera.eye = { 0.0, 0.0, 0.0 };
    m_basic_camera.target = { 0.0, 0.0, -1.0 };
    m_basic_camera.up = { 0.0, 1.0, 0.0 };
    m_basic_camera.aperture = { 45.0 };
    m_basic_camera.cam_near = { 0.01 };
    m_basic_camera.cam_far = { 100.0 };
    CR_addCameraPerspective(m_basic_camera.eye, m_basic_camera.target, m_basic_camera.up, m_basic_camera.aperture, m_basic_camera.cam_near, m_basic_camera.cam_far);
    CR_addLightPointSpot(VEC3(20.0, 20.0, 20.0), VEC3(0.0, 0.0, 0.0), VEC3(2000.0, 2000.0, 2000.0), 30.0, 60.0, 1.0);

    cr_float current_height = 4;
    const cr_float height_offset = 1.0;
    const int num_objects_per_material = 9;

    // diffuse
    for (int i = 0; i < num_objects_per_material; ++i) {
        cr_float color = (i / cr_float(num_objects_per_material - 1));
        CRAY_HANDLE material_id = CR_addMaterialDiffuseReflection(VEC3_XYZ(color));
        const cr_float current_x = -num_objects_per_material * 0.5 + i;
        CR_addPrimitiveSphere(VEC3(current_x, current_height, -15.0), 0.5, false, false, material_id);
    }

    // metal roughness
    current_height -= height_offset;
    for (int i = 0; i < num_objects_per_material; ++i) {
        cr_float roughness = i * 0.02 / cr_float(num_objects_per_material - 1);
        CRAY_HANDLE material_id = CR_addMaterialConductor(VEC3_XYZ(1.0), VEC3_XYZ(1.0), VEC3_XYZ(1.0), roughness);
        const cr_float current_x = -num_objects_per_material * 0.5 + i;
        CR_addPrimitiveSphere(VEC3(current_x, current_height, -15.0), 0.5, false, false, material_id);
    }

    // metal absoprtion
    current_height -= height_offset;
    cr_float etas[3] = { 0.1, 0.5f,1.0 };
    cr_float absorptions[3] = { 0.2, 0.5, 1.0 };
    for (int e = 0; e < 3; ++e) {
        for (int a = 0; a < 3; ++a) {
            CRAY_HANDLE material_id = CR_addMaterialConductor(VEC3_XYZ(1.0), VEC3_XYZ(etas[e]), VEC3_XYZ(absorptions[a]), 0.1);
            int i = e * 3 + a;
            const cr_float current_x = -num_objects_per_material * 0.5 + i;
            CR_addPrimitiveSphere(VEC3(current_x, current_height, -15.0), 0.5, false, false, material_id);
        }
    }

    // glass
    current_height -= height_offset;
    cr_float roughness[3] = { 0.0, 0.01, 0.1 };
    cr_float ior[3] = { 1.1, 1.3, 1.5 };
    for (int r = 0; r < 3; ++r) {
        for (int i = 0; i < 3; ++i) {
            int pos = r * 3 + i;
            CRAY_HANDLE material_id = CR_addMaterialDielectricSpecular(VEC3_XYZ(1.0), VEC3_XYZ(1.0), ior[i], roughness[r]);
            const cr_float current_x = -num_objects_per_material * 0.5 + pos;
            CR_addPrimitiveSphere(VEC3(current_x, current_height, -15.0), 0.5, false, false, material_id);
        }
    }

    // mirror
    current_height -= height_offset;
    for (int i = 0; i < num_objects_per_material; ++i) {
        cr_float mirror_roughness = i * 0.02 / (num_objects_per_material - 1);
        CRAY_HANDLE material_id = CR_addMaterialDielectricSpecularReflection(VEC3_XYZ(1.0), mirror_roughness);
        const cr_float current_x = -num_objects_per_material * 0.5 + i;
        CR_addPrimitiveSphere(VEC3(current_x, current_height, -15.0), 0.5, false, false, material_id);
    }
}

// eof ///////////////////////////////// CRayScenes
