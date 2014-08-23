project(SQEE)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib")

set(SRCDIR "${PROJECT_SOURCE_DIR}/src")
set(INCDIR "${PROJECT_SOURCE_DIR}/include")

set(SOURCES
    "${INCDIR}/libsqee/gl/gl_ext_3_3.hpp"
    "${INCDIR}/libsqee/gl/gl.hpp"
    "${INCDIR}/libsqee/gl/framebuffers.hpp"
    "${INCDIR}/libsqee/gl/shaders.hpp"
    "${INCDIR}/libsqee/gl/textures.hpp"
    "${INCDIR}/libsqee/gl/vaos.hpp"
    "${INCDIR}/libsqee/app/application.hpp"
    "${INCDIR}/libsqee/events/basichandlers.hpp"
    "${INCDIR}/libsqee/events/handler.hpp"
    "${INCDIR}/libsqee/fonts/fontholder.hpp"
    "${INCDIR}/libsqee/menus/basicmenu.hpp"
    "${INCDIR}/libsqee/misc/files.hpp"
    "${INCDIR}/libsqee/misc/stb_image.hpp"
    "${INCDIR}/libsqee/models/mesh.hpp"
    "${INCDIR}/libsqee/scenes/scene.hpp"
    "${INCDIR}/libsqee/sounds/soundmanager.hpp"

    "${SRCDIR}/libsqee/gl/gl_ext_3_3.cpp"
    "${SRCDIR}/libsqee/gl/gl.cpp"
    "${SRCDIR}/libsqee/gl/framebuffers.cpp"
    "${SRCDIR}/libsqee/gl/shaders.cpp"
    "${SRCDIR}/libsqee/gl/textures.cpp"
    "${SRCDIR}/libsqee/gl/vaos.cpp"
    "${SRCDIR}/libsqee/app/application.cpp"
    "${SRCDIR}/libsqee/events/basichandlers.cpp"
    "${SRCDIR}/libsqee/events/handler.cpp"
    "${SRCDIR}/libsqee/fonts/fontholder.cpp"
    "${SRCDIR}/libsqee/menus/basicmenu.cpp"
    "${SRCDIR}/libsqee/misc/files.cpp"
    "${SRCDIR}/libsqee/misc/stb_image.cpp"
    "${SRCDIR}/libsqee/models/mesh.cpp"
    "${SRCDIR}/libsqee/scenes/scene.cpp"
    "${SRCDIR}/libsqee/sounds/soundmanager.cpp"
)

add_library(libsqee SHARED ${SOURCES})
include_directories("${PROJECT_SOURCE_DIR}/include/libsqee")

# TODO: This section need fixing
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/include")
    find_library(SFML_SYSTEM_LIB sfml-system "${PROJECT_SOURCE_DIR}/extlibs/lib")
    find_library(SFML_WINDOW_LIB sfml-window "${PROJECT_SOURCE_DIR}/extlibs/lib")
    find_library(SFGUI_LIB sfgui "${PROJECT_SOURCE_DIR}/extlibs/lib")
    target_link_libraries(libsqee
        ${SFML_SYSTEM_LIB} ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} 
        ${SFGUI_LIB} 
        "${PROJECT_SOURCE_DIR}/extlibs/lib/jsoncpp.a"
        "${PROJECT_SOURCE_DIR}/extlibs/lib/libboost_system.a"
        "${PROJECT_SOURCE_DIR}/extlibs/lib/libboost_filesystem.a"
    )
# END TODO
else ()
    target_link_libraries(libsqee
        sfml-window sfml-system sfml-audio
        jsoncpp GL
        boost_filesystem boost_system
    )
endif ()
	
#add_subdirectory("tests/soko")
#add_subdirectory("tests/fb")
#add_subdirectory("tests/gl")
add_subdirectory("tests/gl2")
