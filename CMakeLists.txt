cmake_minimum_required(VERSION 3.15)

project(learn_opengl LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW3_DIR "${CMAKE_SOURCE_DIR}/lib/cmake/glfw3")
find_package(glfw3 CONFIG REQUIRED)

set(SRC_FILES
    src/main.cpp
    src/camera.cpp
    src/shader.cpp
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
)

target_link_libraries(main PRIVATE glfw)

if (APPLE)
    target_link_libraries(main PRIVATE "-framework OpenGL")
elseif (UNIX)
    target_link_libraries(main PRIVATE GL)
elseif (WIN32)
    target_link_libraries(main PRIVATE opengl32)
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
