cmake_minimum_required(VERSION 3.22)

# Debug message
message("Entering ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")

# Register core library
add_library(lwrb INTERFACE)
target_sources(lwrb PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/lwrb/lwrb.c)
target_include_directories(lwrb INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src/include)
add_library(lwrb_ex INTERFACE)
target_sources(lwrb_ex PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src/lwrb/lwrb_ex.c)
target_include_directories(lwrb_ex INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src/include)

# Register other modules

# Debug message
message("Exiting ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.txt")
