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
void         initialize_plane_VAO(unsigned int& vao);
void         initialize_cube_VAO(unsigned int& vao);
void         draw_stuff(unsigned int& vao, Shader& shader, glm::mat4& transform_matrix, unsigned int vertices_count,
                        unsigned int texture_id, GLenum texture_target);
unsigned int load_cubemap(std::vector<std::filesystem::path> faces);

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
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    FileSystem file_system = FileSystem::get_instance();
    file_system.set_root_marker("vcpkg.json");

    std::filesystem::path vertex_shader_path   = file_system.get_path("shaders/vertex.glsl");
    std::filesystem::path fragment_shader_path = file_system.get_path("shaders/fragment.glsl");
    Shader                shader(vertex_shader_path.c_str(), fragment_shader_path.c_str());

    std::filesystem::path skybox_vertex_shader_path   = file_system.get_path("shaders/skybox_vertex.glsl");
    std::filesystem::path skybox_fragment_shader_path = file_system.get_path("shaders/skybox_fragment.glsl");
    Shader                skybox_shader(skybox_vertex_shader_path.c_str(), skybox_fragment_shader_path.c_str());

    shader.use();
    shader.setInt("texture1", 0);

    skybox_shader.use();
    skybox_shader.setInt("skybox_texture", 0);

    unsigned int plane_VAO, cube_VAO;
    initialize_plane_VAO(plane_VAO);
    initialize_cube_VAO(cube_VAO);

    std::filesystem::path              cube_texture_path  = file_system.get_path("resources/textures/container.jpg");
    std::filesystem::path              plane_texture_path = file_system.get_path("resources/textures/metal.png");
    std::vector<std::filesystem::path> skybox_textures    = {
            file_system.get_path("resources/textures/skybox/right.jpg"),
            file_system.get_path("resources/textures/skybox/left.jpg"),
            file_system.get_path("resources/textures/skybox/top.jpg"),
            file_system.get_path("resources/textures/skybox/bottom.jpg"),
            file_system.get_path("resources/textures/skybox/front.jpg"),
            file_system.get_path("resources/textures/skybox/back.jpg"),
    };

    unsigned int cube_texture   = load_texture(cube_texture_path.c_str());
    unsigned int plane_texture  = load_texture(plane_texture_path.c_str());
    unsigned int skybox_texture = load_cubemap(skybox_textures);

    while (!glfwWindowShouldClose(window))
    {
        float current_frame = glfwGetTime();
        delta_time          = current_frame - last_frame;
        last_frame          = current_frame;
        processInput(window, camera);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glViewport(0, 0, camera->camera_width, camera->camera_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 view       = camera->get_view_matrix();
        glm::mat4 projection = camera->get_projection_matrix();

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        glm::mat4 plane_model_matrix = glm::mat4(1.0f);
        draw_stuff(plane_VAO, shader, plane_model_matrix, 6, plane_texture, GL_TEXTURE_2D);

        glm::mat4 cube_model_matrix = glm::mat4(1.0f);
        cube_model_matrix           = glm::translate(cube_model_matrix, glm::vec3(-1.0f, 0.0f, -1.0f));
        draw_stuff(cube_VAO, shader, cube_model_matrix, 36, cube_texture, GL_TEXTURE_2D);

        cube_model_matrix = glm::mat4(1.0f);
        cube_model_matrix = glm::translate(cube_model_matrix, glm::vec3(2.0f, 0.0f, 0.0f));
        draw_stuff(cube_VAO, shader, cube_model_matrix, 36, cube_texture, GL_TEXTURE_2D);

        glDepthMask(false);
        skybox_shader.use();
        glm::mat4 skybox_view = glm::mat4(glm::mat3(view));
        skybox_shader.setMat4("view", skybox_view);
        skybox_shader.setMat4("projection", projection);
        glm::mat4 skybox_model_matrix = glm::mat4(1.0f);
        draw_stuff(cube_VAO, skybox_shader, skybox_model_matrix, 36, skybox_texture, GL_TEXTURE_CUBE_MAP);
        glDepthMask(true);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    delete camera;
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

void initialize_plane_VAO(unsigned int& vao)
{
    float plane_vertices[] = {
            5.0f,  -0.5001f, 5.0f,  2.0f, 0.0f, 5.0f,  -0.5001f, -5.0f, 2.0f, 2.0f, -5.0f, -0.5001f, -5.0f, 0.0f, 2.0f,
            -5.0f, -0.5001f, -5.0f, 0.0f, 2.0f, -5.0f, -0.5001f, 5.0f,  0.0f, 0.0f, 5.0f,  -0.5001f, 5.0f,  2.0f, 0.0f,
    };

    unsigned int vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), &plane_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glBindVertexArray(0);
}

void initialize_cube_VAO(unsigned int& vao)
{
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

    unsigned int vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), &cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glBindVertexArray(0);
}

void draw_stuff(unsigned int& vao, Shader& shader, glm::mat4& transform_matrix, unsigned int vertices_count,
                unsigned int texture_id, GLenum texture_target)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(texture_target, texture_id);
    glBindVertexArray(vao);
    shader.setMat4("model", transform_matrix);
    glDrawArrays(GL_TRIANGLES, 0, vertices_count);
}

unsigned int load_cubemap(std::vector<std::filesystem::path> faces)
{
    stbi_set_flip_vertically_on_load(false);

    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    int width, height, nr_channels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nr_channels, 0);

        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                         data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load cubemap at path: " << faces[i].string() << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_set_flip_vertically_on_load(true);
    return texture_id;
}
