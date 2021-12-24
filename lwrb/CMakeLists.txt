cmake_minimum_required(VERSION 3.13)

# Debug message
message("LWRB: Entering lib source CMakeLists.txt")

# Set library name
set(LWRB_LIB_NAME "lwrb")

# Register library to the system
add_library(${LWRB_LIB_NAME} INTERFACE)

# Setup generic source files
target_sources(${LWRB_LIB_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/lwrb/lwrb.c
    )

# Setup include directories
target_include_directories(${LWRB_LIB_NAME} INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/include
    )
