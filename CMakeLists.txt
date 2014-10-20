project(SQEE)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra")

add_definitions(-DSQEE_DEBUG)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib")

set(SRCDIR "${PROJECT_SOURCE_DIR}/src")
set(INCDIR "${PROJECT_SOURCE_DIR}/include")

set(SOURCES
# 3rd Party Headers
    "${INCDIR}/libsqee/redist/jsoncpp.hpp"
    "${INCDIR}/libsqee/redist/stb_image.hpp"

# SQEE Headers
    "${INCDIR}/libsqee/defs.hpp"
    "${INCDIR}/libsqee/gl/gl_ext_3_3.hpp"
    "${INCDIR}/libsqee/gl/gl.hpp"
    "${INCDIR}/libsqee/gl/bounds.hpp"
    "${INCDIR}/libsqee/gl/camera.hpp"
    "${INCDIR}/libsqee/gl/framebuffers.hpp"
    "${INCDIR}/libsqee/gl/shaders.hpp"
    "${INCDIR}/libsqee/gl/textures.hpp"
    "${INCDIR}/libsqee/app/application.hpp"
    "${INCDIR}/libsqee/events/handler.hpp"
    "${INCDIR}/libsqee/events/basichandlers.hpp"
    "${INCDIR}/libsqee/text/font.hpp"
    "${INCDIR}/libsqee/text/text.hpp"
    "${INCDIR}/libsqee/menus/basicmenu.hpp"
    "${INCDIR}/libsqee/misc/files.hpp"
    "${INCDIR}/libsqee/misc/containers.hpp"
    "${INCDIR}/libsqee/scenes/scene.hpp"
    "${INCDIR}/libsqee/scenes/basicscenes.hpp"
    "${INCDIR}/libsqee/sounds/soundmanager.hpp"

# 3rd Party Source
    "${SRCDIR}/libsqee/redist/jsoncpp.cpp"
    "${SRCDIR}/libsqee/redist/stb_image.cpp"

# SQEE Source
    "${SRCDIR}/libsqee/gl/gl_ext_3_3.cpp"
    "${SRCDIR}/libsqee/gl/gl.cpp"
    "${SRCDIR}/libsqee/gl/bounds.cpp"
    "${SRCDIR}/libsqee/gl/camera.cpp"
    "${SRCDIR}/libsqee/gl/framebuffers.cpp"
    "${SRCDIR}/libsqee/gl/shaders.cpp"
    "${SRCDIR}/libsqee/gl/textures.cpp"
    "${SRCDIR}/libsqee/app/application.cpp"
    "${SRCDIR}/libsqee/events/basichandlers.cpp"
    "${SRCDIR}/libsqee/events/handler.cpp"
    "${SRCDIR}/libsqee/text/font.cpp"
    "${SRCDIR}/libsqee/text/text.cpp"
    "${SRCDIR}/libsqee/menus/basicmenu.cpp"
    "${SRCDIR}/libsqee/misc/files.cpp"
    "${SRCDIR}/libsqee/misc/containers.cpp"
    "${SRCDIR}/libsqee/scenes/scene.cpp"
    "${SRCDIR}/libsqee/scenes/basicscenes.cpp"
    "${SRCDIR}/libsqee/sounds/soundmanager.cpp"
)

add_library(libsqee SHARED ${SOURCES})
include_directories("${PROJECT_SOURCE_DIR}/include/libsqee")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/include")
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/include/freetype")
    find_library(SFML_SYSTEM_LIB sfml-system "${PROJECT_SOURCE_DIR}/extlibs/lib")
    find_library(SFML_WINDOW_LIB sfml-window "${PROJECT_SOURCE_DIR}/extlibs/lib")
    find_library(SFML_AUDIO_LIB sfml-audio "${PROJECT_SOURCE_DIR}/extlibs/lib")
    find_library(BOOST_SYSTEM_LIB boost_system "${PROJECT_SOURCE_DIR}/extlibs/lib")
    find_library(BOOST_FILESYSTEM_LIB boost_filesystem "${PROJECT_SOURCE_DIR}/extlibs/lib")
    find_library(FREETYPE_LIB freetype "${PROJECT_SOURCE_DIR}/extlibs/lib")
    target_link_libraries(libsqee
        ${SFML_SYSTEM_LIB} ${SFML_WINDOW_LIB} ${SFML_AUDIO_LIB}
        ${BOOST_SYSTEM_LIB} ${BOOST_FILESYSTEM_LIB}
        ${FREETYPE_LIB} openGL32
    )
else ()
    find_package(Freetype REQUIRED)
    include_directories(${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(libsqee
        sfml-window sfml-system sfml-audio
        boost_filesystem boost_system
        freetype GL
    )
endif ()
	
#add_subdirectory("tests/soko")
#add_subdirectory("tests/fb")
add_subdirectory("tests/gl")
add_subdirectory("tests/rpg")
