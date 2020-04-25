/*
File:  CRay.cpp

Description: CRay Source File

Authors:
Kostas Vardis

Implementation file of the engine.
*/

// global includes    //////////////////////////////

// basic global defines
#ifdef _WIN32
#include <windows.h> // used for OutputDebugString and isDebuggerPresent
#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif
#endif

// add this for C++11 support
#if !defined _MSC_VER || _MSC_VER < 1910
//#include "Global/Compatibility.h"
#endif

#if defined (_WIN32) && defined (_MSC_VER)
#define WINMSVC
#endif

#if defined (_WIN32) && defined (_MSC_VER) && !defined(NDEBUG)
#define WINMSVCDEBUG
#endif

#ifdef WINMSVC
#define _DISABLE_EXTENDED_ALIGNED_STORAGE //VS warning
#endif

#ifdef WINMSVCDEBUG
#define CR_BREAK { if (IsDebuggerPresent()) __debugbreak(); }
#else
#define CR_BREAK
#endif

#define PRAGMA_REMINDER __FILE__ ": Reminder: "

#include <cstring>
#define SOURCEPATH_LENGTH                         strlen(SOURCEPATH) + 1
#define SOURCE_FILENAME                          &__FILE__[SOURCEPATH_LENGTH]

#define FILE_NAME(name) (std::string(name).find(SOURCEPATH) != std::string::npos) ? SOURCE_FILENAME : __FILE__
// the following defines are for asserts, popup dialogs and log messages
// asserts

#define CRAY_ASSERT_IF_FALSE_MSG(expr, fmt, ...)         {if(RAYENGINE::isLogLevel(CR_LOGGER_ASSERT)){if (!static_cast<bool>(expr)){RAYENGINE::pushMessage(CR_LOGGER_ASSERT, "Assert: %s (%d): %s: " fmt NEWLINESTR , FILE_NAME(__FILE__), __LINE__, #expr, ##__VA_ARGS__); flushLastMessage(); CR_BREAK}}}
#define CRAY_ASSERT_IF_FALSE(expr)                       {if(RAYENGINE::isLogLevel(CR_LOGGER_ASSERT)){if (!static_cast<bool>(expr)){RAYENGINE::pushMessage(CR_LOGGER_ASSERT, "Assert: %s (%d): %s: " NEWLINESTR , FILE_NAME(__FILE__), __LINE__, #expr); flushLastMessage(); CR_BREAK}}}
#define CRAY_ASSERT_ALWAYS(expr)                         {if(RAYENGINE::isLogLevel(CR_LOGGER_ASSERT)){RAYENGINE::pushMessage(CR_LOGGER_ASSERT, "Assert: %s (%d): %s: " NEWLINESTR , FILE_NAME(__FILE__), __LINE__, #expr); flushLastMessage(); CR_BREAK}}
#define CRAY_ASSERT_ALWAYS_MSG(expr, fmt, ...)           {if(RAYENGINE::isLogLevel(CR_LOGGER_ASSERT)){RAYENGINE::pushMessage(CR_LOGGER_ASSERT, "Assert: %s (%d): %s: " fmt NEWLINESTR , FILE_NAME(__FILE__), __LINE__, #expr, ##__VA_ARGS__); flushLastMessage(); CR_BREAK}}

//#define EYE_SPACE_SHADING
// These should be only enabled inside code for debugging purposes
//#define ENABLE_FORCE_LOG                               // prints verbose log information regardless of Log filter
#ifdef ENABLE_FORCE_LOG
#define CRAY_FORCE_LOG(fmt, ...)                         {RAYENGINE::pushMessage(CR_LOGGER_DEBUG,        fmt NEWLINESTR, ##__VA_ARGS__); flushLastMessage();}
#else
#define CRAY_FORCE_LOG(fmt, ...)
#endif

#define ENABLE_DEBUG_CURRENT_PIXEL                      // allows CRAY_DEBUG to print values for selected pixel (set via CR_debug_setTestPixel)
#ifdef ENABLE_DEBUG_CURRENT_PIXEL
#define CRAY_DEBUG_PIXEL(fmt, ...)                      {if(RAYENGINE::isValidDebugPixel())             {pushMessage(CR_LOGGER_DEBUG,   "Debug: %s(): "     fmt NEWLINESTR, __func__, ##__VA_ARGS__); flushLastMessage();}}
#else
#define CRAY_DEBUG_PIXEL(fmt, ...)
#endif

#define NEWLINESTR "\n"
#ifndef NDEBUG
#define CRAY_INFO(fmt, ...)                              {if(RAYENGINE::isLogLevel(CR_LOGGER_INFO))     {RAYENGINE::pushMessage(CR_LOGGER_INFO,    "Info: %s(): "      fmt NEWLINESTR, __func__, ##__VA_ARGS__);}}
#define CRAY_WARNING(fmt, ...)                           {if(RAYENGINE::isLogLevel(CR_LOGGER_WARNING))  {RAYENGINE::pushMessage(CR_LOGGER_WARNING, "Warning: %s(): "   fmt NEWLINESTR, __func__, ##__VA_ARGS__);}}
#define CRAY_ERROR(fmt, ...)                             {if(RAYENGINE::isLogLevel(CR_LOGGER_ERROR))    {RAYENGINE::pushMessage(CR_LOGGER_ERROR,   "Error: %s(): "     fmt NEWLINESTR, __func__, ##__VA_ARGS__);}}
#define CRAY_DEBUG(fmt, ...)                             {if(RAYENGINE::isLogLevel(CR_LOGGER_DEBUG))    {RAYENGINE::pushMessage(CR_LOGGER_DEBUG,   "Debug: %s(): "     fmt NEWLINESTR, __func__, ##__VA_ARGS__);}}
#else
#define CRAY_INFO(fmt, ...)                              {if(RAYENGINE::isLogLevel(CR_LOGGER_INFO))     {RAYENGINE::pushMessage(CR_LOGGER_INFO,    "Info: "            fmt NEWLINESTR,           ##__VA_ARGS__);}}
#define CRAY_WARNING(fmt, ...)                           {if(RAYENGINE::isLogLevel(CR_LOGGER_WARNING))  {RAYENGINE::pushMessage(CR_LOGGER_WARNING, "Warning: "         fmt NEWLINESTR,           ##__VA_ARGS__);}}
#define CRAY_ERROR(fmt, ...)                             {if(RAYENGINE::isLogLevel(CR_LOGGER_ERROR))    {RAYENGINE::pushMessage(CR_LOGGER_ERROR,   "Error: "           fmt NEWLINESTR,           ##__VA_ARGS__);}}
#define CRAY_DEBUG(fmt, ...)                             {if(RAYENGINE::isLogLevel(CR_LOGGER_DEBUG))    {RAYENGINE::pushMessage(CR_LOGGER_DEBUG,   "Debug: %s(): "     fmt NEWLINESTR, __func__, ##__VA_ARGS__);}}
#endif
#define CHECK_RETURN_IF_UNINITIALIZED_ENGINE()              {if(RAYENGINE::s_global_engine == nullptr)     { CRAY_WARNING("Engine has not been initialized. Call CR_init."); return; }}
#define CHECK_RETURN_IF_UNINITIALIZED_SCENE()               {if(RAYENGINE::s_global_scene == nullptr)      { CRAY_WARNING("Scene empty. Skipping."); return; }}
#define CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(x)       {if(RAYENGINE::s_global_engine == nullptr)     { CRAY_WARNING("Engine has not been initialized. Call CR_init."); return x; }}
#define CHECK_RETURN_IF_STARTED_ENGINE()                    {if(RAYENGINE::s_global_engine->m_started)     { CRAY_WARNING("Rendering in progress. Skipping."); return; }}
#define CHECK_RETURN_VALUE_IF_STARTED_ENGINE(x)             {if(RAYENGINE::s_global_engine->m_started)     { CRAY_WARNING("Rendering in progress. Skipping."); return x; }}
#define CHECK_RETURN_IF_NOT_STARTED_ENGINE()                {if(!RAYENGINE::s_global_engine->m_started)    { CRAY_WARNING("Rendering has not started. Skipping."); return; }}


// includes ////////////////////////////////////////
#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif
#include <cstdarg>
#include <random>
#include <thread>
#include <algorithm>
#include <queue>
#include <vector>
#include <array>
#include <stack>
#include <cstring>
#include <mutex>
#include <future>
#include <memory>
#include <map>
#include <sstream>
#include <iomanip>
#include <string>
#include "CRay.h"

#define GLM_FORCE_SILENT_WARNINGS // disable some GLM warnings
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glm/common.hpp"
#include "glm/exponential.hpp"
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/glm.hpp>

#ifdef DOUBLE_PRECISION
#define mat4 dmat4
#define mat4x4 dmat4x4
#define vec4 dvec4
#define vec3 dvec3
#define vec2 dvec2
#define isDouble true
#define FLOAT_ZERO_NEXT 1.401298464e-45
#define FLOAT_ONE_BEFORE 9.999999404e-01
#define FLOAT_MAX_FULL_INTEGER 0x20000000000000ull // 9007199254740992 or 2^53
#define FLOAT_MAX_FULL_INTEGER_MINUS_ONE 0x1fffffffffffffull // 9007199254740991 2^53 - 1
#else
#define isDouble false
#define FLOAT_ZERO_NEXT 1.401298464e-45
#define FLOAT_ONE_BEFORE 9.999999404e-01
#define FLOAT_MAX_FULL_INTEGER 0x1000000u // 16777216 or 2^24
#define FLOAT_MAX_FULL_INTEGER_MINUS_ONE 0x00FFFFFFu // 16777215 or 2^24-1
#endif

#define FLOAT_TOLERANCE_10   cr_float(1.0e-10) // 0.0000000001
#define FLOAT_TOLERANCE_6    cr_float(1.0e-06) // 0.000001
#define FLOAT_TOLERANCE_5    cr_float(1.0e-05) // 0.00001
#define FLOAT_TOLERANCE_4    cr_float(1.0e-04) // 0.0001
#define FLOAT_TOLERANCE_3    cr_float(1.0e-03) // 0.001
#define FLOAT_TOLERANCE_2    cr_float(1.0e-02) // 0.01
#define FLOAT_TOLERANCE_1    cr_float(1.0e-01) // 0.1
#define FLOAT_TOLERANCE      FLOAT_TOLERANCE_6

#define GLOBAL_RAY_EPSILON  FLOAT_TOLERANCE_6

namespace RAYENGINE {
    // forward declarations and defines
    void flushLastMessage();
    void pushMessage(CRAY_LOGGERENTRY type, const char* fmt, ...);
    bool isLogLevel(enum CRAY_LOGGERENTRY type);
    bool isValidDebugPixel();

    // Helper functions
    bool isValidFloat(cr_float x) {
        bool valid = !glm::isnan(x) && !glm::isinf(x);
        CRAY_ASSERT_IF_FALSE_MSG(valid, "Invalid float value: %f", x)
            return valid;
    }
    bool isValidVec3(const glm::vec3& x) {
        bool valid = !glm::any(glm::isnan(x)) && !glm::any(glm::isinf(x));
        CRAY_ASSERT_IF_FALSE_MSG(valid, "Invalid vec3 values: %f, %f, %f", x.x, x.y, x.z)
            return valid;
    }

    glm::vec3 transformPoint(const glm::mat4x4& mat, const glm::vec3& point) { return mat * glm::vec4(point, 1); }
    glm::vec3 transformVector(const glm::mat4x4& mat, const glm::vec3& point) { return mat * glm::vec4(point, 0); }

    void updateRunningSum(double& old_value, double new_value, double iterations) {
        old_value = old_value + ((new_value - old_value) / iterations);
    }

    template <typename T>
    std::string NumberToString(const T& Number, int32_t decimal_precision = 2) {
        std::stringstream ss;
        ss << std::setprecision(decimal_precision) << std::fixed << Number;
        return ss.str();
    }

    template <typename T>
    std::string convertToCapacitySizeString(const T& size, int32_t decimal_precision = 2) {
        std::string res = "";
        double byte = sizeof(char);
        double KB = 1024 * byte;
        double MB = 1024 * KB;
        double GB = 1024 * MB;
        double s = static_cast<double>(size);
        if (s > GB) {
            s /= GB;
            res = NumberToString(s, decimal_precision).append("GB");
        }
        else if (s > MB) {
            s /= MB;
            res = NumberToString(s, decimal_precision).append("MB");
        }
        else if (s > KB) {
            s /= KB;
            res = NumberToString(s, decimal_precision).append("KB");
        }
        else {
            s /= byte;
            res = NumberToString(s, 0).append("b");
        }
        return res;
    }

    template <typename T>
    std::string convertToTimeString(const T& time_ms) {
        std::string res = "";
        double seconds = 1000;
        double minutes = seconds * 60;
        double hours = minutes * 60;
        double t = static_cast<double>(time_ms);
        if (t > hours) {
            int32_t h = static_cast<int32_t>(t / hours);
            t -= h * hours;
            res += NumberToString(h, 0).append(" hour") + ((h > 1) ? "s" : "");
        }
        if (!res.empty() || t > minutes) {
            int32_t m = static_cast<int32_t>(t / minutes);
            t -= m * minutes;
            res += (res.empty() ? res : ", ") + NumberToString(m, 0).append(" min") + ((m > 1) ? "s" : "");
        }
        if (!res.empty() || t > seconds) {
            int32_t s = static_cast<int32_t>(t / seconds);
            t -= s * seconds;
            res += (res.empty() ? res : ", ") + NumberToString(s, 0).append(" sec") + ((s > 1) ? "s" : "");
        }
        if (!res.empty() || t > 0) {
            res += (res.empty() ? res : ", ") + NumberToString(t, 2).append(" ms");
        }
        return res;
    }

    // source: https://refractiveindex.info
    // sampled at wavelength 526nm
    static std::map<CRAY_IOR_DIELECTRICS, cr_float> dielectrics_ior = {
        // plastics
        {CR_DIELECTRIC_Acrylic_glass, 1.4941},
        {CR_DIELECTRIC_Polystyrene, 1.5992},
        {CR_DIELECTRIC_Polycarbonate, 1.5926},
        // crystals
        {CR_DIELECTRIC_Diamond, 2.4258},
        {CR_DIELECTRIC_Ice, 1.3119},
        {CR_DIELECTRIC_Sapphire, 1.7721},
        // glass
        {CR_DIELECTRIC_Crown_Glass_bk7, 1.5198},
        {CR_DIELECTRIC_Soda_lime_glass, 1.5264},
        // liquids
        {CR_DIELECTRIC_Water_25C, 1.3340},
        {CR_DIELECTRIC_Acetone_20C, 1.3616},
        // gasses
        {CR_DIELECTRIC_Air, 1.00027834},
        {CR_DIELECTRIC_Carbon_dioxide, 1.00045113}
    };
    // source: https://refractiveindex.info
    // sampled at wavelengths 645nm, 526nm, 444nm
    static std::map<enum CRAY_IOR_CONDUCTORS, cr_vec3> conductors_ior = {
        {CR_CONDUCTOR_Alluminium_Al, cr_vec3{1.3211, 0.88045, 0.59765}},
        {CR_CONDUCTOR_Brass_CuZn, cr_vec3{0.44400, 0.58460, 1.1522}},
        {CR_CONDUCTOR_Copper_Cu, cr_vec3{0.28046, 0.85418, 1.3284}},
        {CR_CONDUCTOR_Gold_Au, cr_vec3{0.18601, 0.59580, 1.4120}},
        {CR_CONDUCTOR_Iron_Fe, cr_vec3{2.9067, 2.8761, 2.5515}},
        {CR_CONDUCTOR_Silver_Ag, cr_vec3{0.15865, 0.14215, 0.13534}}
    };
    static std::map<enum CRAY_IOR_CONDUCTORS, cr_vec3> conductors_k = {
        { CR_CONDUCTOR_Alluminium_Al, cr_vec3{7.4178, 6.1455, 5.2337} },
        { CR_CONDUCTOR_Brass_CuZn, cr_vec3{3.6520, 2.5264, 1.8098} },
        { CR_CONDUCTOR_Copper_Cu, cr_vec3{3.5587, 2.4518, 2.2949} },
        { CR_CONDUCTOR_Gold_Au, cr_vec3{3.3762, 2.0765, 1.7827} },
        { CR_CONDUCTOR_Iron_Fe, cr_vec3{3.0833, 2.9136, 2.7490} },
        { CR_CONDUCTOR_Silver_Ag, cr_vec3{3.8930, 3.0051, 2.3275} }
    };

    struct NonCopyable {
        NonCopyable() = default;
        virtual ~NonCopyable() = default;
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
        NonCopyable(NonCopyable&&) = delete;
        NonCopyable& operator = (NonCopyable&&) = delete;
    };

    // RNG
    template<bool is64>
    struct AbstractRNGInterfaceT : public NonCopyable {
        using seed_type = typename std::conditional<is64, uint64_t, uint32_t>::type;
        seed_type seed_uint = { 0 };
        virtual void reset_seed(seed_type seed) = 0;
        virtual cr_float rand_f() = 0;
        virtual cr_float rand_f_range(cr_float min, cr_float max) = 0;
    };

    // Marsaglia, G "Xorshift RNGs". Journal of Statistical Software
    template<bool is64>
    struct FastRNGT : AbstractRNGInterfaceT<is64> {
        using seed_type = typename AbstractRNGInterfaceT<is64>::seed_type;
        using AbstractRNGInterfaceT<is64>::seed_uint;

        //32-bit
        template <bool _is64 = is64,
            typename std::enable_if<(!_is64), bool>::type = 0>
            seed_type rand_uint() {
            seed_type& y = seed_uint;
            y ^= (y << 13);
            y ^= (y >> 17);
            y ^= (y << 5);
            return y;
        }

        //64-bit
        template <bool _is64 = is64,
            typename std::enable_if<(_is64), bool>::type = 0>
            seed_type rand_uint() {
            seed_type& y = seed_uint;
            y ^= (y << 13);
            y ^= (y >> 7);
            y ^= (y << 17);
            return y;
        }

        void reset_seed(seed_type seed) override {
            seed_uint = seed;
        }

        cr_float rand_f() override {
            seed_type res = rand_uint();
            cr_float f = (res & FLOAT_MAX_FULL_INTEGER_MINUS_ONE) / cr_float(FLOAT_MAX_FULL_INTEGER_MINUS_ONE);
            return f;
        }

        cr_float rand_f_range(cr_float min, cr_float max) override {
            return min + rand_f() * (max - min);
        }
    };

    template<bool is64>
    struct RNGT : AbstractRNGInterfaceT<is64> {
        using seed_type = typename AbstractRNGInterfaceT<is64>::seed_type;
        using AbstractRNGInterfaceT<is64>::seed_uint;
        using mt19937_engine = typename std::conditional<is64, std::mt19937_64, std::mt19937>::type;
        mt19937_engine gen;
        std::uniform_real_distribution<cr_float> dist{ 0, FLOAT_ONE_BEFORE };

        void reset_seed(seed_type seed) override {
            gen.seed(seed);
            seed_uint = seed;
        }

        cr_float rand_f() override {
            cr_float f = dist(gen);
            CRAY_ASSERT_IF_FALSE_MSG(f > 0.0 && f < 1.0, "rand_f out of bounds %f", f);
            return f;
        }

        cr_float rand_f_range(cr_float min, cr_float max) override {
            return min + dist(gen) * (max - min);
        }
    };
    using AbstractRNGInterface = typename std::conditional<isDouble, AbstractRNGInterfaceT<true>, AbstractRNGInterfaceT<false>>::type;
    using RNG = typename std::conditional<isDouble, RNGT<true>, RNGT<false>>::type;
    using FastRNG = typename std::conditional<isDouble, FastRNGT<true>, FastRNGT<false>>::type;
    static thread_local std::unique_ptr<AbstractRNGInterface> current_rng = std::make_unique<RNG>();
    cr_float rand_f() { return current_rng->rand_f(); }
    cr_float rand_f_range(cr_float min, cr_float max) { return current_rng->rand_f_range(min, max); }
    void rand_reset() { current_rng.reset(); }
    void rand_reset_seed(uint32_t seed) { current_rng->reset_seed(seed); }
    static bool s_use_fast_rng = false;
    void rand_set_fast_rng(bool fast) {
        if (fast || s_use_fast_rng) {
            current_rng = std::make_unique<FastRNG>();
        }
        else {
            current_rng = std::make_unique<RNG>();
        }
    }
    void rand_use_fast_rng(bool enable) {
        s_use_fast_rng = enable;
    }
    bool rand_is_fast_rng() {
        return s_use_fast_rng;
    }

    //thread_local static RNG current_rng;
    //CR_float rand_f() { return current_rng.rand_f(); }
    //CR_float rand_f_range(CR_float min, CR_float max) { return current_rng.rand_f_range(min, max); }
    //void rand_reset() {}
    //void rand_reset_seed(uint32_t seed) { current_rng.reset_seed(seed); }
    //void rand_set_fast_rng(bool) {}

    struct Stats {
        uint64_t m_ray_depth = { 0 };

        uint64_t m_number_rays = { 0 };
        uint64_t m_number_rays_total = { 0 };
        double m_primitive_intersections = { 0 };
        double m_node_traversals = { 0 };
        double m_primitive_intersections_avg = { 0 };
        double m_node_traversals_avg = { 0 };

        uint64_t m_number_shadow_rays = { 0 };
        uint64_t m_number_shadow_rays_total = { 0 };
        double m_primitive_intersections_shadow = { 0 };
        double m_node_traversals_shadow = { 0 };
        double m_primitive_intersections_shadow_avg = { 0 };
        double m_node_traversals_shadow_avg = { 0 };

        double m_running_sum_iterations = { 1 };
        void addRayTraversal() { ++m_number_rays; }
        void addPrimitiveIntersection() { ++m_primitive_intersections; }
        void addNodeTraversal() { ++m_node_traversals; }
        void addShadowRayTraversal() { ++m_number_shadow_rays; }
        void addShadowPrimitiveIntersection() { ++m_primitive_intersections_shadow; }
        void addShadowNodeTraversal() { ++m_node_traversals_shadow; }
        void addRayDepth(int32_t ray_depth) { m_ray_depth += ray_depth; }

        void clear() {
            m_ray_depth = 0;

            m_number_rays = 0;
            m_number_rays_total = 0;
            m_primitive_intersections = 0;
            m_node_traversals = 0;
            m_primitive_intersections_avg = 0;
            m_node_traversals_avg = 0;

            m_number_shadow_rays = 0;
            m_number_shadow_rays_total = 0;
            m_primitive_intersections_shadow = 0;
            m_node_traversals_shadow = 0;
            m_primitive_intersections_shadow_avg = 0;
            m_node_traversals_shadow_avg = 0;

            m_running_sum_iterations = 1;
        }

        void add(Stats& stats) {
            m_number_rays += stats.m_number_rays_total;
            m_number_shadow_rays += stats.m_number_shadow_rays_total;
            m_ray_depth += stats.m_ray_depth;

            updateRunningSum(m_primitive_intersections_avg, stats.m_primitive_intersections_avg, m_running_sum_iterations);
            updateRunningSum(m_node_traversals_avg, stats.m_node_traversals_avg, m_running_sum_iterations);
            updateRunningSum(m_primitive_intersections_shadow_avg, stats.m_primitive_intersections_shadow_avg, m_running_sum_iterations);
            updateRunningSum(m_node_traversals_shadow_avg, stats.m_node_traversals_shadow_avg, m_running_sum_iterations);

            ++m_running_sum_iterations;

            stats.clear();
        }

        void updateThreadRunningSum() {

            updateRunningSum(m_primitive_intersections_avg, m_number_rays > 0 ? m_primitive_intersections / double(m_number_rays) : 0.0, m_running_sum_iterations);
            updateRunningSum(m_node_traversals_avg, m_number_rays > 0 ? m_node_traversals / double(m_number_rays) : 0.0, m_running_sum_iterations);
            m_number_rays_total += m_number_rays;
            m_number_rays = 0;

            updateRunningSum(m_primitive_intersections_shadow_avg, m_number_shadow_rays > 0 ? m_primitive_intersections_shadow / double(m_number_shadow_rays) : 0.0, m_running_sum_iterations);
            updateRunningSum(m_node_traversals_shadow_avg, m_number_shadow_rays > 0 ? m_node_traversals_shadow / double(m_number_shadow_rays) : 0.0, m_running_sum_iterations);
            m_number_shadow_rays_total += m_number_shadow_rays;
            m_number_shadow_rays = 0;

            m_primitive_intersections = 0;
            m_primitive_intersections_shadow = 0;
            m_node_traversals = 0;
            m_node_traversals_shadow = 0;
            ++m_running_sum_iterations;
        }
    };

    struct DebugData {
        glm::vec2 current_pixel = { glm::vec2(0.0) };
#ifdef ENABLE_DEBUG_CURRENT_PIXEL
        bool enabled = false;
        glm::ivec2 target_pixel_start = { glm::ivec2(-1) };
        glm::ivec2 target_pixel_end = { glm::ivec2(-1) };
#endif
    };

    struct GlobalSettings {

        // stats
        Stats m_stats;

        // debug data
        DebugData m_debug_data;
        const glm::vec2& getCurrentPixel() { return m_debug_data.current_pixel; }
#ifdef ENABLE_DEBUG_CURRENT_PIXEL
        bool isPixel() {
            return 
                (m_debug_data.enabled &&
                glm::all(glm::greaterThanEqual(glm::ivec2(m_debug_data.current_pixel), m_debug_data.target_pixel_start)) &&
                glm::all(glm::lessThanEqual(glm::ivec2(m_debug_data.current_pixel), m_debug_data.target_pixel_end)));
        }
#endif

        // helpers
        std::string m_output_name = { "result" };

        // logging
        enum CRAY_LOGGERENTRY m_minimum_log_level = { CR_LOGGER_WARNING };
        bool m_print_progress_bar_stdout = { true };
        bool m_print_to_stdout = { true };
        FILE* m_file = { nullptr };

        // gamma/tonemapping
        bool m_rgb = { false };
        cr_float m_exposure = { 2.0 };
        cr_float m_gamma = { 2.2 };
        cr_float m_inverse_gamma = { cr_float(1.0) / m_gamma };

        // AO
        int32_t m_ao_samples_per_pixel = { 1 };
        cr_float m_ao_range = { 0.0 };

        // path tracing
        int32_t m_max_depth = { 10 };
        int32_t m_samples_per_pixel = { 1 };
        bool m_russian_roulette = { true };

        // enabled only when interactive rendering is requested
        bool m_interactive = { false };
    };

    static thread_local std::unique_ptr<GlobalSettings> s_thread_settings = { nullptr };
    static GlobalSettings* s_global_settings = { nullptr };

    struct Color {
        Color() : data(glm::vec3(0)) {}
        explicit Color(const cr_vec3& color) : data(color.x, color.y, color.z) {}
        explicit Color(const glm::vec3& color) : data(color) {}
        explicit Color(cr_float color) : data(color) {}
        explicit Color(cr_float x, cr_float y, cr_float z) : data(x, y, z) { }
        union {
            glm::vec3 data = { glm::vec3(0) };
            struct { cr_float x, y, z; };
        };

        Color& operator+=(const Color& rhs) { this->data += rhs.data; return *this; }
        Color operator+(const Color& rhs) const {
            Color tmp = *this;
            return tmp += rhs;
        }
        Color& operator-=(const Color& rhs) { this->data += -rhs.data; return *this; }
        Color operator-(const Color& rhs) const {
            Color tmp = *this;
            return tmp -= rhs;
        }
        Color& operator*=(const Color& rhs) { this->data *= rhs.data; return *this; }
        Color operator*(const Color& rhs) const {
            Color tmp = *this;
            return tmp *= rhs;
        }
        Color& operator/=(const Color& rhs) { this->data /= rhs.data; return *this; }
        Color operator/(const Color& rhs) const {
            Color tmp = *this;
            return tmp /= rhs;
        }
        // overloading float
        Color& operator+=(const cr_float& scalar) { this->data += scalar; return *this; }
        friend Color operator+(const Color& lhs, const cr_float& scalar) {
            Color tmp = lhs;
            return tmp += scalar;
        }
        Color& operator-=(const cr_float& scalar) { this->data -= scalar; return *this; }
        friend Color operator-(const Color& lhs, const cr_float& scalar) {
            Color tmp = lhs;
            return tmp += -scalar;
        }
        Color& operator*=(const cr_float& scalar) { this->data *= scalar; return *this; }
        friend Color operator*(const Color& lhs, const cr_float& scalar) {
            Color tmp = lhs;
            return tmp *= scalar;
        }
        Color& operator/=(const cr_float& scalar) { this->data /= scalar; return *this; }
        friend Color operator/(const Color& lhs, const cr_float& scalar) {
            Color tmp = lhs;
            return tmp /= scalar;
        }

        // overloading vec3
        Color& operator+=(const glm::vec3& scalar) { this->data += scalar; return *this; }
        friend Color operator+(const Color& lhs, const glm::vec3& scalar) {
            Color tmp = lhs;
            return tmp += scalar;
        }
        Color& operator-=(const glm::vec3& scalar) { this->data -= scalar; return *this; }
        friend Color operator-(const Color& lhs, const glm::vec3& scalar) {
            Color tmp = lhs;
            return tmp += -scalar;
        }
        Color& operator*=(const glm::vec3& scalar) { this->data *= scalar; return *this; }
        friend Color operator*(const Color& lhs, const glm::vec3& scalar) {
            Color tmp = lhs;
            return tmp *= scalar;
        }
        Color& operator/=(const glm::vec3& scalar) { this->data /= scalar; return *this; }
        friend Color operator/(const Color& lhs, const glm::vec3& scalar) {
            Color tmp = lhs;
            return tmp /= scalar;
        }

        bool isNot(const Color& color) const { return glm::any(glm::notEqual(this->data, color.data)); }

        static uint8_t toUnsignedByte(cr_float val) { return static_cast<uint8_t>(val * 255.0); }
        static Color Black() { return Color(0.0); }
        static Color White() { return Color(1.0); }
    };

    // Generates a sample on a sphere using a uniform distribution
    // Returns the generated sample
    glm::vec3 getUniformSphereSample() {
        glm::vec2 r;
        r.x = rand_f();
        r.y = rand_f();
        cr_float phi = r.x * 2.0 * glm::pi<cr_float>();

        cr_float cosTheta = 1.0 - 2.0 * r.y;
        cr_float sinTheta = glm::sqrt(1.0 - cosTheta * cosTheta);
        return glm::vec3(glm::cos(phi) * sinTheta, glm::sin(phi) * sinTheta, cosTheta);
    }

    // Returns the PDF from uniform spherical distribution
    cr_float getUniformSphereSamplePDF() {
        // pdf is 1 over 4pi
        return glm::one_over_two_pi<cr_float>() * 0.5;
    }

    // Generates a sample on a disc using a uniform distribution
    // Returns the generated sample
    glm::vec2 getUniformDiscSample() {
        glm::vec2 r;
        r.x = rand_f();
        r.y = rand_f();
        cr_float phi = r.x * r.x * 2.0 * glm::pi<cr_float>();
        cr_float radius = glm::sqrt(r.y);
        return glm::vec2(glm::cos(phi) * radius, glm::sin(phi) * radius);
    }

    // Returns the PDF from uniform disc distribution
    cr_float getUniformDiscSamplePDF() {
        // pdf is 1 over 4pi
        return glm::one_over_pi<cr_float>();
    }

    // Generates a sample on a hemisphere using a uniform distribution
    // Returns the generated sample
    glm::vec3 getUniformHemisphereSample() {
        glm::vec2 r;
        r.x = rand_f();
        r.y = rand_f();
        cr_float radius = glm::sqrt(glm::max(0.0, 1.0 - r.x * r.x));
        cr_float phi = 2.0 * glm::pi<cr_float>() * r.y;
        cr_float x = radius * glm::cos(phi);
        cr_float y = radius * glm::sin(phi);
        return glm::vec3(x, y, r.x);
    }

    // Returns the PDF from uniform hemispherical distribution
    cr_float getUniformHemisphereSamplePDF() {
        // pdf is 1 over 2pi
        return glm::one_over_two_pi<cr_float>();
    }

    // Generates a sample on a hemisphere using a cosine-weighted distribution
    // Returns the generated sample
    glm::vec3 getCosineHemisphereSample() {
        glm::vec2 r;
        r.x = rand_f();
        r.y = rand_f();

        cr_float phi = r.x * 2.0 * glm::pi<cr_float>();

        // theta is acos(sqrt(r.y))
        cr_float sinTheta = glm::sqrt(1 - r.y);
        cr_float cosTheta = glm::sqrt(r.y);
        return glm::vec3(glm::cos(phi) * sinTheta, glm::sin(phi) * sinTheta, cosTheta);
    }

    // Returns the PDF from cosine hemispherical distribution
    cr_float getCosineHemisphereSamplePDF(const glm::vec3& sample) {
        return glm::abs(sample.z) * glm::one_over_pi<cr_float>();
    }

    // Obtains an OBN basis, e.g. for world<->local transformations
    void getOrthonormalBasis(glm::vec3& bitangent, glm::vec3& tangent, const glm::vec3& normal) {
        // calculate tangent, bitangent
        //tangent = glm::cross(normal, glm::vec3(0.0, 1.0, 0.0));
        //if (glm::dot(tangent, tangent) < FLOAT_TOLERANCE_3)
        //    tangent = glm::cross(normal, glm::vec3(1.0, 0.0, 0.0));
        //tangent = glm::normalize(tangent);
        //bitangent = glm::normalize(glm::cross(normal, tangent));
        // Building An Orthonormal Basis, Revisited., Duff et al, JCGT. 2017, Listing 3.
        cr_float sign = std::copysignf(1.0, normal.z);
        const cr_float a = -1.0 / (sign + normal.z);
        const cr_float b = normal.x * normal.y * a;
        tangent = glm::vec3(1.0 + sign * normal.x * normal.x * a, sign * b, -sign * normal.x);
        bitangent = glm::vec3(b, sign + normal.y * normal.y * a, -normal.y);
    }

    // The Fresnel term for dielectric media (e.g. air<->water)
    // Parameters:
    // - cosThetaIncident, the cosine of the incident angle
    // - nIncident, the index of refraction for the incident medium
    // - nIncident, the index of refraction for the transmitted medium
    // returns the Fresnel term
    // source: https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
    // source: https://people.cs.kuleuven.be/~philip.dutre/GI/TotalCompendium.pdf
    Color FresnelDielectric(cr_float cosThetaIncident, cr_float nIncident, cr_float nTransmitted) {
        if (nTransmitted <= 0.0) {
            // return 1 for pure-reflective surfaces (e.g. mirrors)
            return Color::White();
        }
        cosThetaIncident = glm::clamp(cosThetaIncident, cr_float(-1.0), cr_float(1.0));
        //CRAY_ASSERT_IF_FALSE(cosThetaIncident >= -1 && cosThetaIncident <= 1);

        // compute sinThetaIncident
        cr_float sinThetaIncident = glm::sqrt(1.0 - cosThetaIncident * cosThetaIncident);
        CRAY_ASSERT_IF_FALSE(sinThetaIncident >= 0 && sinThetaIncident <= 1);

        // compute ThetaT using Snell's Law
        cr_float sinThetaT = sinThetaIncident * nIncident / nTransmitted;

        // if total internal reflection (transmitted angle > pi/2) no transmission occurs
        if (sinThetaT >= 1) {
            return Color(1);
        }

        // float relative_ior = nT / nI;
        // compute Fresnel for dielectric-dielectric interface
        cr_float cosThetaTransmitted = glm::sqrt(1.0 - sinThetaT * sinThetaT);
        CRAY_ASSERT_IF_FALSE(cosThetaTransmitted > 0 && cosThetaTransmitted <= 1);

        cr_float nIncident_cosThetaIncident = nIncident * cosThetaIncident;
        cr_float nIncident_cosThetaTransmitted = nIncident * cosThetaTransmitted;
        cr_float nTransmitted_cosThetaIncident = nTransmitted * cosThetaIncident;
        cr_float nTransmitted_cosThetaTransmitted = nTransmitted * cosThetaTransmitted;

        cr_float R_parallel = (nTransmitted_cosThetaIncident - nIncident_cosThetaTransmitted) /
            (nTransmitted_cosThetaIncident + nIncident_cosThetaTransmitted);

        cr_float R_perpendicular = (nIncident_cosThetaIncident - nTransmitted_cosThetaTransmitted) /
            (nIncident_cosThetaIncident + nTransmitted_cosThetaTransmitted);

        return Color(((R_parallel * R_parallel) + (R_perpendicular * R_perpendicular)) * 0.5);
    }

    // The Fresnel term for dielectric-conductor media (e.g. air<->metal). Assumes dielectric is air~=1
    // Parameters:
    // - cosThetaIncident, the cosine of the incident angle
    // - nIncident, the index of refraction for the incident medium
    // - nIncident, the index of refraction for the transmitted medium
    // returns the Fresnel term
    // source: https://seblagarde.wordpress.com/2013/04/29/memo-on-fresnel-equations/
    // source: https://people.cs.kuleuven.be/~philip.dutre/GI/TotalCompendium.pdf
    Color FresnelConductor(cr_float cosThetaIncident, cr_float nIncident, const glm::vec3& nTransmitted, const glm::vec3& k) {
        cr_float CosTheta2 = cosThetaIncident * cosThetaIncident;
        cr_float SinTheta2 = 1 - cosThetaIncident;
        glm::vec3 eta = nTransmitted / nIncident;
        glm::vec3 Eta2 = eta * eta;
        glm::vec3 Etak2 = k * k / (nIncident * nIncident);

        glm::vec3 t0 = Eta2 - Etak2 - SinTheta2;
        glm::vec3 a2plusb2 = glm::sqrt(t0 * t0 + cr_float(4.0) * Eta2 * Etak2);
        glm::vec3 t1 = a2plusb2 + CosTheta2;
        glm::vec3 a = glm::sqrt(cr_float(0.5) * (a2plusb2 + t0));
        glm::vec3 t2 = cr_float(2.0) * a * cosThetaIncident;
        glm::vec3 R_perpendicular = (t1 - t2) / (t1 + t2);

        glm::vec3 t3 = CosTheta2 * a2plusb2 + SinTheta2 * SinTheta2;
        glm::vec3 t4 = t2 * SinTheta2;
        glm::vec3 R_parallel = R_perpendicular * (t3 - t4) / (t3 + t4);

        return Color((R_parallel + R_perpendicular) * cr_float(0.5));
    }

    // The Fresnel term using Shlick's approximation.
    // Parameters:
    // - cosThetaIncident, the cosine of the incident angle
    // - nIncident, the index of refraction for the incident medium
    // - nIncident, the index of refraction for the transmitted medium
    // returns the Fresnel term
    cr_float FresnelSchlick(cr_float cosThetaIncident, cr_float nIncident, cr_float nTransmitted) {
        cr_float R0 = (nIncident - nTransmitted) / (nIncident + nTransmitted);
        R0 *= R0;

        cr_float u = 1.0 - cosThetaIncident;
        cr_float u5 = u * u;
        u5 = u5 * u5 * u;

        return glm::min(1.0, R0 + (1.0 - R0) * u5);
    }

    // The Trowbridge-Reitz (GGX) isotropic NDF distribution, describing how the microfacet normals are distributed over the microsurface
    // Parameters:
    // - NH, the cosine of the angle between normal and microfacet normal
    // - m, the roughness (width) parameter
    // returns the NDF
    cr_float Distribution_GGX_isotropic(cr_float NH, cr_float m) {
        if (NH <= 0.0) {
            return 0.0;
        }
        cr_float NH2 = NH * NH;
        cr_float m2 = glm::max(cr_float(0.0), m * m);
        cr_float denom = (NH2 * (m2 - 1) + 1);
        denom = glm::pi<cr_float>() * denom * denom;
        cr_float D = m2 / denom;
        CRAY_ASSERT_IF_FALSE(D > 0);
        return D;
    }

    // The Trowbridge-Reitz (GGX) Shadowing-Masking function, describing the portion of the microsurface visible in one direction I and O
    // Parameters:
    // - hV, cosine of the angle between microfacet normal and (I or O) dir
    // - NV, cosine of the angle between normal and (I or O) dir
    // - m, the roughness (width) parameter
    // returns the Geometric term
    cr_float Geometric1_GGX(cr_float HV, cr_float NV, cr_float m) {
        if (HV / NV <= 0.0) {
            return 0.0;
        }
        cr_float m2 = glm::max(cr_float(0.0), m * m);
        cr_float cos_theta2_v = NV * NV;
        cr_float tan_theta_v = (1.0 - cos_theta2_v) / cos_theta2_v;
        cr_float denom = 1 + glm::sqrt(1 + m2 * tan_theta_v);
        return (2.0) / denom;
    }

    // The bi-directional Trowbridge-Reitz (GGX) Shadowing-Masking function, describing the portion of the microsurface visible in both directions I and O
    // Parameters:
    // - HI, cosine of the angle between microfacet normal and incoming (light) dir
    // - HO, cosine of the angle between microfacet normal and out (view) dirr
    // - NI, cosine of the angle between normal and incoming (light) dir
    // - NO, cosine of the angle between normal and out (view) dirr
    // - m, the roughness (width) parameter
    // returns the Geometric term
    cr_float Geometric_Smith_GGX(cr_float HI, cr_float HO, cr_float NI, cr_float NO, cr_float m) {
        return Geometric1_GGX(HI, NI, m) * Geometric1_GGX(HO, NO, m);
    }

    // Generates a new direction based on the GGX function
    // Parameters:
    // - roughness. the the roughness (width) parameter
    // returns the new direction
    glm::vec3 generateMicrofacetH_GGX(cr_float roughness) {
        glm::vec2 r;
        r.x = rand_f();
        r.y = rand_f();

        // generate microfacet
        cr_float tantheta = roughness * glm::sqrt(r.x) / glm::sqrt(1.0 - r.x);
        cr_float tantheta2 = tantheta * tantheta;
        cr_float costheta = glm::sqrt(1.0 / (1.0 + tantheta2));
        cr_float sintheta = glm::sqrt(1.0 - costheta * costheta);

        cr_float phi = 2.0 * glm::pi<cr_float>() * r.y;
        return glm::vec3(glm::cos(phi) * sintheta, glm::sin(phi) * sintheta, costheta);
    }

    // Converts a [0-1] roughness value to GGX alpha
    // http://www.pbr-book.org/3ed-2018/Reflection_Models/Microfacet_Models.html
    cr_float GGXRoughnessToAlpha(cr_float roughness) {
        roughness = glm::max(roughness, static_cast<cr_float>(1e-3f));
        cr_float x = glm::log(roughness);
        return 1.62142 + 0.819955 * x + 0.1734 * x * x + 0.0171201 * x * x * x +
            0.000640711 * x * x * x * x;
    }

    // Source for the microfacet sampling distributions:
    // Microfacet Models for Refraction through Rough Surfaces (Walter B. et al.), EGSR 2007
    // x1, x2 : uniformly distributed numbers [0, 1)
    // GGX Sampling:
    // theta_m = arctan(a_g * sqrt(x1) / sqrt(1 - x1))
    // phi_m = 2 * p * x2
    cr_float getMicrofacetPDF(cr_float HN, cr_float roughness) {
        return Distribution_GGX_isotropic(HN, roughness) * glm::abs(HN);
    }

    // Basic Tracing Classes
    struct Framebuffer {
        glm::ivec2 m_dimensions = { glm::uvec2(32) };
        std::unique_ptr<float[]> m_float_data_ptr = { nullptr };
        std::unique_ptr<uint8_t[]> m_rgb_data_ptr = { nullptr };
        int32_t m_num_channels = { 3 };
        std::unique_ptr<uint32_t[]> m_iteration_ptr = { nullptr };
        bool m_realloc = { false };

        void requestRealloc()  { m_realloc = true; }

        void setDimensions(int32_t width, int32_t height) {
            glm::ivec2 dimensions = glm::max(glm::ivec2(width, height), glm::ivec2(32));
            if (m_dimensions != dimensions || m_float_data_ptr == nullptr) {
                m_realloc = true;
            }
            m_dimensions = dimensions;
        }

        void alloc() {
            if (!m_realloc) {
                return;
            }
            m_float_data_ptr = std::make_unique<float[]>(m_dimensions.x * m_dimensions.y * m_num_channels);
            if (s_global_settings->m_rgb) {
                m_rgb_data_ptr = std::make_unique<uint8_t[]>(m_dimensions.x * m_dimensions.y * m_num_channels);
            }
            else {
                m_rgb_data_ptr.reset();
            }
            if (s_global_settings->m_interactive) {
                m_iteration_ptr = std::make_unique<uint32_t[]>(m_dimensions.x * m_dimensions.y);
            }
            else {
                m_iteration_ptr.reset();
            }
            m_realloc = false;
        }

        void clear(unsigned char value) {
            memset(m_float_data_ptr.get(), value, getFloatByteSize());
            if (s_global_settings->m_rgb) {
                memset(m_rgb_data_ptr.get(), value, getRGBByteSize());
            }
        }

        void clearIteration(int32_t value) {
            if (m_realloc) {
                alloc();
            }
            if (s_global_settings->m_interactive) {
                memset(m_iteration_ptr.get(), value, m_dimensions.x * m_dimensions.y * sizeof(uint32_t));
            }
        }

        // source: http://filmicworlds.com/blog/filmic-tonemapping-operators/
        glm::vec3 Uncharted2ToneMappingF(const glm::vec3& color) {
            cr_float A = 0.15; cr_float B = 0.50; cr_float C = 0.10;
            cr_float D = 0.20; cr_float E = 0.02; cr_float F = 0.30;
            return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
        }

        glm::vec3 Uncharted2ToneMapping(const glm::vec3& color) {
            return gammaCorrect(Uncharted2ToneMappingF(color * s_thread_settings->m_exposure) * cr_float(1.3790642466494378));
        }

        glm::vec3 ReinhardToneMapping(const glm::vec3& color) {
            return gammaCorrect((color * s_thread_settings->m_exposure) / (glm::vec3(1.0) + color * s_thread_settings->m_exposure));
        }

        glm::vec3 gammaCorrect(const glm::vec3& color) {
            return glm::pow(glm::clamp(color, glm::vec3(0.0), glm::vec3(1.0)), glm::vec3(s_thread_settings->m_inverse_gamma));
        }

        glm::vec3 tonemap(const glm::vec3& color) {
            return s_thread_settings->m_exposure > 0.0 ? Uncharted2ToneMapping(color) : gammaCorrect(color);
        }

        void WriteColor(const Color& color, int32_t x, int32_t y) {
            size_t position = (y * m_dimensions.x * m_num_channels) + (x * m_num_channels);
            glm::vec3 updated_color = color.data;
            if (s_thread_settings->m_interactive) {
                size_t iteration_position = (y * m_dimensions.x) + x;
                cr_float iteration = cr_float(++m_iteration_ptr[iteration_position]);
                glm::vec3 old_val(0.0);
                for (int32_t i = 0; i < m_num_channels; ++i) {
                    old_val[i] = iteration > 1 ? cr_float(m_float_data_ptr[position + i]) : 0.0;
                }
                updated_color = old_val + ((updated_color - old_val) / cr_float(iteration));
            }

            for (int32_t i = 0; i < m_num_channels; ++i) {
                m_float_data_ptr[position + i] = float(updated_color[i]);
            }
            if (s_thread_settings->m_rgb) {
                glm::vec3 gamma_corrected = tonemap(updated_color);
                for (int32_t i = 0; i < m_num_channels; ++i) {
                    m_rgb_data_ptr[position + i] = Color::toUnsignedByte(gamma_corrected[i]);
                }
            }

            CRAY_DEBUG_PIXEL("(%.2f, %.2f) Color: %f, %f, %f, Stored: %f, %f, %f", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y,
                color.x, color.y, color.z, m_float_data_ptr[position + 0], m_float_data_ptr[position + 2], m_float_data_ptr[position + 2]);
        }

        float* getFloatDataPtr() {
            return m_float_data_ptr.get();
        }

        uint64_t getFloatByteSize() {
            return m_dimensions.x * m_dimensions.y * m_num_channels * sizeof(float);
        }

        uint8_t* getRGBDataPtr() {
            return m_rgb_data_ptr.get();
        }

        uint64_t getRGBByteSize() {
            return m_dimensions.x * m_dimensions.y * m_num_channels * sizeof(uint8_t);
        }

        void printStats() {
            uint64_t size = m_dimensions.x * m_dimensions.y * m_num_channels;
            CRAY_INFO("Framebuffer Stats:");
            CRAY_INFO("    Dimensions:                 %ux%u", m_dimensions.x, m_dimensions.y);
            CRAY_INFO("    Pixels:                     %u", m_dimensions.x * m_dimensions.y);
            CRAY_INFO("    Float Buffer Size:          %s", convertToCapacitySizeString(size * sizeof(cr_float)).c_str());
            if (s_global_settings->m_rgb) {
                CRAY_INFO("    RGB Buffer Size:            %s", convertToCapacitySizeString(size * sizeof(uint8_t)).c_str());
            }
            if (s_global_settings->m_interactive) {
                CRAY_INFO("    Iteration Buffer Size:      %s", convertToCapacitySizeString(size * sizeof(uint32_t)).c_str());
            }
        }
    };
    static std::shared_ptr<Framebuffer> s_global_framebuffer = { nullptr };

    struct HitRecord {
        struct IntersectInterface* primitives_to_ignore = { nullptr };
        struct PrimitiveInterface* primitive = { nullptr };
        glm::vec3 normal = { glm::vec3(0) };
        glm::vec2 uv = { glm::vec2(0) };
        glm::vec3 intersection_point = { glm::vec3(0) };
        glm::vec3 intersection_point_object_space = { glm::vec3(0) };
        cr_float distance = { std::numeric_limits<cr_float>::max() };
        bool valid = { false };
        struct MaterialInterface* material = { nullptr };
    };

    struct TextureMappingInterface : public NonCopyable {
        virtual glm::vec2 mapUV(const glm::vec2& p) const = 0;
        virtual glm::vec2 mapUV(const glm::vec3& p) const {
            CRAY_ASSERT_ALWAYS_MSG(false, "Invalid call to TextureMappingInterface");
            return glm::vec2(0);
        }
    };

    struct TextureMappingUV : TextureMappingInterface {
        glm::vec2 mapUV(const glm::vec2& p) const override { return p; }
    };

    struct TextureMappingPlanar : TextureMappingInterface {
        glm::vec2 mapUV(const glm::vec2& p) const override {
            glm::vec2 uv(glm::dot(p, glm::vec2(1, 0)), glm::dot(p, glm::vec2(0, 1)));
            CRAY_ASSERT_IF_FALSE_MSG(uv.x >= 0 && uv.x <= 1 && uv.y >= 0 && uv.y <= 1, "Wrong Planar mapping UVs %f, %f", uv.x, uv.y);
            return uv;
        }
    };

    struct TextureMappingSpherical : TextureMappingInterface {
        glm::vec2 mapUV(const glm::vec2& p) const override {
            CRAY_ASSERT_ALWAYS_MSG(false, "Invalid call to TextureMappingSpherical");
            return glm::vec2(0);
        }
        glm::vec2 mapUV(const glm::vec3& p) const override {
            cr_float theta = glm::clamp(glm::acos(p.y), cr_float(0.0), glm::pi<cr_float>());
            cr_float phi = glm::clamp(glm::atan(p.x, p.z), -glm::pi<cr_float>(), glm::pi<cr_float>());
            cr_float u = ((phi + glm::pi<cr_float>()) * glm::one_over_two_pi<cr_float>());
            cr_float v = theta * glm::one_over_pi<cr_float>();
            CRAY_ASSERT_IF_FALSE_MSG(u >= 0 && u <= 1 && v >= 0 && v <= 1, "Wrong Spherical mapping UVs %f, %f", u, v);
            return glm::vec2(u, v);
        }
    };

    struct TextureSampler {
        std::unique_ptr<TextureMappingInterface> m_mapping = { nullptr };
        CRAY_TEXTURE_WRAPPING m_wrap_mode = { CTW_CLAMP };
        CRAY_TEXTURE_FILTERING m_filtering_mode = { CTF_TRIANGLE };
        explicit TextureSampler(CRAY_TEXTURE_MAPPING mapping_mode, CRAY_TEXTURE_WRAPPING wrap_mode, CRAY_TEXTURE_FILTERING filtering_mode) :
            m_wrap_mode(wrap_mode), m_filtering_mode(filtering_mode) {
            switch (mapping_mode) {
            case CTM_UV:
                m_mapping = std::make_unique<TextureMappingUV>();
                break;
            case CTM_PLANAR:
                m_mapping = std::make_unique<TextureMappingPlanar>();
                break;
            case CTM_SPHERICAL:
                m_mapping = std::make_unique<TextureMappingSpherical>();
                break;
            }
        }

        glm::vec2 mapUV(const glm::vec2& uv) const { return m_mapping->mapUV(uv); }
        glm::vec2 wrapUV(const glm::vec2& uv) const {
            // convert to pixel coordinates and shift by 0.5 (pixel center is assumed to be at 0,0)
            glm::vec2 st = uv;// glm::ivec2(uv * wrap_to - 0.5);
            // wrapping
            if (m_wrap_mode == CTW_CLAMP) {
                // clamp to [0, 1]
                st = glm::clamp(st, glm::vec2(0), glm::vec2(1.0));
            }
            else if (m_wrap_mode == CTW_REPEAT) {
                // drop the integer part
                glm::vec2 i;
                st = glm::modf(st, i);
                if (uv.x == 1.0) st.x = 1.0;
                if (uv.y == 1.0) st.y = 1.0;
                // mirror if negative, i.e. a -0.25 texcoord should result in 0.75
                st.x = st.x < 0 ? st.x + cr_float(1.0) : st.x;
                st.y = st.y < 0 ? st.y + cr_float(1.0) : st.y;
            }
            return st;
        }
    };

    struct ImageInterface {
        ImageInterface() = default;
        virtual ~ImageInterface() = default;
        ImageInterface(const ImageInterface&) = default;
        ImageInterface& operator = (const ImageInterface&) = default;
        ImageInterface(ImageInterface&&) = default;
        ImageInterface& operator = (ImageInterface&&) = default;
        virtual Color getValue(const HitRecord& record, const TextureSampler* sampler) const = 0;
        virtual uint64_t getSize() const { return 0; }
    protected:
    };

    struct TextureInterface {
    private:
        std::shared_ptr<TextureSampler> m_sampler = { nullptr };
        std::shared_ptr<ImageInterface> m_image = { nullptr };
    public:
        TextureInterface(std::shared_ptr<ImageInterface>& image, std::shared_ptr<TextureSampler>& sampler) : m_sampler(sampler), m_image(image) {}
        Color getValue(const HitRecord& record) const { return m_image->getValue(record, m_sampler.get()); }
    protected:
    };

    struct ImageConstant : public ImageInterface {
        explicit ImageConstant(const Color& c) : ImageInterface(), m_color(c) { }
        Color getValue(const HitRecord& record, const TextureSampler* sampler) const override { return m_color; }
    protected:
        Color m_color;
    };

    struct ImageChecker : public ImageInterface {
        ImageChecker(std::shared_ptr<ImageInterface> todd, std::shared_ptr<ImageInterface> teven, cr_float frequency) :
            ImageInterface(), m_odd(todd), m_even(teven), m_frequency(glm::max(cr_float(1.0), frequency)) { }
    protected:
        std::shared_ptr<ImageInterface> m_odd = { nullptr };
        std::shared_ptr<ImageInterface> m_even = { nullptr };
        cr_float m_frequency = { 1.0 };
        Color getValue(const HitRecord& record, const TextureSampler* sampler) const override {
            // map UVs
            glm::vec2 uv_transformed = sampler->mapUV(record.uv);
            // wrap UVs
            uv_transformed = sampler->wrapUV(uv_transformed);
            bool is_odd = (int32_t(m_frequency * uv_transformed.x) + int32_t(m_frequency * uv_transformed.y)) % 2 == 0;
            return (is_odd) ? m_odd->getValue(record, sampler) : m_even->getValue(record, sampler);
        }
    };

    struct ImageFile : public ImageInterface {
        glm::vec2 m_pixel_offset = { glm::vec2(0) };
        glm::vec2 m_dim = { glm::vec2(0) };
        int32_t m_channels = { 0 };
        std::unique_ptr<float[]> m_ptr = { nullptr };
        explicit ImageFile(const uint8_t* ptr, int32_t width, int32_t height, int32_t channels, int32_t size_type, bool is_srgb) : ImageInterface(),
            m_dim(width, height), m_channels(channels) {
            size_t num_elements = m_dim.x * m_dim.y * m_channels;
            m_ptr = std::make_unique<float[]>(num_elements);
            m_pixel_offset = cr_float(1.0) / m_dim;

            // direct copy
            if (size_type == sizeof(float) && is_srgb == false) {
                memcpy(m_ptr.get(), ptr, width * height * channels);
            }
            else {
                // convert to float
                cr_float divisor = size_type == 1 ? 1.0 / 255.0 : 1.0;
                for (size_t i = 0; i < num_elements; ++i) {
                    m_ptr[i] = ptr[i] * divisor;
                    if (is_srgb) {
                        m_ptr[i] = glm::pow(glm::clamp(m_ptr[i], 0.0f, 1.0f), s_thread_settings->m_gamma);
                    }
                }
            }

            // invert image
            float* data_ptr = m_ptr.get();
            uint32_t row_stride = m_dim.x * m_channels;
            std::unique_ptr<float[]> tmpRow = std::make_unique<float[]>(row_stride);
            float* srcRow = static_cast<float*>(data_ptr);
            float* dstRow = static_cast<float*>(data_ptr) + ((int32_t(m_dim.y) - 1) * row_stride);
            for (uint32_t i = 0; i < (uint32_t)(m_dim.y) / 2; ++i) {
                memcpy(tmpRow.get(), dstRow, row_stride * sizeof(float));
                memcpy(dstRow, srcRow, row_stride * sizeof(float));
                memcpy(srcRow, tmpRow.get(), row_stride * sizeof(float));
                srcRow += row_stride;
                dstRow -= row_stride;
            }
        }

        uint64_t getSize() const override { return m_dim.x * m_dim.y * m_channels * sizeof(float); }

        Color getTexel(const glm::ivec2& texel) const {
            // compute texel
            Color texel_color(Color::Black());
            size_t mem_position = (texel.y * m_dim.x * m_channels) + (texel.x * m_channels);
            for (int32_t i = 0; i < m_channels; ++i) {
                texel_color.data[i] = m_ptr[mem_position + i];
            }
            return texel_color;
        }

        glm::vec2 getTransformedUV(const glm::vec2& uv, const TextureSampler* sampler) const {
            // map UVs
            glm::vec2 uv_transformed = sampler->mapUV(uv);
            // wrap UVs
            uv_transformed = sampler->wrapUV(uv_transformed);
            return uv_transformed;
        }

        glm::vec2 getPixel(const glm::vec2& uv) const {
            glm::vec2 pixel = glm::vec2(uv * m_dim);
            pixel = glm::clamp(pixel, glm::vec2(0), glm::vec2(m_dim - cr_float(1.0)));
            return pixel;
        }

        Color getValue(const HitRecord& record, const TextureSampler* sampler) const override {
#ifdef ENABLE_DEBUG_CURRENT_PIXEL
            if (isValidDebugPixel()) {
                CRAY_DEBUG_PIXEL("(%.2f, %.2f) ", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y)
            }
#endif // ENABLE_DEBUG_CURRENT_PIXEL

            Color color(Color::Black());
            // filter
            if (sampler->m_filtering_mode == CTF_BOX) {
                glm::vec2 uv = getTransformedUV(record.uv, sampler);
                glm::ivec2 pixel = getPixel(uv);
                // box filter (truncate to integer)
                color = getTexel(pixel);
            }
            else if (sampler->m_filtering_mode == CTF_TRIANGLE) {
                glm::vec2 base_uvs = (record.uv - glm::vec2(m_pixel_offset * cr_float(0.5)));
                glm::vec2 weights = m_dim * base_uvs;
                glm::vec2 weight_rt = weights - glm::floor(weights);
                glm::vec2 weight_lb = glm::vec2(cr_float(1.0) - weight_rt);
                glm::vec2 pixel_lb = getPixel(getTransformedUV(base_uvs, sampler));
                glm::vec2 pixel_rb = getPixel(getTransformedUV(glm::vec2(base_uvs.x + m_pixel_offset.x, base_uvs.y), sampler));
                glm::vec2 pixel_lt = getPixel(getTransformedUV(glm::vec2(base_uvs.x, base_uvs.y + m_pixel_offset.y), sampler));
                glm::vec2 pixel_rt = getPixel(getTransformedUV(glm::vec2(base_uvs + m_pixel_offset), sampler));
                // bilinear filter (weighted average of 4)
                color = getTexel(pixel_lb) * weight_lb.x * weight_lb.y +
                    getTexel(pixel_rb) * weight_rt.x * weight_lb.y +
                    getTexel(pixel_lt) * weight_lb.x * weight_rt.y +
                    getTexel(pixel_rt) * weight_rt.x * weight_rt.y;
            }
            return color;
        }
    protected:
        Color m_color;
    };

    enum MATERIAL_TYPE {
        MT_NONE = 0x00,
        MT_SIMPLE = 0x01,
        MT_DIFFUSE = 0x02,
        MT_SPECULAR_ROUGH = 0x04,
        MT_SPECULAR_SMOOTH = 0x08
    };
    using MaterialFlags = int32_t;

    struct Ray {
    public:
        glm::vec3 m_origin = { glm::vec3(0) };
        glm::vec3 m_direction = { glm::vec3(0) };
        bool m_visibility = { false };
        glm::vec3 m_direction_inv = { glm::vec3(0) };
        cr_float m_current_distance = { 0.0 };
        MaterialFlags material_flags = { MT_NONE };
        explicit Ray(const glm::vec3& origin, const glm::vec3& direction) : Ray(origin, direction, false) { }
        explicit Ray(const glm::vec3& origin, const glm::vec3& direction, bool visibility) :
            m_origin(origin), m_direction(direction), m_visibility(visibility) {
            m_direction_inv = cr_float(1.0) / m_direction;
        }

        glm::vec3 get_intersection_point(cr_float distance) const { return m_origin + m_direction * distance; }
    };

    struct MaterialInterface {
    protected:
        MaterialFlags m_type = { MT_NONE };
        std::shared_ptr<TextureInterface> m_reflection_texture = { nullptr };
        std::shared_ptr<TextureInterface> m_transmission_texture = { nullptr };
        Color m_reflection_color = { Color::Black() };
        Color m_transmission_color = { Color::Black() };
        glm::vec3 m_eta_t = { glm::vec3(0) }; // no refraction (dielectrics use only the x component)
        cr_float m_eta_i = { 1.0 }; // air
        bool m_supports_reflection = true;
        bool m_supports_transmission = false;
        bool m_is_current_ray_transmissive = false;

    public:
        friend struct MaterialGeneric;
        explicit MaterialInterface(MaterialFlags type = MT_SIMPLE) : m_type(type) {}
        explicit MaterialInterface(MaterialFlags type, const Color& reflection_color, bool supports_reflection, const Color& transmission_color, bool supports_transmission, const glm::vec3& eta_t, cr_float eta_i) :
            m_type(type), m_reflection_color(reflection_color), m_transmission_color(transmission_color),
            m_eta_t(glm::max(glm::vec3(0.0), eta_t)), m_eta_i(eta_i),
            m_supports_reflection(supports_reflection), m_supports_transmission(supports_transmission) {}

        virtual ~MaterialInterface() = default;
        MaterialInterface(const MaterialInterface&) = default;
        MaterialInterface& operator = (const MaterialInterface&) = default;
        MaterialInterface(MaterialInterface&&) = default;
        MaterialInterface& operator = (MaterialInterface&&) = default;
        bool hasType(MATERIAL_TYPE type) { return (m_type & type) > 0; }

        Color GetFresnelDielectric(cr_float HO) const {
            // reverse the indices if the ray is coming from inside the medium (d < 0) (i.e. transmitted ray)
            return Color(HO > 0.0 ? FresnelDielectric(HO, m_eta_i, m_eta_t.x) : FresnelDielectric(glm::abs(HO), m_eta_t.x, m_eta_i));
        }
        Color getReflectionColor(const HitRecord& record) const { return m_reflection_texture == nullptr ? m_reflection_color : m_reflection_color * m_reflection_texture->getValue(record); }
        Color getTransmissionColor(const HitRecord& record) const { return m_transmission_texture == nullptr ? m_transmission_color : m_transmission_color * m_transmission_texture->getValue(record); }

        virtual void preprocess() {}
        virtual cr_float getMaterialPDF(const glm::vec3& vertex_to_prev_direction, const glm::vec3& normal, const glm::vec3& vertex_to_next_direction, bool is_transmission, bool ignore_specular) const = 0;
        virtual Color getBSDF(const glm::vec3& vertex_to_prev_direction, const glm::vec3& vertex_to_next_direction, const HitRecord& record, bool is_transmission, bool ignore_specular) const = 0;
        virtual glm::vec3 spawnNewRayDirection(cr_float& out_pdf, const glm::vec3& vertex_to_prev_direction, const glm::vec3& normal, bool& is_transmission, bool& sampled_specular) const = 0;
        virtual void setDiffuseReflectionTexture(std::shared_ptr<TextureInterface>& reflection_texture) { }
        virtual void setDiffuseTransmissionTexture(std::shared_ptr<TextureInterface>& transmission_texture) { }
        virtual void setSpecularReflectionTexture(std::shared_ptr<TextureInterface>& reflection_texture) { }
        virtual void setSpecularTransmissionTexture(std::shared_ptr<TextureInterface>& transmission_texture) { }
    };

    struct MaterialDiffuse : public MaterialInterface {

        explicit MaterialDiffuse(const Color& diffuse_color, bool supports_reflection, const Color& transmission_color, bool supports_transmission, cr_float eta_t) :
            MaterialInterface(MT_DIFFUSE, diffuse_color, supports_reflection, transmission_color, supports_transmission, glm::vec3(eta_t), 1.0) {}

        void setDiffuseReflectionTexture(std::shared_ptr<TextureInterface>& reflection_texture) override { m_reflection_texture = reflection_texture; }
        void setDiffuseTransmissionTexture(std::shared_ptr<TextureInterface>& transmission_texture) override { m_transmission_texture = transmission_texture; }

        Color getBSDF(const glm::vec3& vertex_to_prev_direction, const glm::vec3& vertex_to_next_direction, const HitRecord& record, bool is_transmission, bool ignore_specular) const override {

            // calculate tangent, bitangent
            glm::vec3 tangent(0);
            glm::vec3 bitangent(0);
            getOrthonormalBasis(bitangent, tangent, record.normal);

            // convert directions to local frame
            glm::vec3 I(glm::dot(vertex_to_next_direction, tangent), glm::dot(vertex_to_next_direction, bitangent), glm::dot(vertex_to_next_direction, record.normal));
            glm::vec3 O(glm::dot(vertex_to_prev_direction, tangent), glm::dot(vertex_to_prev_direction, bitangent), glm::dot(vertex_to_prev_direction, record.normal));
            glm::vec3 N(0, 0, 1);
            I = glm::normalize(I);
            O = glm::normalize(O);
            N = glm::normalize(N);

            bool same_hemisphere_O_I = O.z * I.z > 0.0;

            // is this a reflection or refraction
            if (m_supports_reflection && !m_supports_transmission && is_transmission) {
                CRAY_ASSERT_ALWAYS_MSG(m_supports_reflection && !m_supports_transmission && is_transmission, "(%.2f, %.2f): Pure reflective surface cannot have refractive ray. Degrees %f. Skipping", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, glm::degrees(glm::acos(glm::dot(O, N))));
                return Color(0.0);
            }
            else if (!m_supports_reflection && m_supports_transmission && !is_transmission) {
                // can be called by L_direct
                // CRAY_ASSERT_ALWAYS_MSG(!m_supports_reflection && m_supports_transmission && !is_transmission, "(%.2f, %.2f): Pure refractive surface cannot have reflective ray. Degrees %f. Skipping", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, glm::degrees(glm::acos(glm::dot(O, N))));
                return Color(0.0);
            }
            // if reflective ray and opposite sides of the hemisphere for I/O (return black)
            // if refractive and same sides of the hemisphere for I/O (return black)
            else if ((!is_transmission && !same_hemisphere_O_I) || (is_transmission && same_hemisphere_O_I)) {
                return Color(0.0);
            }

            Color bsdf = !is_transmission ? getReflectionColor(record) : getTransmissionColor(record);
            bsdf *= glm::one_over_pi<cr_float>();
            return bsdf;
        }

        glm::vec3 spawnNewRayDirection(cr_float& out_pdf, const glm::vec3& vertex_to_prev_direction, const glm::vec3& normal, bool& is_transmission, bool& sampled_specular) const override {
            out_pdf = 0.0;
            sampled_specular = false;
            is_transmission = false;

            // calculate tangent, bitangent
            glm::vec3 tangent(0);
            glm::vec3 bitangent(0);
            getOrthonormalBasis(bitangent, tangent, normal);

            // convert outgoing direction to local frame
            glm::vec3 O(glm::dot(vertex_to_prev_direction, tangent), glm::dot(vertex_to_prev_direction, bitangent), glm::dot(vertex_to_prev_direction, normal));
            O = glm::normalize(O);

            // check if the surface is reflective, refractive, or both
            bool reflect = true;
            cr_float fresnel_pdf = 1.0;
            // if its both, choose a sampling direction, otherwise PDF is 1
            if (m_supports_reflection && m_supports_transmission) {
                // calculate fresnel
                cr_float HO = O.z;
                Color F = GetFresnelDielectric(HO);
                cr_float r = rand_f();
                if (r < F.x) {
                    // reflect
                    fresnel_pdf = F.x;
                }
                else {
                    // refract
                    fresnel_pdf = 1 - F.x;
                    reflect = false;
                }
            }
            // if its just refractive
            else if (m_supports_transmission) {
                reflect = false;
            }

            is_transmission = !reflect;

            // retrieve the microfacet (reflection/refraction)
            glm::vec3 I(0.0);
            if (reflect) {
                I = getCosineHemisphereSample();
                CRAY_DEBUG_PIXEL("(%.2f, %.2f): Chose new reflection incoming direction. O: %f %f %f, I: %f %f %f. NI: %f, NO: %f", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, O.x, O.y, O.z, I.x, I.y, I.z, glm::degrees(glm::acos(I.z)), glm::degrees(glm::acos(O.z)));
                out_pdf = getPDFReflection(O, I);
            }
            else {
                I = getCosineHemisphereSample();
                // set different hemisphere for I and O
                if (O.z > 0) {
                    I.z = -I.z;
                }
                I = glm::normalize(I);
                CRAY_DEBUG_PIXEL("(%.2f, %.2f): Chose new refraction incoming direction. O: %f %f %f, I: %f %f %f. NI: %f, NO: %f", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, O.x, O.y, O.z, I.x, I.y, I.z, glm::degrees(glm::acos(I.z)), glm::degrees(glm::acos(O.z)));
                out_pdf = getPDFTransmission(O, I);
            }

            if (normal == glm::vec3(0.0) || I.z == 0.0 || O.z == 0.0) {
                CRAY_DEBUG_PIXEL("(%.2f, %.2f): Zero normal vector or zero directions. O: %f %f %f, I: %f %f %f, N: %f %f %f. Skipping", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, O.x, O.y, O.z, I.x, I.y, I.z, normal.x, normal.y, normal.z);
                return glm::vec3(0.0);
            }

            if (out_pdf <= 0.0) {
                //CRAY_WARNING("(%.2f, %.2f): %s PDF is 0 or negative. %f", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, is_transmission ? "Transmission" : "Reflection", out_pdf);
                return glm::vec3(0.0);
            }

            // align with normal hemisphere
            glm::vec3 sample_direction = glm::normalize(tangent * I.x + bitangent * I.y + normal * I.z);

            // weight the pdf by the Fresnel term (if it has been used)
            out_pdf /= fresnel_pdf;

            return sample_direction;
        }

        cr_float getMaterialPDF(const glm::vec3& vertex_to_prev_direction, const glm::vec3& normal, const glm::vec3& vertex_to_next_direction, bool is_transmission, bool ignore_specular) const override {
            // calculate tangent, bitangent
            glm::vec3 tangent(0);
            glm::vec3 bitangent(0);
            getOrthonormalBasis(bitangent, tangent, normal);

            // convert directions to local frame
            glm::vec3 I(glm::dot(vertex_to_next_direction, tangent), glm::dot(vertex_to_next_direction, bitangent), glm::dot(vertex_to_next_direction, normal));
            glm::vec3 O(glm::dot(vertex_to_prev_direction, tangent), glm::dot(vertex_to_prev_direction, bitangent), glm::dot(vertex_to_prev_direction, normal));
            I = glm::normalize(I);
            O = glm::normalize(O);

            return !is_transmission ? getPDFReflection(O, I) : getPDFTransmission(O, I);
        }

        cr_float getPDFReflection(const glm::vec3& O, const glm::vec3& I) const {
            bool above_hemisphere_O = O.z > 0.0;
            // if the ray is coming from below the hemisphere, skip
            if (!above_hemisphere_O) {
                return 0.0;
            }
            return getCosineHemisphereSamplePDF(I);
        }

        cr_float getPDFTransmission(const glm::vec3& O, const glm::vec3& I) const {
            bool same_hemisphere_O_I = O.z * I.z > 0.0;
            if (same_hemisphere_O_I) {
                CRAY_WARNING("(%.2f, %.2f): I and O are in the same hemisphere. O: %f %f %f, I: %f %f %f. NI: %f, NO: %f", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, O.x, O.y, O.z, I.x, I.y, I.z, glm::degrees(glm::acos(I.z)), glm::degrees(glm::acos(O.z)));
                return 0.0;
            }
            return getCosineHemisphereSamplePDF(I);
        }
    };

    struct MaterialMicrofacetSpecular : public MaterialInterface {

        Color m_conductor_absorption = { Color::Black() };
        cr_float m_alpha = { 0 };
        cr_float m_roughness = { 0 };
        bool m_is_rough_surface = false;
        bool m_is_dielectric = true;

        // constructor for conductors
        explicit MaterialMicrofacetSpecular(const Color& reflection_color, const Color& absorption, const glm::vec3& eta, cr_float roughness) :
            MaterialInterface(MT_SPECULAR_ROUGH, reflection_color, true, Color::Black(), false, eta, 1.0),
            m_conductor_absorption(absorption), m_roughness(roughness), m_is_dielectric(false) {
            preprocess();
        }

        // constructor for dielectrics
        explicit MaterialMicrofacetSpecular(const Color& reflection_color, bool has_reflection, const Color& transmission_color, bool has_transmission,
            cr_float eta, cr_float roughness) :
            MaterialInterface(MT_SPECULAR_ROUGH, reflection_color, has_reflection, transmission_color, has_transmission, glm::vec3(eta), 1.0),
            m_roughness(roughness), m_is_dielectric(true) {
            preprocess();
        }

        void setSpecularReflectionTexture(std::shared_ptr<TextureInterface>& reflection_texture) override { m_reflection_texture = reflection_texture; }
        void setSpecularTransmissionTexture(std::shared_ptr<TextureInterface>& transmission_texture) override { m_transmission_texture = transmission_texture; }

        Color GetFresnelConductor(cr_float HO) const {
            return FresnelConductor(HO, m_eta_i, m_eta_t, m_conductor_absorption.data);
        }

        void preprocess() override {
            m_is_rough_surface = m_roughness > 0.0;
            //m_alpha = m_is_rough_surface ? GGXRoughnessToAlpha(m_roughness) : 0.001;
            m_alpha = m_is_rough_surface ? m_roughness : 0.0;
            m_type = m_is_rough_surface ? MT_SPECULAR_ROUGH : MT_SPECULAR_SMOOTH;
        }

        // The Microfacet BSDF function, describing the scattered light energy towards a particular direction
        // Returns the reflected RGB color
        Color getBSDF(const glm::vec3& vertex_to_prev_direction, const glm::vec3& vertex_to_next_direction, const HitRecord& record, bool is_transmission, bool ignore_specular) const override {
            if (!m_is_rough_surface && ignore_specular) {
                return Color::Black();
            }
            // calculate tangent, bitangent
            glm::vec3 tangent(0);
            glm::vec3 bitangent(0);
            getOrthonormalBasis(bitangent, tangent, record.normal);

            // convert directions to local frame
            glm::vec3 I(glm::dot(vertex_to_next_direction, tangent), glm::dot(vertex_to_next_direction, bitangent), glm::dot(vertex_to_next_direction, record.normal));
            glm::vec3 O(glm::dot(vertex_to_prev_direction, tangent), glm::dot(vertex_to_prev_direction, bitangent), glm::dot(vertex_to_prev_direction, record.normal));
            glm::vec3 N(0, 0, 1);
            I = glm::normalize(I);
            O = glm::normalize(O);
            N = glm::normalize(N);

            bool same_hemisphere_O_I = O.z * I.z > 0.0;

            // is this a reflection or refraction
            if (m_supports_reflection && !m_supports_transmission && is_transmission) {
                CRAY_ASSERT_ALWAYS_MSG(m_supports_reflection && !m_supports_transmission && is_transmission, "(%.2f, %.2f): Pure reflective surface cannot have refractive ray. Degrees %f. Skipping", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, glm::degrees(glm::acos(glm::dot(O, N))));
                return Color(0.0);
            }
            else if (!m_supports_reflection && m_supports_transmission && !is_transmission) {
                // can be called by L_direct
                // CRAY_ASSERT_ALWAYS_MSG(!m_supports_reflection && m_supports_transmission && !is_transmission, "(%.2f, %.2f): Pure refractive surface cannot have reflective ray. Degrees %f. Skipping", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, glm::degrees(glm::acos(glm::dot(O, N))));
                return Color(0.0);
            }
            // if reflective ray and opposite sides of the hemisphere for I/O (return black)
            // if refractive and same sides of the hemisphere for I/O (return black)
            else if ((!is_transmission && !same_hemisphere_O_I) || (is_transmission && same_hemisphere_O_I)) {
                return Color(0.0);
            }

            // Retrieve the halfway vector
            glm::vec3 H(0.0);
            if (!is_transmission) {
                H = O + I;
                H = glm::normalize(H);
                CRAY_DEBUG_PIXEL("(%.2f, %.2f): Retrieved reflection half vector. H: %f %f %f. O: %f %f %f, I: %f %f %f. HI: %f, HO: %f", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, H.x, H.y, H.z, O.x, O.y, O.z, I.x, I.y, I.z,
                    glm::degrees(glm::acos(glm::dot(H, I))), glm::degrees(glm::acos(glm::dot(H, O))));
            }
            else {
                // reverse the indices if the ray is coming from inside the medium (i.e. transmitted ray)
                bool ray_entering_medium = O.z > 0.0;
                cr_float indices_ratio = (ray_entering_medium) ? m_eta_t.x / m_eta_i : m_eta_i / m_eta_t.x;
                H = O + I * indices_ratio;
                // H points into the medium with the lowest IOR, e.g. outside the surface
                if (H.z < 0) {
                    H = -H;
                }
                H = glm::normalize(H);
                CRAY_DEBUG_PIXEL("(%.2f, %.2f): Retrieved refraction half vector. H: %f %f %f. O: %f %f %f, I: %f %f %f. HI: %f, HO: %f", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, H.x, H.y, H.z, O.x, O.y, O.z, I.x, I.y, I.z,
                    glm::degrees(glm::acos(glm::dot(H, I))), glm::degrees(glm::acos(glm::dot(H, O))));
                // handle total internal reflection (all vectors are in the same hemisphere)
                cr_float HI = glm::dot(H, I);
                if (glm::dot(O, H) * HI > 0.0) {
                    //CRAY_DEBUG_PIXEL("(%.2f, %.2f): Total internal reflection HI is %f degrees (< 90)", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, glm::degrees(glm::acos(HI)));
                    return Color(0.0);
                }
            }

            // dot products used frequently
            // cosine of the angle between normal and incident (light) dir -> theta_i
            cr_float NI = I.z;
            // cosine of the angle between normal and out (view) dir -> theta_o
            cr_float NO = O.z;
            // cosine of the angle between microfacet normal and normal -> theta_h
            cr_float NH = H.z;
            // cosine of the angle between microfacet normal out (view) dir -> theta_o
            cr_float HO = glm::dot(O, H);
            // cosine of the angle between microfacet normal and incident (light) dir -> theta_i
            cr_float HI = glm::dot(H, I);

            // calculate fresnel
            Color F = m_is_dielectric ? GetFresnelDielectric(HO) : GetFresnelConductor(HO);

            Color bsdf(Color::Black());
            if (m_is_rough_surface) {
                // Microfacet distribution
                cr_float D = Distribution_GGX_isotropic(NH, m_alpha);
                // Masking/shadowing geometric terms
                cr_float G = Geometric_Smith_GGX(HI, HO, NI, NO, m_alpha);

                if (!is_transmission) {
                    bsdf = (NO * NI > 0.0) ? getReflectionColor(record) * F * D * G * 0.25 : Color::Black();
                    cr_float denom = (glm::abs(NO) * glm::abs(NI));
                    bsdf /= denom;
                }
                else {
                    bsdf = (NO * NI < 0.0) ? getTransmissionColor(record) * m_eta_t.x * m_eta_t.x * (1.0 - F.x) * D * G * glm::abs(HO) * glm::abs(HI) : Color::Black();
                    cr_float denom = m_eta_i * HO + m_eta_t.x * HI;
                    denom *= denom;
                    denom = denom * glm::abs(NO) * glm::abs(NI);
                    bsdf /= denom;
                }
            }
            else {
                if (!is_transmission) {
                    Color color = getReflectionColor(record) * F;
                    bsdf = (NO * NI > 0.0) ? color : Color::Black();
                }
                else {
                    Color color = getTransmissionColor(record) * (1.0 - F.x);
                    bsdf = (NO * NI < 0.0) ? color : Color::Black();
                }
                cr_float denom = glm::abs(NI);
                bsdf /= denom;
            }
            return Color(bsdf);
        }

        glm::vec3 spawnNewRayDirection(cr_float& out_pdf, const glm::vec3& vertex_to_prev_direction, const glm::vec3& normal, bool& is_transmission, bool& sampled_specular) const override {
            out_pdf = 0.0;
            is_transmission = false;

            // calculate tangent, bitangent
            glm::vec3 tangent(0);
            glm::vec3 bitangent(0);
            getOrthonormalBasis(bitangent, tangent, normal);

            // convert outgoing direction to local frame
            glm::vec3 O(glm::dot(vertex_to_prev_direction, tangent), glm::dot(vertex_to_prev_direction, bitangent), glm::dot(vertex_to_prev_direction, normal));
            O = glm::normalize(O);
            // for dirac surfaces (i.e. perfect specular), simply use as half vector the normal as this maximizes the microsurface orientation
            // this is a cheat, since dirac functions must be handled explicitly by integrators. However, this incorporates near-perfect surfaces into MC without much extra work
            glm::vec3 H = m_is_rough_surface ? generateMicrofacetH_GGX(m_alpha) : glm::vec3(0, 0, 1);
            H = glm::normalize(H);
            sampled_specular = !m_is_rough_surface;

            bool above_hemisphere_O = O.z > 0.0;

            // check if the surface is reflective, refractive, or both
            bool reflect = true;
            cr_float fresnel_pdf = 1.0;
            // if its both, choose sampling direction based on Fresnel, otherwise PDF is 1
            if (m_supports_reflection && m_supports_transmission) {
                // calculate fresnel
                cr_float HO = glm::dot(O, H);
                Color F = GetFresnelDielectric(HO);
                cr_float r = rand_f();
                if (r < F.x) {
                    // reflect
                    fresnel_pdf = F.x;
                }
                else {
                    // refract
                    fresnel_pdf = 1 - F.x;
                    reflect = false;
                }
            }
            // if its just refractive
            else if (m_supports_transmission) {
                reflect = false;
            }

            // retrieve the microfacet (reflection/refraction)
            glm::vec3 I(0.0);
            if (reflect) {
                // if the ray is coming from below the hemisphere, skip
                if (!above_hemisphere_O) {
                    return glm::vec3(0.0);
                }
                I = glm::reflect(-O, H);
                I = glm::normalize(I);
                CRAY_DEBUG_PIXEL("(%.2f, %.2f): Chose new reflection incoming direction. H: %f %f %f. O: %f %f %f, I: %f %f %f. HI: %f, HO: %f", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, H.x, H.y, H.z, O.x, O.y, O.z, I.x, I.y, I.z,
                    glm::degrees(glm::acos(glm::dot(H, I))), glm::degrees(glm::acos(glm::dot(H, O))));
            }
            else {
                // reverse the indices if the ray is coming from inside the medium (above_hemisphere_O = false)
                bool ray_entering_medium = above_hemisphere_O;
                cr_float indices_ratio = (ray_entering_medium) ? m_eta_i / m_eta_t.x : m_eta_t.x / m_eta_i;
                // H needs to be in the same hemisphere with O for the refract function. In case the ray is leaving the medium, O is coming from the opposite direction and H needs to be inverted
                glm::vec3 H_aligned_O = (ray_entering_medium) ? H : glm::dot(glm::vec3(0, 0, 1), -O) < 0.0 ? H : -H;
                I = glm::refract(-O, H_aligned_O, indices_ratio);
                CRAY_DEBUG_PIXEL("(%.2f, %.2f): Chose new refraction incoming direction. H: %f %f %f. O: %f %f %f, I: %f %f %f. HI: %f, HO: %f", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, H.x, H.y, H.z, O.x, O.y, O.z, I.x, I.y, I.z,
                    glm::degrees(glm::acos(glm::dot(H, I))), glm::degrees(glm::acos(glm::dot(H, O))));
                bool same_hemisphere_O_I = O.z * I.z > 0.0;
                // handle total internal reflection
                if (I == glm::vec3(0.0) || same_hemisphere_O_I) {
                    //CRAY_WARNING("(%.2f, %.2f): Total internal reflection. Skipping.");
                    return glm::vec3(0.0);
                }
                I = glm::normalize(I);
            }

            if (normal == glm::vec3(0.0) || I.z == 0.0 || O.z == 0.0) {
                CRAY_DEBUG_PIXEL("(%.2f, %.2f): Zero normal vector or zero directions. O: %f %f %f, I: %f %f %f, N: %f %f %f. Skipping", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, O.x, O.y, O.z, I.x, I.y, I.z, normal.x, normal.y, normal.z);
                return glm::vec3(0.0);
            }

            // obtain PDF
            out_pdf = reflect ? getPDFReflection(O, H, I) : getPDFTransmission(O, H, I);

            if (!reflect && out_pdf > 0.0) {
                is_transmission = true;
            }

            if (out_pdf <= 0.0) {
                //CRAY_WARNING("(%.2f, %.2f): %s PDF is 0 or negative. %f", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, is_transmission ? "Transmission" : "Reflection", out_pdf);
                return glm::vec3(0.0);
            }

            // align with normal hemisphere
            glm::vec3 sample_direction = glm::normalize(tangent * I.x + bitangent * I.y + normal * I.z);

            // weight the pdf by the Fresnel term (if it has been used)
            out_pdf /= fresnel_pdf;

            return sample_direction;
        }

        cr_float getMaterialPDF(const glm::vec3& vertex_to_prev_direction, const glm::vec3& normal, const glm::vec3& vertex_to_next_direction, bool is_transmission, bool ignore_specular) const override {
            if (!m_is_rough_surface && ignore_specular) {
                return 0.0;
            }
            // calculate tangent, bitangent
            glm::vec3 tangent(0);
            glm::vec3 bitangent(0);
            getOrthonormalBasis(bitangent, tangent, normal);

            // convert directions to local frame
            glm::vec3 I = glm::vec3(glm::dot(vertex_to_next_direction, tangent), glm::dot(vertex_to_next_direction, bitangent), glm::dot(vertex_to_next_direction, normal));
            glm::vec3 O = glm::vec3(glm::dot(vertex_to_prev_direction, tangent), glm::dot(vertex_to_prev_direction, bitangent), glm::dot(vertex_to_prev_direction, normal));
            I = glm::normalize(I);
            O = glm::normalize(O);
            if (!is_transmission) {
                glm::vec3 H = glm::normalize(O + I);
                return getPDFReflection(O, H, I);
            }
            else {
                bool ray_entering_medium = O.z > 0.0;
                cr_float indices_ratio = (ray_entering_medium) ? m_eta_t.x / m_eta_i : m_eta_i / m_eta_t.x;
                glm::vec3 H = glm::normalize(O + I * indices_ratio);
                return getPDFTransmission(O, H, I);
            }
        }

        cr_float getPDFReflection(const glm::vec3& O, const glm::vec3& H, const glm::vec3& I) const {

            bool same_hemisphere_O_H = O.z * H.z > 0.0;
            bool same_hemisphere_O_I = O.z * I.z > 0.0;
            if (!same_hemisphere_O_I || !same_hemisphere_O_H) {
                return 0.0;
            }
            //CRAY_ASSERT_IF_FALSE_MSG(H.z > 0.0, "(%.2f, %.2f): Reflected half vector on opposite side of the hemisphere (%f degrees < 90))", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, glm::degrees(glm::acos(glm::dot(H, glm::vec3(0, 0, 1)))));
            if (!m_is_rough_surface) {
                return 1.0;
            }
            // obtain pdf for reflection dir
            // ideally, we should sample from the visible microfacet normals (source: PBRT eq: 14.2)
            cr_float out_pdf = getMicrofacetPDF(H.z, m_alpha);
            // convert from half angle to the incoming vector O
            cr_float jacobian_dwh_dwi = 0.25 / glm::abs(glm::dot(H, O));
            out_pdf *= jacobian_dwh_dwi;
            return out_pdf;
        }

        cr_float getPDFTransmission(const glm::vec3& O, const glm::vec3& H, const glm::vec3& I) const {
            bool same_hemisphere_O_I = O.z * I.z > 0.0;
            // obtain pdf for refraction dir
            // for refraction: if the incoming/outgoing directions reside on the same side (e.g. total internal reflection), skip it
            if (same_hemisphere_O_I) {
                CRAY_ASSERT_ALWAYS_MSG(same_hemisphere_O_I, "(%.2f, %.2f): Total internal reflection not captured before (%f degrees < 90))", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, glm::degrees(glm::acos(glm::dot(O, I))));
                return 0.0;
            }
            if (!m_is_rough_surface) {
                return 1.0;
            }
            // ideally, we should sample from the visible microfacet normals (source: PBRT eq: 14.2)
            cr_float out_pdf = getMicrofacetPDF(H.z, m_alpha);
            // convert from half angle to the incoming vector O
            cr_float jacobian_dwh_dwi = m_eta_t.x * m_eta_t.x * glm::abs(glm::dot(I, H));
            cr_float denom = m_eta_i * glm::dot(O, H) + m_eta_t.x * glm::dot(I, H);
            jacobian_dwh_dwi /= denom * denom;
            out_pdf *= jacobian_dwh_dwi;
            return out_pdf;
        }
    };

    struct MaterialGeneric : public MaterialInterface {
        std::shared_ptr<MaterialDiffuse> m_diffuse;
        std::shared_ptr<MaterialMicrofacetSpecular> m_specular;

        explicit MaterialGeneric(std::shared_ptr<MaterialDiffuse> diffuse, std::shared_ptr<MaterialMicrofacetSpecular> specular) : MaterialInterface(MT_NONE),
            m_diffuse(diffuse), m_specular(specular) {
            CRAY_ASSERT_IF_FALSE_MSG(m_diffuse != nullptr && m_specular != nullptr, "Both diffuse and specular materials must be provided.")
                preprocess();
        }

        void setDiffuseReflectionTexture(std::shared_ptr<TextureInterface>& reflection_texture) override { m_diffuse->setDiffuseReflectionTexture(reflection_texture); }
        void setSpecularReflectionTexture(std::shared_ptr<TextureInterface>& reflection_texture) override { m_specular->setSpecularReflectionTexture(reflection_texture); }

        void preprocess() override {
            m_type = m_diffuse->m_type | m_specular->m_type;
        }

        Color getBSDF(const glm::vec3& vertex_to_prev_direction, const glm::vec3& vertex_to_next_direction, const HitRecord& record, bool is_transmission, bool ignore_specular) const override {
            Color bsdf(Color::Black());
            bsdf += m_diffuse->getBSDF(vertex_to_prev_direction, vertex_to_next_direction, record, is_transmission, ignore_specular);
            if (!ignore_specular) {
                bsdf += m_specular->getBSDF(vertex_to_prev_direction, vertex_to_next_direction, record, is_transmission, ignore_specular);
            }
            return bsdf;
        }

        glm::vec3 spawnNewRayDirection(cr_float& out_pdf, const glm::vec3& vertex_to_prev_direction, const glm::vec3& normal, bool& is_transmission, bool& sampled_specular) const override {
            glm::vec3 new_sample_direction(0.0);
            cr_float r = rand_f();
            if (r < 0.5) {
                new_sample_direction = m_diffuse->spawnNewRayDirection(out_pdf, vertex_to_prev_direction, normal, is_transmission, sampled_specular);
                out_pdf += m_specular->getMaterialPDF(vertex_to_prev_direction, normal, new_sample_direction, is_transmission, false);
            }
            else {
                new_sample_direction = m_specular->spawnNewRayDirection(out_pdf, vertex_to_prev_direction, normal, is_transmission, sampled_specular);
                out_pdf += m_diffuse->getMaterialPDF(vertex_to_prev_direction, normal, new_sample_direction, is_transmission, false);
            }
            out_pdf *= 0.5;
            return new_sample_direction;
        }

        cr_float getMaterialPDF(const glm::vec3& vertex_to_prev_direction, const glm::vec3& normal, const glm::vec3& vertex_to_next_direction, bool is_transmission, bool ignore_specular) const override {
            // calculate tangent, bitangent
            glm::vec3 tangent(0);
            glm::vec3 bitangent(0);
            getOrthonormalBasis(bitangent, tangent, normal);

            // convert directions to local frame
            glm::vec3 I = glm::vec3(glm::dot(vertex_to_next_direction, tangent), glm::dot(vertex_to_next_direction, bitangent), glm::dot(vertex_to_next_direction, normal));
            glm::vec3 O = glm::vec3(glm::dot(vertex_to_prev_direction, tangent), glm::dot(vertex_to_prev_direction, bitangent), glm::dot(vertex_to_prev_direction, normal));
            I = glm::normalize(I);
            O = glm::normalize(O);
            glm::vec3 H = glm::normalize(O + I);
            cr_float out_pdf = m_diffuse->getPDFReflection(O, I);
            if (!m_specular->m_is_rough_surface && !ignore_specular) {
                out_pdf += m_specular->getPDFReflection(O, H, I);
                out_pdf *= 0.5;
            }
            return out_pdf;
        }
    };

    struct TransformableInterface {
    public:
        TransformableInterface() = default;
        virtual ~TransformableInterface() = default;
        TransformableInterface(const TransformableInterface&) = default;
        TransformableInterface& operator = (const TransformableInterface&) = default;
        TransformableInterface(TransformableInterface&&) = default;
        TransformableInterface& operator = (TransformableInterface&&) = default;
        void updateTransform(const glm::mat4x4& matrix) {
            m_transform = matrix;
            m_transform_inverse = glm::inverse(matrix);
            m_transform_normal = glm::transpose(m_transform_inverse);
            m_transform_normal_inverse = glm::transpose(m_transform);
        }
        const glm::mat4x4& getTransform() const { return m_transform; }
        const glm::mat4x4& getTransformInverse() const { return m_transform_inverse; }
        const glm::mat4x4& getTransformNormal() const { return m_transform_normal; }
        const glm::mat4x4& getTransformNormalInverse() const { return m_transform_normal_inverse; }
    private:
        glm::mat4x4 m_transform = { glm::mat4x4(1) };
        glm::mat4x4 m_transform_inverse = { glm::mat4x4(1) };
        glm::mat4x4 m_transform_normal = { glm::mat4x4(1) };
        glm::mat4x4 m_transform_normal_inverse = { glm::mat4x4(1) };
    };

    struct CameraInterface : public TransformableInterface {
        cr_float m_aspect_ratio = { 0 };
        glm::vec3 m_eye = { glm::vec3(0) };
        glm::vec3 m_target = { glm::vec3(0,0,-1) };
        glm::vec3 m_up = { glm::vec3(0) };
        glm::vec3 m_right = { glm::vec3(0) };
        glm::vec3 m_dir = { glm::vec3(0) };
        glm::mat4x4 m_wcs_to_ecs = { glm::mat4x4(1) };
        glm::mat4x4 m_ecs_to_wcs = { glm::mat4x4(1) };
        CameraInterface(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) :
            m_eye(position), m_target(target), m_up(up) {}
        virtual ~CameraInterface() = default;
        CameraInterface(const CameraInterface&) = default;
        CameraInterface& operator = (const CameraInterface&) = default;
        CameraInterface(CameraInterface&&) = default;
        CameraInterface& operator = (CameraInterface&&) = default;
        virtual void preprocess() {
            generateUVW();
            m_aspect_ratio = s_global_framebuffer->m_dimensions.x / static_cast<cr_float>(s_global_framebuffer->m_dimensions.y);
        }

        virtual Ray generateRay(int32_t x, int32_t y, bool first) = 0;

        void generateUVW() {
            m_eye = transformPoint(getTransform(), m_eye);
            m_target = transformPoint(getTransform(), m_target);
            m_up = transformPoint(getTransform(), m_up);
            m_dir = glm::normalize(m_target - m_eye);
            m_right = glm::cross(m_dir, m_up);
            m_right = glm::normalize(m_right);
            m_up = glm::cross(m_right, m_dir);
            m_up = glm::normalize(m_up);
            m_wcs_to_ecs = glm::lookAt(m_eye, m_target, m_up);
            m_ecs_to_wcs = glm::inverse(m_wcs_to_ecs);
        }

        void getImageSpaceOffsets(cr_float& s, cr_float& t, int32_t x, int32_t y, bool first) {
            s = cr_float(x + 0.5);
            t = cr_float(y + 0.5);
            // jitter the camera positions if needed
            if (!first || s_thread_settings->m_interactive) {
                s += rand_f_range(-0.5, 0.5);
                t += rand_f_range(-0.5, 0.5);
            }
            s_thread_settings->m_debug_data.current_pixel = glm::vec2(s, t);
            s /= cr_float(s_global_framebuffer->m_dimensions.x);
            t /= cr_float(s_global_framebuffer->m_dimensions.y);
        }
    };

    struct CameraPerspective : public CameraInterface {
    public:
        explicit CameraPerspective(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up,
            cr_float aperture, cr_float near, cr_float far, cr_float lens_radius, cr_float focal_distance) :
            CameraInterface(position, target, up),
            m_aperture(aperture), m_near(near), m_far(far), m_lens_radius(lens_radius), m_focal_distance(focal_distance) {
            if (m_lens_radius <= 0.0 || m_focal_distance <= 0) {
                m_lens_radius = 0.0;
                m_focal_distance = 0.0;
            }
        }
        cr_float m_aperture = { 0.0 };
        cr_float m_near = { FLOAT_TOLERANCE_2 };
        cr_float m_far = { std::numeric_limits<cr_float>::max() };
        cr_float m_lens_radius = { 0.0 };
        cr_float m_focal_distance = { 0.0 };
        cr_float m_near_width = { 0.0 };
        cr_float m_near_height = { 0.0 };
        glm::vec3 m_fpos[2];
        enum FRUSTUM_CORNERS { FC_LTF, FC_LTN }; // left-top-far, left-top-near

        void preprocess() override {
            CameraInterface::preprocess();
            if (m_near <= 0.0) {
                // assume a 35mm film (36mm x 24mm). The relationship between FOV, film and focal length is tan(fov/2) = dim * 0.5 / focal_length
                cr_float focal_length = 0.012 / glm::tan(m_aperture);
                m_near = focal_length;
            }

            cr_float vFOV = m_aperture * cr_float(0.5);
            cr_float half_height_near = m_near * glm::tan(glm::radians(vFOV));
            cr_float half_width_near = half_height_near * m_aspect_ratio;
            m_near_height = half_height_near * cr_float(2.0);
            m_near_width = half_width_near * cr_float(2.0);
            m_fpos[FC_LTN] = glm::vec3(-m_near_width * cr_float(0.5), m_near_height * cr_float(0.5), -m_near);
        }

        Ray generateRay(int32_t x, int32_t y, bool first) override {
            cr_float s = 0.0;
            cr_float t = 0.0;
            getImageSpaceOffsets(s, t, x, y, first);
            glm::vec3 image_plane_pos(m_fpos[FC_LTN].x + m_near_width * s, m_fpos[FC_LTN].y - m_near_height * t, m_fpos[FC_LTN].z);
            glm::vec3 direction = glm::normalize(image_plane_pos);
            if (m_lens_radius <= 0.0) {
                return Ray(m_eye, transformVector(m_ecs_to_wcs, direction));
            }
            else {
                cr_float focal_point_t = -m_focal_distance / direction.z;
                glm::vec3 focal_point = direction * focal_point_t;
                glm::vec2 lens_point = getUniformDiscSample() * m_lens_radius;
                glm::vec3 new_eye = glm::vec3(lens_point.x, lens_point.y, 0.0);
                glm::vec3 new_direction = glm::normalize(focal_point - new_eye);
                return Ray(transformPoint(m_ecs_to_wcs, new_eye), transformVector(m_ecs_to_wcs, new_direction));
            }
        }
    };

    struct CameraOrthographic : public CameraInterface {
    public:
        explicit CameraOrthographic(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up,
            cr_float height, cr_float near, cr_float far) :
            CameraInterface(position, target, up),
            m_height(height), m_near(near), m_far(far) {
        }

        cr_float m_width = { 0.0 };
        cr_float m_height = { 0.0 };
        cr_float m_near = { 0.0 };
        cr_float m_far = { 0.0 };
        glm::vec3 m_fpos[2];
        enum FRUSTUM_CORNERS { FC_LTF, FC_LTN }; // left-top-far, left-top-near

        void preprocess() override {
            CameraInterface::preprocess();
            m_width = m_height * m_aspect_ratio;
            m_fpos[FC_LTF] = glm::vec3(-m_width * cr_float(0.5), m_height * cr_float(0.5), -m_far);
            m_fpos[FC_LTN] = glm::vec3(-m_width * cr_float(0.5), m_height * cr_float(0.5), -m_near);
        }

        Ray generateRay(int32_t x, int32_t y, bool first) override {
            cr_float s = 0.0;
            cr_float t = 0.0;
            getImageSpaceOffsets(s, t, x, y, first);
            glm::vec3 image_plane_pos_far(m_fpos[FC_LTF].x + m_width * s, m_fpos[FC_LTF].y - m_height * t, m_fpos[FC_LTF].z);
            glm::vec3 image_plane_pos_near(m_fpos[FC_LTN].x + m_width * s, m_fpos[FC_LTN].y - m_height * t, m_fpos[FC_LTN].z);
            glm::vec3 direction = glm::normalize(image_plane_pos_far - image_plane_pos_near);
            return Ray(transformPoint(m_ecs_to_wcs, image_plane_pos_near), transformVector(m_ecs_to_wcs, direction));
        }
    };

    struct LightInterface : public TransformableInterface {
        Color m_flux = { Color(glm::vec3(0.0)) };
        bool m_is_point = { false };
        cr_float m_scalar_flux = { 0.0 };
        cr_float m_shared_total_area = { 0.0 };
        cr_float m_total_shared_primitives = { 0.0 };
        LightInterface() = default;
        virtual ~LightInterface() = default;
        LightInterface(const Color& intensity, bool is_point) : m_flux(intensity), m_is_point(is_point) {}
        LightInterface(const LightInterface&) = default;
        LightInterface& operator = (const LightInterface&) = default;
        LightInterface(LightInterface&&) = default;
        LightInterface& operator = (LightInterface&&) = default;
        virtual void preprocess() { m_scalar_flux = fluxToScalar(m_flux); }
        virtual cr_float getFluxScale(const struct PrimitiveInterface* primitive) const { return 1.0; }
        bool isPoint() const { return m_is_point; }
        virtual cr_float getLightPDF(glm::vec3& out_light_normal, const Ray& light_ray, const glm::vec3& normal, const PrimitiveInterface* primitive) const { return 0.0; }
        virtual Color sampleLightSource(cr_float& out_pdf, cr_float& out_distance, glm::vec3& vertex_to_light_direction, const HitRecord& record, const PrimitiveInterface* primitive)  const = 0;
        virtual Color evaluateLemissive(const glm::vec3& normal, const glm::vec3& vertex_to_outgoing_direction, const PrimitiveInterface* primitive) const { return Color::Black(); }
        static cr_float fluxToScalar(const Color& flux) {
            //return (0.299 * flux.x + 0.587 * flux.y + 0.114 * flux.z);
            return glm::max(glm::max(flux.x, flux.y), flux.z);
        }
    };

    struct IntersectInterface {
        cr_float m_surface_area = { 0 };
        cr_float m_inv_surface_area = { 0 };
        IntersectInterface() = default;
        virtual ~IntersectInterface() = default;
        IntersectInterface(const IntersectInterface&) = default;
        IntersectInterface& operator=(const IntersectInterface&) = default;
        IntersectInterface(IntersectInterface&&) = default;
        IntersectInterface& operator = (IntersectInterface&&) = default;
        virtual void makeSurfaceArea() = 0;
        cr_float getSurfaceArea() const { return m_surface_area; }
        cr_float getInvSurfaceArea() const { return m_inv_surface_area; }
        virtual glm::vec3 getCentroid() const = 0;
        virtual bool intersect(const Ray& ray, cr_float current_min, cr_float current_max, HitRecord& record) const = 0;
        bool ignore_intersection(const HitRecord& record) const { return record.primitives_to_ignore == this; }
    };

    struct AABB : public IntersectInterface {
        glm::vec3                            m_min = { glm::vec3(std::numeric_limits<cr_float>::max()) };
        glm::vec3                            m_max = { glm::vec3(std::numeric_limits<cr_float>::max()) };
        enum CORNER_POS {
            CP_LEFT_BOTTOM_BACK,
            CP_RIGHT_BOTTOM_BACK,
            CP_LEFT_TOP_BACK,
            CP_RIGHT_TOP_BACK,
            CP_LEFT_BOTTOM_FRONT,
            CP_RIGHT_BOTTOM_FRONT,
            CP_LEFT_TOP_FRONT,
            CP_RIGHT_TOP_FRONT
        };
        void makeEmpty() {
            m_min = glm::vec3(std::numeric_limits<cr_float>::max());
            m_max = glm::vec3(std::numeric_limits<cr_float>::max());
        }
        bool isValid() const { return !isEmpty() && glm::all(glm::lessThan(m_min, m_max)); }
        bool isEmpty() const { return m_min == glm::vec3(std::numeric_limits<cr_float>::max()) && m_max == glm::vec3(std::numeric_limits<cr_float>::max()); }
        glm::vec3 getCorner(uint32_t pos) const { return glm::vec3(pos & 1 ? m_max.x : m_min.x, pos & 2 ? m_max.y : m_min.y, pos & 4 ? m_max.z : m_min.z); }

        void transform(const glm::mat4x4& matrix, const AABB& box) {
            for (uint32_t c = 0; c < 8; ++c) {
                glm::vec3 p = box.getCorner(c);
                p = transformPoint(matrix, p);
                expandBy(p);
            }
        }

        int getMaxDimension() const {
            glm::vec3 diagonal = m_max - m_min;
            return diagonal.x > diagonal.y ? (diagonal.x > diagonal.z ? 0 : 2) : (diagonal.y > diagonal.z ? 1 : 2);
        }

        void expandBy(const AABB& box) {
            CRAY_ASSERT_IF_FALSE(!box.isEmpty())
                if (box.isEmpty()) {
                    return;
                }
            expandBy(box.m_min);
            expandBy(box.m_max);
        }

        void expandBy(const glm::vec3& v) {
            if (isEmpty()) {
                m_min = v; m_max = v;
                //m_center = v * CR_float(0.5);
                return;
            }
            m_min = glm::min(m_min, v);
            m_max = glm::max(m_max, v);
            //m_center = (m_min + m_max) * CR_float(0.5);
            makeSurfaceArea();
        }

        // Ox + tdx = Bx, solve for t = (b - O) / d
        // need to keep the max smaller and min largest t to ensure that the ray crosses the AABB boundaries and not just the AABB planes
        // source: https://medium.com/@bromanz/another-view-on-the-classic-ray-aabb-intersection-algorithm-for-bvh-traversal-41125138b525
        bool intersect(const Ray& ray, cr_float current_min, cr_float current_max, HitRecord& record) const override {
            glm::vec3 t0s = (m_min - ray.m_origin) * ray.m_direction_inv;
            glm::vec3 t1s = (m_max - ray.m_origin) * ray.m_direction_inv;
            glm::vec3 tsmaller = glm::min(t0s, t1s);
            glm::vec3 tbigger = glm::max(t0s, t1s);
            cr_float tmin = glm::max(tsmaller[0], glm::max(tsmaller[1], glm::max(tsmaller[2], current_min)));
            cr_float tmax = glm::min(tbigger[0], glm::min(tbigger[1], glm::min(tbigger[2], current_max)));
            return (tmin <= tmax);
        }

        void makeSurfaceArea() override {
            cr_float length_x = m_max.x - m_min.x;
            cr_float length_y = m_max.y - m_min.y;
            cr_float length_z = m_max.z - m_min.z;

            m_surface_area = length_x * length_y + length_x * length_z + length_y * length_z;
            m_surface_area *= 2.0;
            m_inv_surface_area = 1.0 / m_surface_area;
        }

        //glm::vec3 getCentroid() const override { return m_center; }
        glm::vec3 getCentroid() const override { return (m_min + m_max) * cr_float(0.5); }
    };

    struct PrimitiveInterface : public IntersectInterface, public TransformableInterface {
        std::weak_ptr<LightInterface> m_light_interface;
        AABB m_box;
        std::shared_ptr<MaterialInterface> m_material = { nullptr };
        bool m_flip_normals = { false };
        bool m_double_sided = { false };
        cr_float m_scalar_flux = { 0.0 };
        Color m_emitted_radiance = { Color::Black() };
        PrimitiveInterface(bool flip_normals, bool double_sided) : m_flip_normals(flip_normals), m_double_sided(double_sided) {}
        virtual ~PrimitiveInterface() = default;
        PrimitiveInterface(const PrimitiveInterface&) = default;
        PrimitiveInterface& operator=(const PrimitiveInterface&) = default;
        PrimitiveInterface(PrimitiveInterface&&) = default;
        PrimitiveInterface& operator = (PrimitiveInterface&&) = default;
        void setLight(std::shared_ptr<LightInterface> light) { m_light_interface = light; }
        virtual void preprocess(const glm::mat4x4& wcs_to_ecs) = 0;
        virtual cr_float getAreaPDF() const { return 0.0; }
        virtual glm::vec3 getSample(glm::vec3& out_normal) const = 0;
        bool isLightSource() { return !m_light_interface.expired(); }
        Color evaluateLemissive(const glm::vec3& normal, const glm::vec3& vertex_to_outgoing_direction) const {
            return m_light_interface.expired() ? Color::Black() : m_light_interface.lock()->evaluateLemissive(normal, vertex_to_outgoing_direction, this);
        }
    protected:
        bool intersectFrontFace(const Ray& ray, const glm::vec3& normal) const { return m_double_sided ? true : glm::dot(normal, ray.m_direction) < 0.0; }
    };

    struct PointLight : public LightInterface {
        explicit PointLight(const Color& intensity) : LightInterface(intensity, true) { m_total_shared_primitives = 1.0; }
    };

    struct AreaLight : public LightInterface {
        bool m_double_sided = { false };
        std::vector<PrimitiveInterface*> m_shared_primitives;
        explicit AreaLight(const Color& flux, bool doublesided) :
            LightInterface(flux, false), m_double_sided(doublesided) {
        }

        void addSharedPrimitive(PrimitiveInterface* primitive) { m_shared_primitives.push_back(primitive); }

        void preprocess() override {
            if (m_shared_primitives.size() == 0.0) {
                CRAY_WARNING("Area light has not any associated primitives.")
            }

            // calculate total area of primitives that share this light source
            for (auto prim : m_shared_primitives) {
                m_shared_total_area += prim->getSurfaceArea();
            }
            // reiterate and set flux/radiance per primitive
            // a diffuse emitter has Flux = L * A * pi => L = F / (A * pi)
            for (auto prim : m_shared_primitives) {
                Color primitive_flux = m_flux * prim->getSurfaceArea() / m_shared_total_area;
                prim->m_scalar_flux = LightInterface::fluxToScalar(primitive_flux);
                prim->m_emitted_radiance = primitive_flux * prim->getInvSurfaceArea() * glm::one_over_pi<cr_float>();
            }
            m_total_shared_primitives = m_shared_primitives.size();
            // this is not needed anymore
            m_shared_primitives.clear();
            m_shared_primitives.shrink_to_fit();

            LightInterface::preprocess();
        }

        Color evaluateLemissive(const glm::vec3& normal, const glm::vec3& vertex_to_next_direction, const PrimitiveInterface* primitive) const override {
            return (m_double_sided || glm::dot(normal, vertex_to_next_direction) < 0.0) ? primitive->m_emitted_radiance : Color::Black();
        }

        cr_float getLightPDF(glm::vec3& out_light_normal, const Ray& light_ray, const glm::vec3& normal, const PrimitiveInterface* primitive) const override {
            if (primitive == nullptr) {
                return 0.0;
            }

            HitRecord primitive_hit_record;
            cr_float current_max = std::numeric_limits<cr_float>::max(); // TODO:: assumes infinite camera
            bool intersect = primitive->intersect(light_ray, 0.0, current_max, primitive_hit_record);
            if (!intersect) { return 0.0; }

            out_light_normal = primitive_hit_record.normal;

            // convert PDF to solid angle
            cr_float dot = glm::abs(glm::dot(normal, light_ray.m_direction));
            if (dot <= 0.0) {
                return 0.0;
            }
            return primitive->getAreaPDF() * primitive_hit_record.distance * primitive_hit_record.distance / dot;
        }

        Color sampleLightSource(cr_float& out_pdf, cr_float& out_distance, glm::vec3& out_vertex_to_light_direction, const HitRecord& record, const PrimitiveInterface* primitive) const override {
            if (primitive == nullptr || record.primitive == primitive) {
                out_pdf = 0.0;
                return Color::Black();
            }

            // get sample on primitive
            glm::vec3 light_point_normal(0.0);
            glm::vec3 light_point = primitive->getSample(light_point_normal);

            out_pdf = primitive->getAreaPDF();

            if (out_pdf <= 0.0) {
                CRAY_DEBUG_PIXEL("(%.2f, %.2f): Zero or negative PDF for sampling area light. Sample: %f, %f, %f, PDF: %f. Skipping", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, light_point.x, light_point.y, light_point.z, out_pdf);
                return Color::Black();
            }
            // calculate direction, distance and intensity
            out_vertex_to_light_direction = glm::vec3(light_point - record.intersection_point);
            cr_float light_distance_squared = glm::dot(out_vertex_to_light_direction, out_vertex_to_light_direction);
            out_distance = glm::sqrt(light_distance_squared);
            out_vertex_to_light_direction = glm::normalize(out_vertex_to_light_direction);

            // convert PDF to solid angle
            cr_float dot = glm::abs(glm::dot(record.normal, out_vertex_to_light_direction));
            if (dot <= 0.0) {
                out_pdf = 0.0;
                return Color::Black();
            }
            out_pdf = out_pdf * light_distance_squared / dot;

            // return emitted light from the light source
            return evaluateLemissive(light_point_normal, out_vertex_to_light_direction, primitive);
        }
    };

    struct DirectionalLight : public PointLight {
        glm::vec3 m_direction = { glm::vec3(0) };
        explicit DirectionalLight(const glm::vec3& direction, const Color& intensity) :
            PointLight(intensity), m_direction(glm::normalize(direction)) {
            CRAY_ASSERT_IF_FALSE_MSG(isValidVec3(m_direction), "Invalid direction for point light. Dir: %f, %f, %.2f", m_direction.x, m_direction.y, m_direction.z)
        }

        Color sampleLightSource(cr_float& out_pdf, cr_float& out_distance, glm::vec3& out_vertex_to_light_direction, const HitRecord& record, const PrimitiveInterface* primitive) const override {
            out_pdf = 1.0;
            out_distance = std::numeric_limits<cr_float>::max();
            out_vertex_to_light_direction = -m_direction;
            // return emitted light from the light source
            return m_flux;
        }
    };

    struct SpotLight : public PointLight {
        glm::vec3 m_position = { glm::vec3(0.0) };
        glm::vec3 m_direction = { glm::vec3(0) };
        glm::vec3 m_target = { glm::vec3(0) };
        cr_float m_falloffAngle = { 0.0 };
        cr_float m_maxAngle = { 0.0 };
        cr_float m_cosFalloffAngle = { 0.0 };
        cr_float m_cosMaxAngle = { 0.0 };
        cr_float m_exponent = { 0.0 };
        explicit SpotLight(const glm::vec3& position, const glm::vec3& target, const Color& intensity, cr_float falloffAngle, cr_float maxAngle, cr_float exponent) :
            PointLight(intensity), m_position(position), m_target(target), m_falloffAngle(falloffAngle), m_maxAngle(maxAngle), m_exponent(exponent) {
        }

        void preprocess() override {
            LightInterface::preprocess();
            m_position = transformPoint(getTransform(), m_position);
            m_target = transformPoint(getTransform(), m_target);
            if (m_falloffAngle > 0.0) {
                m_cosFalloffAngle = glm::cos(glm::radians(m_falloffAngle));
                m_cosMaxAngle = glm::cos(glm::radians(m_maxAngle));
                m_direction = glm::normalize(m_target - m_position);
                CRAY_ASSERT_IF_FALSE_MSG(isValidVec3(m_direction), "Invalid direction for point light. Pos: %f, %f, %f, Tgt: %f, %f, %f, Dir: %f, %f, %f",
                    m_position.x, m_position.y, m_position.z, m_target.x, m_target.y, m_target.z, m_direction.x, m_direction.y, m_direction.z)
            }
        }

        cr_float attenuation(const glm::vec3& light_to_vertex_direction) const {
            if (m_maxAngle <= 0.0 || m_falloffAngle <= 0) {
                return 1.0;
            }
            // cosine of the angle between light direction and light to vertex direction
            cr_float cos_light_vertex = glm::dot(light_to_vertex_direction, m_direction);
            cr_float attenuation = glm::clamp((cos_light_vertex - m_cosMaxAngle) / (m_cosFalloffAngle - m_cosMaxAngle), cr_float(0.0), cr_float(1.0));
            return glm::pow(attenuation, m_exponent);
        }

        Color sampleLightSource(cr_float& out_pdf, cr_float& out_distance, glm::vec3& out_vertex_to_light_direction, const HitRecord& record, const PrimitiveInterface* primitive) const override {
            glm::vec3 light_point = m_position;
            out_pdf = 1.0;
            out_vertex_to_light_direction = glm::vec3(light_point - record.intersection_point);
            cr_float light_distance_squared = glm::dot(out_vertex_to_light_direction, out_vertex_to_light_direction);
            out_distance = glm::sqrt(light_distance_squared);
            out_vertex_to_light_direction = glm::normalize(out_vertex_to_light_direction);

            // return emitted light from the light source
            return m_flux * attenuation(-out_vertex_to_light_direction) / light_distance_squared;
        }
    };

    struct PrimitiveRectangle : public PrimitiveInterface {
        glm::vec3 m_min = { glm::vec3(0.0) };
        glm::vec3 m_max = { glm::vec3(0.0) };
        glm::vec3 m_normal = { glm::vec3(0.0, 1.0, 0.0) };
        glm::vec3 m_normal_ecs = { glm::vec3(0.0, 1.0, 0.0) };
        glm::mat4x4 m_rcs_to_ocs = { glm::mat4x4(1.0) };
        glm::vec3 m_min_ocs = { glm::vec3(-1.0, -1.0, 0.0) };
        glm::vec3 m_max_ocs = { glm::vec3(1.0, 1.0, 0.0) };
        glm::vec3 su = { glm::vec3(0.0) };
        cr_float su_length = { 0.0 };
        glm::vec3 sv = { glm::vec3(0.0) };
        cr_float sv_length = { 0.0 };

        explicit PrimitiveRectangle(const glm::vec3& p_min, const glm::vec3& p_max, const glm::mat4x4& rcs_to_ocs, bool flip_normals, bool double_sided) : PrimitiveInterface(flip_normals, double_sided), m_min(p_min), m_max(p_max), m_rcs_to_ocs(rcs_to_ocs) {
            isValidVec3(m_min); isValidVec3(m_max);
        }

        void preprocess(const glm::mat4x4& wcs_to_ecs) override {
            // store OCS coordinates (XY) for later
            glm::mat4x4 ocs_to_rcs = glm::inverse(m_rcs_to_ocs);

            // convert from rectangle space(RCS->XZ,XY,XZ) to WCS
            updateTransform(getTransform() * ocs_to_rcs);
            m_min = transformPoint(getTransform(), m_min_ocs);
            m_max = transformPoint(getTransform(), m_max_ocs);

            // update wcs matrix to go from OCS->WCS
            m_normal = transformVector(getTransformNormal(), glm::vec3(0.0, 0.0, m_flip_normals ? -1.0 : 1.0));
            m_normal = glm::normalize(m_normal);

            m_normal_ecs = glm::normalize(transformVector(wcs_to_ecs, m_normal));

            glm::vec3 bl = transformPoint(getTransform(), m_min_ocs);
            glm::vec3 tl = transformPoint(getTransform(), glm::vec3(m_min_ocs.x, m_max_ocs.y, 0.0));
            glm::vec3 br = transformPoint(getTransform(), glm::vec3(m_max_ocs.x, m_min_ocs.y, 0.0));
            glm::vec3 tr = transformPoint(getTransform(), glm::vec3(m_max_ocs.x, m_max_ocs.y, 0.0));

            // update bbox
            m_box.expandBy(bl);
            m_box.expandBy(tl);
            m_box.expandBy(br);
            m_box.expandBy(tr);
            makeSurfaceArea();
        }

        void makeSurfaceArea() override {
            glm::vec3 bl = transformPoint(getTransform(), m_min_ocs);
            glm::vec3 tl = transformPoint(getTransform(), glm::vec3(m_min_ocs.x, m_max_ocs.y, 0.0));
            glm::vec3 br = transformPoint(getTransform(), glm::vec3(m_max_ocs.x, m_min_ocs.y, 0.0));
            su = br - bl;
            sv = tl - bl;
            su_length = glm::length(su);
            sv_length = glm::length(sv);
            su = su / su_length;
            sv = sv / sv_length;
            m_surface_area = su_length * sv_length;
            m_inv_surface_area = 1.0 / m_surface_area;
        }

        glm::vec3 getCentroid() const override { return m_box.getCentroid(); }

        cr_float getAreaPDF() const override { return m_inv_surface_area; }

        glm::vec3 getSample(glm::vec3& out_normal) const override {
            glm::vec3 point_on_rectangle_ocs(0.0);
            point_on_rectangle_ocs.x = rand_f_range(m_min_ocs.x, m_max_ocs.x);
            point_on_rectangle_ocs.y = rand_f_range(m_min_ocs.y, m_max_ocs.y);
            glm::vec3 sample = transformPoint(getTransform(), point_on_rectangle_ocs);
            out_normal = m_normal;
            return sample;
        }

        // plane implicit form: Ax + By + Cz + D  = 0
        // plane vector form: (p - p0).n = 0
        // solve (o + td - p0).n = 0 => t = (p - p0).n / (n.d)
        // assuming this is an XY plane (normal:0,0,1) => t = p.z - o.z / d.z
        bool intersect(const Ray& ray, cr_float current_min, cr_float current_max, HitRecord& record) const override {
            if (!intersectFrontFace(ray, m_normal)) { return false; }

            // parallel ray
            if (glm::abs(glm::dot(m_normal, ray.m_direction)) <= std::numeric_limits<cr_float>::epsilon()) {
                return false;
            }
            cr_float t = glm::dot(m_min - ray.m_origin, m_normal) / glm::dot(ray.m_direction, m_normal);
            if (t < current_min || t > current_max) {
                // no plane hit
                return false;
            }
            glm::vec3 intersection_point = ray.get_intersection_point(t);
            cr_float sup = glm::dot(intersection_point - m_min, su);
            cr_float svp = glm::dot(intersection_point - m_min, sv);
            if (sup < 0.0 || sup > su_length || svp < 0.0 || svp > sv_length) {
                // no rectangle hit
                return false;
            }
            record.distance = t;
            // return true for shadow ray
            if (ray.m_visibility == true) {
                return true;
            }

            record.material = m_material.get();
            record.intersection_point = intersection_point;
            record.intersection_point_object_space = transformPoint(getTransformInverse(), record.intersection_point);
            record.intersection_point_object_space.z = 0.0;
            record.normal = m_normal;
            record.uv = glm::vec2((record.intersection_point_object_space - m_min_ocs) / (m_max_ocs - m_min_ocs));
            return true;
        }
    };

    struct PrimitiveSphere : public PrimitiveInterface {
        glm::vec3 m_center;
        cr_float m_radius;
        TextureMappingSpherical m_mapping;

        explicit PrimitiveSphere(const glm::vec3& center, cr_float radius, bool flip_normals, bool double_sided) : PrimitiveInterface(flip_normals, double_sided), m_center(center), m_radius(radius) {
            isValidVec3(m_center); isValidFloat(m_radius);
        }

        void preprocess(const glm::mat4x4& wcs_to_ecs) override {
            glm::vec3 sphere_min = m_center + glm::vec3(-m_radius);
            glm::vec3 sphere_max = m_center + glm::vec3(m_radius);
            AABB box;
            box.expandBy(sphere_min);
            box.expandBy(sphere_max);
            m_center = transformPoint(getTransform(), m_center);
            m_box.transform(getTransform(), box);
            makeSurfaceArea();
        }

        void makeSurfaceArea() override {
            m_surface_area = 4.0 * glm::pi<cr_float>() * m_radius * m_radius;
            m_inv_surface_area = 1.0 / m_surface_area;
        }
        glm::vec3 getCentroid() const override { return m_center; }

        cr_float getAreaPDF() const override {
            return getUniformSphereSamplePDF() / (m_radius * m_radius);
        }

        // Use Monte Carlo techniques for direct lighting calculations, Shirley, P. et al 1996
        glm::vec3 getSample(glm::vec3& out_normal) const override {
            glm::vec3 sample_dir = getUniformSphereSample();
            glm::vec3 sample = m_center + (m_radius * sample_dir);
            out_normal = !m_flip_normals ? sample_dir : -sample_dir;
            return sample;
        }

        // sphere implicit form: (x-cx)^2 + (y-cy)^2 + (z-cz)^2 - r^2 = 0
        // vector form: dot((p - c), (p - c)) = r^2
        // solve quadratic equation of (o + td)^2 - r^2 = 0 for t, where c=0
        bool intersect(const Ray& ray, cr_float current_min, cr_float current_max, HitRecord& record) const override {
            // the sphere is assumed to be at the origin (to simplify equations and increase float accuracy)
            glm::vec3 moved_ray_origin = ray.m_origin - m_center;
            cr_float b = glm::dot(moved_ray_origin, ray.m_direction);
            cr_float c = glm::dot(moved_ray_origin, moved_ray_origin) - glm::dot(m_radius, m_radius);
            cr_float disc = b * b - c;
            if (disc >= 0) {
                cr_float disc_sqrt = glm::sqrt(disc);
                cr_float t = -b - disc_sqrt;
                if (t > current_min&& t < current_max) {
                    record.distance = t;
                    if (ray.m_visibility) {
                        return true;
                    }
                    record.intersection_point = ray.get_intersection_point(t);
                    record.normal = !m_flip_normals ? glm::normalize(record.intersection_point - m_center) : glm::normalize(m_center - record.intersection_point);
                    record.intersection_point_object_space = record.normal;
                    record.material = m_material.get();
                    record.uv = m_mapping.mapUV(record.intersection_point_object_space);
                    return true;
                }
                t = -b + disc_sqrt;
                if (t > current_min&& t < current_max) {
                    record.distance = t;
                    if (ray.m_visibility) {
                        return true;
                    }
                    record.intersection_point = ray.get_intersection_point(t);
                    record.normal = !m_flip_normals ? glm::normalize(record.intersection_point - m_center) : glm::normalize(m_center - record.intersection_point);
                    record.intersection_point_object_space = record.normal;
                    record.material = m_material.get();
                    record.uv = m_mapping.mapUV(record.intersection_point_object_space);
                    return true;
                }
            }
            return false;
        }
    };

    /*
    struct PrimitiveTriangleMesh : public PrimitiveInterface {
        std::vector<glm::vec3> positions;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> texcoords;
        std::vector<int64_t> face_indices;

        explicit PrimitiveTriangleMesh(
            float* positions, int64_t num_positions,
            float* normals, int64_t num_normals,
            float* texcoords, int64_t num_texcoords,
            float* face_indices, int64_t num_indices,
            bool flip_normals, bool double_sided) : PrimitiveInterface(flip_normals, double_sided) { }
    };
    */
    struct PrimitiveTriangle : public PrimitiveInterface {

        struct TriangleVertex {
            glm::vec3                            m_position = { glm::vec3(0.0, 0.0, 0.0) };
            glm::vec3                            m_normal = { glm::vec3(std::numeric_limits<cr_float>::max()) };
            glm::vec3                            m_texcoord = { glm::vec3(0.0, 0.0, 0.0) };
            glm::vec3                            m_tangent = { glm::vec3(1.0, 0.0, 0.0) };
            glm::vec3                            m_bitangent = { glm::vec3(0.0, 0.0, 1.0) };
        };

#define NUM_VERTICES 3
        TriangleVertex                           m_triangle_vertex[NUM_VERTICES];
        glm::vec3                                m_plane_normal = { glm::vec3(std::numeric_limits<cr_float>::max()) };
        glm::vec3                                m_e_1 = { glm::vec3(0.0, 0.0, 0.0) };
        glm::vec3                                m_e_2 = { glm::vec3(0.0, 0.0, 0.0) };
        glm::vec3                                m_center = { glm::vec3(0.0, 0.0, 0.0) };
        glm::vec3                                m_normal_ecs = { glm::vec3(0.0, 1.0, 0.0) };

        explicit PrimitiveTriangle(bool flip_normals, bool double_sided) : PrimitiveInterface(flip_normals, double_sided) { }

        void addPosition(int32_t index, const glm::vec3& position) { isValidVec3(position); m_triangle_vertex[index].m_position = transformPoint(getTransform(), position); }
        void addNormal(int32_t index, const glm::vec3& normal) {
            isValidVec3(normal); m_triangle_vertex[index].m_normal = glm::normalize(transformVector(getTransformNormal(), normal));
            if (m_flip_normals) {
                m_triangle_vertex[index].m_normal = -m_triangle_vertex[index].m_normal;
            }
        }
        void addTexCoord(int32_t index, const glm::vec3& texcoord) {
            isValidVec3(texcoord);
            m_triangle_vertex[index].m_texcoord = texcoord;
        }

        void preprocess(const glm::mat4x4& wcs_to_ecs) override {
            m_e_1 = m_triangle_vertex[1].m_position - m_triangle_vertex[0].m_position;
            m_e_2 = m_triangle_vertex[2].m_position - m_triangle_vertex[0].m_position;
            makeSurfaceArea();
            m_plane_normal = glm::normalize(m_plane_normal);

            bool use_plane_normal = false;
            for (int i = 0; i < NUM_VERTICES; ++i) {
                if (m_triangle_vertex[i].m_normal.x == std::numeric_limits<cr_float>::max() ||
                    glm::any(glm::isnan(m_triangle_vertex[i].m_normal)) || glm::any(glm::isinf(m_triangle_vertex[i].m_normal))) {
                    use_plane_normal = true;
                    break;
                }
            }

            for (int i = 0; i < NUM_VERTICES && use_plane_normal; ++i) {
                m_triangle_vertex[i].m_normal = m_plane_normal;
            }

            for (int i = 0; i < NUM_VERTICES; ++i) {
                getOrthonormalBasis(m_triangle_vertex[i].m_bitangent, m_triangle_vertex[i].m_tangent, m_triangle_vertex[i].m_normal);
            }
            for (int i = 0; i < NUM_VERTICES; ++i) {
                m_box.expandBy(m_triangle_vertex[i].m_position);
            }
            for (int i = 0; i < NUM_VERTICES; ++i) {
                m_center += m_triangle_vertex[i].m_position;
            }
            m_center /= 3.0;

            m_normal_ecs = glm::normalize(transformVector(wcs_to_ecs, m_triangle_vertex[0].m_normal));
        }

        glm::vec3 getCentroid() const override { return m_center; }

        void makeSurfaceArea() override {
            m_plane_normal = glm::cross(m_e_1, m_e_2);
            m_surface_area = glm::length(m_plane_normal) * 0.5;
            m_inv_surface_area = 1.0 / m_surface_area;
            m_plane_normal = glm::normalize(!m_flip_normals ? m_plane_normal : -m_plane_normal);
        }

        cr_float getAreaPDF() const override { return m_inv_surface_area; }

        glm::vec3 getSample(glm::vec3& out_normal) const override {
            glm::vec2 r;
            r.x = glm::sqrt(rand_f());
            r.y = rand_f();
            cr_float a = 1.0 - r.x;
            cr_float b = r.x * r.y;
            cr_float c = 1.0 - a - b;
            glm::vec3 sample = m_triangle_vertex[0].m_position * a + m_triangle_vertex[1].m_position * b + m_triangle_vertex[2].m_position * c;
            out_normal = m_triangle_vertex[0].m_normal * a + m_triangle_vertex[1].m_normal * b + m_triangle_vertex[2].m_normal * c;
            out_normal = glm::normalize(out_normal);
            out_normal = m_plane_normal;
            return sample;
        }


        int MaxDimension(const glm::vec3& v) const {
            return (v.x > v.y) ? ((v.x > v.z) ? 0 : 2) : ((v.y > v.z) ? 1 : 2);
        }

        bool intersect(const Ray& ray, cr_float current_min, cr_float current_max, HitRecord& record) const override {
            if (!intersectFrontFace(ray, m_triangle_vertex[0].m_normal)) { return false; }
            glm::vec3 q = glm::cross(ray.m_direction, m_e_2);
            cr_float a = glm::dot(m_e_1, q);

            // ray parallel check
            if (glm::abs(a) < glm::epsilon<cr_float>()) {
                return false;
            }

            glm::vec3 s = (ray.m_origin - m_triangle_vertex[0].m_position) / a;
            glm::vec3 r = glm::cross(s, m_e_1);
            cr_float t = glm::dot(m_e_2, r);
            if (t < current_min || t > current_max) {
                // no plane hit
                return false;
            }

            glm::vec2 b = glm::vec2(glm::dot(s, q), glm::dot(r, ray.m_direction));
            if (glm::any(glm::bvec3(b.x < 0.0, b.y < 0.0, b.x + b.y > 1.0))) {
                // no plane hit
                return false;
            }

            record.distance = t;
            if (ray.m_visibility) {
                return true;
            }
            cr_float bz = 1.0 - b.x - b.y;
            record.normal = m_triangle_vertex[0].m_normal * bz + m_triangle_vertex[1].m_normal * b.x + m_triangle_vertex[2].m_normal * b.y;
            record.intersection_point = m_triangle_vertex[0].m_position * bz + m_triangle_vertex[1].m_position * b.x + m_triangle_vertex[2].m_position * b.y;
            record.uv = m_triangle_vertex[0].m_texcoord * bz + m_triangle_vertex[1].m_texcoord * b.x + m_triangle_vertex[2].m_texcoord * b.y;
            record.intersection_point_object_space = glm::vec3(record.uv, 0.0);
            record.material = m_material.get();
            return true;
        }
    };

    template <typename T>
    struct VectorHandler {
        std::vector<std::shared_ptr<T>> m_items;
        CRAY_HANDLE add_item(std::shared_ptr<T> item) {
            m_items.push_back(item);
            return m_items.size();
        }
        std::shared_ptr<T> find_item(CRAY_HANDLE handle) {
            return ((handle - 1) < m_items.size()) ? m_items[handle - 1] : nullptr;
        }
        void destroy_all() {
            m_items.clear();
            m_items.shrink_to_fit();
        }
    };

    struct IntegratorInterface : public NonCopyable {
        CRAY_INTEGRATOR m_type = { CR_INTEGRATOR_NONE };
        virtual void renderBlock(struct ThreadBlock& block) const = 0;
    };
    static std::unique_ptr<IntegratorInterface> s_global_integrator = { nullptr };

    struct AccelerationStructureInterface : public NonCopyable {
        CRAY_ACCELERATION_STRUCTURE m_type = { CR_AS_NONE };
        VectorHandler<PrimitiveInterface> m_primitive_list;
        uint64_t m_primitive_size = { 0 };
        CRAY_HANDLE addPrimitive(std::shared_ptr<PrimitiveInterface> prim, int size) { m_primitive_size += size; return m_primitive_list.add_item(prim); }
        virtual void clear() { m_primitive_size = 0; }
        virtual void preprocess(const glm::mat4x4& wcs_to_ecs) {
            for (auto& prim : m_primitive_list.m_items) {
                prim->preprocess(wcs_to_ecs);
            }
        }
        virtual void printBuildStats() const = 0;
        virtual void printTracingStats(int32_t total_pixels, uint32_t iterations) const = 0;
        virtual bool intersect(const Ray& ray, cr_float current_min, cr_float current_max, HitRecord& record) const { return false; }
        virtual bool intersectVisibility(const Ray& ray, cr_float current_min, cr_float current_max, HitRecord& record) const { return false; }
        VectorHandler<PrimitiveInterface>& getPrimitiveList() { return m_primitive_list; }
    };

    struct AccelerationStructureArray : public AccelerationStructureInterface {
        AccelerationStructureArray() { m_type = CR_AS_ARRAY; }
        ~AccelerationStructureArray() override { clear(); }

        void printBuildStats() const override {
            CRAY_INFO("Primitive Array Structure Build Stats:");
            CRAY_INFO("    Primitives:                 %u", m_primitive_list.m_items.size());
            CRAY_INFO("    Total primitive size:       %s", convertToCapacitySizeString(m_primitive_size).c_str());
        }

        void printTracingStats(int32_t total_pixels, uint32_t iterations) const override {
            cr_float div = cr_float(total_pixels) * cr_float(s_global_settings->m_samples_per_pixel) * cr_float(iterations);
            CRAY_INFO("Primitive Array Structure Tracing Stats:");
            CRAY_INFO("    Rays:                       %u (%.2f per spp)", s_global_settings->m_stats.m_number_rays, s_global_settings->m_stats.m_number_rays / div);
            CRAY_INFO("    Shadow rays:                %u (%.2f per spp)", s_global_settings->m_stats.m_number_shadow_rays, s_global_settings->m_stats.m_number_shadow_rays / div);
            CRAY_INFO("    Prim Intersect:             %.2f per ray", s_global_settings->m_stats.m_primitive_intersections_avg);
            CRAY_INFO("    Prim Intersect Shadow:      %.2f per ray", s_global_settings->m_stats.m_primitive_intersections_shadow_avg);
        }

        bool intersect(const Ray& ray, cr_float current_min, cr_float current_max, HitRecord& record) const override {
            ray.m_visibility == true ? s_thread_settings->m_stats.addShadowRayTraversal() : s_thread_settings->m_stats.addRayTraversal();
            for (auto& prim : m_primitive_list.m_items) {
                ray.m_visibility == true ? s_thread_settings->m_stats.addShadowPrimitiveIntersection() : s_thread_settings->m_stats.addPrimitiveIntersection();
                if (!prim->ignore_intersection(record) && prim->intersect(ray, current_min, current_max, record)) {
                    record.primitive = prim.get();
                    current_max = record.distance;
                    record.valid = true;
                    // just return for shadow rays
                    if (ray.m_visibility) {
                        return true;
                    }
                }
            }
            return record.valid;
        }

        bool intersectVisibility(const Ray& ray, cr_float current_min, cr_float current_max, HitRecord& record) const override {
            return intersect(ray, current_min, current_max, record);
        }

        void preprocess(const glm::mat4x4& wcs_to_ecs) override {
            AccelerationStructureInterface::preprocess(wcs_to_ecs);
        }

        void clear() override { m_primitive_list.destroy_all(); }
    };

    struct AccelerationStructureBVH : public AccelerationStructureInterface {
        AccelerationStructureBVH() { m_type = CR_AS_BVH; }
        ~AccelerationStructureBVH() override { clear(); }

        struct Node {
            AABB bounds;
            cr_float split_axis_value = { 0.0 };
            std::unique_ptr<Node> left_child = { nullptr };
            std::unique_ptr<Node> right_child = { nullptr };
            std::vector<std::shared_ptr<PrimitiveInterface>> primitive_list;
            int32_t split_axis = { 0 };
        };
        std::unique_ptr<Node> m_root = { nullptr };

        size_t m_num_nodes = { 0 };
        size_t m_leaf_nodes = { 0 };
        size_t m_children_nodes = { 0 };
        double m_children_per_node_avg = { 0.0 };
        size_t m_max_depth = { 0 };

        bool intersect(const Ray& ray, cr_float current_min, cr_float current_max, HitRecord& record) const override {
            s_thread_settings->m_stats.addRayTraversal();
            intersectNode(m_root.get(), ray, current_min, current_max, record);
            return record.valid;
        }

        bool intersectVisibility(const Ray& ray, cr_float current_min, cr_float current_max, HitRecord& record) const override {
            s_thread_settings->m_stats.addShadowRayTraversal();
            intersectNodeVisibility(m_root.get(), ray, current_min, current_max, record);
            return record.valid;
        }

        bool intersectNode(const Node* node, const Ray& ray, cr_float current_min, cr_float& current_max, HitRecord& record) const {
            if (node == nullptr) {
                return false;
            }
            s_thread_settings->m_stats.addNodeTraversal();
            if (!node->bounds.intersect(ray, current_min, current_max, record)) {
                return false;
            }
            // if we have reached a leaf
            if (node->left_child == nullptr && node->right_child == nullptr) {
                for (const auto& prim : node->primitive_list) {
                    s_thread_settings->m_stats.addPrimitiveIntersection();
                    if (!prim->ignore_intersection(record) && prim->intersect(ray, current_min, current_max, record)) {
                        current_max = record.distance;
                        record.valid = true;
                        record.primitive = prim.get();
                    }
                }
                return record.valid;
            }

            // otherwise traverse
            HitRecord left_record;
            left_record.primitives_to_ignore = record.primitives_to_ignore;
            HitRecord right_record;
            right_record.primitives_to_ignore = record.primitives_to_ignore;
            // choose visiting preference based on the ray direction
            // source: http://www.pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies.html
            Node* nodes_to_look[2];
            if (ray.m_direction[node->split_axis] > 0) {
                nodes_to_look[0] = node->left_child.get();
                nodes_to_look[1] = node->right_child.get();
            }
            else {
                nodes_to_look[0] = node->right_child.get();
                nodes_to_look[1] = node->left_child.get();
            }
            intersectNode(nodes_to_look[0], ray, current_min, current_max, left_record);
            intersectNode(nodes_to_look[1], ray, current_min, current_max, right_record);

            // find the closest hit
            if (left_record.valid && left_record.distance < record.distance) {
                record = left_record;
            }
            if (right_record.valid && right_record.distance < record.distance) {
                record = right_record;
            }

            return record.valid;
        }

        bool intersectNodeVisibility(const Node* node, const Ray& ray, cr_float current_min, cr_float& current_max, HitRecord& record) const {
            if (node == nullptr) {
                return false;
            }
            s_thread_settings->m_stats.addShadowNodeTraversal();
            if (!node->bounds.intersect(ray, current_min, current_max, record)) {
                return false;
            }
            // if we have reached a leaf
            if (node->left_child == nullptr && node->right_child == nullptr) {
                for (const auto& prim : node->primitive_list) {
                    s_thread_settings->m_stats.addShadowPrimitiveIntersection();
                    if (!prim->ignore_intersection(record) && prim->intersect(ray, current_min, current_max, record)) {
                        current_max = record.distance;
                        record.valid = true;
                        record.primitive = prim.get();
                        return true;
                    }
                }
                return false;
            }

            // otherwise traverse
            // choose visiting preference based on the ray direction
            // source: http://www.pbr-book.org/3ed-2018/Primitives_and_Intersection_Acceleration/Bounding_Volume_Hierarchies.html
            Node* nodes_to_look[2];
            if (ray.m_direction[node->split_axis] > 0) {
                nodes_to_look[0] = node->left_child.get();
                nodes_to_look[1] = node->right_child.get();
            }
            else {
                nodes_to_look[0] = node->right_child.get();
                nodes_to_look[1] = node->left_child.get();
            }
            HitRecord node_record;
            node_record.primitives_to_ignore = record.primitives_to_ignore;
            if (intersectNodeVisibility(nodes_to_look[0], ray, current_min, current_max, node_record)) {
                record = std::move(node_record);
                return true;
            }
            if (intersectNodeVisibility(nodes_to_look[1], ray, current_min, current_max, node_record)) {
                record = std::move(node_record);
                return true;
            }
            return false;
        }


        void printBuildStats() const override {
            CRAY_INFO("BVH Build Stats:");
            CRAY_INFO("    Primitives:                 %u", m_primitive_list.m_items.size());
            CRAY_INFO("    Total nodes:                %u", m_num_nodes);
            CRAY_INFO("    Leaf nodes:                 %u", m_leaf_nodes);
            CRAY_INFO("    Children nodes:             %u (%.2f per node)", m_children_nodes, m_children_per_node_avg);
            CRAY_INFO("    Max depth:                  %d", m_max_depth);
            CRAY_INFO("    Node size:                  %s", convertToCapacitySizeString(sizeof(Node)).c_str());
            CRAY_INFO("    Total nodes size:           %s", convertToCapacitySizeString(m_num_nodes * sizeof(Node)).c_str());
            CRAY_INFO("    Total primitives size:      %s", convertToCapacitySizeString(m_primitive_size).c_str());
        }

        void printTracingStats(int32_t total_pixels, uint32_t iterations) const override {
            cr_float div = cr_float(total_pixels) * cr_float(s_global_settings->m_samples_per_pixel) * cr_float(iterations);
            CRAY_INFO("BVH Tracing Stats:");
            CRAY_INFO("    Rays:                       %u (%.2f per spp)", s_global_settings->m_stats.m_number_rays, s_global_settings->m_stats.m_number_rays / div);
            CRAY_INFO("    Shadow rays:                %u (%.2f per spp)", s_global_settings->m_stats.m_number_shadow_rays, s_global_settings->m_stats.m_number_shadow_rays / div);
            CRAY_INFO("    Prim Intersect:             %.2f per ray", s_global_settings->m_stats.m_primitive_intersections_avg);
            CRAY_INFO("    Prim Intersect Shadow:      %.2f per ray", s_global_settings->m_stats.m_primitive_intersections_shadow_avg);
            CRAY_INFO("    Node traversals:            %.2f per ray", s_global_settings->m_stats.m_node_traversals_avg);
            CRAY_INFO("    Node traversals Shadow:     %.2f per ray", s_global_settings->m_stats.m_node_traversals_shadow_avg);
        }

        void clear() override {
            m_root.reset();
            m_primitive_list.destroy_all();
            m_num_nodes = 0;
            m_leaf_nodes = 0;
            m_children_nodes = 0;
            m_children_per_node_avg = 0;
            m_max_depth = 0;
        }

        void preprocess(const glm::mat4x4& wcs_to_ecs) override {
            AccelerationStructureInterface::preprocess(wcs_to_ecs);
            m_root = std::make_unique<Node>();
            CRAY_FORCE_LOG("Start Splitting Root");
            m_max_depth = 0;
            recursiveBuild(m_root.get(), 0, m_primitive_list.m_items.size(), 0);
            CRAY_FORCE_LOG("End Splitting Root");
            CRAY_FORCE_LOG("Max BVH depth: %d", m_max_depth);
            m_leaf_nodes = m_num_nodes - m_children_nodes;
            if (m_children_nodes > 0.0) {
                m_children_per_node_avg = m_primitive_list.m_items.size() / double(m_children_nodes);
            }
            //printBuildStats();
        }

        bool recursiveBuild(Node* node, size_t start, size_t end, size_t depth) {

            if (start == std::numeric_limits<std::size_t>::max() || end == std::numeric_limits<std::size_t>::max() ||
                start >= end) {
                return false;
            }
            m_max_depth = glm::max(depth, m_max_depth);
            ++m_num_nodes;

            // compute centroids and bounds
            AABB centroid_bounds;
            AABB bounds;
            for (size_t prim_index = start; prim_index < end; ++prim_index) {
                PrimitiveInterface* prim = m_primitive_list.m_items[prim_index].get();
                glm::vec3 centroid = prim->m_box.getCentroid();
                centroid_bounds.expandBy(centroid);
                bounds.expandBy(prim->m_box);
                //CRAY_FORCE_LOG("Bounds (%u): %.2f %.2f %.2f, %.2f %.2f %.2f, %.2f %.2f %.2f",
                //    prim_index, centroid_bounds.m_min.x, centroid_bounds.m_min.y, centroid_bounds.m_min.z, centroid_bounds.m_max.x, centroid_bounds.m_max.y, centroid_bounds.m_max.z, centroid.x, centroid.y, centroid.z);
                //CRAY_FORCE_LOG("Bounds (%u): %.2f %.2f %.2f, %.2f %.2f %.2f", prim_index, bounds.m_min.x, bounds.m_min.y, bounds.m_min.z, bounds.m_max.x, bounds.m_max.y, bounds.m_max.z);
            }
            node->bounds = bounds;

            // add to leaf if certain amount of primitives
            if (addMinimumPrimitivesToLeaf(node, start, end)) {
                return true;
            }

            // find max extent and choose split axis
            int split_axis = centroid_bounds.getMaxDimension();
            node->split_axis = split_axis;

            CRAY_FORCE_LOG("Bounds (%d): %.2f %.2f %.2f, %.2f %.2f %.2f", split_axis, bounds.m_min.x, bounds.m_min.y, bounds.m_min.z, bounds.m_max.x, bounds.m_max.y, bounds.m_max.z);

            // choose candidates for SAH by binning
            struct BinInfo {
                size_t num_primitives = { 0 };
                AABB m_bounds;
                cr_float right_point = { 0 };
                cr_float cost = { 0 };
                size_t last_primitive_index = { 0 };
            };
            constexpr size_t num_bins = 10;
            std::array<BinInfo, num_bins> bins;

            // sort primitives in the list by their centroids on the requested axis
            std::sort(m_primitive_list.m_items.begin() + start, m_primitive_list.m_items.begin() + end,
                [split_axis](const std::shared_ptr<PrimitiveInterface> a, const std::shared_ptr<PrimitiveInterface> b) {
                    return a->m_box.getCentroid()[split_axis] < b->m_box.getCentroid()[split_axis];
                });
            cr_float centroid_min = centroid_bounds.m_min[split_axis];
            cr_float centroid_max = centroid_bounds.m_max[split_axis];
            cr_float centroid_range = centroid_max - centroid_min;
            // in case primitives have the centroid, e.g. overlapping primitives
            // then just add them to the list, otherwise we end on infinite recursion
            if (centroid_range < 0.001) {
                node->primitive_list.reserve(end - start);
                for (size_t prim_index = start; prim_index < end; ++prim_index) {
                    std::shared_ptr<PrimitiveInterface> prim = m_primitive_list.m_items[prim_index];
                    node->primitive_list.push_back(prim);
                    CRAY_FORCE_LOG("Added overlapping primitive (%d) on axis (%d)", prim_index, node->split_axis);
                }
                if (node->primitive_list.size() > 4) {
                    CRAY_DEBUG("Added %d primitives with similar centroids (range %f, axis: %d) on one node", node->primitive_list.size(), centroid_range, node->split_axis);
                }
                ++m_children_nodes;
                return true;
            }
            cr_float centroid_step = centroid_range / cr_float(num_bins);
            // create a number of bins to use as candidates for SAH
            for (size_t i = 0; i < num_bins - 1; ++i) {
                BinInfo& bin = bins[i];
                bin.right_point = centroid_min + (i + 1) * centroid_step;
            }
            // to account for accuracy issues
            bins[num_bins - 1].right_point = centroid_max;

            // calculate the number of primitives and area per bin
            for (size_t prim_index = start; prim_index < end; ++prim_index) {
                PrimitiveInterface* prim = m_primitive_list.m_items[prim_index].get();
                bool found_bin = false;
                for (size_t i = 0; i < num_bins; ++i) {
                    BinInfo& bin = bins[i];
                    const glm::vec3& centroid = prim->m_box.getCentroid();
                    if (centroid[split_axis] <= bin.right_point) {
                        bin.num_primitives++;
                        bin.m_bounds.expandBy(prim->m_box);
                        found_bin = true;
                        //  CRAY_FORCE_LOG("Added prim %.2f %.2f %.2f to bucket %d", centroid.x, centroid.y, centroid.z, i);
                        break;
                    }
                }
                CRAY_ASSERT_IF_FALSE_MSG(found_bin, "Primitive %f not entered to bin on axis %d", prim_index, prim->getCentroid()[split_axis], split_axis);
            }

            // splitting strategy - SAH
            cr_float t_traversal = 1;
            cr_float t_intersect = 1;
            size_t index_smaller_bin = 0;
            cr_float lowest_cost = std::numeric_limits<cr_float>::max();
            size_t current_primitive_index = start;
            AABB sa_bounds;
            size_t num_primitives = end - start;
            size_t num_primitives_left = 0;
            cr_float total_surface_area = bounds.getSurfaceArea();
            for (size_t i = 0; i < num_bins - 1; ++i) {
                BinInfo& bin = bins[i];
                // calculate left side of bin
                num_primitives_left += bin.num_primitives;
                if (bin.num_primitives > 0) {
                    sa_bounds.expandBy(bin.m_bounds);
                }
                const cr_float sa = sa_bounds.getSurfaceArea();
                const cr_float probability_left = sa / total_surface_area;
                // calculate right side of bin
                AABB sb_bounds;
                for (size_t j = i + 1; j < num_bins; ++j) {
                    if (bins[j].num_primitives > 0) {
                        sb_bounds.expandBy(bins[j].m_bounds);
                    }
                }
                const cr_float sb = sb_bounds.getSurfaceArea();
                const cr_float probability_right = sb / total_surface_area;
                const size_t num_primitives_right = num_primitives - num_primitives_left;

                // calculate the SAH cost and keep the lowest one
                bin.cost = t_traversal + (probability_left * num_primitives_left + probability_right * num_primitives_right) * t_intersect;
                if (bin.cost < lowest_cost) {
                    lowest_cost = bin.cost;
                    index_smaller_bin = i;
                }
                current_primitive_index += bin.num_primitives;
                bin.last_primitive_index = current_primitive_index;
                // CRAY_FORCE_LOG("Bucket %d cost %.2f", i, bin.cost);
            }

            CRAY_FORCE_LOG("Lowest cost %d %.2f", index_smaller_bin, lowest_cost);

            // add to leaf if certain amount of primitives
            if (addMinimumPrimitivesToLeaf(node, start, end)) {
                return true;
            }

            // recurse and continue the splitting process
            size_t mid = bins[index_smaller_bin].last_primitive_index;
            if (mid > start) {
                std::unique_ptr<Node> left_node = std::make_unique<Node>();
                CRAY_FORCE_LOG("Start splitting Left(%d) on %d %d (Depth %d)", split_axis, start, mid, depth);
                if (recursiveBuild(left_node.get(), start, mid, depth + 1)) {
                    node->left_child = std::move(left_node);
                }
                CRAY_FORCE_LOG("Done splitting Left(%d) on %d %d (Depth %d)", split_axis, start, mid, depth);
            }
            if (mid < end) {
                std::unique_ptr<Node> right_node = std::make_unique<Node>();
                CRAY_FORCE_LOG("Start splitting Right(%d) on %d %d (Depth %d)", split_axis, mid, end, depth);
                if (recursiveBuild(right_node.get(), mid, end, depth + 1)) {
                    node->right_child = std::move(right_node);
                }
                CRAY_FORCE_LOG("Done splitting Right(%d) on %d %d (Depth %d)", split_axis, mid, end, depth);
            }
            return true;
        }

        bool addMinimumPrimitivesToLeaf(Node* node, size_t start, size_t end) {
            // if four children add them to leaf, assuming the splitting is done correctly till now
            // this avoids exploding the number of nodes
            if (end - start <= 4) {
                node->primitive_list.reserve(end - start);
                for (size_t prim_index = start; prim_index < end; ++prim_index) {
                    std::shared_ptr<PrimitiveInterface> prim = m_primitive_list.m_items[prim_index];
                    node->primitive_list.push_back(prim);
                }
                CRAY_FORCE_LOG("Added %u leaf primitives (indices %d to %d) on axis (%d)", end - start, start, end - 1, node->split_axis);
                ++m_children_nodes;
                return true;
            }
            return false;
        }

        size_t height(Node* node) const {
            if (node == nullptr) {
                return 0;
            }
            size_t left_height = height(node->left_child.get()) + 1;
            size_t right_height = height(node->right_child.get()) + 1;
            return left_height > right_height ? left_height : right_height;
        }
    };

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

        std::string getDurationDoubleStr() const {
            double duration = getDurationDouble();
            std::string res = NumberToString(duration, 2) + "ms";
            if (duration > 1000) {
                res += " (" + convertToTimeString(duration) + ")";
            }
            return res;
        }

        double getDurationDouble() const {
            return time_span.count();
        }

        uint64_t getDurationUint() const {
            return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(timer_end - timer_start).count());
        }
    };

    struct ThreadBlock {
        glm::ivec2 start = { glm::ivec2(0) };
        glm::ivec2 end = { glm::ivec2(0) };
        glm::ivec2 max_dim = { glm::ivec2(1) };
        int32_t current_block_id = { 0 };
        uint32_t seed = { 0 };
        glm::ivec2 toIndex2D(int32_t tile_size) {
            return glm::ivec2(
                current_block_id % tile_size,  // cols (width),
                current_block_id / tile_size); // rows (height)
        }
    };

    struct Scene {
        bool m_rebuild = { true };
        std::vector<glm::mat4x4> m_matrix_stack;
        glm::mat4x4 m_current_world_matrix = { glm::mat4x4(1.0) };
        VectorHandler<TextureInterface> m_textures;
        uint64_t m_texture_size = { 0 };
        VectorHandler<ImageInterface> m_images;
        VectorHandler<TextureSampler> m_samplers;
        VectorHandler<MaterialInterface> m_materials;
        VectorHandler<LightInterface> m_light_list;
        cr_float m_total_flux = { 0.0 };
        std::vector<std::pair<const LightInterface*, const PrimitiveInterface*>> m_light_elements; // total list of lights
        std::vector<std::pair<const LightInterface*, const PrimitiveInterface*>> m_light_cdf;
        std::unique_ptr<CameraInterface> m_camera = { nullptr };
        std::unique_ptr<AccelerationStructureInterface> m_acceleration_structure = { nullptr };

        CRAY_HANDLE addPrimitive(std::shared_ptr<PrimitiveInterface> prim, CRAY_HANDLE material_id, int size) {
            auto material = getMaterial(material_id);
            if (material == nullptr) {
                CRAY_WARNING("Material %d does not exists. Skipping");
                return CRAY_INVALID_HANDLE;
            }
            prim->m_material = material;
            prim->updateTransform(m_current_world_matrix);
            m_rebuild = true;
            return m_acceleration_structure->addPrimitive(prim, size);
        }
        std::shared_ptr<PrimitiveInterface> getPrimitive(CRAY_HANDLE primitive_id) { return m_acceleration_structure->m_primitive_list.find_item(primitive_id); }
        CRAY_HANDLE addLight(std::shared_ptr<LightInterface> light) {
            light->updateTransform(m_current_world_matrix);
            CRAY_HANDLE light_id = m_light_list.add_item(light);
            if (light->isPoint()) {
                m_light_elements.push_back(std::make_pair(light.get(), nullptr));
            }
            return light_id;
        }
        std::shared_ptr<LightInterface> getLight(CRAY_HANDLE light_id) { return m_light_list.find_item(light_id); }
        void setCamera(std::unique_ptr<CameraInterface> camera) {
            m_camera = std::move(camera); m_camera->updateTransform(m_current_world_matrix);
        }
        CRAY_HANDLE addMaterial(std::shared_ptr<MaterialInterface> material) { return m_materials.add_item(material); }
        std::shared_ptr<MaterialInterface> getMaterial(CRAY_HANDLE material_id) { return m_materials.find_item(material_id); }
        CRAY_HANDLE addTexture(std::shared_ptr<TextureInterface> texture) { return m_textures.add_item(texture); }
        std::shared_ptr<TextureInterface> getTexture(CRAY_HANDLE texture_id) { return m_textures.find_item(texture_id); }
        CRAY_HANDLE addImage(std::shared_ptr<ImageInterface> image) { CRAY_HANDLE image_id = m_images.add_item(image); m_texture_size = image->getSize(); return image_id; }
        std::shared_ptr<ImageInterface> getImage(CRAY_HANDLE image_id) { return m_images.find_item(image_id); }
        CRAY_HANDLE addTextureSampler(std::shared_ptr<TextureSampler> texture_sampler) { return m_samplers.add_item(texture_sampler); }
        std::shared_ptr<TextureSampler> getTextureSampler(CRAY_HANDLE sampler_id) { return m_samplers.find_item(sampler_id); }
        void setAccelerationStructure(std::unique_ptr<AccelerationStructureInterface> acceleration_structure) { m_acceleration_structure = std::move(acceleration_structure); }

        bool intersect(const Ray& ray, cr_float current_min, cr_float current_max, HitRecord& record) const { return m_acceleration_structure->intersect(ray, current_min, current_max, record); }
        bool intersectVisibility(const Ray& ray, cr_float current_min, cr_float current_max, HitRecord& record) const { return m_acceleration_structure->intersectVisibility(ray, current_min, current_max, record); }

        void clearStats() {
            s_global_settings->m_stats.clear();
        }

        void printStats(uint32_t iterations) {
            int32_t total_pixels = s_global_framebuffer->m_dimensions.x * s_global_framebuffer->m_dimensions.y;
            cr_float div = cr_float(total_pixels) * cr_float(s_global_settings->m_samples_per_pixel) * cr_float(iterations);
            CRAY_INFO("Scene stats:");
            CRAY_INFO("    Cameras:                    %u", 1);
            CRAY_INFO("    Lights (unique):            %u", m_light_list.m_items.size());
            CRAY_INFO("    Total Light sources:        %u", m_light_elements.size());
            CRAY_INFO("    Materials:                  %u", m_materials.m_items.size());
            CRAY_INFO("    Textures (size):            %u (%s)", m_images.m_items.size(), convertToCapacitySizeString(m_texture_size).c_str());
            CRAY_INFO("Integrator stats:");
            CRAY_INFO("    Samples per pixel (spp):    %u", s_global_settings->m_samples_per_pixel);
            if (s_global_integrator->m_type == CR_INTEGRATOR_PT) {
                CRAY_INFO("    Type:                       Path Tracing");
                CRAY_INFO("    Max depth:                  %u (%.2f per spp)", s_global_settings->m_max_depth, s_global_settings->m_stats.m_ray_depth / div);
                CRAY_INFO("    Russian roulette:           %s", s_global_settings->m_russian_roulette ? "yes" : "no");
            }
            else if (s_global_integrator->m_type == CR_INTEGRATOR_AO) {
                CRAY_INFO("    Type:                       Ambient Occlusion");
                CRAY_INFO("    AO Samples per spp:         %u", s_global_settings->m_ao_samples_per_pixel);
                if (s_global_settings->m_ao_range > 0.0) {
                    CRAY_INFO("    Range:                      %.2f", s_global_settings->m_ao_range);
                }
                else {
                    CRAY_INFO("    Range:                      Infinite");
                }
            }
            m_acceleration_structure->printBuildStats();
            m_acceleration_structure->printTracingStats(total_pixels, iterations);
            s_global_framebuffer->printStats();
        }

        void clear() {
            m_acceleration_structure.reset();
            m_camera.reset();
            m_light_list.destroy_all();
            m_materials.destroy_all();
            m_textures.destroy_all();
            m_images.destroy_all();
            m_light_elements.clear();
            m_light_elements.shrink_to_fit();
            m_light_cdf.clear();
            m_light_cdf.shrink_to_fit();
            m_total_flux = 0.0;
            m_texture_size = 0;
            m_rebuild = true;
        }

        void preprocess() {
            // rebuild camera parameters if film dimensions have changed, e.g. via GUI
            if (m_camera && (m_rebuild || s_global_framebuffer->m_realloc)) {
                m_camera->preprocess();
            }

            if (!m_rebuild) {
                CRAY_INFO("Preprocessing scene skipped");
                return;
            }

            CRAY_INFO("Preprocessing scene");
            if (m_acceleration_structure) {
#ifdef EYE_SPACE_SHADING
                for (auto& prim : m_acceleration_structure->m_primitive_list.m_items) {
                    prim->updateTransform(m_camera->m_wcs_to_ecs * prim->getTransform());
                }
#endif
                m_acceleration_structure->preprocess(m_camera->m_wcs_to_ecs);
            }
            for (auto& light : m_light_list.m_items) {
#ifdef EYE_SPACE_SHADING
                light->updateTransform(m_camera->m_wcs_to_ecs * light_pair->getTransform());
#endif
                light->preprocess();
            }

            if (m_light_elements.size() > 1) {
                // calculate flux distribution and build an approximate CDF containing as many light sources as possible
                cr_float min_value = std::numeric_limits<cr_float>::max();
                cr_float max_value = 0.0;
                // find flux range
                auto light_pair_iter = m_light_elements.begin();
                while (light_pair_iter != m_light_elements.end()) {
                    const LightInterface* light = light_pair_iter->first;
                    // assume that light source flux is equally distributed amongst primitives
                    // while this is approximate, it avoids missing lights of tiny areas that could result in an entire area not being lit
                    // during rendering, the correct flux is used
                    // For accurate rendering, one light per primitive should be supplied
                    cr_float scalar_flux = light->m_scalar_flux / light->m_total_shared_primitives;
                    //const PrimitiveInterface* primitive = light_pair_iter->second;
                    //CR_float scalar_flux = primitive->m_scalar_flux;
                    if (scalar_flux < FLOAT_TOLERANCE_5) {
                        light_pair_iter = m_light_elements.erase(light_pair_iter);
                        continue;
                    }
                    min_value = glm::min(min_value, scalar_flux);
                    max_value = glm::max(max_value, scalar_flux);
                    m_total_flux += scalar_flux;
                    ++light_pair_iter;
                }

                // generate number of samples (use a range of up to 10000 for adequate spacing)
                size_t max_samples = 10000;
                size_t num_samples = size_t(m_total_flux / min_value);
                num_samples = glm::clamp(num_samples, size_t(m_light_elements.size()), max_samples);

                // build CDF
                m_light_cdf.reserve(num_samples);
                cr_float flux_per_sample = m_total_flux / cr_float(num_samples + 1);
                size_t cur_sample = 0;
                size_t total_samples = 0;
                cr_float cur_flux = 0;
                size_t lights_added = 0;
                for (auto light_pair : m_light_elements) {
                    const LightInterface* light = light_pair.first;
                    //const PrimitiveInterface* primitive = light_pair.second;
                    bool added_light = false;
                    for (size_t sample = cur_sample; sample < num_samples; ++sample) {
                        ++total_samples;
                        m_light_cdf.push_back(light_pair);
                        cur_flux += flux_per_sample;
                        if (!added_light) {
                            ++lights_added;
                            added_light = true;
                        }
                        // assume that light source flux is equally distributed amongst primitives
                        // while this is approximate, it avoids missing lights of tiny areas that could result in an entire area not being lit
                        // during rendering, the correct flux is used
                        // For accurate rendering, one light per primitive should be supplied
                        cr_float scalar_flux = light->m_scalar_flux / light->m_total_shared_primitives;
                        //CR_float scalar_flux = primitive->m_scalar_flux;
                        if (cur_flux + flux_per_sample > scalar_flux) {
                            cur_sample = sample + 1;
                            cur_flux = 0;
                            added_light = false;
                            break;
                        }
                    }
                }

                CRAY_ASSERT_IF_FALSE_MSG(lights_added == m_light_elements.size(), "Not all lights added to CDF %d!=%u", lights_added, m_light_elements.size())
                    // resize if we did not fill the array properly
                    if (m_light_cdf.size() > max_samples) {
                        m_light_cdf.resize(max_samples);
                    }
                CRAY_DEBUG("Created CDF for %u light sources with %d samples", m_light_list.m_items.size(), max_samples);
            }

            m_rebuild = false;
        }
    };

    static std::unique_ptr<Scene> s_global_scene = { nullptr };
    struct LoggerMessage {
        CRAY_LOGGERENTRY type = { CR_LOGGER_NOTHING };
        std::string message = { "" };

        explicit LoggerMessage(CRAY_LOGGERENTRY _type, const std::string& _message) : type(_type), message(_message) {}
    };

    struct Engine : public NonCopyable {
        ~Engine() { destroy(); }

        double m_total_time = { 0.0 };
        std::vector<std::future<void>> m_thread_list;
        std::deque<LoggerMessage> m_logger_queue;
        bool m_started = { false };
        bool m_stop_requested = { false };
        bool m_image_ready = { false };

        int32_t m_actual_threads = { 0 };
        int32_t m_block_size = { 16 };
        std::atomic<int32_t> m_current_block_index = { -1 };
        std::atomic<uint32_t> m_current_iteration = { 1 };
        glm::ivec2 m_blocks2D = { glm::uvec2(0) };
        int32_t m_total_blocks = { 0 };

        // number of threads (as a rule of thumb, it should not exceed the system cores)
        int32_t m_requested_threads = { 1 };
        bool m_use_main_thread = { true };
        int32_t m_barrier_thread_count = 0;

        Timer m_thread_timer;

        std::mutex completion_mutex;
        std::mutex percentage_mutex;

        void destroy() {
            endThreads();
            s_global_settings = nullptr;
        }

        void endThreads() {
            for (auto& th : m_thread_list) {
                th.get();
            }
            m_thread_list.clear();
            m_thread_list.shrink_to_fit();
        }

        void resetExecutionData() {
            endThreads();
            m_started = true;
            m_stop_requested = false;
            m_image_ready = false;
            m_total_time = 0.0;
            m_blocks2D = glm::uvec2(0);
            m_total_blocks = 0;
            m_current_block_index = -1;
        }

        void printPercentage() {
            if (!s_thread_settings->m_print_to_stdout || !s_thread_settings->m_print_progress_bar_stdout) {
                return;
            }
            std::lock_guard<std::mutex> lk(percentage_mutex);
            const size_t barWidth = 100;
            const cr_float step = barWidth / 100.0;
            const cr_float percentage = CR_getProgressPercentage();
            size_t pos = (size_t)(percentage * step);
            if (pos < 2) {
                pos = 2;
            }
            size_t i = 0;
            char buffer[256];
            sprintf(buffer, "\r[");
            for (i = 2; i < pos; ++i) {
                buffer[i] = '=';
            }
            buffer[pos] = '>';
            for (i = pos + 1; i < barWidth; ++i) {
                buffer[i] = ' ';
            }
            sprintf(&buffer[i], "] %.2f%%", percentage);
            fputs(buffer, stdout);
        }

        void run() {
            ThreadBlock block;
            block.max_dim = s_global_framebuffer->m_dimensions;
            std::string id = getThreadIdStr();
            if (m_actual_threads > 0) {
                s_thread_settings = std::make_unique<GlobalSettings>();
            }
            *s_thread_settings = *s_global_settings;
            s_thread_settings->m_stats.clear();
            rand_set_fast_rng(s_thread_settings->m_interactive);
            CRAY_DEBUG("Starting thread %s", id.c_str());

            while (getNextBlock(block)) {
                s_global_integrator->renderBlock(block);
                s_thread_settings->m_stats.updateThreadRunningSum();
                if (m_stop_requested) {
                    break;
                }
                printPercentage();
            }

            // notify finish_rendering when finished to wrap up
            std::lock_guard<std::mutex> lk(completion_mutex);
            --m_barrier_thread_count;
            CRAY_DEBUG("Finished thread %s: Waiting: %d", id.c_str(), m_barrier_thread_count);
            s_global_settings->m_stats.add(s_thread_settings->m_stats);

            if (m_barrier_thread_count <= 0) {
                if (s_thread_settings->m_print_to_stdout && s_thread_settings->m_print_progress_bar_stdout) {
                    fputs("\n", stdout);
                }
                finish_rendering();
            }

        }

        void start_threaded_execution() {
            m_blocks2D = glm::ivec2(glm::ceil(glm::vec2(s_global_framebuffer->m_dimensions) / cr_float(m_block_size)));
            m_blocks2D = glm::max(m_blocks2D, glm::ivec2(1));
            m_total_blocks = m_blocks2D.x * m_blocks2D.y;

            // actual number of threads. Use the main thread if requested, but skip for GUI operations
            // also, don't spawn more threads than blocks
            if (s_global_settings->m_interactive) {
                m_actual_threads = glm::min(static_cast<int32_t>(std::thread::hardware_concurrency()), m_total_blocks);
                m_actual_threads = glm::max(2, m_actual_threads - 1);
                //m_actual_threads = 1;
            }
            else {
                m_actual_threads = glm::min(m_requested_threads, m_total_blocks);
                if (m_use_main_thread) {
                    m_actual_threads = glm::max(0, m_actual_threads - 1);
                }
            }
            m_barrier_thread_count = m_use_main_thread ? m_actual_threads + 1 : m_actual_threads;

            // reserve threads
            if (m_actual_threads > 0) {
                m_thread_list.reserve(m_actual_threads);
            }
            for (int i = 0; i < m_actual_threads; ++i) {
                m_thread_list.push_back(std::async(std::launch::async, &Engine::run, this));
            }
            if (m_use_main_thread || m_actual_threads == 0) {
                run();
                // wait for any threads to finish
                endThreads();
            }
        }

        void finish_rendering() {
            m_started = false;
            m_stop_requested = false;
            m_image_ready = true;

            m_thread_timer.stop();
            rand_reset();

            int total_threads = m_actual_threads;
            if (m_use_main_thread) {
                total_threads++;
            }
            CRAY_INFO("Renderer stats:");
            if (m_actual_threads == 0) {
                CRAY_INFO("    Threads:                    0 (serial execution)");
                CRAY_INFO("    Tile Size:                  %dx%d", s_global_framebuffer->m_dimensions.x, s_global_framebuffer->m_dimensions.y);
            }
            else {
                CRAY_INFO("    Threads:                    %u%s", total_threads, m_use_main_thread ? " (with main thread)" : "");
                CRAY_INFO("    Tile Size:                  %d", m_block_size);
            }
            s_global_scene->printStats(m_current_iteration);

            if (!s_global_settings->m_interactive) {
                CRAY_INFO("Finished (%u threads, %d blocks) in %s", total_threads, m_total_blocks, m_thread_timer.getDurationDoubleStr().c_str());
            }
            else {
                CRAY_INFO("Finished (%u threads, %d blocks) interactive session with %u iterations", total_threads, m_total_blocks, uint32_t(m_current_iteration));
                m_current_iteration = 1;
            }
            s_global_integrator.reset();
            setInteractiveRender(false);
            s_global_scene->clearStats();
        }

        void start() {
            m_logger_queue.clear();
            m_thread_timer.start();
            s_global_scene->preprocess();
            s_global_scene->clearStats();
            s_global_framebuffer->alloc();
            s_global_framebuffer->clear(0);
            m_thread_timer.stop();
            *s_thread_settings = *s_global_settings;
            s_thread_settings->m_stats.clear();
            CRAY_INFO("Preprocessing scene took: %s", m_thread_timer.getDurationDoubleStr().c_str());

            resetExecutionData();
            m_thread_timer.start();

            start_threaded_execution();
        }

        // Threaded execution
        std::string getThreadIdStr() {
            auto myid = std::this_thread::get_id();
            std::stringstream ss;
            ss << myid;
            return ss.str();
        }

        bool getNextBlock(ThreadBlock& th) {
            // lock-free implementation. m_current_block_index can exceed m_total_blocks, but it doesn't matter as the thread has finished
            th.current_block_id = ++m_current_block_index;

            if (th.current_block_id >= m_total_blocks) {
                if (!s_thread_settings->m_interactive) {
                    return false;
                }
                else {
                    // restart the process for interactive rendering
                    m_current_block_index = -1;
                    ++m_current_iteration;
                    th.current_block_id = 0;
                }
            }

            glm::ivec2 index2D = th.toIndex2D(m_blocks2D.x);
            th.start = index2D * m_block_size;
            th.end = (index2D + 1) * m_block_size;
            th.end = glm::min(th.end, th.max_dim);

            // maintain a consistent seed per tile
            th.seed = (th.current_block_id + 1) * 13;
            // obtain a different seed per iteration for interactive rendering
            // since the seed is restarted per sample the quality of the image is affected
            // and should be used only for visualization purposes
            if (s_thread_settings->m_interactive) {
                th.seed *= m_current_iteration * 17;
            }
            rand_reset_seed(th.seed);

            return true;
        }

        std::unique_ptr<GlobalSettings> s_global_normal_settings = { nullptr };
        std::unique_ptr<GlobalSettings> s_global_interactive_settings = { nullptr };
        void setInteractiveRender(bool enable) {
            // init local settings (this must be called during init)
            if (s_global_interactive_settings == nullptr) {
                s_global_interactive_settings = std::make_unique<GlobalSettings>();
            }
            if (s_global_normal_settings == nullptr) {
                s_global_normal_settings = std::make_unique<GlobalSettings>();
                s_global_settings = s_global_normal_settings.get();
            }

            // inform the framebuffer to realloc if required
            if (s_global_settings->m_interactive != enable) {
                s_global_framebuffer->requestRealloc();
            }

            // if interactive, update only the needed settings
            if (enable) {
                *(s_global_interactive_settings.get()) = *(s_global_normal_settings.get());
                s_global_interactive_settings->m_russian_roulette = true;
                s_global_interactive_settings->m_ao_samples_per_pixel = 1;
                s_global_interactive_settings->m_samples_per_pixel = 1;
                s_global_interactive_settings->m_interactive = true;
                m_current_iteration = 1;
                s_global_settings = s_global_interactive_settings.get();
                s_global_framebuffer->clearIteration(0);
            }
            else {
                s_global_settings = s_global_normal_settings.get();
            }
            s_global_settings->m_interactive = enable;
            *s_thread_settings = *s_global_settings;
        }
    };
    static std::unique_ptr<Engine> s_global_engine = { nullptr };

    struct IntegratorAO : public IntegratorInterface {

        void renderBlock(ThreadBlock& block) const override {

            Ray ray(glm::vec3(0), glm::vec3(0));
            for (int32_t row = block.start.y; row < block.end.y && !s_global_engine->m_stop_requested; row++) {        // height
                for (int32_t col = block.start.x; col < block.end.x && !s_global_engine->m_stop_requested; col++) {    // width
                    Color color;
                    for (int32_t sample = 0; sample < s_thread_settings->m_samples_per_pixel && !s_global_engine->m_stop_requested; ++sample) {
                        Ray camera_ray = s_global_scene->m_camera->generateRay(col, row, sample == 0);
                        color += evaluateRayRadiance(camera_ray);
                    }
                    color.data /= cr_float(s_thread_settings->m_samples_per_pixel);

                    s_global_framebuffer->WriteColor(color, col, row);
                }
            }
        }

        Color evaluateRayRadiance(Ray& r) const {
            // world intersect
            Color color(0.0);
            HitRecord record;
            cr_float current_min = 0.0;
            cr_float current_max = std::numeric_limits<cr_float>::max(); // TODO:: assumes infinite camera
            if (s_global_scene->intersect(r, current_min, current_max, record)) {
                color = evaluateSampleAO(record);
            }
            return color;
        }

        Color evaluateSampleAO(HitRecord& record) const {
            Color ao_color(0, 0, 0);
            for (int32_t sample = 0; sample < s_thread_settings->m_ao_samples_per_pixel && !s_global_engine->m_stop_requested; ++sample) {
                glm::vec3 sample_direction = getCosineHemisphereSample();
                cr_float pdf = getCosineHemisphereSamplePDF(sample_direction);
                // align with normal hemisphere
                glm::vec3 tangent(0);
                glm::vec3 bitangent(0);
                getOrthonormalBasis(bitangent, tangent, record.normal);

                sample_direction = glm::normalize(tangent * sample_direction.x + bitangent * sample_direction.y + record.normal * sample_direction.z);

                Ray sample_ray(record.intersection_point + sample_direction * GLOBAL_RAY_EPSILON, sample_direction, true);
                cr_float current_max = s_thread_settings->m_ao_range > 0.0 ? s_thread_settings->m_ao_range : std::numeric_limits<cr_float>::max();
                HitRecord ao_record;

                if (!s_global_scene->intersectVisibility(sample_ray, 0.0, current_max, ao_record)) {
                    ao_color += Color(glm::dot(record.normal, sample_direction) / pdf);
                }
            }
            ao_color /= cr_float(s_thread_settings->m_ao_samples_per_pixel);

            return ao_color;
        }
    };

    struct IntegratorPath : public IntegratorInterface {
        void renderBlock(ThreadBlock& block) const override {
            if (s_global_scene->m_materials.m_items.empty() || s_global_scene->m_light_list.m_items.empty() || s_global_scene->m_acceleration_structure->m_primitive_list.m_items.empty()) {
                return;
            }

            Ray ray(glm::vec3(0), glm::vec3(0));
            for (int32_t row = block.start.y; row < block.end.y && !s_global_engine->m_stop_requested; row++) {        // height
                for (int32_t col = block.start.x; col < block.end.x && !s_global_engine->m_stop_requested; col++) {    // width
                    Color color;
                    for (int32_t sample = 0; sample < s_thread_settings->m_samples_per_pixel && !s_global_engine->m_stop_requested; ++sample) {
                        Ray camera_ray = s_global_scene->m_camera->generateRay(col, row, sample == 0);
#ifdef ENABLE_DEBUG_CURRENT_PIXEL
                        if (isValidDebugPixel()) {
                            CRAY_DEBUG_PIXEL("(%.2f, %.2f, %u) ", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, sample)
                        }
#endif // ENABLE_DEBUG_CURRENT_PIXEL
                        Color transport_operators(1.0);
                        bool is_transmission = false;
                        bool sampled_specular = false;
                        bool break_path = false;
                        HitRecord record;
                        int32_t current_depth = 0;
                        for (; current_depth < s_thread_settings->m_max_depth && !break_path; ++current_depth) {
                            if (current_depth == 0) {
                                cr_float current_min(0.0);
                                cr_float current_max = std::numeric_limits<cr_float>::max(); // TODO:: assumes infinite camera
                                if (current_depth == 0 && !s_global_scene->intersect(camera_ray, current_min, current_max, record)) {
                                    CRAY_DEBUG_PIXEL("(%.2f, %.2f) (Depth: %d) No intersection with scene", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, current_depth)
                                        break;
                                }
                                // add L_emissive
                                if (record.primitive != nullptr && record.primitive->isLightSource()) {
                                    color += record.primitive->evaluateLemissive(record.normal, camera_ray.m_direction);
                                    CRAY_DEBUG_PIXEL("(%.2f, %.2f) (Depth: %d) L_e: %f %f %f, normal: %f %f %f, Ray Dir: %f %f %f, angle: %f",
                                        s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, current_depth,
                                        color.x, color.y, color.z, record.normal.x, record.normal.y, record.normal.z,
                                        camera_ray.m_direction.x, camera_ray.m_direction.y, camera_ray.m_direction.z, glm::degrees(glm::acos(glm::dot(record.normal, camera_ray.m_direction))));
                                }
                            }
                            color += evaluatePathRadiance(camera_ray, transport_operators, record, current_depth, is_transmission, sampled_specular, break_path);
                        }
                        s_thread_settings->m_stats.addRayDepth(current_depth);
                    }
                    color.data /= cr_float(s_thread_settings->m_samples_per_pixel);

                    s_global_framebuffer->WriteColor(color, col, row);
                }
            }
        }

        Color evaluatePathRadiance(Ray& outgoing_ray, Color& transport_operators, HitRecord& record, int32_t current_depth,
            bool& is_transmission, bool& sampled_specular, bool& break_path) const {
            Color radiance(Color::Black());
#ifdef ENABLE_DEBUG_CURRENT_PIXEL
            if (isValidDebugPixel()) {
                CRAY_DEBUG_PIXEL("(%.2f, %.2f) ", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y)
            }
#endif // ENABLE_DEBUG_CURRENT_PIXEL

            glm::vec3 vertex_to_prev_direction = -outgoing_ray.m_direction;
            cr_float light_selection_pdf(0.0);
            auto& selected_light_source = chooseLightSource(current_depth, light_selection_pdf);
            // evaluate direct light contribution by light sampling with MIS, but ignore any delta materials
            Color light_sample = directLightSample(vertex_to_prev_direction, record, current_depth, false, selected_light_source);

            //  if (s_global_scene->m_light_elements.size() > 1) {
            //     light_selection_pdf = selected_light_source.second->m_scalar_flux / s_global_scene->m_total_flux;
            // }

            CRAY_DEBUG_PIXEL("(%.2f, %.2f) (Depth: %d) Computing L throughput for next", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, current_depth)
                // spawn a new incoming direction based on the material properties
                cr_float transport_pdf(0.0);
            glm::vec3 vertex_to_next_direction = record.material->spawnNewRayDirection(transport_pdf, vertex_to_prev_direction, record.normal, is_transmission, sampled_specular);

            // check for invalid ray and break the loop, e.g. for total internal reflection
            if (vertex_to_next_direction == glm::vec3(0.0)) {
                CRAY_DEBUG_PIXEL("(%.2f, %.2f) (Depth: %d): Incorrect new direction. Ray skipped.", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, current_depth);
                break_path = true;
                radiance += transport_operators * light_sample / light_selection_pdf;
                return radiance;
            }
            // evaluate transport operator (current vertex to next)
            Color bsdf_next_vertex = Color(record.material->getBSDF(vertex_to_prev_direction, vertex_to_next_direction, record, is_transmission, false));

            // check for zero throughput
            if (bsdf_next_vertex.data == glm::vec3(0.0)) {
                CRAY_DEBUG_PIXEL("(%.2f, %.2f) (Depth: %d): Zero BSDF. Ray skipped.", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, current_depth);
                break_path = true;
                radiance += transport_operators * light_sample / light_selection_pdf;
                return radiance;
            }

            // ndotI factor
            cr_float ndotI = glm::abs(glm::dot(record.normal, vertex_to_next_direction));

            // update the transport operators for the next iteration
            Color current_throughput = bsdf_next_vertex * ndotI / transport_pdf;

            // BSDF MIS
            // set the new ray direction as the one calculated from the material
            outgoing_ray = Ray(record.intersection_point + vertex_to_next_direction * GLOBAL_RAY_EPSILON, vertex_to_next_direction);
            IntersectInterface* last_prim = record.primitive;
            glm::vec3 normal = record.normal;
            record = HitRecord();
            record.primitives_to_ignore = !is_transmission ? last_prim : nullptr;
            Color bsdf_sample(Color::Black());
            cr_float current_min(0.0);
            cr_float current_max = std::numeric_limits<cr_float>::max(); // TODO:: assumes infinite camera
            // check the scene for intersection and, in case of intersection, use the result for BSDF MIS weighting
            bool intersected = s_global_scene->intersect(outgoing_ray, current_min, current_max, record);
            if (!intersected) {
                break_path = true;
                radiance += transport_operators * light_sample / light_selection_pdf;
                return radiance;
            }

            bsdf_sample = BSDFSampleWeight(outgoing_ray, normal, record, current_depth, current_throughput,
                transport_pdf, vertex_to_next_direction, sampled_specular, selected_light_source);
            Color current_path = transport_operators * (light_sample + bsdf_sample) / light_selection_pdf;
            radiance += current_path;

            // if its the last ray, skip the throughput computations
            if (current_depth == s_thread_settings->m_max_depth - 1) {
                break_path = true;
                return radiance;
            }
            transport_operators *= current_throughput;

            CRAY_DEBUG_PIXEL("(%.2f, %.2f) (Depth: %d) Total T: %f, %f, %f, Current T: %f, %f, %f, BSDF: %f, %f, %f, ndotI: %f", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, current_depth,
                transport_operators.x, transport_operators.y, transport_operators.z, current_throughput.x, current_throughput.y, current_throughput.z,
                bsdf_next_vertex.x, bsdf_next_vertex.y, bsdf_next_vertex.z, ndotI);

            // apply Russian Roulette on the path throughput
            // note: do this last, after a couple of indirect bounces, in order to reduce variance and capture the most important illumination parts
            if (s_thread_settings->m_russian_roulette && current_depth > 2) {
                cr_float max_throughput = glm::max(glm::max(transport_operators.x, transport_operators.y), transport_operators.z);
                // if throughput is still high (i.e. most of the energy is still carried out in the scene), scale the estimator and continue
                if (rand_f() < max_throughput) {
                    transport_operators /= max_throughput;
                }
                else {
                    // if throughput is very low (i.e. r [0,1) higher than the MAX throughput), randomly terminate the ray
                    break_path = true;
                    return radiance;
                }
            }

            return radiance;
        }

        cr_float MISWeightPowerHeuristic(cr_float fPdf, cr_float gPdf) const {
            cr_float f = fPdf * fPdf;
            cr_float g = gPdf * gPdf;
            return f / (f + g);
        }

        std::pair<const LightInterface*, const PrimitiveInterface*>& chooseLightSource(int32_t current_depth, cr_float& out_light_selection_pdf) const {
            // choose a light source
            if (s_global_scene->m_light_elements.size() == 1) {
                out_light_selection_pdf = 1.0;
                size_t light_index = glm::clamp(glm::floor(rand_f_range(0.0, s_global_scene->m_light_elements.size())), cr_float(0.0), cr_float(s_global_scene->m_light_elements.size() - 1));
                auto& light_pair = s_global_scene->m_light_elements[light_index];
                CRAY_DEBUG_PIXEL("(%.2f, %.2f) (Depth: %u) Sampled Li (type: %s index: %u) with Discrete PDF: %f", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, current_depth,
                    light_pair.first->isPoint() ? "point" : "area", light_index, out_light_selection_pdf);
                return light_pair;
            }

            size_t light_index = glm::clamp(glm::floor(rand_f_range(0.0, s_global_scene->m_light_cdf.size())), cr_float(0.0), cr_float(s_global_scene->m_light_cdf.size() - 1));
            auto& light_pair = s_global_scene->m_light_cdf[light_index];
            const PrimitiveInterface* primitive = light_pair.second;
            // power-based PDF = F_primitive / F_total
            out_light_selection_pdf = primitive->m_scalar_flux / s_global_scene->m_total_flux;

            CRAY_DEBUG_PIXEL("(%.2f, %.2f) (Depth: %u) Sampled Li %u (type: %s) with power distribution PDF: %f", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, current_depth,
                light_index, light_pair.first->isPoint() ? "point" : "area", out_light_selection_pdf);
            return light_pair;
        }

        Color directLightSample(const glm::vec3& vertex_to_prev_direction, const HitRecord& record, int32_t current_depth,
            bool is_transmission, std::pair<const LightInterface*, const PrimitiveInterface*>& sampled_light) const {

#ifdef ENABLE_DEBUG_CURRENT_PIXEL
            if (isValidDebugPixel()) {
                CRAY_DEBUG_PIXEL("(%.2f, %.2f) ", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y)
            }
#endif // ENABLE_DEBUG_CURRENT_PIXEL

            // Light Sampling -> select a light sample and weight the contribution via MIS
            Color light_sampling_mis(Color::Black());
            cr_float light_pdf(0.0);
            cr_float distance_to_light = std::numeric_limits<cr_float>::max();
            glm::vec3 vertex_to_light_direction(0.0);
            const LightInterface* light_source = sampled_light.first;

            Color light_color = light_source->sampleLightSource(light_pdf, distance_to_light, vertex_to_light_direction, record, sampled_light.second);

            HitRecord light_record;
            cr_float visibility(0.0);
            Color bsdf(Color::Black());
            cr_float ndotL(0.0);
            cr_float bsdf_pdf(0.0);
            cr_float light_sampling_weight(0.0);
            if (light_pdf > 0.0 && light_color.isNot(Color::Black())) {
                // check for visibility
                light_record.primitives_to_ignore = record.primitive;
                // trace a shadow ray to the light source
                Ray light_ray(record.intersection_point + vertex_to_light_direction * GLOBAL_RAY_EPSILON, vertex_to_light_direction, true);
                visibility = s_global_scene->intersectVisibility(light_ray, 0.0, distance_to_light - GLOBAL_RAY_EPSILON * 2.0, light_record) ? 0.0 : 1.0;

                // test
                //HitRecord primitive_hit_record;
                //CR_float current_max = std::numeric_limits<CR_float>::max();
                //Ray light_ray2(record.intersection_point + vertex_to_light_direction * GLOBAL_RAY_EPSILON, vertex_to_light_direction, true);
                //bool intersect = sampled_light.second->intersect(light_ray2, 0.0, current_max, primitive_hit_record);
                //if the ray hit a different light source (due to inaccuracies, etc) don't waste the sample, just re-weight the PDF
                //if (visibility == 0.0 && light_record.primitive && !light_record.primitive->m_light_interface.expired()
                //    && light_record.primitive->m_light_interface.lock().get() != sampled_light.first) {
                //    light_source->sampleLightSource(light_pdf, distance_to_light, vertex_to_light_direction, record, light_record.primitive);
                //    visibility = 1.0;
                //    sampled_light.first = light_record.primitive->m_light_interface.lock().get();
                //    sampled_light.second = light_record.primitive;
                //}

                // ndotI factor
                bsdf = Color(record.material->getBSDF(vertex_to_prev_direction, vertex_to_light_direction, record, is_transmission, true));
                ndotL = glm::abs(glm::dot(record.normal, vertex_to_light_direction));
                light_sampling_mis = light_color * bsdf * ndotL * visibility / light_pdf;
                if (!light_source->isPoint() && bsdf.isNot(Color::Black())) { // apply MIS for non-dirac distributions
                    bsdf_pdf = record.material->getMaterialPDF(vertex_to_prev_direction, record.normal, vertex_to_light_direction, is_transmission, true);
                    light_sampling_weight = MISWeightPowerHeuristic(light_pdf, bsdf_pdf);
                    light_sampling_mis *= light_sampling_weight;
                }
            }
            CRAY_DEBUG_PIXEL("(%.2f, %.2f) (Depth: %u) Ld_L: %f, %f, %f, Li: %f, %f, %f, BSDF: %f, %f, %f, ndotL: %f, visibility: %f, distance_ints: %f, "
                "distance_L: %f, Light PDF: %f, BSDF PDF: %f, MIS Weight: %f", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, current_depth,
                light_sampling_mis.x, light_sampling_mis.y, light_sampling_mis.z,
                light_color.x, light_color.y, light_color.z,
                bsdf.x, bsdf.y, bsdf.z, ndotL, visibility,
                light_record.distance == std::numeric_limits<cr_float>::max() ? 100000.0 : light_record.distance,
                distance_to_light == std::numeric_limits<cr_float>::max() ? 100000.0 : distance_to_light, light_pdf, bsdf_pdf, light_sampling_weight);

            return light_sampling_mis;
        }

        Color BSDFSampleWeight(const Ray& light_ray, const glm::vec3& normal, const HitRecord& record, int32_t current_depth,
            const Color& throughput, const cr_float bsdf_pdf, const glm::vec3& vertex_to_next_direction,
            bool sampled_specular, const std::pair<const LightInterface*, const PrimitiveInterface*>& sampled_light) const {
#ifdef ENABLE_DEBUG_CURRENT_PIXEL
            if (isValidDebugPixel()) {
                CRAY_DEBUG_PIXEL("(%.2f, %.2f, %d) ", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, current_depth)
            }
#endif // ENABLE_DEBUG_CURRENT_PIXEL
            // BSDF Sampling -> select a bsdf direction and weight the contribution via MIS
            Color bsdf_sampling_mis = Color::Black();
            cr_float light_pdf(0.0);
            cr_float visibility(0.0);
            Color light_color(Color::Black());
            cr_float bsdf_sampling_weight(0.0);
            if (bsdf_pdf > 0.0 && throughput.isNot(Color::Black())) {
                // apply MIS for BSDF sampling
                glm::vec3 light_normal(0.0);
                const LightInterface* light_source = sampled_light.first;
                const PrimitiveInterface* light_primitive = sampled_light.second;
                // retrieve the PDF of the light source in the new direction (if the ray hits the light source)
                light_pdf = light_source->getLightPDF(light_normal, light_ray, normal, light_primitive);
                if (light_pdf > 0.0 && record.primitive == light_primitive) {
                    visibility = 1.0;
                    light_color = light_source->evaluateLemissive(light_normal, vertex_to_next_direction, light_primitive);
                    bsdf_sampling_mis = light_color * throughput;
                }
                if (!light_source->isPoint() && !sampled_specular && light_pdf > 0.0) { // apply MIS for non-dirac distributions
                    bsdf_sampling_weight = MISWeightPowerHeuristic(bsdf_pdf, light_pdf);
                    bsdf_sampling_mis *= bsdf_sampling_weight;
                }
            }
            CRAY_DEBUG_PIXEL("(%.2f, %.2f) (Depth: %u) L_B: %f, %f, %f, Li: %f, %f, %f, visibility: %f, distance: %f, Light PDF: %f, BSDF PDF: %f, MIS Weight: %f", s_thread_settings->getCurrentPixel().x, s_thread_settings->getCurrentPixel().y, current_depth,
                bsdf_sampling_mis.x, bsdf_sampling_mis.y, bsdf_sampling_mis.z,
                light_color.x, light_color.y, light_color.z, visibility,
                record.distance == std::numeric_limits<cr_float>::max() ? 100000.0 : record.distance, light_pdf, bsdf_pdf, bsdf_sampling_weight);
            return bsdf_sampling_mis;
        }
    };

#ifdef CRAY_ENABLE_TESTS
    struct IntegratorTestColor : public IntegratorInterface {
        int m_num_channels = 3;
        /*
        void renderBlock(ThreadBlock& block) const override {
            block.start =glm::ivec2(0);
            block.end = glm::ivec2(s_global_engine->m_block_size);
            glm::ivec2 m_dimensions = block.end - block.start;
            std::unique_ptr<uint8_t[]> m_data_ptr = std::make_unique<uint8_t[]>(m_dimensions.x * m_dimensions.y * m_num_channels);
            std::unique_ptr<CR_float[]> m_float_data_ptr = std::make_unique<CR_float[]>(m_dimensions.x * m_dimensions.y * m_num_channels);

            Ray ray(glm::vec3(0), glm::vec3(0));
            for (uint32_t row = block.start.y; row < block.end.y && !s_global_engine->m_stop_requested; row++) {        // height
                for (uint32_t col = block.start.x; col < block.end.x && !s_global_engine->m_stop_requested; col++) {    // width
                    ray.m_direction = glm::vec3(col / static_cast<CR_float>(s_global_image->m_dimensions.x - 1), row / static_cast<CR_float>(s_global_image->m_dimensions.y - 1), 0.2);
                    Color color = evaluateRayRadiance(ray);

                    size_t position = (col * m_dimensions.x * m_num_channels) + (row * m_num_channels);
                    glm::vec3 gamma_corrected = glm::pow(glm::clamp(color.data, glm::vec3(0.0), glm::vec3(1.0)), glm::vec3(s_thread_settings->m_inverse_gamma));
                    for (size_t i = 0; i < m_num_channels; ++i) {
                        m_float_data_ptr[position + i] = gamma_corrected[i];
                        m_data_ptr[position + i] = Color::toUnsignedByte(gamma_corrected[i]);
                    }

                }
            }
        }
    */
        void renderBlock(ThreadBlock& block) const override {
            Ray ray(glm::vec3(0), glm::vec3(0));
            for (int32_t row = block.start.y; row < block.end.y && !s_global_engine->m_stop_requested; row++) {        // height
                for (int32_t col = block.start.x; col < block.end.x && !s_global_engine->m_stop_requested; col++) {    // width
                    ray.m_direction = glm::vec3(col / static_cast<cr_float>(s_global_framebuffer->m_dimensions.x - 1.0), row / static_cast<cr_float>(s_global_framebuffer->m_dimensions.y - 1.0), 0.2);
                    Color color = evaluateRayRadiance(ray);
                    s_global_framebuffer->WriteColor(color, col, row);
                }
            }
        }

        Color evaluateRayRadiance(Ray& r) const {
            return Color(r.m_direction);
        }
    };

    struct IntegratorTestCamera : public IntegratorInterface {

        void renderBlock(ThreadBlock& block) const override {

            Ray ray(glm::vec3(0), glm::vec3(0));
            for (int32_t row = block.start.y; row < block.end.y && !s_global_engine->m_stop_requested; row++) {        // height
                for (int32_t col = block.start.x; col < block.end.x && !s_global_engine->m_stop_requested; col++) {    // width
                    Ray camera_ray = s_global_scene->m_camera->generateRay(col, row, true);
                    Color color = evaluateRayRadiance(camera_ray);
                    s_global_framebuffer->WriteColor(color, col, row);
                }
            }
        }

        Color evaluateRayRadiance(Ray& r) const {
            Color c = Color(r.m_direction * cr_float(0.5) + cr_float(0.5));
            return c;
        }
    };

    struct IntegratorTestNoise : public IntegratorInterface {

        void renderBlock(ThreadBlock& block) const override {

            Ray ray(glm::vec3(0), glm::vec3(0));
            for (int32_t row = block.start.y; row < block.end.y && !s_global_engine->m_stop_requested; row++) {        // height
                for (int32_t col = block.start.x; col < block.end.x && !s_global_engine->m_stop_requested; col++) {    // width
                    Color color;
                    for (int32_t sample = 0; sample < s_thread_settings->m_samples_per_pixel; ++sample) {
                        // generate numbers
                        cr_float s = rand_f();
                        Ray noise_ray(glm::vec3(s, s, s), glm::vec3(0));
                        color += evaluateRayRadiance(noise_ray);
                    }
                    color.data /= cr_float(s_thread_settings->m_samples_per_pixel);

                    s_global_framebuffer->WriteColor(color, col, row);
                }
            }
        }

        Color evaluateRayRadiance(Ray& r) const {
            Color c = Color(r.m_origin);
            return c;
        }
    };

    struct IntegratorTestUV : public IntegratorInterface {

        void renderBlock(ThreadBlock& block) const override {

            Ray ray(glm::vec3(0), glm::vec3(0));
            for (int32_t row = block.start.y; row < block.end.y && !s_global_engine->m_stop_requested; row++) {        // height
                for (int32_t col = block.start.x; col < block.end.x && !s_global_engine->m_stop_requested; col++) {    // width
                    Ray camera_ray = s_global_scene->m_camera->generateRay(col, row, true);
                    Color color = evaluateRayRadiance(camera_ray);
                    s_global_framebuffer->WriteColor(color, col, row);
                }
            }
        }

        Color evaluateRayRadiance(Ray& r) const {
            // world intersect
            HitRecord record;
            cr_float current_min = 0.0;
            cr_float current_max = std::numeric_limits<cr_float>::max(); // TODO:: assumes infinite camera
            if (s_global_scene->intersect(r, current_min, current_max, record)) {
                return Color(record.uv.x, record.uv.y, 0.0);
            }
            else
                return Color::Black();
        }
    };

    struct IntegratorTestNormal : public IntegratorInterface {

        void renderBlock(ThreadBlock& block) const override {

            Ray ray(glm::vec3(0), glm::vec3(0));
            for (int32_t row = block.start.y; row < block.end.y && !s_global_engine->m_stop_requested; row++) {        // height
                for (int32_t col = block.start.x; col < block.end.x && !s_global_engine->m_stop_requested; col++) {    // width
                    Ray camera_ray = s_global_scene->m_camera->generateRay(col, row, true);
                    Color color = evaluateRayRadiance(camera_ray);
                    s_global_framebuffer->WriteColor(color, col, row);
                }
            }
        }

        Color evaluateRayRadiance(Ray& r) const {
            // world intersect
            HitRecord record;
            cr_float current_min = 0.0;
            cr_float current_max = std::numeric_limits<cr_float>::max(); // TODO:: assumes infinite camera
            if (s_global_scene->intersect(r, current_min, current_max, record)) {
                return Color(record.normal * cr_float(0.5) + cr_float(0.5));
            }
            else
                return Color::Black();
        }
    };

    struct IntegratorTestMaterial : public IntegratorInterface {

        void renderBlock(ThreadBlock& block) const override {

            Ray ray(glm::vec3(0), glm::vec3(0));
            for (int32_t row = block.start.y; row < block.end.y && !s_global_engine->m_stop_requested; row++) {        // height
                for (int32_t col = block.start.x; col < block.end.x && !s_global_engine->m_stop_requested; col++) {    // width
                    Ray camera_ray = s_global_scene->m_camera->generateRay(col, row, true);
                    Color color = evaluateRayRadiance(camera_ray);
                    s_global_framebuffer->WriteColor(color, col, row);
                }
            }
        }

        Color evaluateRayRadiance(Ray& r) const {
            // world intersect
            HitRecord record;
            cr_float current_min = 0.0;
            cr_float current_max = std::numeric_limits<cr_float>::max(); // TODO:: assumes infinite camera
            if (s_global_scene->intersect(r, current_min, current_max, record)) {
                return Color(record.material->getBSDF(-r.m_direction, -r.m_direction, record, false, false));
            }
            else
                return Color::Black();
        }
    };
#endif // CRAY_ENABLE_TESTS

    // logging/reporting
    std::string getCurrentTime() {
        std::stringstream m_time_sstr;
        std::time_t now = std::time(nullptr);
        std::tm* tm = std::localtime(&now);
        m_time_sstr << std::setfill('0') << std::setw(2) << tm->tm_mday << "/";
        m_time_sstr << std::setfill('0') << std::setw(2) << tm->tm_mon << "/";
        m_time_sstr << tm->tm_year + 1900 << " ";
        m_time_sstr << std::setfill('0') << std::setw(2) << tm->tm_hour << ":";
        m_time_sstr << std::setfill('0') << std::setw(2) << tm->tm_min << ":";
        m_time_sstr << std::setfill('0') << std::setw(2) << tm->tm_sec << " ";
        return std::string(m_time_sstr.str());
    }

    static std::mutex logger_message_mutex;
    void flushLastMessage() {
        std::lock_guard<std::mutex> lock(logger_message_mutex);
        if (s_global_engine->m_logger_queue.empty()) {
            return;
        }
        LoggerMessage& entry = s_global_engine->m_logger_queue.back();
#if defined (_WIN32) && defined (_MSC_VER)
        if (IsDebuggerPresent()) {
            OutputDebugStringA(LPCSTR(entry.message.c_str()));
        }
#endif
        printf("%s", entry.message.c_str());
        fflush(stdout);
        s_global_engine->m_logger_queue.pop_back();
    }

    bool isLogLevel(enum CRAY_LOGGERENTRY type) {
        return s_global_settings->m_minimum_log_level >= type;
    }

    void pushMessage(CRAY_LOGGERENTRY type, const char* fmt, ...) {
        std::lock_guard<std::mutex> lock(logger_message_mutex);
        va_list args;
        va_start(args, fmt);
#ifdef _WIN32
        int len = _vscprintf(fmt, args) + 1;
#else
        va_list argcopy;
        va_copy(argcopy, args);
        int len = vsnprintf(nullptr, 0, fmt, argcopy) + 1;
        va_end(argcopy);
#endif
        if (len <= 1) { va_end(args); return; }
        std::unique_ptr<char[]> text = std::make_unique<char[]>(len);
        vsnprintf(text.get(), len, fmt, args);

        if (s_global_settings->m_file == nullptr) {
            s_global_engine->m_logger_queue.emplace_back(type, text.get());
        }
        else {
            fputs(text.get(), s_global_settings->m_file);
        }
        if (s_global_settings->m_print_to_stdout) {
            fputs(text.get(), stdout);
            // fflush(stdout);
        }
    }

#ifdef ENABLE_DEBUG_CURRENT_PIXEL
    bool isValidDebugPixel() {
        if (!s_thread_settings->isPixel() || s_thread_settings->m_minimum_log_level < CR_LOGGER_DEBUG) {
            return false;
        }
        return true;
    }
#endif

} // namespace RAYENGINE

using namespace RAYENGINE;

void createDefaultScene() {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    if (s_global_scene == nullptr) {
        s_global_scene = std::make_unique<Scene>();
        auto as = std::make_unique<AccelerationStructureBVH>();
        s_global_scene->setAccelerationStructure(std::move(as));
        auto camera = std::make_unique<CameraPerspective>(glm::vec3(0.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0), 45.0, 0.1, 1000.0, 0.0, 0.0);
        s_global_scene->setCamera(std::move(camera));
    }
}

void CR_setAccelerationStructure(CRAY_ACCELERATION_STRUCTURE acceleration_structure_type) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    createDefaultScene();
    switch (acceleration_structure_type) {
    case CR_AS_ARRAY: s_global_scene->setAccelerationStructure(std::make_unique<AccelerationStructureArray>()); break;
    case CR_AS_BVH: s_global_scene->setAccelerationStructure(std::make_unique<AccelerationStructureBVH>()); break;
    default: { CRAY_WARNING("Invalid CRAY_ACCELERATION_STRUCTURE type %d. Skipping"); } break;
    }
}

cr_float CR_getDielectricIOR(CRAY_IOR_DIELECTRICS dielectric) { return dielectrics_ior.find(dielectric) != dielectrics_ior.end() ? dielectrics_ior.at(dielectric) : dielectrics_ior.at(CR_DIELECTRIC_Air); }
cr_vec3 CR_getConductorIOR(CRAY_IOR_CONDUCTORS conductor) { return conductors_ior.find(conductor) != conductors_ior.end() ? conductors_ior.at(conductor) : conductors_ior.at(CR_CONDUCTOR_Alluminium_Al); }
cr_vec3 CR_getConductorAbsorption(CRAY_IOR_CONDUCTORS conductor) { return conductors_k.find(conductor) != conductors_k.end() ? conductors_k.at(conductor) : conductors_k.at(CR_CONDUCTOR_Alluminium_Al); }

CRAY_ACCELERATION_STRUCTURE  CR_getAccelerationStructure() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CR_AS_NONE)
    return s_global_scene->m_acceleration_structure ? s_global_scene->m_acceleration_structure->m_type : CR_AS_NONE;
}

CRAY_HANDLE CR_addMaterialDiffuseReflection(cr_vec3 diffuse_reflection_color) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    return s_global_scene->addMaterial(std::make_shared<MaterialDiffuse>(Color(diffuse_reflection_color), true, Color::Black(), false, 0.0));
}

CRAY_HANDLE CR_addMaterialDiffuseTransmission(cr_vec3 diffuse_transmission_color) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    return s_global_scene->addMaterial(std::make_shared<MaterialDiffuse>(Color::Black(), false, Color(diffuse_transmission_color), true, 0.0));
}

CRAY_HANDLE CR_addMaterialDiffuse(cr_vec3 diffuse_reflection_color, cr_vec3 diffuse_transmission_color, cr_float eta_t) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    return s_global_scene->addMaterial(std::make_shared<MaterialDiffuse>(Color(diffuse_reflection_color), true, Color(diffuse_transmission_color), true, eta_t));
}

CRAY_HANDLE CR_addImageConstant(cr_float r, cr_float g, cr_float b) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    return s_global_scene->addImage(std::make_shared<ImageConstant>(Color(r, g, b)));
}

CRAY_HANDLE CR_addImage(const uint8_t* ptr, int32_t width, int32_t height, int32_t channels, int32_t size_type, bool is_srgb) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    if (ptr == nullptr) {
        CRAY_WARNING("Empty image pointer. Skipping");
        return CRAY_INVALID_HANDLE;
    }
    createDefaultScene();
    return s_global_scene->addImage(std::make_shared<ImageFile>(ptr, width, height, channels, size_type, is_srgb));

}

CRAY_HANDLE CR_addImageCheckerboard(cr_vec3 tex1, cr_vec3 tex2, cr_float frequency) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    return s_global_scene->addImage(std::make_shared<ImageChecker>(std::make_shared<ImageConstant>(Color(tex1)), std::make_shared<ImageConstant>(Color(tex2)), frequency));
}

CRAY_HANDLE CR_addTextureSampler(CRAY_TEXTURE_MAPPING mapping_mode, CRAY_TEXTURE_WRAPPING wrap_mode, CRAY_TEXTURE_FILTERING filtering_mode) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    return s_global_scene->addTextureSampler(std::make_shared<TextureSampler>(mapping_mode, wrap_mode, filtering_mode));
}


CRAY_HANDLE CR_addTexture(CRAY_HANDLE image_id, CRAY_HANDLE sampler_id) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    auto image = s_global_scene->getImage(image_id);
    if (image == nullptr) {
        CRAY_WARNING("Image %d does not exist. Skipping");
        return CRAY_INVALID_HANDLE;
    }
    auto sampler = s_global_scene->getTextureSampler(sampler_id);
    if (sampler == nullptr) {
        CRAY_WARNING("Sampler %d does not exist. Skipping");
        return CRAY_INVALID_HANDLE;
    }
    return s_global_scene->addTexture(std::make_shared<TextureInterface>(image, sampler));
}

void CR_addMaterialDiffuseReflectionTexture(CRAY_HANDLE material_id, CRAY_HANDLE texture_id) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    createDefaultScene();
    auto material = s_global_scene->getMaterial(material_id);
    if (material == nullptr) {
        CRAY_WARNING("Material %d does not exist. Skipping");
        return;
    }
    auto texture = s_global_scene->getTexture(texture_id);
    if (texture == nullptr) {
        CRAY_WARNING("Texture %d does not exist. Skipping");
        return;
    }
    if (material->hasType(MT_DIFFUSE)) {
        material->setDiffuseReflectionTexture(texture);
    }
    else {
        CRAY_WARNING("Texture %d does not have a diffuse component. Skipping");
        return;
    }
}

void CR_addMaterialDiffuseTransmissionTexture(CRAY_HANDLE material_id, CRAY_HANDLE texture_id) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    createDefaultScene();
    auto material = s_global_scene->getMaterial(material_id);
    if (material == nullptr) {
        CRAY_WARNING("Material %d does not exist. Skipping");
        return;
    }
    auto texture = s_global_scene->getTexture(texture_id);
    if (texture == nullptr) {
        CRAY_WARNING("Texture %d does not exist. Skipping");
        return;
    }
    if (material->hasType(MT_DIFFUSE)) {
        material->setDiffuseTransmissionTexture(texture);
    }
    else {
        CRAY_WARNING("Texture %d does not have a diffuse component. Skipping");
        return;
    }
}

void CR_addMaterialSpecularReflectionTexture(CRAY_HANDLE material_id, CRAY_HANDLE texture_id) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    createDefaultScene();
    auto material = s_global_scene->getMaterial(material_id);
    if (material == nullptr) {
        CRAY_WARNING("Material %d does not exist. Skipping");
        return;
    }
    auto texture = s_global_scene->getTexture(texture_id);
    if (texture == nullptr) {
        CRAY_WARNING("Texture %d does not exist. Skipping");
        return;
    }
    if (material->hasType(MT_SPECULAR_ROUGH) || material->hasType(MT_SPECULAR_SMOOTH)) {
        material->setSpecularReflectionTexture(texture);
    }
    else {
        CRAY_WARNING("Texture %d does not have a specular component. Skipping");
        return;
    }
}

void CR_addMaterialSpecularTransmissionTexture(CRAY_HANDLE material_id, CRAY_HANDLE texture_id) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    createDefaultScene();
    auto material = s_global_scene->getMaterial(material_id);
    if (material == nullptr) {
        CRAY_WARNING("Material %d does not exist. Skipping");
        return;
    }
    auto texture = s_global_scene->getTexture(texture_id);
    if (texture == nullptr) {
        CRAY_WARNING("Texture %d does not exist. Skipping");
        return;
    }
    if (material->hasType(MT_SPECULAR_ROUGH) || material->hasType(MT_SPECULAR_SMOOTH)) {
        material->setSpecularTransmissionTexture(texture);
    }
    else {
        CRAY_WARNING("Texture %d does not have a specular component. Skipping");
        return;
    }
}

CRAY_HANDLE CR_addMaterialDiffuseSpecularReflection(cr_vec3 diffuse_reflection_color, cr_vec3 specular_reflection_color, cr_float eta_t, cr_float roughness) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    std::shared_ptr<MaterialDiffuse> diffuse = std::make_shared<MaterialDiffuse>(Color(diffuse_reflection_color), true, Color::Black(), false, eta_t);
    std::shared_ptr<MaterialMicrofacetSpecular> specular = std::make_shared<MaterialMicrofacetSpecular>(Color(specular_reflection_color), true,
        Color::Black(), false, eta_t, roughness);
    return s_global_scene->addMaterial(std::make_shared<MaterialGeneric>(diffuse, specular));
}

CRAY_HANDLE CR_addMaterialConductor(cr_vec3 reflection_color, cr_vec3 absorption, cr_vec3 eta_t, cr_float roughness) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    return s_global_scene->addMaterial(std::make_shared<MaterialMicrofacetSpecular>(Color(reflection_color), Color(absorption), glm::vec3(eta_t.x, eta_t.y, eta_t.z), roughness));
}

CRAY_HANDLE CR_addMaterialDielectricSpecularReflection(cr_vec3 reflection_color, cr_float roughness) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    return s_global_scene->addMaterial(std::make_shared<MaterialMicrofacetSpecular>(Color(reflection_color), true, Color(Color::Black()), false, 0.0, roughness));
}

CRAY_HANDLE CR_addMaterialDielectricSpecularTransmission(cr_vec3 transmission_color, cr_float eta_t, cr_float roughness) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    return s_global_scene->addMaterial(std::make_shared<MaterialMicrofacetSpecular>(Color::Black(), false, Color(transmission_color), true, eta_t, roughness));
}

void CR_pushMatrix(cr_mat4 matrix) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    glm::mat4x4 glm_mat(matrix.x0, matrix.y0, matrix.z0, matrix.w0, matrix.x1, matrix.y1, matrix.z1, matrix.w1, matrix.x2, matrix.y2, matrix.z2, matrix.w2, matrix.x3, matrix.y3, matrix.z3, matrix.w3);
    s_global_scene->m_matrix_stack.push_back(glm_mat);
    s_global_scene->m_current_world_matrix = s_global_scene->m_current_world_matrix * glm_mat;
}

void CR_popMatrix() {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    s_global_scene->m_current_world_matrix = glm::mat4(1.0);
    if (s_global_scene->m_matrix_stack.empty()) {
        return;
    }
    s_global_scene->m_matrix_stack.pop_back();
    // recalculate the world matrix (instead of using inverses)
    for (auto& mat : s_global_scene->m_matrix_stack) {
        s_global_scene->m_current_world_matrix = s_global_scene->m_current_world_matrix * mat;
    }
}

cr_mat4 CR_rotate(cr_float degrees, cr_vec3 axis) {
    glm::mat4x4 glm_mat = glm::rotate(glm::radians(degrees), glm::vec3(axis.x, axis.y, axis.z));
    return cr_mat4{ glm_mat[0][0], glm_mat[0][1], glm_mat[0][2], glm_mat[0][3], glm_mat[1][0], glm_mat[1][1], glm_mat[1][2], glm_mat[1][3], glm_mat[2][0], glm_mat[2][1], glm_mat[2][2], glm_mat[2][3], glm_mat[3][0], glm_mat[3][1], glm_mat[3][2], glm_mat[3][3] };
}

cr_mat4 CR_translate(cr_vec3 translate) {
    glm::mat4x4 glm_mat = glm::translate(glm::vec3(translate.x, translate.y, translate.z));
    return cr_mat4{ glm_mat[0][0], glm_mat[0][1], glm_mat[0][2], glm_mat[0][3], glm_mat[1][0], glm_mat[1][1], glm_mat[1][2], glm_mat[1][3], glm_mat[2][0], glm_mat[2][1], glm_mat[2][2], glm_mat[2][3], glm_mat[3][0], glm_mat[3][1], glm_mat[3][2], glm_mat[3][3] };
}

cr_mat4 CR_scale(cr_vec3 scale) {
    glm::mat4x4 glm_mat = glm::scale(glm::vec3(scale.x, scale.y, scale.z));
    return cr_mat4{ glm_mat[0][0], glm_mat[0][1], glm_mat[0][2], glm_mat[0][3], glm_mat[1][0], glm_mat[1][1], glm_mat[1][2], glm_mat[1][3], glm_mat[2][0], glm_mat[2][1], glm_mat[2][2], glm_mat[2][3], glm_mat[3][0], glm_mat[3][1], glm_mat[3][2], glm_mat[3][3] };
}

cr_mat4 CR_mulMatrix(cr_mat4 matrix1, cr_mat4 matrix2) {
    glm::mat4x4 glm_mat1(matrix1.x0, matrix1.y0, matrix1.z0, matrix1.w0, matrix1.x1, matrix1.y1, matrix1.z1, matrix1.w1, matrix1.x2, matrix1.y2, matrix1.z2, matrix1.w2, matrix1.x3, matrix1.y3, matrix1.z3, matrix1.w3);
    glm::mat4x4 glm_mat2(matrix2.x0, matrix2.y0, matrix2.z0, matrix2.w0, matrix2.x1, matrix2.y1, matrix2.z1, matrix2.w1, matrix2.x2, matrix2.y2, matrix2.z2, matrix2.w2, matrix2.x3, matrix2.y3, matrix2.z3, matrix2.w3);
    glm::mat4x4 glm_mat = glm_mat1 * glm_mat2;
    return cr_mat4{ glm_mat[0][0], glm_mat[0][1], glm_mat[0][2], glm_mat[0][3], glm_mat[1][0], glm_mat[1][1], glm_mat[1][2], glm_mat[1][3],
        glm_mat[2][0], glm_mat[2][1], glm_mat[2][2], glm_mat[2][3], glm_mat[3][0], glm_mat[3][1], glm_mat[3][2], glm_mat[3][3] };
}

CRAY_HANDLE CR_addMaterialDielectricSpecular(cr_vec3 reflection_color, cr_vec3 transmission_color, cr_float roughness, cr_float ior) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    return s_global_scene->addMaterial(std::make_shared<MaterialMicrofacetSpecular>(Color(reflection_color), true, Color(transmission_color), true, roughness, ior));
}

void CR_addCameraThinLens(cr_vec3 position, cr_vec3 target, cr_vec3 up, cr_float aperture, cr_float far, cr_float lens_radius, cr_float focal_distance) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    createDefaultScene();
    auto camera = std::make_unique<CameraPerspective>(glm::vec3(position.x, position.y, position.z), glm::vec3(target.x, target.y, target.z), glm::vec3(up.x, up.y, up.z), aperture, 0.0, far, lens_radius, focal_distance);
    s_global_scene->setCamera(std::move(camera));
}

void CR_addCameraPerspective(cr_vec3 position, cr_vec3 target, cr_vec3 up, cr_float aperture, cr_float near, cr_float far) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    createDefaultScene();
    auto camera = std::make_unique<CameraPerspective>(glm::vec3(position.x, position.y, position.z), glm::vec3(target.x, target.y, target.z), glm::vec3(up.x, up.y, up.z), aperture, near, far, 0.0, 0.0);
    s_global_scene->setCamera(std::move(camera));
}

void CR_addCameraOrthographic(cr_vec3 position, cr_vec3 target, cr_vec3 up, cr_float height, cr_float near, cr_float far) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    createDefaultScene();
    auto camera = std::make_unique<CameraOrthographic>(glm::vec3(position.x, position.y, position.z), glm::vec3(target.x, target.y, target.z), glm::vec3(up.x, up.y, up.z), height, near, far);
    s_global_scene->setCamera(std::move(camera));
}

void CR_addLightPointSpot(cr_vec3 position, cr_vec3 target, cr_vec3 intensity, cr_float falloffAngle, cr_float maxAngle, cr_float exponent) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    createDefaultScene();
    auto light = std::make_unique<SpotLight>(glm::vec3(position.x, position.y, position.z), glm::vec3(target.x, target.y, target.z), Color(intensity.x, intensity.y, intensity.z), falloffAngle, maxAngle, exponent);
    s_global_scene->addLight(std::move(light));
}

void CR_addLightPointOmni(cr_vec3 position, cr_vec3 intensity) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    createDefaultScene();
    auto light = std::make_unique<SpotLight>(glm::vec3(position.x, position.y, position.z), glm::vec3(0.0), Color(intensity.x, intensity.y, intensity.z), 0.0, 0.0, 0.0);
    if (glm::all(glm::lessThan(glm::vec3(intensity.x, intensity.y, intensity.z), glm::vec3(FLOAT_TOLERANCE_3)))) {
        CRAY_WARNING("Near zero flux %f %f %f. Skipping.", intensity.x, intensity.y, intensity.z);
        return;
    }
    s_global_scene->addLight(std::move(light));
}

void CR_addLightDirectional(cr_vec3 direction, cr_vec3 intensity) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    createDefaultScene();
    if (glm::all(glm::lessThan(glm::vec3(intensity.x, intensity.y, intensity.z), glm::vec3(FLOAT_TOLERANCE_3)))) {
        CRAY_WARNING("Near zero flux %f %f %f. Skipping.", intensity.x, intensity.y, intensity.z);
        return;
    }
    s_global_scene->addLight(std::make_shared<DirectionalLight>(glm::vec3(direction.x, direction.y, direction.z), Color(intensity.x, intensity.y, intensity.z)));
}

void CR_addLightDirectionalFromTo(cr_vec3 position, cr_vec3 target, cr_vec3 intensity) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    createDefaultScene();
    if (glm::all(glm::lessThan(glm::vec3(intensity.x, intensity.y, intensity.z), glm::vec3(FLOAT_TOLERANCE_3)))) {
        CRAY_WARNING("Near zero flux %f %f %f. Skipping.", intensity.x, intensity.y, intensity.z);
        return;
    }
    s_global_scene->addLight(std::make_shared<DirectionalLight>(glm::vec3(glm::normalize(glm::vec3(target.x, target.y, target.z) - glm::vec3(position.x, position.y, position.z))), Color(intensity.x, intensity.y, intensity.z)));
}

CRAY_HANDLE CR_addLightAreaDiffuseEmitter(cr_vec3 flux, bool doublesided) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    if (glm::all(glm::lessThan(glm::vec3(flux.x, flux.y, flux.z), glm::vec3(FLOAT_TOLERANCE_3)))) {
        CRAY_WARNING("Near zero flux %f %f %f. Skipping.", flux.x, flux.y, flux.z);
        return CRAY_INVALID_HANDLE;
    }
    return s_global_scene->addLight(std::make_shared<AreaLight>(Color(flux.x, flux.y, flux.z), doublesided));
}

void CR_addAreaLightToPrimitive(CRAY_HANDLE primitive_id, CRAY_HANDLE light_id) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    createDefaultScene();
    auto prim = s_global_scene->getPrimitive(primitive_id);
    if (prim == nullptr) {
        CRAY_WARNING("Primitive %d does not exist. Skipping");
        return;
    }
    auto light = s_global_scene->getLight(light_id);
    AreaLight* area_light = dynamic_cast<AreaLight*>(light.get());
    if (area_light == nullptr) {
        CRAY_WARNING("Area light %d does not exist. Skipping");
        return;
    }
    prim->setLight(light);
    s_global_scene->m_light_elements.push_back(std::make_pair(light.get(), prim.get()));
    area_light->addSharedPrimitive(prim.get());
}

CRAY_HANDLE CR_addPrimitiveSphere(cr_vec3 position, cr_float radius, bool flip_normals, bool double_sided, CRAY_HANDLE material_id) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    return s_global_scene->addPrimitive(std::make_shared<PrimitiveSphere>(glm::vec3(position.x, position.y, position.z), radius, flip_normals, double_sided), material_id, sizeof(PrimitiveSphere));
}

CRAY_HANDLE CR_addPrimitiveRectangleXY(cr_float min_x, cr_float max_x, cr_float min_y, cr_float max_y, cr_float offset, bool flip_normals, bool double_sided, CRAY_HANDLE material_id) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    glm::vec3 p_min(min_x, min_y, offset);
    glm::vec3 p_max(max_x, max_y, offset);
    glm::vec3 scale = glm::vec3(2.0 / (max_x - min_x), 2.0 / (max_y - min_y), 1.0);
    glm::vec3 translate(-1.0 - scale.x * min_x, -1.0 - scale.y * min_y, 0.0);
    glm::mat4x4 rcs_to_ocs = glm::scale(scale) * glm::translate(translate) * glm::translate(glm::vec3(0.0, 0.0, -offset));
    return s_global_scene->addPrimitive(std::make_shared<PrimitiveRectangle>(p_min, p_max, rcs_to_ocs, flip_normals, double_sided), material_id, sizeof(PrimitiveRectangle));
};

CRAY_HANDLE CR_addPrimitiveRectangleXZ(cr_float min_x, cr_float max_x, cr_float min_z, cr_float max_z, cr_float offset, bool flip_normals, bool double_sided, CRAY_HANDLE material_id) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    glm::vec3 p_min(min_x, offset, min_z);
    glm::vec3 p_max(max_x, offset, max_z);
    glm::vec3 scale = glm::vec3(2.0 / (max_x - min_x), 1.0, 2.0 / (max_z - min_z));
    glm::vec3 translate(-1.0 - scale.x * min_x, 0.0, -1.0 - scale.z * min_z);
    glm::mat4x4 rcs_to_ocs = glm::rotate(glm::half_pi<cr_float>(), glm::vec3(-1.0, 0.0, 0.0)) * glm::scale(scale) * glm::translate(translate) * glm::translate(glm::vec3(0.0, -offset, 0.0));
    return s_global_scene->addPrimitive(std::make_shared<PrimitiveRectangle>(p_min, p_max, rcs_to_ocs, flip_normals, double_sided), material_id, sizeof(PrimitiveRectangle));
};

CRAY_HANDLE CR_addPrimitiveRectangleYZ(cr_float min_y, cr_float max_y, cr_float min_z, cr_float max_z, cr_float offset, bool flip_normals, bool double_sided, CRAY_HANDLE material_id) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    glm::vec3 p_min(offset, min_y, min_z);
    glm::vec3 p_max(offset, max_y, max_z);
    glm::vec3 scale = glm::vec3(1.0, 2.0 / (max_y - min_y), 2.0 / (max_z - min_z));
    glm::vec3 translate(0.0, -1.0 - scale.y * min_y, -1.0 - scale.z * min_z);
    glm::mat4x4 rcs_to_ocs = glm::rotate(glm::half_pi<cr_float>(), glm::vec3(0.0, 1.0, 0.0)) * glm::scale(scale) * glm::translate(translate) * glm::translate(glm::vec3(-offset, 0.0, 0.0));
    return s_global_scene->addPrimitive(std::make_shared<PrimitiveRectangle>(p_min, p_max, rcs_to_ocs, flip_normals, double_sided), material_id, sizeof(PrimitiveRectangle));
};

CRAY_HANDLE CR_addPrimitiveTriangle(bool flip_normals, bool double_sided, CRAY_HANDLE material_id) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CRAY_INVALID_HANDLE)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(CRAY_INVALID_HANDLE)
    createDefaultScene();
    return s_global_scene->addPrimitive(std::make_shared<PrimitiveTriangle>(flip_normals, double_sided), material_id, sizeof(PrimitiveTriangle));
}

void CR_addPrimitiveTriangleVertexPositions(CRAY_HANDLE triangle_id, int32_t index, cr_vec3 position) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    PrimitiveInterface* prim = s_global_scene->getPrimitive(triangle_id).get();
    PrimitiveTriangle* triangle = dynamic_cast<PrimitiveTriangle*>(prim);
    if (triangle == nullptr) {
        CRAY_WARNING("Triangle %d does not exist. Skipping");
        return;
    }
    triangle->addPosition(index, glm::vec3(position.x, position.y, position.z));
}

void CR_addPrimitiveTriangleVertexNormals(CRAY_HANDLE triangle_id, int32_t index, cr_vec3 normal) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    PrimitiveInterface* prim = s_global_scene->getPrimitive(triangle_id).get();
    PrimitiveTriangle* triangle = dynamic_cast<PrimitiveTriangle*>(prim);
    if (triangle == nullptr) {
        CRAY_WARNING("Triangle %d does not exist. Skipping");
        return;
    }
    triangle->addNormal(index, glm::vec3(normal.x, normal.y, normal.z));
}

void CR_addPrimitiveTriangleVertexTexcoords(CRAY_HANDLE triangle_id, int32_t index, cr_vec3 texcoord) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    PrimitiveInterface* prim = s_global_scene->getPrimitive(triangle_id).get();
    PrimitiveTriangle* triangle = dynamic_cast<PrimitiveTriangle*>(prim);
    if (triangle == nullptr) {
        CRAY_WARNING("Triangle %d does not exist. Skipping");
        return;
    }
    triangle->addTexCoord(index, glm::vec3(texcoord.x, texcoord.y, texcoord.z));
}

size_t CR_getNumMaterials() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(0u)
    return s_global_scene->m_materials.m_items.size();
}

size_t CR_getNumPrimitives() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(0u)
    return s_global_scene->m_acceleration_structure->getPrimitiveList().m_items.size();
}

void CR_reserveMaterials(size_t size) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    s_global_scene->m_materials.m_items.reserve(size);
}

void CR_reservePrimitives(size_t size) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    s_global_scene->m_acceleration_structure->getPrimitiveList().m_items.reserve(size);
}

void CR_setIntegrator(CRAY_INTEGRATOR integrator_type) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    switch (integrator_type) {
    case CR_INTEGRATOR_PT: s_global_integrator = std::make_unique<IntegratorPath>(); break;
    case CR_INTEGRATOR_AO: s_global_integrator = std::make_unique<IntegratorAO>(); break;
#ifdef CRAY_ENABLE_TESTS
    case CR_INTEGRATOR_TEST_COLOR: s_global_integrator = std::make_unique<IntegratorTestColor>(); break;
    case CR_INTEGRATOR_TEST_CAMERA: s_global_integrator = std::make_unique<IntegratorTestCamera>(); break;
    case CR_INTEGRATOR_TEST_NOISE: s_global_integrator = std::make_unique<IntegratorTestNoise>(); break;
    case CR_INTEGRATOR_TEST_UV: s_global_integrator = std::make_unique<IntegratorTestUV>(); break;
    case CR_INTEGRATOR_TEST_NORMAL: s_global_integrator = std::make_unique<IntegratorTestNormal>(); break;
    case CR_INTEGRATOR_TEST_MATERIAL: s_global_integrator = std::make_unique<IntegratorTestMaterial>(); break;
#endif
    default: { CRAY_WARNING("Invalid CRAY_INTEGRATOR type %d. Skipping"); } return;
    }
    s_global_integrator->m_type = integrator_type;
}

CRAY_INTEGRATOR CR_getIntegrator() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CR_INTEGRATOR_NONE)
    return s_global_integrator ? s_global_integrator->m_type : CR_INTEGRATOR_NONE;
}

// Main resource handling
void CR_init() {
    s_global_engine = std::make_unique<Engine>();
    s_global_scene = nullptr;
    s_global_integrator = nullptr;
    s_global_framebuffer = std::make_shared<Framebuffer>();
    s_thread_settings = std::make_unique<GlobalSettings>();

    s_global_engine->setInteractiveRender(false);
    CR_setIntegrator(CR_INTEGRATOR_PT);
    createDefaultScene();
    CRAY_INFO("Engine Initialized");
}

void CR_destroy() {
    // not called CR_init
    if (s_global_engine == nullptr) {
        return;
    }
    CRAY_INFO("Destroying Engine");
    // ensure that resources are released upon destroy
    s_global_engine.reset();
    s_global_scene.reset();
    s_global_integrator.reset();
    s_global_framebuffer.reset();
    s_thread_settings.reset();
}

float* CR_getFilmFloatDataPtr() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(nullptr)
    return s_global_framebuffer->getFloatDataPtr();
}

uint64_t CR_getFilmFloatByteSize() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(0u)
    return s_global_framebuffer->getRGBByteSize();
}

uint8_t* CR_getFilmRGBDataPtr() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(nullptr)
    return s_global_framebuffer->getRGBDataPtr();
}

uint64_t CR_getFilmRGBByteSize() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(0u)
    return s_global_framebuffer->getRGBByteSize();
}

void CR_enableRGBBuffer(bool enable) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    if (s_global_settings->m_rgb != enable) {
        s_global_settings->m_rgb = enable;
        s_global_framebuffer->requestRealloc();
    }
    *s_thread_settings.get() = *s_global_settings;
}

bool CR_isRGBBufferEnabled() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(false)
    return s_global_settings->m_rgb;
}

void CR_setOutputName(const char* name) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    s_global_settings->m_output_name = name;
    *s_thread_settings.get() = *s_global_settings;
}

const char* CR_getOutputName() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE("")
    return s_global_settings->m_output_name.c_str();
}

int32_t CR_getFilmWidth() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(0)
    return s_global_framebuffer->m_dimensions.x;
}

int32_t CR_getFilmHeight() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(0)
    return s_global_framebuffer->m_dimensions.y;
}

int32_t CR_getFilmChannels() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(0)
   return s_global_framebuffer->m_num_channels;
}

void CR_setFilmDimensions(int32_t width, int32_t height) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    s_global_framebuffer->setDimensions(width, height);
    *s_thread_settings.get() = *s_global_settings;
}

void CR_setNumThreads(int32_t num_threads) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    s_global_engine->m_requested_threads = glm::max(0, num_threads);
    *s_thread_settings.get() = *s_global_settings;
}

int32_t CR_getNumThreads() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(0)
    return s_global_engine->m_requested_threads;
}

void CR_useMainThread(bool use_main_thread) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    s_global_engine->m_use_main_thread = use_main_thread;
    *s_thread_settings.get() = *s_global_settings;
}

bool CR_usesMainThread() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(false)
    return s_global_engine->m_use_main_thread;
}

void CR_setBlockSize(int32_t block_size) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    s_global_engine->m_block_size = glm::max(16, block_size);
    *s_thread_settings.get() = *s_global_settings;
}

int32_t CR_getBlockSize() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(0)
    return s_global_engine->m_block_size;
}

void CR_setSamplesPerPixel(int32_t spp) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    s_global_settings->m_samples_per_pixel = glm::max(1, spp);
    *s_thread_settings.get() = *s_global_settings;
}

int32_t CR_getSamplesPerPixel() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(0)
    return s_global_settings->m_samples_per_pixel;
}

void CR_setAOSamplesPerPixel(int32_t spp) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    s_global_settings->m_ao_samples_per_pixel = glm::max(1, spp);
    *s_thread_settings.get() = *s_global_settings;
}

int32_t CR_getAOSamplesPerPixel() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(0)
    return s_global_settings->m_ao_samples_per_pixel;
}

void CR_setAORange(cr_float range) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    s_global_settings->m_ao_range = glm::max(cr_float(0.0), range);
    *s_thread_settings.get() = *s_global_settings;
}

cr_float CR_getAORange() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(cr_float(0.0))
    return s_global_settings->m_ao_range;
}

void CR_setRayDepth(int32_t depth) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    s_global_settings->m_max_depth = glm::max(0, depth);
    *s_thread_settings.get() = *s_global_settings;
}

int32_t CR_getRayDepth() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(0)
    return s_global_settings->m_max_depth;
}

void CR_setRussianRoulette(bool enable) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    s_global_settings->m_russian_roulette = enable;
    *s_thread_settings.get() = *s_global_settings;
}

bool CR_getRussianRoulette() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(false)
    return s_global_settings->m_russian_roulette;
}

void CR_setGamma(cr_float gamma) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    s_global_settings->m_gamma = glm::max(cr_float(0.1), gamma);
    s_global_settings->m_inverse_gamma = 1.0 / s_global_settings->m_gamma;
    *s_thread_settings.get() = *s_global_settings;
}

cr_float CR_getGamma() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(cr_float(0.0))
    return s_global_settings->m_gamma;
}

void CR_setExposure(cr_float exposure) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    s_global_settings->m_exposure = glm::max(cr_float(0.0), exposure);
    *s_thread_settings.get() = *s_global_settings;
}

cr_float CR_getExposure() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(cr_float(0.0))
    return s_global_settings->m_exposure;
}

void CR_start() {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    CHECK_RETURN_IF_UNINITIALIZED_SCENE()
    CRAY_INFO("Starting rendering");
    s_global_engine->start();
}

void CR_stop() {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_NOT_STARTED_ENGINE()
    s_global_engine->m_stop_requested = true;
}

void CR_clearScene() {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    CHECK_RETURN_IF_UNINITIALIZED_SCENE()
    s_global_scene->clear();
    *s_thread_settings.get() = *s_global_settings;
}

bool CR_started() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(false)
    return s_global_engine->m_started;
}

bool CR_finished() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(false)
        bool done = s_global_engine->m_image_ready || !s_global_engine->m_started;
    if (!done) {
        return false;
    }
    s_global_engine->endThreads();
    *s_thread_settings.get() = *s_global_settings;
    return true;
}

cr_float CR_getProgressPercentage() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(0.0)
    if (s_global_engine->m_image_ready) {
        return 100.0;
    }
    if (!s_global_engine->m_started) {
        return 0;
    }
    return glm::min(100.0, 100.0 * s_global_engine->m_current_block_index / cr_float(s_global_engine->m_total_blocks));
}

void CR_printProgressBar(bool enable) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    s_global_settings->m_print_progress_bar_stdout = enable;
    *s_thread_settings.get() = *s_global_settings;
}

bool CR_IsPrintProgressBarEnabled() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(false)
    return s_global_settings->m_print_progress_bar_stdout;
}

void CR_printTostdout(bool enable) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    s_global_settings->m_print_to_stdout = enable;
    *s_thread_settings.get() = *s_global_settings;
}

bool CR_isPrintTostdoutEnabled() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(false)
    return s_global_settings->m_print_to_stdout;
}

void CR_setLogFile(void* file) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    s_global_settings->m_file = static_cast<FILE*>(file);
    *s_thread_settings.get() = *s_global_settings;
}

void* CR_getLogFile() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(nullptr)
    return s_global_settings->m_file;
}

bool CR_getLastLogMessage(char* msg, size_t* msg_length, CRAY_LOGGERENTRY* type) {
    std::lock_guard<std::mutex> lock(logger_message_mutex);
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(true)
    if (s_global_engine->m_logger_queue.empty()) {
        return false;
    }
    LoggerMessage& entry = s_global_engine->m_logger_queue.front();
    if (entry.message.size() + 1 > * msg_length) {
        *msg_length = entry.message.size();
        return true;
    }
    entry.message.copy(msg, entry.message.size());
    msg[entry.message.size()] = '\0';
    *type = entry.type;
    s_global_engine->m_logger_queue.pop_front();
    return true;
}

void CR_setMinimumLogLevel(CRAY_LOGGERENTRY type) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    s_global_settings->m_minimum_log_level = type;
    *s_thread_settings.get() = *s_global_settings;
}

CRAY_LOGGERENTRY CR_getMinimumLogLevel() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(CR_LOGGER_NOTHING)
    return s_global_settings->m_minimum_log_level;
}

bool CR_debug_intersectPixel(cr_vec3* position, int32_t pixel_x, int32_t pixel_y) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(false)
    if (s_global_scene == nullptr || s_global_scene->m_camera == nullptr || s_global_scene->m_acceleration_structure == nullptr) {
        pushMessage(CR_LOGGER_INFO, "Pixel (%d %d) info: Empty scene", pixel_x, pixel_y);
        return false;
    }
    CR_debug_setTestPixelRange(pixel_x, pixel_y, pixel_x, pixel_y);
    Ray ray = s_global_scene->m_camera->generateRay(pixel_x, pixel_y, true);
    cr_float current_min(0.0);
    cr_float current_max = std::numeric_limits<cr_float>::max(); // TODO:: assumes infinite camera
    HitRecord record;
    bool res = s_global_scene->intersect(ray, current_min, current_max, record);
    if (res) {
        position->x = record.intersection_point.x;
        position->y = record.intersection_point.y;
        position->z = record.intersection_point.z;
        pushMessage(CR_LOGGER_INFO, "Pixel (%d %d) info: Position: %f %f %f\n", pixel_x, pixel_y, position->x, position->y, position->z);
    }
    else {
        pushMessage(CR_LOGGER_INFO, "Pixel (%d %d) info: No intersection\n", pixel_x, pixel_y);
    }
    return res;
}

bool CR_setInteractiveRender(bool enable) {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(false)
    CHECK_RETURN_VALUE_IF_STARTED_ENGINE(false)
    if (s_global_engine->m_use_main_thread && enable) {
        CRAY_WARNING("Interactive mode is not supported when the use of main thread is enabled (see CR_setNumThreads). Skipping.")
            return false;
    }
    s_global_engine->setInteractiveRender(enable);
    return true;
}

void CR_useFastRNG(bool enable) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CHECK_RETURN_IF_STARTED_ENGINE()
    rand_use_fast_rng(enable);
}

bool CR_isInteractiveRendeEnabled() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(false)
    return s_global_settings->m_interactive;
}

bool CR_isFastRNGEnabled() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(false)
    return rand_is_fast_rng();
}

void CR_debug_updateCameraPerspective(cr_vec3 position, cr_vec3 target, cr_vec3 up, cr_float aperture, cr_float near, cr_float far) {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    if (!s_global_settings->m_interactive) {
        return;
    }

    CameraPerspective* pcamera = dynamic_cast<CameraPerspective*>(s_global_scene->m_camera.get());
    if (pcamera != nullptr) {
        pcamera->m_target = glm::vec3(target.x, target.y, target.z);
        pcamera->m_up = glm::vec3(up.x, up.y, up.z);
        pcamera->m_aperture = aperture;
        pcamera->m_near = near;
        pcamera->m_far = far;
        pcamera->m_eye = glm::vec3(position.x, position.y, position.z);
        pcamera->preprocess();
        s_global_framebuffer->clearIteration(0);
    }
    CameraOrthographic* ocamera = dynamic_cast<CameraOrthographic*>(s_global_scene->m_camera.get());
    if (ocamera != nullptr) {
        ocamera->m_target = glm::vec3(target.x, target.y, target.z);
        ocamera->m_up = glm::vec3(up.x, up.y, up.z);
        ocamera->m_near = near;
        ocamera->m_far = far;
        ocamera->m_eye = glm::vec3(position.x, position.y, position.z);
        ocamera->preprocess();
        s_global_framebuffer->clearIteration(0);
    }
}

void CR_debug_enableTestPixelDebug(bool enable) {
#ifdef ENABLE_DEBUG_CURRENT_PIXEL
    s_global_settings->m_debug_data.enabled = enable;
    *s_thread_settings.get() = *s_global_settings;
#endif
}

void CR_debug_setTestPixelRange(int32_t start_x, int32_t start_y, int32_t end_x, int32_t end_y) {
#ifdef ENABLE_DEBUG_CURRENT_PIXEL
    s_global_settings->m_debug_data.target_pixel_start = glm::vec2(start_x, start_y);
    s_global_settings->m_debug_data.target_pixel_end = glm::vec2(end_x, end_y);
    s_global_settings->m_debug_data.target_pixel_end = glm::max(s_global_settings->m_debug_data.target_pixel_end, s_global_settings->m_debug_data.target_pixel_start);
    s_global_settings->m_debug_data.target_pixel_start = glm::clamp(s_global_settings->m_debug_data.target_pixel_start, glm::ivec2(0), s_global_framebuffer->m_dimensions - 1);
    s_global_settings->m_debug_data.target_pixel_end = glm::clamp(s_global_settings->m_debug_data.target_pixel_end, glm::ivec2(0), s_global_framebuffer->m_dimensions - 1);
    *s_thread_settings.get() = *s_global_settings;
#endif
}

bool CR_debug_isTestPixelDebugEnabled() {
    CHECK_RETURN_VALUE_IF_UNINITIALIZED_ENGINE(false)
#ifdef ENABLE_DEBUG_CURRENT_PIXEL
    return s_global_settings->m_debug_data.enabled;
#else
    return false;
#endif // ENABLE_DEBUG_CURRENT_PIXEL
}

#ifdef CRAY_ENABLE_TESTS
void CR_testColorIntegrator() {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CR_setGamma(1.0);
    CR_setExposure(0.0);
    CR_addCameraPerspective(VEC3(0.0, 0.0, 0.0), VEC3(0.0, 0.0, -1.0), VEC3(0.0, 1.0, 0.0), 90.0, 1.0, 100.0);
    s_global_integrator = std::make_unique<IntegratorTestColor>();
}

void CR_testCameraIntegrator() {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CR_setGamma(1.0);
    CR_setExposure(0.0);
    CR_addCameraPerspective(VEC3(0.0, 0.0, 0.0), VEC3(0.0, 0.0, -1.0), VEC3(0.0, 1.0, 0.0), 35.0, 1.0, 100.0);
    s_global_integrator = std::make_unique<IntegratorTestCamera>();
}

void CR_testNoiseIntegrator() {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CR_setGamma(1.0);
    CR_setExposure(0.0);
    CR_addCameraPerspective(VEC3(0.0, 0.0, 0.0), VEC3(0.0, 0.0, -1.0), VEC3(0.0, 1.0, 0.0), 35.0, 1.0, 100.0);
    s_global_integrator = std::make_unique<IntegratorTestNoise>();
}

void CR_testUVIntegrator() {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CR_setGamma(1.0);
    CR_setExposure(0.0);
    s_global_integrator = std::make_unique<IntegratorTestUV>();
}

void CR_testNormalIntegrator() {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CR_setGamma(1.0);
    CR_setExposure(0.0);
    s_global_integrator = std::make_unique<IntegratorTestNormal>();
}

void CR_testMaterialIntegrator() {
    CHECK_RETURN_IF_UNINITIALIZED_ENGINE()
    CR_setGamma(1.0);
    CR_setExposure(0.0);
    s_global_integrator = std::make_unique<IntegratorTestMaterial>();
}

#endif // CRAY_ENABLE_TESTS
