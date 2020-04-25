# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.5)

project ("CRayEngineMini")
if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
    endif()
endif()
MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "OS: ${CMAKE_SYSTEM_NAME}")
MESSAGE(STATUS "System Version: ${CMAKE_SYSTEM_VERSION}")
MESSAGE(STATUS "System Processor: ${CMAKE_SYSTEM_PROCESSOR}")
if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    set(ARCHSUFFIX "x64")
    set(ARCH 64)
else()
    set(ARCHSUFFIX "x86")
    set(ARCH 32)
endif()
MESSAGE(STATUS "System Architecture: ${ARCH}-bit")

set(OUTPUT_PREFIX "CRay_mini_")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(Threads REQUIRED)
#MESSAGE(STATUS "CMAKE_THREAD_LIBS_INIT ${CMAKE_THREAD_LIBS_INIT}")
file(TO_CMAKE_PATH "${CMAKE_SOURCE_DIR}" NORMALIZED_SOURCE_DIR)
add_definitions(-DSOURCEPATH="${NORMALIZED_SOURCE_DIR}")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
    # using Clang
    MESSAGE(STATUS "Using ${CMAKE_CXX_COMPILER_ID}")
    set(C_REDUCE_ERRORS -Wno-double-promotion -Wno-unused-parameter)
    set(CXX_REDUCE_ERRORS -Wno-double-promotion -Wno-unused-parameter)
    set(C_MAIN_FLAGS -fPIC -Wall -Wextra -march=native ${C_REDUCE_ERRORS})
    set(CXX_MAIN_FLAGS -fPIC -Wall -Wextra -march=native ${CXX_REDUCE_ERRORS})
    #add_definitions(-D_POSIX_C_SOURCE=199309L)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # using GCC
    MESSAGE(STATUS "Using ${CMAKE_CXX_COMPILER_ID}")
    set(C_REDUCE_ERRORS -Wno-double-promotion -Wno-unused-parameter -mfpmath=sse)
    set(CXX_REDUCE_ERRORS -Wno-double-promotion -Wno-unused-parameter -mfpmath=sse)
    set(C_MAIN_FLAGS -fPIC -Wall -Wextra -march=native ${C_REDUCE_ERRORS})
    set(CXX_MAIN_FLAGS -fPIC -Wall -Wextra -march=native ${CXX_REDUCE_ERRORS})
    #add_definitions(-D_POSIX_C_SOURCE=199309L)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # using Visual Studio C++
    MESSAGE(STATUS "Using ${CMAKE_CXX_COMPILER_ID}")
    #find_package(SSE) // future extension
    set(C_REDUCE_ERRORS /wd4100 /wd4127 /wd4201 /wd4204 /wd4244 /wd4267 /wd4305 /wd4996)
    set(CXX_REDUCE_ERRORS /wd4100 /wd4127 /wd4201 /wd4204 /wd4244 /wd4267 /wd4305 /wd4996)
    set(C_MAIN_FLAGS /permissive- /W4 /fp:precise ${C_REDUCE_ERRORS} /MP)
    set(CXX_MAIN_FLAGS /permissive- /W4 /fp:precise ${CXX_REDUCE_ERRORS} /MP /EHsc)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    #/wd4100 unreferenced formal parameter
    #/wd4127 condition expression is constant
    #/wd4201 nonstandard extension used: nameless struct/union
    #/wd4204 nonstandard extension used: non-constant aggregate initializer
    #/wd4244 conversion from 'type1' to 'type2', possible loss of data
    #/wd4267 'argument': conversion from 'size_t' to 'int32_t', possible loss of data
    #/wd4305 'initializing': truncation from 'double' to 'cr_float'
    #/wd4996 This function or variable may be unsafe
else()
    MESSAGE(WARNING "Using non-tested compiler ${CMAKE_CXX_COMPILER_ID}")
endif()
add_compile_options(
"$<$<COMPILE_LANGUAGE:C>:${C_MAIN_FLAGS}>"
"$<$<COMPILE_LANGUAGE:CXX>:${CXX_MAIN_FLAGS}>"
)
MESSAGE(STATUS "Using C Flags: ${C_MAIN_FLAGS}")
MESSAGE(STATUS "Using C++ Flags: ${CXX_MAIN_FLAGS}")

# Set the output folder where your program will be created
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/bin)

# Set other values
set(CMAKE_DEBUG_POSTFIX d)

MACRO(LISTFILES title filelist)
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo_append "   ${title}:")
    foreach(file ${filelist})
    get_filename_component(classname "${file}" NAME_WE)
    get_filename_component(classnameext "${file}" EXT)
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo_append " ${classname}${classnameext}")
    endforeach()
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo "")
ENDMACRO(LISTFILES)

# generate filters for visual studio
MACRO(GENERATE_VS_FILTERS folder)
    IF(WIN32) # Check if we are on Windows
        if(MSVC) # Check if we are using the Visual Studio compiler
            # generate filters for VS
            set(_src_root_path "${CMAKE_SOURCE_DIR}/Projects/${folder}/")
            foreach(_source IN ITEMS ${SOURCES})
            get_filename_component(_source_path "${_source}" PATH)
            file(RELATIVE_PATH _source_path_rel "${_src_root_path}" "${_source_path}")
            string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
            source_group("${_group_path}" FILES "${_source}")
            endforeach()
        endif()
    endif()
ENDMACRO(GENERATE_VS_FILTERS)

#MESSAGE(${CMAKE_CURRENT_BINARY_DIR})
#MESSAGE(${CMAKE_CURRENT_SOURCE_DIR})
#MESSAGE(${CMAKE_SOURCE_DIR})

if (TESTS_ENABLE)
    add_definitions(-DCRAY_ENABLE_TESTS)
endif()

file(GLOB_RECURSE global_files Projects/Global/*.c* Projects/Global/*.h* Projects/Global/*.inl)
add_subdirectory(Projects/ConsoleExample)

option(QT_ENABLE "Enable Qt" OFF)
set(QT_CUSTOM_PATH "" CACHE PATH "Set custom Qt path")
if (QT_ENABLE)
    MESSAGE(STATUS "Qt GUI is enabled")
    add_subdirectory(Projects/QtGUI)
endif()
if (TESTS_ENABLE)
    enable_testing()
    option(TESTS_ENABLE "Enable tests" OFF)
    include(CMakeDependentOption)
    cmake_dependent_option(TESTS_GENERATE "Adds the option to re-generate the tests when ctest is called" OFF "TESTS_ENABLE" OFF)
    MESSAGE(STATUS "Tests are enabled")
    add_subdirectory(Projects/Tests)
endif()

IF(WIN32) # Check if we are on Windows
    if(MSVC) # Check if we are using the Visual Studio compiler
        # Turn on the ability to create folders to organize projects (.vcproj)
        # It creates "CMakePredefinedTargets" folder by default and adds CMake
        # defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
        set_property(GLOBAL PROPERTY USE_FOLDERS ON)
        set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "console_example")
    endif()
endif()
