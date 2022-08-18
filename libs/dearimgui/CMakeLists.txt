project(DearImGui CXX)

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/source/*.cpp")

if (SQEE_STATIC_LIB)
    add_library(dearimgui STATIC ${HEADERS} ${SOURCES})
    target_compile_definitions(dearimgui PUBLIC SQEE_IMGUI_STATIC)
else ()
    add_library(dearimgui SHARED ${HEADERS} ${SOURCES})
    target_compile_definitions(dearimgui PRIVATE SQEE_IMGUI_EXPORT)
    if (SQEE_GNU OR SQEE_CLANG)
        target_compile_options(dearimgui PRIVATE -fvisibility=hidden)
    endif ()
endif ()

target_include_directories(dearimgui PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_include_directories(dearimgui PRIVATE "${PROJECT_SOURCE_DIR}/include/dearimgui")

set_property(TARGET dearimgui PROPERTY CXX_STANDARD 20)
set_property(TARGET dearimgui PROPERTY CXX_STANDARD_REQUIRED True)

################################################################################

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(dearimgui PRIVATE -Wall -Wextra)
elseif (SQEE_MSVC)
    target_compile_options(dearimgui PRIVATE /W3)
    target_set_msvc_options(dearimgui)
endif ()

################################################################################

if (SQEE_LINUX)
    target_compile_definitions(dearimgui PRIVATE SQEE_LINUX)
elseif (SQEE_WINDOWS)
    target_compile_definitions(dearimgui PRIVATE SQEE_WINDOWS)
endif ()

if (SQEE_GNU)
    target_compile_definitions(dearimgui PRIVATE SQEE_GNU)
elseif (SQEE_CLANG)
    target_compile_definitions(dearimgui PRIVATE SQEE_CLANG)
elseif (SQEE_MSVC)
    target_compile_definitions(dearimgui PRIVATE SQEE_MSVC)
endif ()
