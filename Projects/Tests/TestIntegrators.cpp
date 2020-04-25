/*
File:  TestIntegrators.cpp

Description: TestIntegrators Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include <thread>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <cstdio>
#include "CRay/CRay.h"
#include "Scenes/SceneLoader.h"
#include "Scenes/SceneLoaderHelper.h"

#include <stdexcept>
#define CRAY_THROW                                   { CR_destroy(); closeFile(); throw std::runtime_error("Failed"); }
#define CRAY_BREAK                                   CRAY_THROW

#define SOURCEPATH_LENGTH                               strlen(SOURCEPATH) + 1
#define SOURCE_FILENAME                                 &__FILE__[SOURCEPATH_LENGTH]

#include <string>
#define FILE_NAME(name) ((std::string(name).find(SOURCEPATH) != std::string::npos) ? std::string(SOURCE_FILENAME) : __FILE__)

// the following defines are for asserts, popup dialogs and log messages
// asserts
#define WRITE_MESSAGE(x)                                { std::ostringstream ss; ss << x; writeMessage(ss.str().c_str()); }
#define CRAY_ASSERT_IF_FALSE_MSG(expr, msg)             { if (!static_cast<bool>(expr)){WRITE_MESSAGE("Assert: " << FILE_NAME(__FILE__) << " (" << __LINE__  << ") " << #expr << " Msg: " << msg << std::endl); CRAY_BREAK} }
#define CRAY_ASSERT_ALWAYS_MSG(msg)                     { WRITE_MESSAGE("Assert: " << FILE_NAME(__FILE__) << " (" << __LINE__  << ") " << " Msg: " << msg << std::endl); CRAY_BREAK }
#define SAFE_DELETE(_x)                                 { if ((_x) != nullptr) { delete   (_x);  _x = nullptr; } }
#define SAFE_DELETE_ARRAY_POINTER(_x)                   { if ((_x) != nullptr) { delete[] (_x);  _x = nullptr; } }

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

static FILE* log_file = nullptr;

void openFile(bool clear) {
#ifdef _WIN32
    _mkdir("Logs");
#else
    mkdir("Logs", 0777);
#endif
    log_file = fopen("Logs/test_log.txt", clear ? "w" : "a");
}

void closeFile() {
    if (log_file) {
        fclose(log_file);
    }
    log_file = nullptr;
}

void writeVerboseLog(const char* str) {
    if (log_file) {
        fputs(str, log_file);
    }
}

void writeMessage(const char* str) {
    writeVerboseLog(str);
    fputs(str, stdout);
    fflush(stdout);
}

void readLogs() {
    size_t str_length = 1024;
    std::unique_ptr<char[]> msg = std::make_unique<char[]>(str_length);
    CRAY_LOGGERENTRY type = CR_LOGGER_NOTHING;
    while (CR_getLastLogMessage(msg.get(), &str_length, &type)) {
        if (str_length > 1024) {
            // if string was bigger than the max, get it again
            msg = std::make_unique<char[]>(str_length);
            CR_getLastLogMessage(msg.get(), &str_length, &type);
            str_length = 1024;
            msg = std::make_unique<char[]>(str_length);
        }
        WRITE_MESSAGE(msg.get());
    }
}

void loaded_scene_info() {
    std::string err = get_scene_error_log();
    if (!err.empty()) {
        WRITE_MESSAGE(err.c_str());
    }
    std::string warn = get_scene_warning_log();
    if (!warn.empty()) {
        WRITE_MESSAGE(warn.c_str());
    }
    std::string info = get_scene_info_log();
    if (!info.empty()) {
        WRITE_MESSAGE(info.c_str());
    }
}

void buildScene(const std::string& filename) {
    bool res = load_scene_xml(filename.c_str());
    loaded_scene_info();
    if (!res) {
        clear_scene_xml();
        CRAY_ASSERT_ALWAYS_MSG("Scene " << filename << " could not be loaded");
    }
    res = generate_scene_xml();
    loaded_scene_info();
    if (!res) {
        clear_scene_xml();
        CRAY_ASSERT_ALWAYS_MSG("Scene " << filename << " could not be generated");
    }
    WRITE_MESSAGE("Scene " << filename << " loaded\n");
}

int32_t getPixelLocation(int row, int col, int width, int height, int num_channels) {
    return (row < 0 || row >= height || col < 0 || col >= width) ? -1 : (row * width * num_channels) + (col * num_channels);
}

template<typename T>
float compareImages(int neighbourhood, float allowed_threshold, float& percentage_diff, float& max_difference_found, const void* _source_ptr, const void* _test_ptr, void* _failed_image_ptr) {
    const T* source_ptr = static_cast<const T*>(_source_ptr);
    const T* test_ptr = static_cast<const T*>(_test_ptr);
    T* failed_image_ptr = static_cast<T*>(_failed_image_ptr);
    int32_t width = CR_getFilmWidth();
    int32_t height = CR_getFilmHeight();
    int32_t num_channels = CR_getFilmChannels();
    int64_t num_elements = width * height * num_channels;

    int32_t pixels[9];
    float max_value = std::numeric_limits<float>::min();
    float min_value = std::numeric_limits<float>::max();
    float src_img = 0;
    float test_img = 0;
    float img_total = 0;
    float different_pixels = 0;
    WRITE_MESSAGE("Started comparing images in neighbourhood of " << neighbourhood << " pixels and tolerance of " << allowed_threshold << std::endl);
    for (int32_t row = 0; row < height; row++) {
        for (int32_t col = 0; col < width; col++) {
            pixels[0] = getPixelLocation(row, col, width, height, num_channels);
            pixels[1] = getPixelLocation(row - 1, col + 0, width, height, num_channels);
            pixels[2] = getPixelLocation(row + 1, col + 0, width, height, num_channels);
            pixels[3] = getPixelLocation(row + 0, col - 1, width, height, num_channels);
            pixels[4] = getPixelLocation(row + 0, col + 1, width, height, num_channels);
            pixels[5] = getPixelLocation(row - 1, col + 1, width, height, num_channels);
            pixels[6] = getPixelLocation(row + 1, col - 1, width, height, num_channels);
            pixels[7] = getPixelLocation(row - 1, col - 1, width, height, num_channels);
            pixels[8] = getPixelLocation(row + 1, col + 1, width, height, num_channels);

            bool pixel_same = true;
            for (int32_t channel = 0; channel < num_channels; channel++) {
                img_total = 0;
                src_img = 0;
                test_img = 0;

                if (source_ptr[pixels[0] + channel] < min_value) {
                    min_value = source_ptr[pixels[0] + channel];
                }
                if (source_ptr[pixels[0] + channel] > max_value) {
                    max_value = source_ptr[pixels[0] + channel];
                }

                for (int32_t p = 0; p < neighbourhood; ++p) {
                    if (pixels[p] >= 0 && pixels[p] < num_elements) {
                        src_img += source_ptr[pixels[p] + channel];
                        test_img += test_ptr[pixels[p] + channel];
                        ++img_total;
                    }
                }
                float src_pixel_mean = src_img / img_total;
                float test_pixel_mean = test_img / img_total;

                // absolute difference
                float difference = std::abs(src_pixel_mean - test_pixel_mean);
                bool is_different = difference > allowed_threshold;
                if (is_different) {
                    if (pixel_same) {
                        ++different_pixels;
                        pixel_same = false;
                    }
                    int failed_pixel_location = pixels[0] + channel;
                    failed_image_ptr[failed_pixel_location] = static_cast<T>(difference);
                    //WRITE_MESSAGE("Pixel: " << col << ", " << row << ", " << channel << ": Values (rendered/test/diff) : " << src_pixel_mean << "/" << test_pixel_mean << "/" << difference << std::endl);
                }
                if (difference > max_difference_found) {
                    max_difference_found = difference;
                }
            }
        }
    }

    percentage_diff = 100.0f * different_pixels / float(width * height);
    WRITE_MESSAGE("Finished comparing images. Min value: " << min_value << ". Max value: " << max_value << ". Max difference found: " << max_difference_found << ". Total different pixels: "
        << std::fixed << std::setprecision(2) << different_pixels << " out of " << width * height << " (" << percentage_diff << "%)" << std::endl);

    return different_pixels;
}

std::string getPath(const std::string& str) {
    size_t pos = std::string(str).find_last_of("\\/");
    std::string path = "";
    if (pos != std::string::npos) {
        path = std::string(str).substr(0, pos);
        path += "/";
    }
    return path;
}

std::string getFile(const std::string& str) {
    size_t pos = std::string(str).find_last_of("\\/");
    std::string file = str;
    if (pos != std::string::npos && pos + 1 < str.size()) {
        file = std::string(str).substr(pos + 1);
    }
    return file;
}

int main(int argc, char *argv[]) {
    bool clear_log = false;
    clear_log = argc > 1 && SCENE_LOADER::CompareStrings(argv[1], "--clearlog");
    openFile(clear_log);

    CR_init();

    bool use_rgb = false;
    bool generate = false;
    bool screenshot = false;
    std::string test_path = getPath(argv[0]);

    std::string test_name = "";
    std::string scene_path = "";
    std::string scene_filename = "";
    std::string integrator_name = "";
    CRAY_INTEGRATOR integrator = CR_INTEGRATOR_NONE;
    for (int i = 1; i < argc; ++i) {
        std::string param = argv[i];
        if (SCENE_LOADER::CompareStrings(param.c_str(), "--generate")) {
            generate = SCENE_LOADER::CompareStrings(argv[++i], "true");
        }
        else if (SCENE_LOADER::CompareStrings(param.c_str(), "--screenshot")) {
            screenshot = SCENE_LOADER::CompareStrings(argv[++i], "true");
        }
        else if (SCENE_LOADER::CompareStrings(param.c_str(), "--rgb")) {
            use_rgb = SCENE_LOADER::CompareStrings(argv[++i], "true");
        }
        else if (SCENE_LOADER::CompareStrings(param.c_str(), "--test_path")) {
            test_path = argv[++i];
            test_path.append("/");
        }
        else if (SCENE_LOADER::CompareStrings(param.c_str(), "--test_name")) {
            test_name = argv[++i];
        }
        else if (SCENE_LOADER::CompareStrings(param.c_str(), "--scene")) {
            std::string scene_full_path = argv[++i];
            scene_path = getPath(scene_full_path);
            scene_filename = getFile(scene_full_path);
        }
        else if (SCENE_LOADER::CompareStrings(param.c_str(), "--integrator")) {
            param = argv[++i];
            if (SCENE_LOADER::CompareStrings(param.c_str(), "colorintegrator")) {
                integrator = CR_INTEGRATOR_TEST_COLOR;
            }
            else if (SCENE_LOADER::CompareStrings(param.c_str(), "cameraintegrator")) {
                integrator = CR_INTEGRATOR_TEST_CAMERA;
            }
            else if (SCENE_LOADER::CompareStrings(param.c_str(), "noiseintegrator")) {
                integrator = CR_INTEGRATOR_TEST_NOISE;
            }
            else if (SCENE_LOADER::CompareStrings(param.c_str(), "uvintegrator")) {
                integrator = CR_INTEGRATOR_TEST_UV;
            }
            else if (SCENE_LOADER::CompareStrings(param.c_str(), "normalintegrator")) {
                integrator = CR_INTEGRATOR_TEST_NORMAL;
            }
            else if (SCENE_LOADER::CompareStrings(param.c_str(), "materialintegrator")) {
                integrator = CR_INTEGRATOR_TEST_MATERIAL;
            }
            else if (SCENE_LOADER::CompareStrings(param.c_str(), "aointegrator")) {
                integrator = CR_INTEGRATOR_AO;
            }
            else if (SCENE_LOADER::CompareStrings(param.c_str(), "pathintegrator")) {
                integrator = CR_INTEGRATOR_PT;
            }
            else {
                CRAY_ASSERT_ALWAYS_MSG("No integrator found. Error");
            }
            integrator_name = param;
        }
    }

    // test if file can be written, to avoid rendering early
    std::string filename_write_test = test_path + "test_integrators_test_file.bmp";
    std::ofstream myfile;
    myfile.open(filename_write_test.c_str(), std::ios::out | std::ios::binary);
    if (!myfile.is_open()) {
        CRAY_ASSERT_ALWAYS_MSG("The image cannot be created on the given path: " << filename_write_test << ". Please provide a correct filepath." << std::endl);
    }
    else {
        // delete it
        myfile.close();
        std::remove(filename_write_test.c_str());
    }

    if (!screenshot) {
        CRAY_ASSERT_IF_FALSE_MSG(test_name.empty() == false, "No test name provided");
        WRITE_MESSAGE("Started test " << test_name << std::endl);
    }
    else {
        CRAY_ASSERT_IF_FALSE_MSG(test_name.empty() == false, "No name provided");
        WRITE_MESSAGE("Started generating screenshot " << test_name << std::endl);
    }

    if (!scene_filename.empty()) {
        buildScene(scene_path + scene_filename);
    }
    else {
        CR_setSamplesPerPixel(100);
        CR_setFilmDimensions(512, 512);
        CR_setBlockSize(16);
        CR_setExposure(0.0);
        CR_setGamma(1.0);
    }
    if (integrator == CR_INTEGRATOR_TEST_COLOR) {
        CR_testColorIntegrator();
        CR_setExposure(0.0);
        CR_setGamma(1.0);
    }
    else if (integrator == CR_INTEGRATOR_TEST_CAMERA) {
        CR_testCameraIntegrator();
        CR_setExposure(0.0);
        CR_setGamma(1.0);
    }
    else if (integrator == CR_INTEGRATOR_TEST_NOISE) {
        CR_testNoiseIntegrator();
        CR_setExposure(0.0);
        CR_setGamma(1.0);
    }
    else if (integrator == CR_INTEGRATOR_TEST_UV) {
        CR_testUVIntegrator();
        CR_setExposure(0.0);
        CR_setGamma(1.0);
    }
    else if (integrator == CR_INTEGRATOR_TEST_NORMAL) {
        CR_testNormalIntegrator();
        CR_setExposure(0.0);
        CR_setGamma(1.0);
    }
    else if (integrator == CR_INTEGRATOR_TEST_MATERIAL) {
        CR_testMaterialIntegrator();
        CR_setExposure(0.0);
        CR_setGamma(1.0);
    }
    else if (integrator == CR_INTEGRATOR_AO) {
        CR_setIntegrator(CR_INTEGRATOR_AO);
        CR_setExposure(0.0);
        CR_setGamma(1.0);
    }
    else if (integrator == CR_INTEGRATOR_PT) {
        CR_setIntegrator(CR_INTEGRATOR_PT);
    }

    //CR_useFastRNG(true);
    WRITE_MESSAGE("Max system threads/cores: " << std::thread::hardware_concurrency() << std::endl);
    CR_setNumThreads(std::max(1, static_cast<int32_t>(std::thread::hardware_concurrency())));
    CR_useMainThread(true);
    CR_setMinimumLogLevel(CR_LOGGER_INFO);
    CR_enableRGBBuffer(use_rgb);
    CR_setLogFile(log_file);
    CR_printTostdout(true);
    CR_printProgressBar(false);
    CR_start();

    readLogs();

    WRITE_MESSAGE("Finished rendering" << std::endl);
    bool is_hdr = !CR_isRGBBufferEnabled();
    std::string ext = is_hdr ? ".hdr" : ".bmp";

    int32_t width = CR_getFilmWidth();
    int32_t height = CR_getFilmHeight();
    int32_t num_channels = CR_getFilmChannels();

    const void* ptr = nullptr;
    if (is_hdr) {
        ptr = static_cast<const void*>(CR_getFilmFloatDataPtr());
    }
    else {
        ptr = static_cast<const void*>(CR_getFilmRGBDataPtr());
    }
    CRAY_ASSERT_IF_FALSE_MSG(ptr != nullptr, "Rendered framebuffer empty");

    WRITE_MESSAGE("Save location: " << test_path << std::endl);

    // hack code to generate screenshots with test integrators
    if (generate || screenshot) {
        std::string test_filename_with_path = test_path + test_name + ext;
        if (screenshot) {
            WRITE_MESSAGE("Saving screenshot: " << test_name << ext << std::endl);
        }
        else {
            WRITE_MESSAGE("Test generated. Saving test image: " << test_name << ext << std::endl);
        }

        if (is_hdr) {
            stbi_write_hdr(test_filename_with_path.c_str(), width, height, num_channels, static_cast<const float*>(ptr));
        }
        else {
            stbi_write_bmp(test_filename_with_path.c_str(), width, height, num_channels, static_cast<const uint8_t*>(ptr));
        }
        CR_destroy();
        WRITE_MESSAGE("Finished " << test_name << std::endl);
        closeFile();
        return 0;
    }

    // clean old files
    std::string rendered_filename = test_name + "_rendered" + ext;
    std::string rendered_filename_with_path = test_path + rendered_filename;
    std::string diff_filename = test_name + "_diff" + ext;
    std::string diff_filename_with_path = test_path + diff_filename;
    WRITE_MESSAGE("Deleting old files " << rendered_filename << ", " << diff_filename << " from last test iteration" << std::endl);
    std::remove(rendered_filename_with_path.c_str());
    std::remove(diff_filename_with_path.c_str());

    std::string test_filename = test_name + TESTSUFFIX + ext;
    std::string test_filename_with_path = test_path + test_filename;
    bool test_failed = false;

    int32_t test_width = 0;
    int32_t test_num_channels = 0;
    int32_t test_height = 0;
    int64_t num_elements = width * height * num_channels;
    void* source_data_ptr = nullptr;
    void* test_data_ptr = nullptr;
    void* failed_image_ptr = nullptr;
    // store and load the rendered file in order to test against the stored images, accounting for any differences during file I/O
    if (is_hdr) {
        stbi_write_hdr(rendered_filename_with_path.c_str(), width, height, num_channels, static_cast<const float*>(ptr));
        source_data_ptr = stbi_loadf(rendered_filename_with_path.c_str(), &test_width, &test_height, &test_num_channels, 3);
        test_data_ptr = stbi_loadf(test_filename_with_path.c_str(), &test_width, &test_height, &test_num_channels, 3);
        failed_image_ptr = new float[num_elements];
        memset(failed_image_ptr, 0, num_elements * sizeof(float));
    }
    else {
        stbi_write_bmp(rendered_filename_with_path.c_str(), width, height, num_channels, ptr);
        source_data_ptr = stbi_load(rendered_filename_with_path.c_str(), &test_width, &test_height, &test_num_channels, 3);
        test_data_ptr = stbi_load(test_filename_with_path.c_str(), &test_width, &test_height, &test_num_channels, 3);
        failed_image_ptr = new uint8_t[num_elements];
        memset(failed_image_ptr, 0, num_elements);
    }
    CRAY_ASSERT_IF_FALSE_MSG(test_data_ptr != nullptr, "Test image " << test_filename_with_path << " not found");
    CRAY_ASSERT_IF_FALSE_MSG(test_width == width, "Different dimensions between rendered and test image (" << test_width << "!=" << width << ")");
    CRAY_ASSERT_IF_FALSE_MSG(test_height == height, "Different dimensions between rendered and test image (" << test_height << "!=" << height << ")");
    CRAY_ASSERT_IF_FALSE_MSG(test_num_channels == num_channels, "Different channels between rendered and test image (" << test_num_channels << "!=" << num_channels << ")");

    float different_pixels = 0;
    float max_difference_found = 0;
    float threshold = 0.0;
    float threshold_offset = 0.0f;
    int neighbourhood = 1;
    float percentage_diff = 0.0;
    if (integrator != CR_INTEGRATOR_TEST_COLOR && integrator != CR_INTEGRATOR_TEST_CAMERA && integrator != CR_INTEGRATOR_TEST_NOISE) {
        neighbourhood = 9;
        threshold_offset = is_hdr ? 0.3f : 5.0f;
    }
    if (is_hdr) {
        threshold = std::numeric_limits<float>::epsilon() + threshold_offset;
        different_pixels = compareImages<float>(neighbourhood, threshold, percentage_diff, max_difference_found, source_data_ptr, test_data_ptr, failed_image_ptr);
    }
    else {
        threshold = std::numeric_limits<uint8_t>::epsilon() + threshold_offset;
        different_pixels = compareImages<uint8_t>(neighbourhood, threshold, percentage_diff, max_difference_found, source_data_ptr, test_data_ptr, failed_image_ptr);
    }

    if (different_pixels > 0 && max_difference_found > threshold) {
        if (percentage_diff > 1) {
            WRITE_MESSAGE("Found more pixel differences than allowed. Test failed" << std::endl);
            WRITE_MESSAGE("Saving rendered image: " << rendered_filename << " and failed difference image: " << diff_filename << std::endl);
            test_failed = true;
        }
        else {
            WRITE_MESSAGE("Less than 1% of pixels are different (" << percentage_diff << "%). Conditionally accept." << std::endl);
            WRITE_MESSAGE("Saving rendered image: " << rendered_filename << " and difference image: " << diff_filename << std::endl);
        }

        if (is_hdr) {
            stbi_write_hdr(diff_filename_with_path.c_str(), width, height, num_channels, static_cast<const float*>(failed_image_ptr));
        }
        else {
            stbi_write_bmp(diff_filename_with_path.c_str(), width, height, num_channels, static_cast<const uint8_t*>(failed_image_ptr));
        }
    }
    else {
        std::remove(rendered_filename_with_path.c_str());
    }

    if (is_hdr) {
        float* fptr = static_cast<float*>(failed_image_ptr);
        SAFE_DELETE_ARRAY_POINTER(fptr)
        failed_image_ptr = nullptr;
    }
    else {
        uint8_t* uptr = static_cast<uint8_t*>(failed_image_ptr);
        SAFE_DELETE_ARRAY_POINTER(uptr)
        failed_image_ptr = nullptr;
    }
    if (source_data_ptr != nullptr) {
        stbi_image_free(source_data_ptr);
        source_data_ptr = nullptr;
    }
    if (test_data_ptr != nullptr) {
        stbi_image_free(test_data_ptr);
        test_data_ptr = nullptr;
    }

    CR_destroy();

    CRAY_ASSERT_IF_FALSE_MSG(test_failed == false, "Image comparison failed");

    WRITE_MESSAGE("Finished test " << test_name);

    closeFile();

    return 0;
}
