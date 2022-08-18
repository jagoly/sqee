project(FormatLib CXX)

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/source/*.cc")

if (SQEE_STATIC_LIB)
    add_library(format STATIC ${HEADERS} ${SOURCES})
else ()
    add_library(format SHARED ${HEADERS} ${SOURCES})
    target_compile_definitions(format PRIVATE FMT_EXPORT)
    target_compile_definitions(format PUBLIC FMT_SHARED)
endif ()

target_include_directories(format PUBLIC "${PROJECT_SOURCE_DIR}/include")

set_property(TARGET format PROPERTY CXX_STANDARD 20)
set_property(TARGET format PROPERTY CXX_STANDARD_REQUIRED True)

################################################################################

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(format PRIVATE -Wall -Wextra)
elseif (SQEE_MSVC)
    target_compile_options(format PRIVATE /W3)
    target_set_msvc_options(format)
endif ()
