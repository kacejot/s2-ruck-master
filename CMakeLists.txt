cmake_minimum_required(VERSION 3.18)

project(s2-ruck-master)

add_compile_definitions(UE_BUILD_SHIPPING=1)
add_compile_definitions(PLATFORM_WINDOWS PLATFORM_MICROSOFT OVERRIDE_PLATFORM_HEADER_NAME=Windows UBT_COMPILED_PLATFORM=Win64)

add_subdirectory(deps/RE-UE4SS)
add_subdirectory(deps/minhook)

file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

file(GLOB IMGUI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui/backends/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui/backends/*.h"
)

add_library(${PROJECT_NAME} SHARED
    ${SOURCES}
    ${IMGUI_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/imgui"
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    UE4SS
    minhook
)
