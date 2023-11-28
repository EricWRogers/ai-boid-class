#pragma once
#include <execution>

#include <Canis/Entity.hpp>
#include <Canis/Window.hpp>
#include <Canis/InputManager.hpp>
#include <Canis/AssetManager.hpp>
#include <Canis/External/entt.hpp>

#include <Canis/DataStructure/QuadTree.hpp>

#include <Canis/ECS/Components/RectTransformComponent.hpp>
#include <Canis/ECS/Components/ColorComponent.hpp>
#include <Canis/ECS/Components/Sprite2DComponent.hpp>
#include <Canis/ECS/Components/Camera2DComponent.hpp>

#include "../Components/BoidComponent.hpp"

const float MAX_ALIGNMENT_DISTANCE = 15.0f;
const float MAX_COHESION_DISTANCE = 20.0f;
const float MAX_SEPARATION_DISTANCE = 10.0f;

const float WANDER_CIRCLE_OFFSET = 50.0f;
const float WANDER_CIRCLE_RADIUS = 30.0f;
const float WANDER_ANGLE_DELTA_MAX = 2.0f;

const float USER_BEHAVIOR_WEIGHT = 0.3f;
const float SEPARATION_WEIGHT = 1.0f;
const float ALIGNMENT_WEIGHT = 0.3f;
const float COHESION_WEIGHT = 0.15f;

const float SPEED_MULTIPLIER = 100.0f;
const float DRAG = 0.95f;
const float MAXSPEED = 40.0f;


class BoidSystem : public Canis::System
{
private:
    glm::vec2 Seek(glm::vec2 _agentPosition, glm::vec2 _targetPosition)
    {
        glm::vec2 seek = _targetPosition - _agentPosition;
        return glm::normalize(seek);
    }

    glm::vec2 Alignment(entt::entity _agent, glm::vec2 _agentPosition, entt::registry &_registry)
    {
        glm::vec2 alignment = glm::vec2(0.0f);
        int numNeighbors = 0;

        auto view = _registry.view<Canis::RectTransformComponent, Canis::ColorComponent, Canis::Sprite2DComponent, BoidComponent>();
        for (auto [entity, rect_transform, color, sprite, boid] : view.each())
        {
            if (_agent != entity)
            {
                numNeighbors++;
                if (glm::distance(_agentPosition, rect_transform.position) <= MAX_ALIGNMENT_DISTANCE)
                {
                    alignment += boid.velocity;
                }
            }
        }

        if (alignment != glm::vec2(0.0f))
        {
            return glm::normalize(alignment / (numNeighbors + 0.0f));
        }

        return glm::vec2(0.0f);
    }

    glm::vec2 Cohesion(entt::entity _agent, glm::vec2 _agentPosition, entt::registry &_registry)
    {
        glm::vec2 cohesion = glm::vec2(0.0f);
        int numNeighbors = 0;

        auto view = _registry.view<Canis::RectTransformComponent, Canis::ColorComponent, Canis::Sprite2DComponent, BoidComponent>();
        for (auto [entity, rect_transform, color, sprite, boid] : view.each())
        {
            if (_agent != entity)
            {
                numNeighbors++;
                if (glm::distance(_agentPosition, rect_transform.position) <= MAX_COHESION_DISTANCE)
                {
                    cohesion += rect_transform.position;
                }
            }
        }

        cohesion /= numNeighbors;

        return glm::normalize((cohesion - _agentPosition));
    }

    glm::vec2 Separation(entt::entity _agent, glm::vec2 _agentPosition, entt::registry &_registry)
    {
        glm::vec2 separation = glm::vec2(0.0f);
        int numNeighbors = 0;

        auto view = _registry.view<Canis::RectTransformComponent, Canis::ColorComponent, Canis::Sprite2DComponent, BoidComponent>();
        for (auto [entity, rect_transform, color, sprite, boid] : view.each())
        {
            if (_agent != entity)
            {
                numNeighbors++;
                if (glm::distance(_agentPosition, rect_transform.position) <= MAX_SEPARATION_DISTANCE)
                {
                    separation += (_agentPosition - rect_transform.position);
                }
            }
        }

        if (separation != glm::vec2(0.0f))
        {
            return glm::normalize(separation);
        }

        return glm::vec2(0.0f);
    }

public:
    Canis::QuadTree *quadTree = new Canis::QuadTree(glm::vec2(0.0f), 2560.0f);
    Canis::QuadTree *nextQuadTree = new Canis::QuadTree(glm::vec2(0.0f), 2560.0f);

    glm::vec2 mouseWorldPosition;

    BoidSystem() : Canis::System() {

    }

    ~BoidSystem() {

    }

    void Create()
    {

    }

    void Ready()
    {
        Canis::GLTexture shipImage = Canis::AssetManager::GetTexture("assets/textures/PlayerShip.png")->GetTexture();
        for (int i = 0; i < 100; i++)
        {
            glm::vec2 size = glm::vec2(shipImage.width/8,shipImage.height/8);
            float x = rand() % window->GetScreenWidth() + 1.0f;
            float y = rand() % window->GetScreenHeight() + 1.0f;
            Canis::Entity e = GetScene().CreateEntity();
            Canis::RectTransformComponent rect;
            rect.active = true;
            rect.position = glm::vec2(x-(window->GetScreenWidth()/2),y-(window->GetScreenHeight()/2));
            rect.size = size;
            rect.depth = 1.0f;
            rect.scale = 0.1f;
            e.AddComponent<Canis::RectTransformComponent>(rect);
            e.AddComponent<Canis::ColorComponent>(
                glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
            );
            e.AddComponent<Canis::Sprite2DComponent>(
                glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), // uv
                shipImage // texture
            );
            e.AddComponent<BoidComponent>(
                glm::vec2(0.0f, 0.0f), // velocity
                glm::vec2(0.0f, 0.0f), // acceleration
                0.95f, // drag
                0.0f, // speed
                8.0f  // maxSpeed
            );
        }
    }

    void Update(entt::registry &_registry, float _deltaTime)
    {
        glm::vec2 seekTarget, alignmentTarget, cohesionTarget, separationTarget, cameraPosition;

        auto cam = _registry.view<Canis::Camera2DComponent>();
        for(auto[entity, camera2D] : cam.each()) {
            cameraPosition = camera2D.position;
        }

        auto view = _registry.view<Canis::RectTransformComponent, Canis::ColorComponent, Canis::Sprite2DComponent, BoidComponent>();
        for (auto [entity, rect_transform, color, sprite, boid] : view.each())
        {
            

            // SEEK
            seekTarget = Seek(rect_transform.position, inputManager->mouse+(cameraPosition-(glm::vec2(window->GetScreenWidth(), window->GetScreenHeight())/2.0f)));
            // Alignment
            alignmentTarget = Alignment( entity, rect_transform.position, _registry);
            // Cohesion
            cohesionTarget = Cohesion(entity, rect_transform.position, _registry);
            // Separation
            separationTarget = Separation(entity, rect_transform.position, _registry);
            
            boid.acceleration = (seekTarget * USER_BEHAVIOR_WEIGHT) + 
                                (alignmentTarget * ALIGNMENT_WEIGHT) + 
                                (cohesionTarget * COHESION_WEIGHT) + 
                                (separationTarget * SEPARATION_WEIGHT);

            
            rect_transform.rotation = atan2(boid.velocity.y,boid.velocity.x);

            // update velocity
            boid.velocity += boid.acceleration;

            // clamp velocity to maxSpeed
            if (glm::length(boid.velocity) > boid.maxSpeed) {
                boid.velocity = glm::normalize(boid.velocity);
                boid.velocity *= boid.maxSpeed;
            }

            // apply drag
            boid.velocity *= boid.drag;

            // update position
            rect_transform.position += boid.velocity;
        }
    }
};

bool DecodeBoidSystem(const std::string &_name, Canis::Scene *_scene)
{
    if (_name == "BoidSystem")
    {
        _scene->CreateSystem<BoidSystem>();
        return true;
    }
    return false;
}

/*
void UpdateComponents(const float deltaTime, entt::_registry &_registry)
{
    glm::vec2 seekTarget, alignmentTarget, cohesionTarget, separationTarget, cameraPosition;

    delete quadTree;
    quadTree = nextQuadTree;
    nextQuadTree = new Canis::QuadTree(glm::vec2(0.0f), 2560.0f);

    auto view = _registry.view<Canis::RectTransformComponent, BoidComponent>();
    auto cam = _registry.view<const Canis::Camera2DComponent>();

    float cameraSize;
    for (auto [entity, camera2D] : cam.each())
    {
        cameraPosition = camera2D.position;
        cameraSize = camera2D.scale;
    }

    mouseWorldPosition = (input->mouse*2.0f) + cameraPosition-(glm::vec2(window->GetScreenWidth()*2.0f, window->GetScreenHeight()*2.0f)/2.0f);

    glm::vec2 alignment = glm::vec2(0.0f);
    glm::vec2 cohesion = glm::vec2(0.0f);
    glm::vec2 separation = glm::vec2(0.0f);
    glm::vec2 acceleration;

    int alignNumNeighbors = 0;
    int cohNumNeighbors = 0;
    int sepNumNeighbors = 0;

    float distance = 0.0f;

    std::vector<Canis::QuadPoint> quadPoints = {};

    for (auto [entity, rect_transform, boid] : view.each())
    {
        alignment = glm::vec2(0.0f);
        cohesion = glm::vec2(0.0f);
        separation = glm::vec2(0.0f);
        // glm::vec2 mouseWorldPosition = input->mouse+(cameraPosition-(glm::vec2(window->GetScreenWidth(), window->GetScreenHeight())/2.0f));
        alignNumNeighbors = 0;
        cohNumNeighbors = 0;

        quadPoints.clear(); // does not unalocate the memory
        if (quadTree->PointsQuery(rect_transform.position, MAX_COHESION_DISTANCE+0.0f, quadPoints))
        {
            for (Canis::QuadPoint point : quadPoints)
            {
                distance = glm::distance(rect_transform.position, point.position);
                if (distance <= MAX_COHESION_DISTANCE && entity != point.entity)
                {
                    cohNumNeighbors++;
                    cohesion += point.position;

                    if (distance <= MAX_ALIGNMENT_DISTANCE)
                    {
                        alignNumNeighbors++;
                        alignment += _registry.get<const BoidComponent>(point.entity).velocity;

                        if (distance <= MAX_SEPARATION_DISTANCE)
                        {
                            separation += (rect_transform.position - point.position);
                        }
                    }
                }
            }
        }
        
        // Seek
        seekTarget = glm::normalize(mouseWorldPosition - rect_transform.position);
        // Alignment
        alignmentTarget = (alignment != glm::vec2(0.0f)) ? glm::normalize(alignment / (alignNumNeighbors + 0.0f)) : glm::vec2(0.0f);
        // Cohesion
        cohesionTarget = (cohNumNeighbors > 0) ? glm::normalize((cohesion/static_cast<float>(cohNumNeighbors)) - rect_transform.position) : glm::vec2(0.0f);
        // Separation
        separationTarget = (separation != glm::vec2(0.0f)) ? glm::normalize(separation) : glm::vec2(0.0f);

        acceleration = ((seekTarget * USER_BEHAVIOR_WEIGHT) +
                            (alignmentTarget * ALIGNMENT_WEIGHT) +
                            (cohesionTarget * COHESION_WEIGHT) +
                            (separationTarget * SEPARATION_WEIGHT))
                            * SPEED_MULTIPLIER;

        rect_transform.rotation = glm::atan(boid.velocity.y, boid.velocity.x);
        

        // update velocity
        boid.velocity += (acceleration * deltaTime);

        // apply drag
        boid.velocity *= DRAG;

        // update position
        rect_transform.position += boid.velocity;
        
        nextQuadTree->AddPoint(rect_transform.position, entity);
    }
}
*/