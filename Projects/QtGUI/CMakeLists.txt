set(PROJECT_NAME "Qt_GUI")
message(STATUS "Building QtGUI" )

set(INCLUDE_DIR
   "${CMAKE_SOURCE_DIR}/3rdParty/include"
   "${CMAKE_SOURCE_DIR}/Projects/Global"
   "${CMAKE_SOURCE_DIR}/Projects"
   "${CMAKE_SOURCE_DIR}/Projects/QtGUI"
   "${CMAKE_SOURCE_DIR}/Projects/QtGUI/Code")
include_directories(${INCLUDE_DIR})

file(GLOB_RECURSE CRAY_SOURCES ${CMAKE_SOURCE_DIR}/Projects/CRay/*.*)
file(GLOB_RECURSE LOADER_SOURCES ${CMAKE_SOURCE_DIR}/Projects/Loader/*.*)
file(GLOB_RECURSE SCENE_SOURCES ${CMAKE_SOURCE_DIR}/Projects/Scenes/*.*)
file(GLOB_RECURSE SOURCES *.ui *.cpp *.h* *.qrc )
set(SOURCES ${SOURCES} ${CRAY_SOURCES} ${LOADER_SOURCES} ${SCENE_SOURCES} ${CMAKE_SOURCE_DIR}/3rdParty/include/tinyxml/tinyxml2.cpp ${CMAKE_SOURCE_DIR}/Projects/Global/Exit.cpp )

# Find includes in corresponding build directories
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Parse resources from a Qt resource file
set(CMAKE_AUTORCC ON)
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_CUSTOM_PATH})

# Find the QtWidgets library
find_package(Qt5 COMPONENTS Widgets OpenGL REQUIRED HINTS ${QT_CUSTOM_PATH})

if(NOT ${Qt5_DIR} STREQUAL "" AND NOT ${QT_CUSTOM_PATH} STREQUAL "")
    if ("${QT_CUSTOM_PATH}/lib/cmake/Qt5" STREQUAL "${Qt5_DIR}")
        message(STATUS "Qt5 was found at the requested path")
        set(CUSTOM_QT_COPY_OPERATION ON)
    else()
        message(WARNING "Qt5 was not found at the requested path ${QT_CUSTOM_PATH}, but at ${Qt5_DIR}")
    endif()
endif()

# Tell CMake to create the executable
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/QtGUI.rc)
set_property(TARGET ${PROJECT_NAME} PROPERTY PROJECT_LABEL ${VS_NAME})
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${OUTPUT_PREFIX}${PROJECT_NAME}${ARCHSUFFIX})
set_target_properties(${PROJECT_NAME} PROPERTIES PROJECT_LABEL  ${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

# Use the Widgets module from Qt 5
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::OpenGL)

install (TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}${ARCHSUFFIX})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/bin/build${ARCHSUFFIX}${CMAKE_SYSTEM_NAME}${CMAKE_CXX_COMPILER_ID})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/bin/build${ARCHSUFFIX}${CMAKE_SYSTEM_NAME}${CMAKE_CXX_COMPILER_ID})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/bin/build${ARCHSUFFIX}${CMAKE_SYSTEM_NAME}${CMAKE_CXX_COMPILER_ID})
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}Debug )
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}Release )
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}MinSizeRel )
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}RelWithDebInfo )

if(WIN32) # Check if we are on Windows
    if(MSVC) # Check if we are on MSVC
        set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS /SUBSYSTEM:CONSOLE)
    endif()
endif()

if(CUSTOM_QT_COPY_OPERATION)
    add_custom_target(${PROJECT_NAME}_postbuild DEPENDS ${PROJECT_NAME})
    if(WIN32)
        if(MSVC)
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} ARGS -E copy
                ${QT_CUSTOM_PATH}/bin/Qt5Core$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>${CMAKE_SHARED_LIBRARY_SUFFIX}
                ${QT_CUSTOM_PATH}/bin/Qt5Gui$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>${CMAKE_SHARED_LIBRARY_SUFFIX}
                ${QT_CUSTOM_PATH}/bin/Qt5Widgets$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>${CMAKE_SHARED_LIBRARY_SUFFIX}
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<CONFIG>
                COMMENT "Copying Qt runtime libs step 1/5")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} ARGS -E make_directory
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<CONFIG>/plugins/platforms
                COMMENT "Copying Qt runtime libs step 2/5")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} ARGS -E copy
                ${QT_CUSTOM_PATH}/plugins/platforms/qwindows$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>${CMAKE_SHARED_LIBRARY_SUFFIX}
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<CONFIG>/plugins/platforms
                COMMENT "Copying Qt runtime libs step 3/5")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} ARGS -E make_directory
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<CONFIG>/plugins/styles
                COMMENT "Copying Qt runtime libs step 4/5")
                add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} ARGS -E copy
                ${QT_CUSTOM_PATH}/plugins/styles/qwindowsvistastyle$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>${CMAKE_SHARED_LIBRARY_SUFFIX}
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<CONFIG>/plugins/styles
                COMMENT "Copying Qt runtime libs step 5/5")
        else()
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} ARGS -E copy
                ${QT_CUSTOM_PATH}/bin/Qt5Core${CMAKE_SHARED_LIBRARY_SUFFIX}
                ${QT_CUSTOM_PATH}/bin/Qt5Gui${CMAKE_SHARED_LIBRARY_SUFFIX}
                ${QT_CUSTOM_PATH}/bin/Qt5Widgets${CMAKE_SHARED_LIBRARY_SUFFIX}
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<CONFIG>
                COMMENT "Copying Qt runtime libs step 1/5")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} ARGS -E make_directory
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<CONFIG>/plugins/platforms
                COMMENT "Copying Qt runtime libs step 2/5")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} ARGS -E copy
                ${QT_CUSTOM_PATH}/plugins/platforms/qwindows${CMAKE_SHARED_LIBRARY_SUFFIX}
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<CONFIG>/plugins/platforms
                COMMENT "Copying Qt runtime libs step 3/5")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} ARGS -E make_directory
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<CONFIG>/plugins/styles
                COMMENT "Copying Qt runtime libs step 4/5")
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} ARGS -E copy
                ${QT_CUSTOM_PATH}/plugins/styles/qwindowsvistastyle${CMAKE_SHARED_LIBRARY_SUFFIX}
                ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<CONFIG>/plugins/styles
                COMMENT "Copying Qt runtime libs step 5/5")
        endif()
    else()
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} ARGS -E copy
            ${QT_CUSTOM_PATH}/lib/libQt5Core${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${QT_CUSTOM_PATH}/lib/libQt5Gui${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${QT_CUSTOM_PATH}/lib/libQt5Widgets${CMAKE_SHARED_LIBRARY_SUFFIX}
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}$<CONFIG>
            COMMENT "Copying Qt runtime libs")
    endif()
endif()

GENERATE_VS_FILTERS("QtGUI/Code" ${SOURCES})
