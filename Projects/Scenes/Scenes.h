/*
File:  Scenes.h

Description: Scenes Header File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifndef SCENES_H
#define SCENES_H

#include "CRay/CRay.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BasicCamera {
    cr_vec3 eye;
    cr_vec3 target;
    cr_vec3 up;
    cr_float aperture;
    cr_float cam_near;
    cr_float cam_far;
} BasicCamera;
extern BasicCamera m_basic_camera;
void scene_obj();
void scene_simple_spheres();
void scene_cornell();
void scene_mis();
void scene_many_elements();
void single_sphere();
void single_triangle();
void scene_demo();
#ifdef __cplusplus
};
#endif
#endif //SCENES_H

// eof /////////////////////////////////
