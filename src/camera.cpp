#include "camera.hpp"

#include "glm/common.hpp"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include "glm/trigonometric.hpp"

const glm::vec3 DEFAULT_CAMERA_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 DEFAULT_WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);
const float DEFAULT_YAW = -90.0f;
const float DEFAULT_PITCH = 0.0f;
const float DEFAULT_SPEED = 2.5f;
const float DEFAULT_SENSITIVITY_X = 0.05f;
const float DEFAULT_SENSITIVITY_Y = 0.03f;
const float DEFAULT_FOV = 60.0f;

const float DEFAULT_CAMERA_WIDTH = 800;
const float DEFAULT_CAMERA_HEIGHT = 600;
const float DEFAULT_CAMERA_NEAR_PLANE = 0.1f;
const float DEFAULT_CAMERA_FAR_PLANE = 100.0f;

const float PITCH_MAX = 89.0f;
const float PITCH_MIN = -89.0f;
const float FOV_MAX = 90.0f;
const float FOV_MIN = 30.0f;

Camera::Camera()
{
    camera_position = DEFAULT_CAMERA_POSITION;
    world_up = DEFAULT_WORLD_UP;

    yaw = DEFAULT_YAW;
    pitch = DEFAULT_PITCH;
    fov = DEFAULT_FOV;

    movement_speed = DEFAULT_SPEED;
    mouse_sensitivity_x = DEFAULT_SENSITIVITY_X;
    mouse_sensitivity_y = DEFAULT_SENSITIVITY_Y;

    camera_width = DEFAULT_CAMERA_WIDTH;
    camera_height = DEFAULT_CAMERA_HEIGHT;
    camera_near_plane = DEFAULT_CAMERA_NEAR_PLANE;
    camera_far_plane = DEFAULT_CAMERA_FAR_PLANE;

    update_camera_vectors();
}

Camera::Camera(const glm::vec3 p_camera_position,
               const glm::vec3 p_world_up,
               const float p_yaw,
               const float p_pitch,
               const float p_fov,
               const float p_movement_speed,
               const float p_mouse_sensitivity_x,
               const float p_mouse_sensitivity_y,
               const float p_camera_width,
               const float p_camera_height,
               const float p_camera_near_plane,
               const float p_camera_far_plane)
{
    camera_position = p_camera_position;
    world_up = p_world_up;

    yaw = p_yaw;
    pitch = p_pitch;
    fov = p_fov;

    movement_speed = p_movement_speed;
    mouse_sensitivity_x = p_mouse_sensitivity_x;
    mouse_sensitivity_y = p_mouse_sensitivity_y;

    camera_width = p_camera_width;
    camera_height = p_camera_height;
    camera_near_plane = p_camera_near_plane;
    camera_far_plane = p_camera_far_plane;

    update_camera_vectors();
}

const glm::mat4 Camera::get_view_matrix()
{
    return glm::lookAt(camera_position, camera_position + camera_direction, world_up);
}

const glm::mat4 Camera::get_projection_matrix()
{
    return glm::perspective(glm::radians(fov), camera_width / camera_height, camera_near_plane, camera_far_plane);
}

void Camera::process_keyboard(CameraMovement p_direction, float p_delta_time)
{
    float velocity = movement_speed * p_delta_time;

    if (p_direction == FORWARD)
    {
        camera_position += camera_direction * velocity;
    }
    if (p_direction == BACKWARD)
    {
        camera_position -= camera_direction * velocity;
    }
    if (p_direction == LEFT)
    {
        camera_position -= camera_right * velocity;
    }
    if (p_direction == RIGHT)
    {
        camera_position += camera_right * velocity;
    }
}

void Camera::process_mouse_movement(float p_x_offset, float p_y_offset, GLboolean p_constrain_pitch)
{
    float x_offset = p_x_offset * mouse_sensitivity_x;
    float y_offset = p_y_offset * mouse_sensitivity_y;

    yaw += x_offset;
    pitch += y_offset;

    if (p_constrain_pitch)
    {
        pitch = glm::max(pitch, PITCH_MIN);
        pitch = glm::min(pitch, PITCH_MAX);
    }

    update_camera_vectors();
}

void Camera::process_mouse_scroll(float p_y_offset)
{
    fov -= (float) p_y_offset;

    fov = glm::max(fov, FOV_MIN);
    fov = glm::min(fov, FOV_MAX);
}

void Camera::update_camera_vectors()
{
    glm::vec3 direction;

    direction.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    direction.y = glm::sin(glm::radians(pitch));
    direction.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

    camera_direction = glm::normalize(direction);
    camera_right = glm::normalize(glm::cross(camera_direction, world_up));
    camera_up = glm::normalize(glm::cross(camera_right, camera_direction));
}
