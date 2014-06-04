project(SQEE)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/lib")

set(SRCDIR "${PROJECT_SOURCE_DIR}/src")
set(INCDIR "${PROJECT_SOURCE_DIR}/include")

set(SFML_LIBS sfml-window sfml-graphics sfml-system)

set(SOURCES
    "${SRCDIR}/libsqee/application.cpp"
    "${SRCDIR}/libsqee/handler.cpp"
    "${SRCDIR}/libsqee/extra/handlers.cpp"
    "${SRCDIR}/libsqee/entity.cpp"
    "${SRCDIR}/libsqee/scene.cpp"
    "${SRCDIR}/libsqee/textureholder.cpp"
    "${INCDIR}/libsqee/application.hpp"
    "${INCDIR}/libsqee/handler.hpp"
    "${INCDIR}/libsqee/extra/handlers.hpp"
    "${INCDIR}/libsqee/entity.hpp"
    "${INCDIR}/libsqee/scene.hpp"
    "${INCDIR}/libsqee/textureholder.hpp"
    "${INCDIR}/libsqee/base.hpp"
    "${INCDIR}/libsqee/extra.hpp"
)

add_library(libsqee SHARED ${SOURCES})
link_libraries(${SFML_LIBS})
include_directories("${PROJECT_SOURCE_DIR}/include/libsqee")
set_target_properties(libsqee PROPERTIES PREFIX "")

add_subdirectory("tests/soko")
