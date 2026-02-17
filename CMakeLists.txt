cmake_minimum_required(VERSION 3.18)

project(s2-ruck-master)

add_subdirectory(deps/pluginloader)
add_subdirectory(deps/dx12-imgui-overlay)

file(GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    dx12-imgui-overlay
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/mini/src"
)

target_compile_features(${PROJECT_NAME} PUBLIC
    cxx_std_20
)

pluginloader_add(winmm STANDALONE_ROLE)

target_link_libraries(pluginloader_winmm PUBLIC
    ${PROJECT_NAME}
)
