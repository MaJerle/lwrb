# Library core sources
set(lwrb_core_SRCS 
    ${CMAKE_CURRENT_LIST_DIR}/src/lwrb/lwrb.c
)

# Library extended sources
set(lwrb_ex_SRCS 
    ${CMAKE_CURRENT_LIST_DIR}/src/lwrb/lwrb_ex.c
)

# Setup include directories
set(lwrb_include_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/src/include
)

# Register library to the system
add_library(lwrb)
target_sources(lwrb PUBLIC ${lwrb_core_SRCS})
target_include_directories(lwrb PUBLIC ${lwrb_include_DIRS})
target_compile_options(lwrb PUBLIC ${LWRB_COMPILE_OPTIONS})
target_compile_definitions(lwrb PUBLIC ${LWRB_COMPILE_DEFINITIONS})

# Register extended part
add_library(lwrb_ex)
target_sources(lwrb_ex PUBLIC ${lwrb_ex_SRCS})
target_include_directories(lwrb_ex PUBLIC ${lwrb_include_DIRS})
target_compile_options(lwrb_ex PUBLIC ${LWRB_COMPILE_OPTIONS})
target_compile_definitions(lwrb_ex PUBLIC ${LWRB_COMPILE_DEFINITIONS} LWRB_EXTENDED)
