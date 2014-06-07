project(SQEE)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib")

set(SRCDIR "${PROJECT_SOURCE_DIR}/src")
set(INCDIR "${PROJECT_SOURCE_DIR}/include")

set(SOURCES
    "${SRCDIR}/libsqee/application.cpp"
    "${SRCDIR}/libsqee/handler.cpp"
    "${SRCDIR}/libsqee/extra/handlers.cpp"
    "${SRCDIR}/libsqee/entity.cpp"
    "${SRCDIR}/libsqee/scene.cpp"
    "${SRCDIR}/libsqee/textureholder.cpp"
    "${SRCDIR}/libsqee/extra/helpers.cpp"
    "${INCDIR}/libsqee/application.hpp"
    "${INCDIR}/libsqee/handler.hpp"
    "${INCDIR}/libsqee/extra/handlers.hpp"
    "${INCDIR}/libsqee/entity.hpp"
    "${INCDIR}/libsqee/scene.hpp"
    "${INCDIR}/libsqee/textureholder.hpp"
    "${INCDIR}/libsqee/extra/helpers.hpp"
    "${INCDIR}/libsqee/base.hpp"
    "${INCDIR}/libsqee/extra.hpp"
)

add_library(libsqee SHARED ${SOURCES})
include_directories("${PROJECT_SOURCE_DIR}/include/libsqee")

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    include_directories("${PROJECT_SOURCE_DIR}/extlibs/include")
    FIND_LIBRARY(SFML_SYSTEM_LIB sfml-system "${PROJECT_SOURCE_DIR}/extlibs/lib")
    FIND_LIBRARY(SFML_GRAPHICS_LIB sfml-graphics "${PROJECT_SOURCE_DIR}/extlibs/lib")
    FIND_LIBRARY(SFML_WINDOW_LIB sfml-window "${PROJECT_SOURCE_DIR}/extlibs/lib")
    FIND_LIBRARY(SFGUI_LIB sfgui "${PROJECT_SOURCE_DIR}/extlibs/lib")
    TARGET_LINK_LIBRARIES(libsqee 
        ${SFML_SYSTEM_LIB} ${SFML_GRAPHICS_LIB} ${SFML_WINDOW_LIB} 
        ${SFGUI_LIB} 
        "${PROJECT_SOURCE_DIR}/extlibs/lib/jsoncpp.a"
        "${PROJECT_SOURCE_DIR}/extlibs/lib/libboost_system.a"
        "${PROJECT_SOURCE_DIR}/extlibs/lib/libboost_filesystem.a"
    )
else ()
    link_libraries(sfml-window sfml-graphics sfml-system sfgui jsoncpp boost_filesystem boost_system)
endif ()
	
add_subdirectory("tests/soko")
