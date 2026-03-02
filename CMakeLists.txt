cmake_minimum_required(VERSION 3.15)

project(learn_opengl LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW3_DIR "${CMAKE_SOURCE_DIR}/lib/cmake/glfw3")
find_package(glfw3 CONFIG REQUIRED)

add_library(assimp_lib SHARED IMPORTED)
set_target_properties(assimp_lib PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/external/assimp/lib/libassimp.so"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/external/assimp"
)

set(SRC_FILES
    src/main.cpp
    src/camera.cpp
    src/shader.cpp
    src/mesh.cpp
    src/model.cpp
    external/glad/glad.c
    external/stb_image/stb_image.cpp
)

add_executable(main ${SRC_FILES})

target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/external/GLFW
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb_image
    ${CMAKE_CURRENT_SOURCE_DIR}/external/assimp
)

target_link_libraries(main PRIVATE glfw assimp_lib)

if (APPLE)
    target_link_libraries(main PRIVATE "-framework OpenGL")
elseif (UNIX)
    target_link_libraries(main PRIVATE GL)
elseif (WIN32)
    target_link_libraries(main PRIVATE opengl32)
endif()
