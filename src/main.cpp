#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstddef>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <ostream>
#include <stb_image.h>
#include <string>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "learn_opengl/camera.hpp"
#include "learn_opengl/shader.hpp"

struct PointLight
{
    glm::vec3 position;
    glm::vec3 color;

    // Attenuation value
    // F_att = 1 / (d ^ 2 * a + d * b + c)
    float a;
    float b;
    float c;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

const int W_WIDTH = 1600;
const int W_HEIGHT = 900;
const char* W_NAME = "Test";

// MOUSE
float last_mouse_X = (float) W_WIDTH / 2;
float last_mouse_Y = (float) W_HEIGHT / 2;
bool mouse_entered = false;

// DELTA TIME
float delta_time = 0.0f;
float last_frame = 0.0f;

bool is_spotlight_on = false;

void framebuffer_size_callback(GLFWwindow* window, int w, int h);
void processInput(GLFWwindow* window, Camera* camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
unsigned int load_texture(char const* path);
void set_spotlight(bool state);

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

    Camera* camera = new Camera();
    camera->camera_width = (float) W_WIDTH;
    camera->camera_height = (float) W_HEIGHT;
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

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Getting shader ready
    Shader shader("../shaders/vertex.glsl", "../shaders/fragment.glsl");
    Shader light_shader("../shaders/light_vertex.glsl", "../shaders/light_fragment.glsl");

    float vertices[] = {
            // positions          // normals           // texture coords
            -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 0.0f,
            0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 1.0f, 1.0f,
            -0.5f, 0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
            -0.5f, 0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

            -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, -1.0f, 0.0f,  0.0f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f, -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f,  -1.0f, 0.0f,  0.0f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            0.5f,  -0.5f, 0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  1.0f, 1.0f,
            0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.0f, 1.0f,

            -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
            -0.5f, 0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.0f, 1.0f};

    unsigned int VBO, cube_VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &cube_VAO);

    glBindVertexArray(cube_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int light_VAO;
    glGenVertexArrays(1, &light_VAO);
    glBindVertexArray(light_VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    std::string texture_path = "../resources/textures/container2.png";
    unsigned int diffuseMap = load_texture(texture_path.c_str());

    texture_path = "../resources/textures/container2_specular.png";
    unsigned int specularMap = load_texture(texture_path.c_str());

    shader.use();

    // ----------------------- MATERIAL ------------------------
    shader.setFloat("material.shininess", 32.0f);
    shader.setInt("material.diffuse", 0);
    shader.setInt("material.specular", 1);

    // ------------------ DIRECTIONAL LIGHT --------------------
    glm::vec3 directional_light_color = glm::vec3(1.0f, 1.0f, 1.0f);
    shader.setVec3("directional_light.direction", glm::vec3(0.0f, 0.0f, -1.0f));
    shader.setVec3("directional_light.ambient", glm::vec3(0.1f, 0.1f, 0.1f) * directional_light_color);
    shader.setVec3("directional_light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f) * directional_light_color);
    shader.setVec3("directional_light.specular", glm::vec3(1.0f, 1.0f, 1.0f) * directional_light_color);

    // ---------------------- SPOTLIGHT ------------------------
    glm::vec3 spot_light_color = glm::vec3(1.0f, 1.0f, 1.0f);
    shader.setVec3("spot_light.ambient", glm::vec3(0.0f, 0.0f, 0.0f) * spot_light_color);
    shader.setVec3("spot_light.diffuse", glm::vec3(1.5f, 1.5f, 1.5f) * spot_light_color);
    shader.setVec3("spot_light.specular", glm::vec3(1.0f, 1.0f, 1.0f) * spot_light_color);
    shader.setFloat("spot_light.a", 0.0075f);
    shader.setFloat("spot_light.b", 0.045f);
    shader.setFloat("spot_light.c", 1.0f);
    shader.setFloat("spot_light.cut_off", glm::cos(glm::radians(6.5f)));
    shader.setFloat("spot_light.outer_cut_off", glm::cos(glm::radians(8.0f)));
    shader.setBool("spot_light.enable", false);

    // -------------------- POINT LIGHTS -----------------------
    const int POINT_LIGHTS_NR = 4;
    PointLight point_lights[] = {
            {
                    glm::vec3(0.7f, 0.2f, 2.0f),
                    glm::vec3(1.0f, 1.0f, 1.0f),
                    0.032f,
                    0.09f,
                    1.0f,
                    glm::vec3(0.1f, 0.1f, 0.1f),
                    glm::vec3(0.7f, 0.7f, 0.7f),
                    glm::vec3(1.0f, 1.0f, 1.0f),
            },
            {
                    glm::vec3(2.3f, -3.3f, -4.0f),
                    glm::vec3(1.0f, 1.0f, 1.0f),
                    0.032f,
                    0.09f,
                    1.0f,
                    glm::vec3(0.1f, 0.1f, 0.1f),
                    glm::vec3(0.7f, 0.7f, 0.7f),
                    glm::vec3(1.0f, 1.0f, 1.0f),
            },
            {
                    glm::vec3(-4.0f, 2.0f, -12.0f),
                    glm::vec3(1.0f, 1.0f, 1.0f),
                    0.032f,
                    0.09f,
                    1.0f,
                    glm::vec3(0.1f, 0.1f, 0.1f),
                    glm::vec3(0.7f, 0.7f, 0.7f),
                    glm::vec3(1.0f, 1.0f, 1.0f),
            },
            {
                    glm::vec3(0.0f, 0.0f, -3.0f),
                    glm::vec3(1.0f, 1.0f, 1.0f),
                    0.032f,
                    0.09f,
                    1.0f,
                    glm::vec3(0.1f, 0.1f, 0.1f),
                    glm::vec3(0.7f, 0.7f, 0.7f),
                    glm::vec3(1.0f, 1.0f, 1.0f),
            },
    };
    for (int i = 0; i < POINT_LIGHTS_NR; i++)
    {
        std::string point_light_indexer = "point_lights[" + std::to_string(i) + "]";

        shader.setVec3(point_light_indexer + ".position", point_lights[i].position);

        shader.setFloat(point_light_indexer + ".a", point_lights[i].a);
        shader.setFloat(point_light_indexer + ".b", point_lights[i].b);
        shader.setFloat(point_light_indexer + ".c", point_lights[i].c);

        shader.setVec3(point_light_indexer + ".ambient", point_lights[i].ambient);
        shader.setVec3(point_light_indexer + ".diffuse", point_lights[i].diffuse);
        shader.setVec3(point_light_indexer + ".specular", point_lights[i].specular);
    }

    glm::vec3 cube_positions[] = {glm::vec3(0.0f, 0.0f, 0.0f),
                                  glm::vec3(2.0f, 5.0f, -15.0f),
                                  glm::vec3(-1.5f, -2.2f, -2.5f),
                                  glm::vec3(-3.8f, -2.0f, -12.3f),
                                  glm::vec3(2.4f, -0.4f, -3.5f),
                                  glm::vec3(-1.7f, 3.0f, -7.5f),
                                  glm::vec3(1.3f, -2.0f, -2.5f),
                                  glm::vec3(1.5f, 2.0f, -2.5f),
                                  glm::vec3(1.5f, 0.2f, -1.5f),
                                  glm::vec3(-1.3f, 1.0f, -1.5f)};

    while (!glfwWindowShouldClose(window))
    {
        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        processInput(window, camera);

        shader.use();

        glm::mat4 view = camera->get_view_matrix();
        shader.setMat4("view", view);

        glm::mat4 projection = camera->get_projection_matrix();
        shader.setMat4("projection", projection);

        shader.setVec3("u_view_pos", camera->camera_position);

        shader.setVec3("spot_light.direction", camera->camera_direction);
        shader.setVec3("spot_light.position", camera->camera_position);
        shader.setBool("spot_light.enable", is_spotlight_on);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        for (int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cube_positions[i]);

            float angle_in_radians = 20.0f * i;
            glm::vec3 rotation_axis = glm::vec3(1, 1, 1);
            model = glm::rotate(model, angle_in_radians, rotation_axis);
            shader.setMat4("model", model);

            glBindVertexArray(cube_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        light_shader.use();
        light_shader.setMat4("projection", projection);
        light_shader.setMat4("view", view);

        for (int i = 0; i < POINT_LIGHTS_NR; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::mat4(1.0f);
            model = glm::translate(model, point_lights[i].position);
            model = glm::scale(model, glm::vec3(0.1f));
            light_shader.setMat4("model", model);
            light_shader.setVec3("light_color", point_lights[i].color);

            glBindVertexArray(light_VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cube_VAO);
    glDeleteVertexArrays(1, &light_VAO);
    glDeleteBuffers(1, &VBO);
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
        last_mouse_X = xpos;
        last_mouse_Y = ypos;
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

    if (button == GLFW_MOUSE_BUTTON_1)
    {
        bool spotlight_enable = action == GLFW_PRESS ? true : false;
        set_spotlight(spotlight_enable);
    }
}

unsigned int load_texture(char const* path)
{
    unsigned int texture_id;
    glGenTextures(1, &texture_id);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format = GL_RED;
        if (nrComponents == 1)
        {
            format = GL_RED;
        }
        else if (nrComponents == 3)
        {
            format = GL_RGB;
        }
        else if (nrComponents == 4)
        {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

void set_spotlight(bool state)
{
    is_spotlight_on = state;
}
