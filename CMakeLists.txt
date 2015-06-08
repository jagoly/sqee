project(SQEE)
cmake_minimum_required(VERSION 2.8)

set(SQEE_USE_BUNDLED_LIBS FALSE CACHE BOOLEAN "Use libraries and headers in the extlibs dir")
set(SQEE_STATIC_LIB FALSE CACHE BOOLEAN "Build SQEE as a static library")

set(SQEE_LINUX FALSE CACHE BOOLEAN "Build for Linux")
set(SQEE_WINOWS FALSE CACHE BOOLEAN "Build for Windows")
set(SQEE_ANDROID FALSE CACHE BOOLEAN "Build for Android")
# android not currently implemented

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wno-unused-parameter -Wno-comment -Wno-strict-aliasing")
if(${SQEE_WINDOWS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows -static-libstdc++ -static-libgcc")
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSQEE_DEBUG -O1")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

set(SRCDIR "${PROJECT_SOURCE_DIR}/src/sqee")
set(INCDIR "${PROJECT_SOURCE_DIR}/include/sqee")
set(EMBDIR "${PROJECT_SOURCE_DIR}/embed")

set(SOURCES
# 3rd Party Source
    "${INCDIR}/redist/gl_ext_3_3.hpp" "${SRCDIR}/redist/gl_ext_3_3.cpp"
    "${INCDIR}/redist/stb_image.hpp"  "${SRCDIR}/redist/stb_image.cpp"
    "${INCDIR}/redist/jsoncpp.hpp"    "${SRCDIR}/redist/jsoncpp.cpp"
    "${INCDIR}/redist/tinyformat.hpp"
    "${INCDIR}/redist/lmccop.hpp"

# SQEE Source
    "${INCDIR}/forward.hpp"
    "${INCDIR}/debug/glcallback.hpp"
    "${INCDIR}/debug/misc.hpp"            "${SRCDIR}/debug/misc.cpp"
    "${INCDIR}/gl/drawing.hpp"            "${SRCDIR}/gl/drawing.cpp"
    "${INCDIR}/gl/framebuffers.hpp"       "${SRCDIR}/gl/framebuffers.cpp"
    "${INCDIR}/gl/preprocessor.hpp"       "${SRCDIR}/gl/preprocessor.cpp"
    "${INCDIR}/gl/shaders.hpp"            "${SRCDIR}/gl/shaders.cpp"
    "${INCDIR}/gl/uniformbuffers.hpp"     "${SRCDIR}/gl/uniformbuffers.cpp"
    "${INCDIR}/gl/textures.hpp"           "${SRCDIR}/gl/textures.cpp"
    "${INCDIR}/scripts/chaiscript.hpp"    "${SRCDIR}/scripts/chaiscript.cpp"
    "${INCDIR}/scripts/scene.hpp"         "${SRCDIR}/scripts/scene.cpp"
    "${INCDIR}/scripts/handler.hpp"       "${SRCDIR}/scripts/handler.cpp"
    "${INCDIR}/scripts/intergration.hpp"  "${SRCDIR}/scripts/intergration.cpp"
    "${INCDIR}/app/application.hpp"       "${SRCDIR}/app/application.cpp"
    "${INCDIR}/app/logging.hpp"           "${SRCDIR}/app/logging.cpp"
    "${INCDIR}/app/settings.hpp"          "${SRCDIR}/app/settings.cpp"
    "${INCDIR}/handlers/handler.hpp"      "${SRCDIR}/handlers/handler.cpp"
    "${INCDIR}/handlers/basics.hpp"       "${SRCDIR}/handlers/basics.cpp"
    "${INCDIR}/text/font.hpp"             "${SRCDIR}/text/font.cpp"
    "${INCDIR}/text/text.hpp"             "${SRCDIR}/text/text.cpp"
    "${INCDIR}/maths/culling.hpp"         "${SRCDIR}/maths/culling.cpp"
    "${INCDIR}/maths/general.hpp"         "${SRCDIR}/maths/general.cpp"
    "${INCDIR}/maths/physics.hpp"         "${SRCDIR}/maths/physics.cpp"
    "${INCDIR}/menus/basicmenu.hpp"       "${SRCDIR}/menus/basicmenu.cpp"
    "${INCDIR}/render/cameras.hpp"        "${SRCDIR}/render/cameras.cpp"
    "${INCDIR}/render/mesh.hpp"           "${SRCDIR}/render/mesh.cpp"
    "${INCDIR}/render/skin.hpp"           "${SRCDIR}/render/skin.cpp"
    "${INCDIR}/render/animation.hpp"      "${SRCDIR}/render/animation.cpp"
    "${INCDIR}/render/skeleton.hpp"       "${SRCDIR}/render/skeleton.cpp"
    "${INCDIR}/misc/resholder.hpp"
    "${INCDIR}/misc/files.hpp"            "${SRCDIR}/misc/files.cpp"
    "${INCDIR}/scenes/scene.hpp"          "${SRCDIR}/scenes/scene.cpp"
    "${INCDIR}/scenes/basics.hpp"         "${SRCDIR}/scenes/basics.cpp"
    "${INCDIR}/sounds/soundmanager.hpp"   "${SRCDIR}/sounds/soundmanager.cpp"

# Compiled-In Data
    "${SRCDIR}/data/textfont.c"
    "${SRCDIR}/data/volumes.c"
)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/work)
macro (addbin input varname)
    string(REGEX MATCH "([^/]+)$" filename ${EMBDIR}/${input})
    string(REGEX REPLACE "\\.| " "_" filename ${filename})
    file(READ ${EMBDIR}/${input} filedata HEX)
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata ${filedata})
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/work/${filename}.c"
               "const unsigned char ${varname}[] = {${filedata}};")
    list(APPEND EMBEDSOURCES "${CMAKE_CURRENT_BINARY_DIR}/work/${filename}.c")
    message(STATUS "Added binary file \"${input}\"")
endmacro ()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")

include_directories("${PROJECT_SOURCE_DIR}/include")
if(${SQEE_USE_BUNDLED_LIBS})
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/include/")
endif()

if(${SQEE_STATIC_LIB})
    add_library(sqee STATIC ${SOURCES} ${EMBEDSOURCES})
else()
    add_library(sqee SHARED ${SOURCES} ${EMBEDSOURCES})
endif()

if(${SQEE_USE_BUNDLED_LIBS})
    if(${SQEE_LINUX})
        list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_SOURCE_DIR}/extlibs/libs/lin64)
    elseif(${SQEE_WINDOWS})
        list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_SOURCE_DIR}/extlibs/libs/win64)
    endif()
endif()


find_library(SFML_WINDOW_LIB sfml-window /)
find_library(SFML_SYSTEM_LIB sfml-system /)
find_library(SFML_AUDIO_LIB sfml-audio /)
find_library(REACTPHYSICS3D_LIB reactphysics3d /)
find_library(OPENGL_LIB NAMES GL opengl32 PATHS /)
find_library(OPENAL_LIB NAMES openal openal32 PATHS /)

target_link_libraries(sqee
    ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB} ${SFML_AUDIO_LIB}
    ${REACTPHYSICS3D_LIB} ${OPENGL_LIB} ${OPENAL_LIB}
)

#add_subdirectory("tests/soko")
#add_subdirectory("tests/fb")
add_subdirectory("tests/gl")
add_subdirectory("tests/chai")
add_subdirectory("tests/rpg")
