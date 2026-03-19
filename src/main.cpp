#include <filesystem>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstddef>
#include <cstdlib>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <map>
#include <ostream>
#include <stb_image.h>
#include <string>
#include <vector>

#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "learn_opengl/camera.hpp"
#include "learn_opengl/file_system.hpp"
#include "learn_opengl/shader.hpp"

const int   W_WIDTH  = 640;
const int   W_HEIGHT = 480;
const char* W_NAME   = "Test";

// MOUSE
float last_mouse_X  = (float) W_WIDTH / 2;
float last_mouse_Y  = (float) W_HEIGHT / 2;
bool  mouse_entered = false;

// DELTA TIME
float delta_time = 0.0f;
float last_frame = 0.0f;

void         framebuffer_size_callback(GLFWwindow* window, int w, int h);
void         processInput(GLFWwindow* window, Camera* camera);
void         mouse_callback(GLFWwindow* window, double xpos, double ypos);
void         scroll_callback(GLFWwindow* window, double xpos, double ypos);
void         mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
unsigned int load_texture(const char* path);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(W_WIDTH, W_HEIGHT, W_NAME, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    Camera* camera          = new Camera();
    camera->camera_width    = (float) W_WIDTH;
    camera->camera_height   = (float) W_HEIGHT;
    camera->camera_position = glm::vec3(0.0f, 0.0f, 3.0f);

    glfwSetWindowUserPointer(window, camera);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initiate GLAD" << std::endl;
        return -1;
    }

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    FileSystem file_system = FileSystem::get_instance();
    file_system.set_root_marker("vcpkg.json");

    std::filesystem::path vertex_shader_path   = file_system.get_path("shaders/vertex.glsl");
    std::filesystem::path fragment_shader_path = file_system.get_path("shaders/fragment.glsl");
    Shader                shader(vertex_shader_path.c_str(), fragment_shader_path.c_str());

    std::filesystem::path outline_vertex_shader_path   = file_system.get_path("shaders/outline_vertex.glsl");
    std::filesystem::path outline_fragment_shader_path = file_system.get_path("shaders/outline_fragment.glsl");
    Shader                outline_shader(outline_vertex_shader_path.c_str(), outline_fragment_shader_path.c_str());

    float cube_vertices[] = {
            // back face
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // top-right
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  // bottom-right
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // top-right
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,  // top-left
            // front face
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,  // bottom-right
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // top-right
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // top-right
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,  // top-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
            // left face
            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // top-right
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // top-left
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // bottom-right
            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // top-right
            // right face
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // top-left
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // top-right
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // top-left
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // bottom-left
            // bottom face
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f,  // top-left
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   // bottom-left
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   // bottom-left
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // bottom-right
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
            // top face
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // bottom-right
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // top-right
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // bottom-right
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f   // bottom-left
    };

    float plane_vertices[] = {
            5.0f,  -0.5001f, 5.0f,  2.0f, 0.0f, 5.0f,  -0.5001f, -5.0f, 2.0f, 2.0f, -5.0f, -0.5001f, -5.0f, 0.0f, 2.0f,
            -5.0f, -0.5001f, -5.0f, 0.0f, 2.0f, -5.0f, -0.5001f, 5.0f,  0.0f, 0.0f, 5.0f,  -0.5001f, 5.0f,  2.0f, 0.0f,
    };

    // Cube VAO
    unsigned int cube_VAO, cube_VBO;
    glGenVertexArrays(1, &cube_VAO);
    glGenBuffers(1, &cube_VBO);
    glBindVertexArray(cube_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, cube_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glBindVertexArray(0);

    // Plane VAO
    unsigned int plane_VAO, plane_VBO;
    glGenVertexArrays(1, &plane_VAO);
    glGenBuffers(1, &plane_VBO);
    glBindVertexArray(plane_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, plane_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), &plane_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));

    std::filesystem::path cube_texture_path   = file_system.get_path("resources/textures/marble.jpg");
    std::filesystem::path plane_texture_path  = file_system.get_path("resources/textures/metal.png");
    std::filesystem::path grass_texture_path  = file_system.get_path("resources/textures/grass.png");
    std::filesystem::path window_texture_path = file_system.get_path("resources/textures/window.png");

    unsigned int cube_texture   = load_texture(cube_texture_path.c_str());
    unsigned int plane_texture  = load_texture(plane_texture_path.c_str());
    unsigned int grass_texture  = load_texture(grass_texture_path.c_str());
    unsigned int window_texture = load_texture(window_texture_path.c_str());

    std::vector<glm::vec3> window_locations;
    window_locations.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
    window_locations.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
    window_locations.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
    window_locations.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
    window_locations.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

    float window_vertices[] = {-0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
                               0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,
                               -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -0.5f, 0.5f,  0.0f, 0.0f, 1.0f};
    // Window VAO
    unsigned int window_VAO, window_VBO;
    glGenVertexArrays(1, &window_VAO);
    glGenBuffers(1, &window_VBO);
    glBindVertexArray(window_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, window_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(window_vertices), &window_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));

    glBindVertexArray(0);

    shader.use();
    shader.setInt("texture1", 0);

    while (!glfwWindowShouldClose(window))
    {
        float current_frame = glfwGetTime();
        delta_time          = current_frame - last_frame;
        last_frame          = current_frame;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window, camera);

        shader.use();
        glm::mat4 model = glm::mat4(1.0f);

        glm::mat4 view = camera->get_view_matrix();
        shader.setMat4("view", view);

        glm::mat4 projection = camera->get_projection_matrix();
        shader.setMat4("projection", projection);

        // Plane
        glDisable(GL_CULL_FACE);
        glBindVertexArray(plane_VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, plane_texture);

        model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Draw cube
        glEnable(GL_CULL_FACE);
        glBindVertexArray(cube_VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cube_texture);

        // First cube
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Second cube
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draw windows
        glDisable(GL_CULL_FACE);
        glBindVertexArray(window_VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, window_texture);

        std::map<float, glm::vec3> sorted_windows;
        for (auto& window_loc : window_locations)
        {
            float d           = glm::length(camera->camera_position - window_loc);
            sorted_windows[d] = window_loc;
        }

        for (auto it = sorted_windows.rbegin(); it != sorted_windows.rend(); it++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    free(camera);
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
    glViewport(0, 0, w, h);
}

void processInput(GLFWwindow* window, Camera* camera)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera->process_keyboard(FORWARD, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera->process_keyboard(BACKWARD, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera->process_keyboard(LEFT, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera->process_keyboard(RIGHT, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera->process_keyboard(UP, delta_time);
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        camera->process_keyboard(DOWN, delta_time);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));

    if (!mouse_entered)
    {
        mouse_entered = true;
        last_mouse_X  = xpos;
        last_mouse_Y  = ypos;
    }

    float x_offset = xpos - last_mouse_X;
    float y_offset = last_mouse_Y - ypos;

    last_mouse_X = xpos;
    last_mouse_Y = ypos;

    camera->process_mouse_movement(x_offset, y_offset);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    camera->process_mouse_scroll(y_offset);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_3 && action == GLFW_PRESS)
    {
        Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
        camera->reset_fov();
    }
}

unsigned int load_texture(const char* path)
{
    unsigned int texture_id;
    glGenTextures(1, &texture_id);

    int            width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);

    if (data)
    {
        GLenum      format     = GL_RED;
        std::string format_str = "GL_RED";
        if (nrComponents == 1)
        {
            format     = GL_RED;
            format_str = "GL_RED";
        }
        else if (nrComponents == 3)
        {
            format     = GL_RGB;
            format_str = "GL_RGB";
        }
        else if (nrComponents == 4)
        {
            format     = GL_RGBA;
            format_str = "GL_RGBA";
        }

        std::cout << "Loaded " << path << " with format " << format_str << std::endl;

        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        int wrapping_mode = GL_REPEAT;
        if (format == GL_RGBA)
        {
            wrapping_mode = GL_CLAMP_TO_EDGE;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping_mode);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }

    return texture_id;
}
