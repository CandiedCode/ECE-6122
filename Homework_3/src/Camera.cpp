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

Camera::Camera(glm::vec3 position, glm::vec3 world_up, float yaw, float pitch)
    : position_(position), world_up_(world_up), yaw_(yaw), pitch_(pitch), movement_speed_(5.0F), front_(0.0F, 0.0F, -1.0F)
{
    UpdateCameraVectors();
}

auto Camera::GetViewMatrix() const -> glm::mat4
{
    return glm::lookAt(position_, position_ + front_, up_);
}

void Camera::ProcessKeyboard(int direction, float deltaTime)
{
    float velocity = movement_speed_ * deltaTime;
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
        break;  // No action for unknown direction
    }
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch)
{
    const float sensitivity = 0.1F;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw_ += xOffset;
    pitch_ += yOffset;

    if (constrainPitch) {
        pitch_ = std::clamp(pitch_, -89.0F, 89.0F);
    }

    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset)
{
    movement_speed_ += yOffset * 0.5F;
    movement_speed_ = std::clamp(movement_speed_, 1.0F, 20.0F);
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = std::cos(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
    front.y = std::sin(glm::radians(pitch_));
    front.z = std::sin(glm::radians(yaw_)) * std::cos(glm::radians(pitch_));
    front_ = glm::normalize(front);

    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_ = glm::normalize(glm::cross(right_, front_));
}
