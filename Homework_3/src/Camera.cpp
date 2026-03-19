/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-03-15
 * Description:  Homework 3: 3D World Scene Rendering with OpenGL and Assimp
 *
 *
 * @file Camera.cpp
 * @brief Camera setup and control for free-fly FPS camera with view matrix
 */

#include "Camera.h"
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 world_up, float yaw, float pitch)
    : position_(position), front_(0.0F, 0.0F, -1.0F), world_up_(world_up), yaw_(yaw), pitch_(pitch), movement_speed_(5.0F)
{
    UpdateCameraVectors();
}

auto Camera::GetViewMatrix() const -> glm::mat4
{
    // The view matrix is calculated using the camera's position, front vector, and up vector. We use glm::lookAt to create the view matrix,
    // which transforms world coordinates to view coordinates based on the camera's orientation and position in the world. The "center"
    // parameter for lookAt is calculated as position + front, which represents the point the camera is looking at. The "up" parameter is
    // the camera's up vector, which defines the camera's vertical orientation. This view matrix will be used in the vertex shader to
    // transform vertices from world space to view space, allowing us to render the scene from the camera's perspective.
    return glm::lookAt(position_, position_ + front_, up_);
}

void Camera::ProcessDirection(int direction, float deltaTime)
{
    float velocity = movement_speed_ * deltaTime;
    std::cout << "Processing keyboard input: direction=" << direction << ", deltaTime=" << deltaTime << ", velocity=" << velocity << "\n";

    // https://github.com/jhurley-blip/ECE4122-6122-OpenGL/blob/master/common/controls.cpp#L75-L89
    switch (direction)
    {
    case kFORWARD:
        position_ += glm::normalize(glm::vec3(front_.x, 0.0F, front_.z)) * velocity;
        break;
    case kBACKWARD:
        position_ -= glm::normalize(glm::vec3(front_.x, 0.0F, front_.z)) * velocity;
        break;
    case kLEFT:
        position_ -= right_ * velocity;
        break;
    case kRIGHT:
        position_ += right_ * velocity;
        break;
    default:
        break; // No action for unknown direction
    }
}

void Camera::ProcessCameraView(float xOffset, float yOffset, bool constrainPitch)
{
    constexpr float kBaseSensitivity = 0.1F; // 1  make the view move pretty fast, 0.1 makes it more reasonable
    constexpr float kDefaultSpeed = 5.0F;

    // Scale sensitivity with movement speed (normalized to default speed)
    float sensitivity = kBaseSensitivity * (movement_speed_ / kDefaultSpeed);

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw_ += xOffset;
    pitch_ += yOffset;

    if (constrainPitch)
    {
        pitch_ = std::clamp(pitch_, -89.0F, 89.0F);
    }

    UpdateCameraVectors();
}

void Camera::ProcessMovementSpeed(float yOffset)
{
    movement_speed_ += yOffset;
    movement_speed_ = std::clamp(movement_speed_, 1.0F, 50.0F);
    std::cout << "Camera movement speed adjusted to: " << movement_speed_ << "\n";
}

void Camera::UpdateCameraVectors()
{
    // https://learnopengl.com/Getting-started/Camera
    glm::vec3 direction;
    direction.x = std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
    direction.y = std::sin(glm::radians(pitch_));
    direction.z = std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
    front_ = glm::normalize(direction);

    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_ = glm::normalize(glm::cross(right_, front_));
}
