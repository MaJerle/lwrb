cmake_minimum_required(VERSION 3.22)

# Register core library
add_library(lwrb INTERFACE)
target_sources(lwrb PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/lwrb/lwrb.c)
target_include_directories(lwrb INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src/include)

# Register other modules
