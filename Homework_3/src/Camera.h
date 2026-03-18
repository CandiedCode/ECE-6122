/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-03-15
 * Description:  Homework 3: 3D World Scene Rendering with OpenGL and Assimp
 *
 *
 * @file Camera.h
 * @brief Camera header file for free-fly FPS camera with view matrix
 * See https://learnopengl.com/Getting-started/Camera and https://www.songho.ca/opengl/gl_camera.html to learn more about camera setup and
 * control in OpenGL
 */

#ifndef HOMEWORK_3_SRC_CAMERA_H_
#define HOMEWORK_3_SRC_CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
  public:
    static constexpr int kFORWARD = 0;
    static constexpr int kBACKWARD = 1;
    static constexpr int kLEFT = 2;
    static constexpr int kRIGHT = 3;

    // @brief Create a camera with the specified position, world up vector, yaw, and pitch
    // @details The camera will be initialized with a default front vector of (0, 0, -1) and the right and up vectors will be calculated
    // based on the provided parameters. The movement speed is set to a default value of 5.0 units per second.
    // @param position The initial position of the camera in world space
    // @param world_up The world up vector (usually (0, 1, 0))
    // @param yaw The initial yaw angle (in degrees) for the camera's orientation
    // @param pitch The initial pitch angle (in degrees) for the camera's orientation
    Camera(glm::vec3 position = glm::vec3(0.0F, 0.0F, 3.0F), glm::vec3 world_up = glm::vec3(0.0F, 1.0F, 0.0F), float yaw = -90.0F,
           float pitch = 0.0F);

    // @brief Get the view matrix calculated using the camera's position and orientation
    // @return The view matrix as a glm::mat4 that can be used in the vertex shader to transform world coordinates to view coordinates
    [[nodiscard]] auto GetViewMatrix() const -> glm::mat4;

    // @brief Get the current position of the camera
    // @return The position of the camera as a glm::vec3
    [[nodiscard]] auto GetPosition() const -> glm::vec3
    {
        return position_;
    }
    // @brief Get the current front vector of the camera
    // @return The front vector of the camera as a glm::vec3
    [[nodiscard]] auto GetFront() const -> glm::vec3
    {
        return front_;
    }
    // @brief Get the current up vector of the camera
    // @return The up vector of the camera as a glm::vec3
    [[nodiscard]] auto GetUp() const -> glm::vec3
    {
        return up_;
    }
    // @brief Get the current right vector of the camera
    // @return The right vector of the camera as a glm::vec3
    [[nodiscard]] auto GetRight() const -> glm::vec3
    {
        return right_;
    }

    // @brief Process keyboard input to move the camera in the specified direction
    // @param direction The direction to move (FORWARD, BACKWARD, LEFT, RIGHT)
    // @param deltaTime The time elapsed since the last frame (used to ensure consistent movement speed)
    void ProcessKeyboard(int direction, float deltaTime);

    // @brief Process mouse movement to rotate the camera's orientation
    // @param xOffset The horizontal mouse movement (in pixels)
    // @param yOffset The vertical mouse movement (in pixels)
    // @param constrainPitch Whether to constrain the pitch angle to prevent flipping (default: true)
    void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);

    // @brief Process mouse scroll input to zoom in/out by adjusting the camera's movement speed
    // @param yOffset The vertical scroll amount (positive for scroll up, negative for scroll down)
    void ProcessMouseScroll(float yOffset);

  private:
    // Camera attributes
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 world_up_;

    // Camera angles (Yaw: rotate around Y axis, Pitch: rotate around X axis)
    float yaw_;
    float pitch_;

    // Movement speed in units per second
    float movement_speed_;

    // @brief Update the camera's Front, Right, and Up vectors based on the current yaw and pitch angles
    void UpdateCameraVectors();
};

#endif // HOMEWORK_3_SRC_CAMERA_H_
