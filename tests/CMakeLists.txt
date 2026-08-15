cmake_minimum_required(VERSION 3.22)

# Setup project
project(LwLibPROJECT C)

enable_testing()
add_executable(${CMAKE_PROJECT_NAME})
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/test_main.c
)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/
)

# Include file that can add more sources and prepare lib parameters
include(${TEST_CMAKE_FILE_NAME})

# Add subdir with lwrb and link to project
set(LWRB_COMPILE_DEFINITIONS LWRB_DEV)
add_subdirectory("../lwrb" lwrb)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC lwrb)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC lwrb_ex)

# Add test
add_test(NAME Test COMMAND $<TARGET_FILE:${CMAKE_PROJECT_NAME}>)
