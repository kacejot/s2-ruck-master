cmake_minimum_required(VERSION 3.18)

project(s2-ruck-master)

add_compile_definitions(UE_BUILD_SHIPPING=1)
add_compile_definitions(PLATFORM_WINDOWS PLATFORM_MICROSOFT OVERRIDE_PLATFORM_HEADER_NAME=Windows UBT_COMPILED_PLATFORM=Win64)

add_subdirectory(deps/imgui)
add_subdirectory(deps/minhook)
add_subdirectory(deps/pluginloader)

file(GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_library(${PROJECT_NAME} STATIC
    ${SOURCES}
)

pluginloader_add(d3d11 LOADER_ROLE)

target_link_libraries(${PROJECT_NAME} PUBLIC
    minhook
)

target_link_libraries(pluginloader_d3d11 PUBLIC
    ${PROJECT_NAME}
)