cmake_minimum_required(VERSION 3.16)

project(opengl_learn
    VERSION 0.1.0
    LANGUAGES C CXX
)

# ---- Tooling ----
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---- Language standard ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- GLFW ----
set(GLFW3_DIR "${CMAKE_SOURCE_DIR}/lib/cmake/glfw3")
find_package(glfw3 CONFIG REQUIRED)

# ---- GLAD ----
file(GLOB GLAD_SOURCES CONFIGURE_DEPENDS
    src/glad.c
)

add_library(glad STATIC
    ${GLAD_SOURCES}
)

target_include_directories(glad
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

# ---- Application (auto scan) ----
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
    src/*.c
)

add_executable(main
    ${APP_SOURCES}
)

target_include_directories(main
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
)

# Prevent GLFW from including OpenGL headers
target_compile_definitions(main
    PRIVATE
        GLFW_INCLUDE_NONE
)

target_link_libraries(main
    PRIVATE
        glad
        glfw
)

# ---- Platform-specific OpenGL ----
if (APPLE)
    target_link_libraries(main PRIVATE "-framework OpenGL")
elseif (UNIX)
    target_link_libraries(main PRIVATE GL)
elseif (WIN32)
    target_link_libraries(main PRIVATE opengl32)
endif()
