#pragma once
#include <glm/glm.hpp>

struct BoidComponent
{
    glm::vec2 velocity;
    glm::vec2 acceleration;
    float drag = 0.0f;
    float speed = 0.0f;
    float maxSpeed = 0.0f;
};