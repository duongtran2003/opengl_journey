cmake_minimum_required(VERSION 3.16)
project(opengl_learn LANGUAGES C CXX)

# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- GLFW ----
# Tell CMake where glfwConfig.cmake lives
set(GLFW3_DIR "${CMAKE_SOURCE_DIR}/lib/cmake/glfw3")
find_package(glfw3 CONFIG REQUIRED)

# ---- GLAD ----
add_library(glad STATIC
    src/glad.c
)

target_include_directories(glad PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# ---- Executable ----
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(main
    ${APP_SOURCES}
)

# IMPORTANT: stop GLFW from including OpenGL headers
target_compile_definitions(main PRIVATE GLFW_INCLUDE_NONE)

target_include_directories(main PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(main
    glad
    glfw
)

# ---- Platform-specific OpenGL ----
if (APPLE)
    target_link_libraries(main "-framework OpenGL")
elseif (UNIX)
    target_link_libraries(main GL)
elseif (WIN32)
    target_link_libraries(main opengl32)
endif()
