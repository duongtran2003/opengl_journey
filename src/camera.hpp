#pragma once

#include <glad/glad.h>

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
  public:
    glm::vec3 camera_position;
    glm::vec3 camera_direction;
    glm::vec3 camera_up;
    glm::vec3 camera_right;
    glm::vec3 world_up;

    float yaw;
    float pitch;
    float fov;

    float camera_width;
    float camera_height;
    float camera_near_plane;
    float camera_far_plane;

    float movement_speed;
    float mouse_sensitivity_x;
    float mouse_sensitivity_y;

    Camera();
    Camera(const glm::vec3 p_camera_position,
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
           const float p_camera_far_plane);

    const glm::mat4 get_view_matrix();
    const glm::mat4 get_projection_matrix();
    void clamp_camera_to_ground();
    void process_keyboard(CameraMovement p_direction, float p_delta_time);
    void process_mouse_movement(float p_x_offset, float p_y_offset, GLboolean p_constrain_pitch = true);
    void process_mouse_scroll(float p_y_offset);

  private:
    void update_camera_vectors();
};
