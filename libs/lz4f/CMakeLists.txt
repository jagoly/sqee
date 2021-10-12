project(LZ4F C)

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/lz4frame.h"
                          "${PROJECT_SOURCE_DIR}/source/*.h")

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/source/*.c")

if (SQEE_STATIC_LIB)
    add_library(lz4f STATIC ${HEADERS} ${SOURCES})
else ()
    add_library(lz4f SHARED ${HEADERS} ${SOURCES})
    if (SQEE_GNU OR SQEE_CLANG)
        target_compile_options(lz4f PRIVATE -fvisibility=hidden)
    elseif (SQEE_MSVC)
        target_compile_definitions(lz4f PRIVATE LZ4_DLL_EXPORT=1)
        target_compile_definitions(lz4f PUBLIC LZ4_DLL_IMPORT=1)
    endif ()
endif ()

target_include_directories(lz4f PUBLIC "${PROJECT_SOURCE_DIR}/include")

set_property(TARGET lz4f PROPERTY C_STANDARD 99)
set_property(TARGET lz4f PROPERTY C_STANDARD_REQUIRED True)

################################################################################

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(lz4f PRIVATE -Wall)
elseif (SQEE_MSVC)
    target_compile_options(lz4f PRIVATE /W3)
    target_set_msvc_options(lz4f)
endif ()
