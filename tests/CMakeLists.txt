set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/tests")

add_executable(scratch "scratch.cpp")
target_link_libraries(scratch sqee)
