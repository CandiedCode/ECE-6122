/**
 * Author: Jennifer Cwagenberg
 * Class: ECE6122
 * Last Date Modified: 2026-03-16
 * Description: Homework 3: 3D World Scene Rendering with OpenGL and Assimp
 *
 * @file SceneObject.h
 * @brief Scene object with transform data
 */

#ifndef HOMEWORK_3_SRC_SCENEOBJECT_H_
#define HOMEWORK_3_SRC_SCENEOBJECT_H_

#include "Model.h"
#include <glm/glm.hpp>

struct SceneObject
{
    Model *model;
    glm::vec3 position;
    float rotation_degrees;
    glm::vec3 rotation_axis;
    glm::vec3 scale;
    float shininess;
};

#endif // HOMEWORK_3_SRC_SCENEOBJECT_H_
