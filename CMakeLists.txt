cmake_minimum_required(VERSION 3.22)

# Setup project
project(LwLibPROJECT C)

if(NOT PROJECT_IS_TOP_LEVEL)
    add_subdirectory(lwrb)
else()
    set(TEST_CMAKE_FILE_NAME "test_basic/cmake.cmake")
    add_subdirectory(tests)
endif()