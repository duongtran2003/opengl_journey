cmake_minimum_required(VERSION 3.15)
project(learn_opengl LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad     
    ${CMAKE_CURRENT_SOURCE_DIR}/external/stb_image     
)

target_link_libraries(main PRIVATE 
    glfw 
    assimp::assimp
    glm::glm
)

if (UNIX)
    target_link_libraries(main PRIVATE GL dl)
elseif (WIN32)
    target_link_libraries(main PRIVATE opengl32)
endif()
