# API 
Constructing and rendering a scene is relatively simple using the provided API. A complete API reference is available in [PDF](Docs/latex/refman.pdf) and [HTML](Docs/html/index.html) formats.

Note: as explained in the [Build](Build.md) document, a console and Qt GUI application frontend have also been implemented to quickly render scenes through an XML-based scene file format (such scene files and test models are provided in the [data](data) folder). These applications make heavy use of the API functions and can serve as a more advanced implementation guide.

## Common Interface
The following snippet shows how to perform initialization and basic setup for path tracing:

```cpp
# init
CR_init();
CR_setFilmDimensions(512, 512);
CR_setBlockSize(16);
CR_setNumThreads(12); // for a 12-core CPU

# add integrator
CR_setSamplesPerPixel(100);
CR_setRayDepth(10);
CR_setIntegrator(CR_INTEGRATOR_PT);

# build scene (see below)
buildScene();

# start rendering
CR_start();

# retrieve rendered image ptr
float* ptr = CR_getFilmFloatDataPtr());

# use the image, e.g. write to a file or display on screen

# release internal resources
CR_destroy();
```

The following snippet shows how to perform initialization and basic setup for path tracing, when integrating with a GUI application and/or when previewing using progressing rendering:

```cpp
# init phase
void init() {
    # init
    CR_init();
    CR_setFilmDimensions(512, 512);
    CR_setBlockSize(16);
    CR_setNumThreads(11); // leave the main thread alone, e.g. 11 threads for a 12-core CPU
    CR_useMainThread(false); // notify to not use the calling thread for rendering

    # add integrator
    CR_setSamplesPerPixel(100);
    CR_setRayDepth(10);
    CR_setIntegrator(CR_INTEGRATOR_PT);

    # build scene (see below)
    buildScene();

    # start rendering
    CR_start();
}

# render phase, e.g. within a render loop
void render() {
    # check if rendering has finished, e.g. to display certain information
    if (CR_finished()) {
        // ...
    }

    # retrieve the data pointer and display it
    uint8_t* rgb_ptr = CR_getFilmRGBDataPtr();
    if (rgb_ptr != nullptr) {
        // display the image
    }

    # Optional: retrieve the current progress and logs since the last iteration
    cr_float progress = CR_getProgressPercentage();
    printLogData();
}


# retrieve all log messages
void printLogData() {
    size_t str_length = 1024;
    std::unique_ptr<char[]> msg = std::make_unique<char[]>(str_length);
    CRAY_LOGGERENTRY type = CR_LOGGER_NOTHING;
    while (CR_getLastLogMessage(msg.get(), &str_length, &type)) {
        if (str_length > 1024) {
            // if string was bigger than the max, realloc and retrieve the message again
            msg = std::make_unique<char[]>(str_length);
            CR_getLastLogMessage(msg.get(), &str_length, &type);
            msg = std::make_unique<char[]>(str_length);
        }
        // display the message
        std::cout << msg.get();
    }
}

# end (not necessary since resources are released on exit, but enables cleaner reusability)
void finish() {
    # release internal resources
    CR_destroy();
}
```

## Scene Generation
The following snippet shows how to create a basic Cornell box with an area light and various materials:

```cpp
void buildScene() {
    # build cornell box
    # create a camera at the center, looking at -Z, with a 35 degree aperture
    CR_addCameraPerspective(VEC3(0,0,0), VEC3(0,0,-1), VEC3(0,1,0), 35, 0.1, 100);

    # add the rectangle area light
    CRAY_HANDLE light_id = CR_addLightAreaDiffuseEmitter(VEC3(17.0, 12.0, 4.0), true);
    CRAY_HANDLE primitive_id = CR_addPrimitiveRectangleXZ(-0.2, 0.2, -0.2, 0.2, 0.999, false, false, CR_addMaterialDiffuseReflection(VEC3_XYZ(0.0)));
    CR_addAreaLightToPrimitive(primitive_id, light_id);

    # add two spheres, one perfect mirror, one matte
    CRAY_HANDLE material_mirror_id = CR_addMaterialDielectricSpecularReflection(VEC3_XYZ(1.0), 0.0);
    CR_addPrimitiveSphere(VEC3(-0.5, -0.75, 0.0), 0.25, false, false, material_mirror_id);
    CRAY_HANDLE material_diffuse_orange_id = CR_addMaterialDiffuseReflection(VEC3(0.8, 0.8, 0.3));
    CR_addPrimitiveSphere(VEC3(0.5, -0.75, 0.0), 0.25, false, false, material_diffuse_orange_id);

    # add the room rectangles
    CRAY_HANDLE material_red_id = CR_addMaterialDiffuseReflection(VEC3(0.63, 0.065, 0.05));
    CRAY_HANDLE material_white_id = CR_addMaterialDiffuseReflection(VEC3(0.725, 0.71, 0.68));
    CRAY_HANDLE material_green_id = CR_addMaterialDiffuseReflection(VEC3(0.14, 0.45, 0.091));
    CR_addPrimitiveRectangleXZ(-1.0, 1.0, -1.0, 1.0, 1.0, false, false, material_white_id);   // top
    CR_addPrimitiveRectangleXZ(-1.0, 1.0, -1.0, 1.0, -1.0, true, false, material_white_id);   // bottom
    CR_addPrimitiveRectangleXY(-1.0, 1.0, -1.0, 1.0, -1.0, false, false, material_white_id);  // back
    CR_addPrimitiveRectangleYZ(-1.0, 1.0, -1.0, 1.0, -1.0, true, false, material_red_id);     // left
    CR_addPrimitiveRectangleYZ(-1.0, 1.0, -1.0, 1.0, 1.0, false, false, material_green_id);   // right
}
```
