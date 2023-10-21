cmake_minimum_required(VERSION 3.22)

# Register core library
add_library(lwrb INTERFACE)
target_sources(lwrb PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/lwrb/lwrb.c)
target_include_directories(lwrb INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src/include)

# Register extended part
add_library(lwrb_ex INTERFACE)
target_sources(lwrb_ex PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/lwrb/lwrb_ex.c)
target_compile_definitions(lwrb_ex INTERFACE LWRB_EXTENDED)
target_include_directories(lwrb_ex INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src/include)
