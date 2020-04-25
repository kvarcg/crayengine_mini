/*
File:  Console_example.h

Description: Console_example Source File

Authors:
Kostas Vardis

This is a free engine. If you use this code as is or any part of it in any kind of project or product, please
acknowledge the source and its author(s).
*/

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h> // used for OutputDebugString, isDebuggerPresent
#endif

#if defined (_WIN32) && defined (_MSC_VER)
#define WINMSVC
#endif

#if defined (WINMSVC) && !defined(NDEBUG)
#define WINMSVCDEBUG
#endif

#ifdef WINMSVCDEBUG
#include <crtdbg.h>
#define my_malloc(s) _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define my_free(s) _free_dbg(s, _NORMAL_BLOCK)
#define _CRTDBG_MAP_ALLOC
#else
#define my_malloc malloc
#define my_free free
#endif // WINMSVCDEBUG

#define SAFE_FREE(_x)         { if ((_x) != NULL) { my_free    ((_x)); _x = NULL; } }

#include "Scenes/SceneLoader.h"
#include "Scenes/Scenes.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include <math.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#include <pthread.h>
#include <unistd.h> // for usleep
#include <time.h>
#include <sys/stat.h>
#else
#include <direct.h>
#endif

void printHelp(void);
bool parseOptions(int argc, char* argv[]);
size_t getPathLength(const char* str);

static FILE* log_file = NULL;

#ifdef _WIN32
#define MAKE_STR_LEN(len, fmt, args) int len = _vscprintf(fmt, args) + 1
#else                       
#define MAKE_STR_LEN(len, fmt, args) \
va_list argcopy;            \
va_copy(argcopy, args);     \
int len = vsnprintf(NULL, 0, fmt, argcopy) + 1; \
va_end(argcopy);
#endif

#define MAKE_STR_VARGS(fmt, text) \
va_list args;                   \
va_start(args, fmt);            \
MAKE_STR_LEN(len, fmt, args);   \
if (len > 1) {                  \
text = (char*)my_malloc(len * sizeof(char)); \
vsnprintf(text, len, fmt, args); } \
else { va_end(args); }

char* makeStr(const char* fmt, ...) {
    char* text = NULL;
    MAKE_STR_VARGS(fmt, text);
    return text;
}

void printMessage(const char* fmt, ...) {
    char* text = NULL;
    MAKE_STR_VARGS(fmt, text);
    if (text == NULL) {
        return;
    }
#ifdef WINMSVCDEBUG
    if (IsDebuggerPresent()) {
        OutputDebugStringA(text);
    }
#endif
    if (log_file) {
        fputs(text, log_file);
    }
    fputs(text, stdout);
    fflush(stdout);
    SAFE_FREE(text);
}
size_t getPathLength(const char* str) {
    char* delimiterPos = (strrchr(str, '/'));
    if (delimiterPos == 0) {
        delimiterPos = (strrchr(str, '\\'));
    }
    if (delimiterPos == 0) {
        return 0;
    }
    uintptr_t slash_pos = (uintptr_t)delimiterPos;
    uintptr_t start_pos = (uintptr_t)&str[0];
    return slash_pos - start_pos + 2;
}

const char* get_file(const char* filename) {
    char* slash = strrchr(filename, '\\');
    if (slash == NULL) {
        slash = strrchr(filename, '/');
    }
    return (slash == NULL) ? filename : slash + 1;
}

const char* get_extension(const char* filename) {
    char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

void openFile() {
#ifdef _WIN32
    _mkdir("Logs");
#else
    mkdir("Logs", 0777);
#endif
    log_file = fopen("Logs/console_log.txt", "w");
}

void closeFile() {
    if (log_file) {
        fclose(log_file);
    }
    log_file = NULL;
}

typedef struct Options {
    char* default_scene;
    char* default_path;
    const char* scene;
    const char* output_name;
    char* full_output_name;
    bool has_custom_output_name;
    char* output_path;
    const char* output_ext;
    int num_threads;
    int width;
    int height;
    int block_size;
    int spp;
    int log_level;
    int integrator;
    bool is_hdr;

    int ao_spp;
    float ao_range;

    int pt_ray_depth;
    bool pt_russian_roulette;
} Options;
static Options options;

void makeDefaultOptions(char* argv[]) {

    options.default_scene = NULL;
#ifdef WINMSVCDEBUG
    if (IsDebuggerPresent()) {
        options.default_scene = makeStr("%s%s", SOURCEPATH, "/data/materials_test1.scene");
    }
#endif // WINMSVCDEBUG

    // filenames and paths
    options.full_output_name = NULL;
    options.scene = NULL;
    options.output_name = "result";
    options.output_path = "";
    options.default_path = "";
    options.has_custom_output_name = false;
    size_t path_length = getPathLength(argv[0]);
    if (path_length > 0) {
        char* path = (char*)my_malloc(sizeof(char) * path_length);
        memcpy(path, argv[0], path_length);
        path[path_length - 1] = '\0';
        options.output_path = path;
        path = (char*)my_malloc(sizeof(char) * path_length);
        memcpy(path, argv[0], path_length);
        path[path_length - 1] = '\0';
        options.default_path = path;
    }

    // settings
    options.num_threads = 1;
    options.width = 512;
    options.height = 512;
    options.block_size = 16;
    options.log_level = CR_LOGGER_ERROR;
    options.is_hdr = true;
    options.output_ext = "hdr";

    // scene data
    options.spp = 50;
    options.integrator = CR_INTEGRATOR_PT;
    options.ao_spp = 1;
    options.ao_range = 0;
    options.pt_ray_depth = 10;
    options.pt_russian_roulette = true;
}

void loaded_scene_info() {
    const char* err = get_scene_error_log();
    if (strcmp(err, "") != 0) {
        printMessage(err);
    }
    const char* warn = get_scene_warning_log();
    if (strcmp(warn, "") != 0) {
        printMessage(warn);
    }
    const char* info = get_scene_info_log();
    if (strcmp(info, "") != 0) {
        printMessage(info);
    }
}

void destroy(bool print_message) {
    if (strcmp(options.output_path, "") != 0) {
        SAFE_FREE(options.output_path);
    }
    SAFE_FREE(options.full_output_name);
    SAFE_FREE(options.default_scene);
    SAFE_FREE(options.default_path);
    CR_destroy();
    closeFile();
    if (print_message) {
        printMessage("Info: Destroyed engine\n");
    }
}

bool validateOutput() {
    SAFE_FREE(options.full_output_name);
    if (!options.has_custom_output_name) {
        options.output_name = CR_getOutputName();
        const char* ext = get_extension(options.output_name);
        if (strcmp(ext, "") == 0) {
            ext = options.output_ext;
        }
        else {
            options.output_ext = ext;
        }
        options.full_output_name = makeStr("%s.%s", options.output_name, options.output_ext);
    }
    else {
        options.full_output_name = makeStr("%s%s", options.output_path, options.output_name);
    }

    // test if file can be written, to avoid rendering early
    FILE* f = fopen(options.full_output_name, "wb");
    if (f != NULL) {
        fclose(f);
        // delete it
        remove(options.full_output_name);
    }
    else {
        printMessage("Error: The image cannot be created on the given path: %s. Please provide a correct filepath.\n", options.full_output_name);
        return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    openFile();

    if (!parseOptions(argc, argv)) {
        printHelp();
        destroy(false);
        return 0;
    }
    bool res = load_scene_xml(options.scene);
    loaded_scene_info();
    if (!res) {
        destroy(true);
        return 0;
    }

    CR_destroy();
    CR_init();

    res = generate_scene_xml();
    loaded_scene_info();
    if (!res) {
        destroy(true);
        return 0;
    }
    clear_scene_xml();

    if (!validateOutput()) {
        destroy(true);
        return 0;
    }

    CR_enableRGBBuffer(!options.is_hdr);

    printMessage("Info: Loaded scene file: %s\n", options.scene);
    CR_setLogFile(log_file);
    CR_printTostdout(true);

    CR_start();

    bool is_hdr = !CR_isRGBBufferEnabled();
    const void* ptr = NULL;
    if (is_hdr) {
        ptr = (const void*)(CR_getFilmFloatDataPtr());
    }
    else {
        ptr = (const void*)(CR_getFilmRGBDataPtr());
    }
    int32_t width = CR_getFilmWidth();
    int32_t height = CR_getFilmHeight();
    int32_t num_channels = CR_getFilmChannels();

    printMessage("Info: Saving output to %s\n", options.full_output_name);
    if (!strcmp(options.output_ext, "hdr")) {
        stbi_write_hdr(options.full_output_name, width, height, num_channels, (const float*)(ptr));
    }
    else if (!strcmp(options.output_ext, "png")) {
        stbi_write_png(options.full_output_name, width, height, num_channels, ptr, width * num_channels);
    }
    else if (!strcmp(options.output_ext, "jpg")) {
        stbi_write_jpg(options.full_output_name, width, height, num_channels, ptr, 95);
    }
    else if (!strcmp(options.output_ext, "bmp")) {
        stbi_write_bmp(options.full_output_name, width, height, num_channels, (const uint8_t*)(ptr));
    }

    destroy(true);
    return 0;
}

void printHelp() {
    const char* help_text = "List of available commands:\n"
        " --scene       # full path of the scene to load. (Default: None -> MUST BE PROVIDED). \n"
        " --output      # filename to save. (Default: <current directory>/result.hdr). Supported formats: bmp/png/jpg/hdr\n"
        " --help        # displays this message\n";
    printMessage(help_text);
}

bool validateArgument(const char* token, int index, int num_arguments) {
    if (index + 1 >= num_arguments) {
        printMessage("Error: Incorrect argument: %s\n", token);
        return false;
    }
    return true;
}

bool parseOptions(int argc, char* argv[]) {
    makeDefaultOptions(argv);
    for (int i = 1; i < argc; ++i) {
        const char* token = argv[i];
        if (strcmp("--scene", token) == 0 && validateArgument("--scene", i, argc)) {
            options.scene = argv[++i];
        }
        else if (strcmp("--output", token) == 0 && validateArgument("--output", i, argc)) {
            const char* full_output = argv[++i];
            // get path
            if (strcmp(options.output_path, "") != 0) {
                SAFE_FREE(options.output_path);
            }
            options.output_path = "";
            size_t path_length = getPathLength(full_output);
            if (path_length > 0) {
                char* path = (char*)my_malloc(sizeof(char) * path_length);
                memcpy(path, full_output, path_length);
                path[path_length - 1] = '\0';
                options.output_path = path;
            }

            // get file
            options.output_name = get_file(full_output);
            if (!strcmp(options.output_name, "") || options.output_name[0] == '.') {
                printMessage("Error: Invalid output name: %s for file: %s\n", options.output_name, full_output);
                return false;
            }
            // get extension
            options.output_ext = get_extension(full_output);
            if (!strcmp(options.output_ext, "hdr")) {
                options.is_hdr = true;
            }
            else if (!strcmp(options.output_ext, "png") || !strcmp(options.output_ext, "jpg") || !strcmp(options.output_ext, "bmp")) {
                options.is_hdr = false;
            }
            else {
                printMessage("Error: Invalid output format: %s for file: %s\n", strcmp(options.output_ext, "") ? options.output_ext : "empty", full_output);
                return false;
            }
            options.has_custom_output_name = true;
        }
        else if (strcmp("--help", token) == 0) {
            return false;
        }
        else {
            printMessage("Error: Invalid argument: %s\n", token);
            return false;
        }
    }

#ifdef WINMSVCDEBUG
    if (options.scene == NULL) {
        options.scene = options.default_scene;
    }
#endif

    if (options.scene == NULL) {
        printMessage("Error: No scene file provided\n");
        return false;
    }

    return true;
}
