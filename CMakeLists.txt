cmake_minimum_required(VERSION 3.22)

# Setup project
project(LwLibPROJECT C)

if(NOT PROJECT_IS_TOP_LEVEL)
    add_subdirectory(lwrb)
else()
    add_executable(${PROJECT_NAME})
    target_sources(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/dev/main.c
    )
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/dev
    )

    # Add subdir with lwrb and link to project
    set(LWRB_COMPILE_DEFINITIONS LWRB_DEV)
    add_subdirectory(lwrb)
    target_link_libraries(${PROJECT_NAME} lwrb)
    target_link_libraries(${PROJECT_NAME} lwrb_ex)

    # Add compile options to the library, which will propagate options to executable through public link
    target_compile_definitions(lwrb PUBLIC WIN32 _DEBUG CONSOLE LWRB_DEV)
    target_compile_options(lwrb PUBLIC -Wall -Wextra -Wpedantic)
endif()