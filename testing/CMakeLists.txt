project(SQEE_TESTING CXX)

################################################################################

file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")

add_executable(sqee-testing ${HEADERS} ${SOURCES})

target_set_output_directory(sqee-testing "testing")

target_include_directories(sqee-testing PRIVATE "${PROJECT_SOURCE_DIR}")

################################################################################

set_property(TARGET sqee-testing PROPERTY CXX_STANDARD 20)
set_property(TARGET sqee-testing PROPERTY CXX_STANDARD_REQUIRED True)

if (SQEE_GNU OR SQEE_CLANG)
    target_compile_options(sqee-testing PRIVATE -Wall -Wextra)
elseif (SQEE_MSVC)
    target_compile_options(sqee-testing PRIVATE /W3 /wd4251)
    target_set_msvc_options(sqee-testing)
endif ()

target_link_libraries(sqee-testing PRIVATE sqee)
