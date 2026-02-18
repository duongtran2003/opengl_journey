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

#include "ext/matrix_transform.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "learn_opengl/camera.hpp"
#include "learn_opengl/model.hpp"
#include "learn_opengl/shader.hpp"

const int   W_WIDTH = 1600;
const int   W_HEIGHT = 900;
const char* W_NAME = "Test";

// MOUSE
float last_mouse_X = (float) W_WIDTH / 2;
float last_mouse_Y = (float) W_HEIGHT / 2;
bool  mouse_entered = false;

// DELTA TIME
float delta_time = 0.0f;
float last_frame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int w, int h);
void processInput(GLFWwindow* window, Camera* camera);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

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

    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    Shader shader("../shaders/vertex.glsl", "../shaders/fragment.glsl");

    std::string model_path = "../resources/models/backpack/backpack.obj";
    Model       model(model_path.c_str());

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

        glm::mat4 model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, glm::vec3(0.0f, 0.0f, 0.0f));
        model_matrix = glm::scale(model_matrix, glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setMat4("model", model_matrix);
        model.draw(shader);

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
}
