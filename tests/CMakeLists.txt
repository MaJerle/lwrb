cmake_minimum_required(VERSION 3.22)

# Setup project
project(LwLibPROJECT C)

# Set default compile flags for GCC
if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    message(STATUS "GCC detected, adding compile flags")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

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
add_subdirectory("../lwrb" lwrb)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC lwrb)
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC lwrb_ex)
target_compile_definitions(lwrb PUBLIC LWRB_DEV)
target_compile_definitions(lwrb_ex PUBLIC LWRB_DEV)

# Add test
add_test(NAME Test COMMAND $<TARGET_FILE:${CMAKE_PROJECT_NAME}>)
