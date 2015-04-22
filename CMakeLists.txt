project(SQEE)
cmake_minimum_required(VERSION 2.8)

set(SQEE_USE_BUNDLED_LIBS FALSE CACHE BOOLEAN "Use libraries and headers in the extlibs dir")
set(SQEE_STATIC_LIB FALSE CACHE BOOLEAN "Build SQEE as a static library")
set(SQEE_STATIC_DEPS FALSE CACHE BOOLEAN "Link some libraries statically (varies by platform)")

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -Wno-unused-parameter -Wno-missing-braces")
if(SQEE_STATIC_DEPS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSFML_STATIC")
    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")
    endif()
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSQEE_DEBUG")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
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
    "${INCDIR}/gl/maths.hpp"
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
    "${INCDIR}/menus/basicmenu.hpp"
    "${INCDIR}/render/cameras.hpp"
    "${INCDIR}/render/mesh.hpp"
    "${INCDIR}/render/skin.hpp"
    "${INCDIR}/render/animation.hpp"
    "${INCDIR}/render/skeleton.hpp"
    "${INCDIR}/render/lightsky.hpp"
    "${INCDIR}/render/lightspot.hpp"
    "${INCDIR}/render/lightpoint.hpp"
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
    "${SRCDIR}/gl/maths.cpp"
    "${SRCDIR}/gl/misc.cpp"
    "${SRCDIR}/gl/framebuffers.cpp"
    "${SRCDIR}/gl/preprocessor.cpp"
    "${SRCDIR}/gl/shaders.cpp"
    "${SRCDIR}/gl/uniformbuffers.cpp"
    "${SRCDIR}/gl/textures.cpp"
    "${SRCDIR}/scripts/scene.cpp"
    "${SRCDIR}/scripts/handler.cpp"
    "${SRCDIR}/scripts/intergration.cpp"
    "${SRCDIR}/app/application.cpp"
    "${SRCDIR}/app/logging.cpp"
    "${SRCDIR}/app/settings.cpp"
    "${SRCDIR}/events/basichandlers.cpp"
    "${SRCDIR}/text/font.cpp"
    "${SRCDIR}/text/text.cpp"
    "${SRCDIR}/menus/basicmenu.cpp"
    "${SRCDIR}/render/cameras.cpp"
    "${SRCDIR}/render/mesh.cpp"
    "${SRCDIR}/render/skin.cpp"
    "${SRCDIR}/render/animation.cpp"
    "${SRCDIR}/render/skeleton.cpp"
    "${SRCDIR}/render/lightsky.cpp"
    "${SRCDIR}/render/lightspot.cpp"
    "${SRCDIR}/render/lightpoint.cpp"
    "${SRCDIR}/misc/files.cpp"
    "${SRCDIR}/scenes/basicscenes.cpp"
    "${SRCDIR}/sounds/soundmanager.cpp"
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

addbin("tinytext/texture.bin" ttTexture)
addbin("tinytext/indices.bin" ttIndices)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build")

include_directories("${PROJECT_SOURCE_DIR}/include")
if(${SQEE_USE_BUNDLED_LIBS})
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/include/all")
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        include_directories("${PROJECT_SOURCE_DIR}/extlibs/include/lin64")
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        include_directories("${PROJECT_SOURCE_DIR}/extlibs/include/win32")
    endif()
endif()

if(${SQEE_STATIC_LIB})
    add_library(sqee STATIC ${SOURCES} ${EMBEDSOURCES})
else()
    add_library(sqee SHARED ${SOURCES} ${EMBEDSOURCES})
endif()

if(${SQEE_USE_BUNDLED_LIBS})
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_SOURCE_DIR}/extlibs/libs/lin64)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_SOURCE_DIR}/extlibs/libs/win32)
    endif()
endif()

if(${SQEE_STATIC_DEPS})
    find_library(SFML_WINDOW_LIB sfml-window-s /)
    find_library(SFML_SYSTEM_LIB sfml-system-s /)
    find_library(SFML_AUDIO_LIB sfml-audio-s /)
else()
    find_library(SFML_WINDOW_LIB sfml-window /)
    find_library(SFML_SYSTEM_LIB sfml-system /)
    find_library(SFML_AUDIO_LIB sfml-audio /)
endif()

find_library(OPENGL_LIB NAMES GL opengl32 PATHS /)
find_library(OPENAL_LIB NAMES openal openal32 PATHS /)
find_library(SNDFILE_LIB sndfile /)

target_link_libraries(sqee
    ${SFML_WINDOW_LIB} ${SFML_SYSTEM_LIB} ${SFML_AUDIO_LIB}
    ${OPENGL_LIB} ${OPENAL_LIB} ${SNDFILE_LIB}
)

if(${SQEE_STATIC_DEPS})
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_link_libraries(sqee pthread udev X11 Xrandr)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        target_link_libraries(sqee winmm gdi32)
    endif()
endif()
	
#add_subdirectory("tests/soko")
#add_subdirectory("tests/fb")
add_subdirectory("tests/gl")
add_subdirectory("tests/chai")
add_subdirectory("tests/rpg")
