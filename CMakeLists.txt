project(SQEE)
cmake_minimum_required(VERSION 2.8)

set(SQEE_USE_BUNDLED_LIBS FALSE CACHE BOOLEAN "Use libraries and headers in the extlibs dir")
set(SQEE_STATIC_LIB FALSE CACHE BOOLEAN "Build SQEE as a static library")

set(SQEE_LINUX FALSE CACHE BOOLEAN "Build for Linux")
set(SQEE_WINOWS FALSE CACHE BOOLEAN "Build for Windows")
set(SQEE_ANDROID FALSE CACHE BOOLEAN "Build for Android")
# android not currently implemented

set(CMAKE_CXX_FLAGS "-std=c++11 -O2 -Wall -Wextra -Wno-unused-parameter -Wno-missing-braces -Wno-comment -Wno-strict-aliasing")
if(${SQEE_WINDOWS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows -static-libstdc++ -static-libgcc")
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSQEE_DEBUG")
endif()

set(SRCDIR "${PROJECT_SOURCE_DIR}/src/sqee")
set(INCDIR "${PROJECT_SOURCE_DIR}/include/sqee")
set(EMBDIR "${PROJECT_SOURCE_DIR}/embed")

set(SOURCES
# 3rd Party Headers
    "${INCDIR}/redist/gl_ext_3_3.hpp"
    "${INCDIR}/redist/jsoncpp.hpp"
    "${INCDIR}/redist/lmccop.hpp"
    "${INCDIR}/redist/stb_image.hpp"
    "${INCDIR}/redist/tinyformat.hpp"

# SQEE Headers
    "${INCDIR}/forward.hpp"
    "${INCDIR}/debug/glcallback.hpp"
    "${INCDIR}/debug/misc.hpp"
    "${INCDIR}/gl/misc.hpp"
    "${INCDIR}/gl/framebuffers.hpp"
    "${INCDIR}/gl/preprocessor.hpp"
    "${INCDIR}/gl/shaders.hpp"
    "${INCDIR}/gl/uniformbuffers.hpp"
    "${INCDIR}/gl/textures.hpp"
    "${INCDIR}/scripts/chaiscript.hpp"
    "${INCDIR}/scripts/scene.hpp"
    "${INCDIR}/scripts/handler.hpp"
    "${INCDIR}/scripts/intergration.hpp"
    "${INCDIR}/app/application.hpp"
    "${INCDIR}/app/logging.hpp"
    "${INCDIR}/app/settings.hpp"
    "${INCDIR}/events/handler.hpp"
    "${INCDIR}/events/basichandlers.hpp"
    "${INCDIR}/text/font.hpp"
    "${INCDIR}/text/text.hpp"
    "${INCDIR}/maths/culling.hpp"
    "${INCDIR}/maths/general.hpp"
    "${INCDIR}/menus/basicmenu.hpp"
    "${INCDIR}/render/cameras.hpp"
    "${INCDIR}/render/mesh.hpp"
    "${INCDIR}/render/skin.hpp"
    "${INCDIR}/render/animation.hpp"
    "${INCDIR}/render/skeleton.hpp"
    "${INCDIR}/misc/resholder.hpp"
    "${INCDIR}/misc/indexedmap.hpp"
    "${INCDIR}/misc/files.hpp"
    "${INCDIR}/scenes/scene.hpp"
    "${INCDIR}/scenes/basicscenes.hpp"
    "${INCDIR}/sounds/soundmanager.hpp"

# 3rd Party Source
    "${SRCDIR}/redist/gl_ext_3_3.cpp"
    "${SRCDIR}/redist/jsoncpp.cpp"
    "${SRCDIR}/redist/stb_image.cpp"

# SQEE Source
    "${SRCDIR}/debug/misc.cpp"
    "${SRCDIR}/gl/misc.cpp"
    "${SRCDIR}/gl/framebuffers.cpp"
    "${SRCDIR}/gl/preprocessor.cpp"
    "${SRCDIR}/gl/shaders.cpp"
    "${SRCDIR}/gl/uniformbuffers.cpp"
    "${SRCDIR}/gl/textures.cpp"
    "${SRCDIR}/scripts/chaiscript.cpp"
    "${SRCDIR}/scripts/scene.cpp"
    "${SRCDIR}/scripts/handler.cpp"
    "${SRCDIR}/scripts/intergration.cpp"
    "${SRCDIR}/app/application.cpp"
    "${SRCDIR}/app/logging.cpp"
    "${SRCDIR}/app/settings.cpp"
    "${SRCDIR}/events/basichandlers.cpp"
    "${SRCDIR}/text/font.cpp"
    "${SRCDIR}/text/text.cpp"
    "${SRCDIR}/maths/culling.cpp"
    "${SRCDIR}/maths/general.cpp"
    "${SRCDIR}/menus/basicmenu.cpp"
    "${SRCDIR}/render/cameras.cpp"
    "${SRCDIR}/render/mesh.cpp"
    "${SRCDIR}/render/skin.cpp"
    "${SRCDIR}/render/animation.cpp"
    "${SRCDIR}/render/skeleton.cpp"
    "${SRCDIR}/misc/files.cpp"
    "${SRCDIR}/scenes/basicscenes.cpp"
    "${SRCDIR}/sounds/soundmanager.cpp"

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
find_library(OPENGL_LIB NAMES GL opengl32 PATHS /)
find_library(OPENAL_LIB NAMES openal openal32 PATHS /)

target_link_libraries(sqee
    ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB} ${SFML_AUDIO_LIB}
    ${OPENGL_LIB} ${OPENAL_LIB}
)

#add_subdirectory("tests/soko")
#add_subdirectory("tests/fb")
add_subdirectory("tests/gl")
add_subdirectory("tests/chai")
add_subdirectory("tests/rpg")
