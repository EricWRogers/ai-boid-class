#pragma once
#include <execution>
#include <emmintrin.h>
#include <immintrin.h>

#include <SDL.h>

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


struct BoidThreadInfo
{
    void *boidSystem;
    entt::registry *reg;
    std::vector<entt::entity> *boids;
    Canis::QuadTree::QuadTreeData *quadTree;
    unsigned int startIndex = 0;
    unsigned int endIndex = 0;
    glm::vec2 mouseWorldPosition;
    float deltaTime;
};

static int BoidThreadUpdate(void *_info)
{
    BoidThreadInfo *boidThreadInfo = static_cast<BoidThreadInfo *>(_info);
    glm::vec2 seekTarget, alignmentTarget, cohesionTarget, separationTarget = glm::vec2(0.0f);

    glm::vec2 alignment = glm::vec2(0.0f);
    glm::vec2 cohesion = glm::vec2(0.0f);
    glm::vec2 separation = glm::vec2(0.0f);
    glm::vec2 acceleration;

    int alignNumNeighbors = 0;
    int cohNumNeighbors = 0;
    int sepNumNeighbors = 0;

    float distance = 0.0f;
    std::vector<Canis::QuadTree::QuadPoint> quadPoints = {};
    std::vector<unsigned int> queue = {};

    int max = boidThreadInfo->endIndex;
    for (int i = boidThreadInfo->startIndex; i < max; i++)
    {
        auto [rect_transform, boid] = boidThreadInfo->reg->get<Canis::RectTransformComponent, BoidComponent>((*boidThreadInfo->boids)[i]);
        alignment = glm::vec2(0.0f);
        cohesion = glm::vec2(0.0f);
        separation = glm::vec2(0.0f);
        // glm::vec2 mouseWorldPosition = input->mouse+(cameraPosition-(glm::vec2(window->GetScreenWidth(), window->GetScreenHeight())/2.0f));
        alignNumNeighbors = 0;
        cohNumNeighbors = 0;

        quadPoints.clear(); // does not unalocate the memory
        if (Canis::QuadTree::PointsQueryFast(*(boidThreadInfo->quadTree), queue, rect_transform.position, MAX_COHESION_DISTANCE + 0.0f, quadPoints))
        {
            int quadPointSize = quadPoints.size();
            for (int p = 0; p < quadPointSize; p++)//Canis::QuadPoint point : quadPoints)
            {
                distance = glm::distance(rect_transform.position, quadPoints[p].position);
                if (distance <= MAX_COHESION_DISTANCE && (*boidThreadInfo->boids)[i] != quadPoints[p].entity)
                {
                    cohNumNeighbors++;
                    cohesion += quadPoints[p].position;

                    if (distance <= MAX_ALIGNMENT_DISTANCE)
                    {
                        alignNumNeighbors++;
                        alignment += quadPoints[p].velocity;

                        if (distance <= MAX_SEPARATION_DISTANCE)
                        {
                            separation += (rect_transform.position - quadPoints[p].position);
                        }
                    }
                }
            }
        }

        // Seek
        seekTarget = glm::normalize(boidThreadInfo->mouseWorldPosition - rect_transform.position);
        // Alignment
        alignmentTarget = (alignment != glm::vec2(0.0f)) ? glm::normalize(alignment / (alignNumNeighbors + 0.0f)) : glm::vec2(0.0f);
        // Cohesion
        cohesionTarget = (cohNumNeighbors > 0) ? glm::normalize((cohesion / static_cast<float>(cohNumNeighbors)) - rect_transform.position) : glm::vec2(0.0f);
        // Separation
        separationTarget = (separation != glm::vec2(0.0f)) ? glm::normalize(separation) : glm::vec2(0.0f);

        acceleration = ((seekTarget * USER_BEHAVIOR_WEIGHT) +
                        (alignmentTarget * ALIGNMENT_WEIGHT) +
                        (cohesionTarget * COHESION_WEIGHT) +
                        (separationTarget * SEPARATION_WEIGHT)) *
                        SPEED_MULTIPLIER;

        rect_transform.rotation = glm::atan(boid.velocity.y, boid.velocity.x);

        // update velocity
        boid.velocity += (acceleration * boidThreadInfo->deltaTime);

        // clamp velocity to maxSpeed
        //if (glm::length(boid.velocity) > MAXSPEED)
        //{
        //    boid.velocity = glm::normalize(boid.velocity) * MAXSPEED;
        //}

        // apply drag
        boid.velocity *= DRAG;

        // update position
        rect_transform.position += boid.velocity;
    }
    return 0;
}


class BoidSystem : public Canis::System
{
private:
    

public:
    Canis::QuadTree::QuadTreeData *quadTree = new Canis::QuadTree::QuadTreeData;;
    Canis::QuadTree::QuadTreeData *nextQuadTree = new Canis::QuadTree::QuadTreeData;;

    float dt;
    entt::registry *reg;
    
    glm::vec2 mouseWorldPosition;
    glm::vec2 cameraPosition;
    std::vector<entt::entity> boidEntities = {};

    SDL_Thread *threadID00 = 0;
    SDL_Thread *threadID01 = 0;
    SDL_Thread *threadID02 = 0;
    SDL_Thread *threadID03 = 0;
    SDL_Thread *threadID04 = 0;
    SDL_Thread *threadID05 = 0;
    SDL_Thread *threadID06 = 0;
    SDL_Thread *threadID07 = 0;
    SDL_Thread *threadID08 = 0;
    SDL_Thread *threadID09 = 0;
    SDL_Thread *threadID10 = 0;
    SDL_Thread *threadID11 = 0;
    SDL_Thread *threadID12 = 0;
    SDL_Thread *threadID13 = 0;
    SDL_Thread *threadID14 = 0;
    SDL_Thread *threadID15 = 0;
    SDL_Thread *threadID16 = 0;
    SDL_Thread *threadID17 = 0;
    SDL_Thread *threadID18 = 0;
    SDL_Thread *threadID19 = 0;
    SDL_Thread *threadID20 = 0;
    SDL_Thread *threadID21 = 0;
    SDL_Thread *threadID22 = 0;
    SDL_Thread *threadID23 = 0;
    SDL_Thread *threadID24 = 0;
    SDL_Thread *threadID25 = 0;
    SDL_Thread *threadID26 = 0;
    SDL_Thread *threadID27 = 0;
    SDL_Thread *threadID28 = 0;
    SDL_Thread *threadID29 = 0;
    SDL_Thread *threadID30 = 0;
    SDL_Thread *threadID31 = 0;

    Canis::InputManager *input;

    float boidCount = 200000;

    BoidThreadInfo BuildInfo(float boidCount, float threadCount, float currentThread) {
        BoidThreadInfo boidThreadInfo;
        boidThreadInfo.boidSystem = this;
        boidThreadInfo.reg = reg;
        boidThreadInfo.boids = &boidEntities;
        boidThreadInfo.deltaTime = dt;
        boidThreadInfo.quadTree = quadTree;
        boidThreadInfo.mouseWorldPosition = mouseWorldPosition;
        boidThreadInfo.startIndex = (boidCount/threadCount) * (0+currentThread);
        boidThreadInfo.endIndex = (boidCount/threadCount) * (1+currentThread);
        return boidThreadInfo;
    }

    glm::vec2 seekTarget, alignmentTarget, cohesionTarget, separationTarget;

    BoidSystem() : Canis::System() {
        Canis::QuadTree::Init(*quadTree, glm::vec2(0.0f), 2560.0f);
        Canis::QuadTree::Init(*nextQuadTree, glm::vec2(0.0f), 2560.0f);
    }

    ~BoidSystem() {
        delete quadTree;
        delete nextQuadTree;
    }

    void Create()
    {
        
    }

    void Ready()
    {
        Canis::GLTexture shipImage = Canis::AssetManager::GetTexture("assets/textures/PlayerShip.png")->GetTexture();
        for (int i = 0; i < boidCount; i++)
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
                glm::vec2(0.0f, 0.0f) // velocity
            );

            boidEntities.push_back(e.entityHandle);
        }
    }

    void Update(entt::registry &_registry, float _deltaTime)
    {
        Canis::QuadTree::QuadTreeData *tempTree = quadTree;
        quadTree = nextQuadTree;
        nextQuadTree = tempTree;
        Canis::QuadTree::Reset(*nextQuadTree);

        dt = _deltaTime;
        reg = &_registry;

        input = inputManager;

        auto cam = _registry.view<const Canis::Camera2DComponent>();

        float cameraSize;
        for (auto [entity, camera2D] : cam.each())
        {
            cameraPosition = camera2D.position;
            cameraSize = camera2D.scale;
        }

        mouseWorldPosition = (input->mouse * 2.0f) + cameraPosition - (glm::vec2(window->GetScreenWidth() * 2.0f, window->GetScreenHeight() * 2.0f) / 2.0f);
        float threadCount = 32;

        BoidThreadInfo threadInfoID00 = BuildInfo(boidCount,threadCount,0);
        BoidThreadInfo threadInfoID01 = BuildInfo(boidCount,threadCount,1);
        BoidThreadInfo threadInfoID02 = BuildInfo(boidCount,threadCount,2);
        BoidThreadInfo threadInfoID03 = BuildInfo(boidCount,threadCount,3);
        BoidThreadInfo threadInfoID04 = BuildInfo(boidCount,threadCount,4);
        BoidThreadInfo threadInfoID05 = BuildInfo(boidCount,threadCount,5);
        BoidThreadInfo threadInfoID06 = BuildInfo(boidCount,threadCount,6);
        BoidThreadInfo threadInfoID07 = BuildInfo(boidCount,threadCount,7);
        BoidThreadInfo threadInfoID08 = BuildInfo(boidCount,threadCount,8);
        BoidThreadInfo threadInfoID09 = BuildInfo(boidCount,threadCount,9);
        BoidThreadInfo threadInfoID10 = BuildInfo(boidCount,threadCount,10);
        BoidThreadInfo threadInfoID11 = BuildInfo(boidCount,threadCount,11);
        BoidThreadInfo threadInfoID12 = BuildInfo(boidCount,threadCount,12);
        BoidThreadInfo threadInfoID13 = BuildInfo(boidCount,threadCount,13);
        BoidThreadInfo threadInfoID14 = BuildInfo(boidCount,threadCount,14);
        BoidThreadInfo threadInfoID15 = BuildInfo(boidCount,threadCount,15);
        BoidThreadInfo threadInfoID16 = BuildInfo(boidCount,threadCount,16);
        BoidThreadInfo threadInfoID17 = BuildInfo(boidCount,threadCount,17);
        BoidThreadInfo threadInfoID18 = BuildInfo(boidCount,threadCount,18);
        BoidThreadInfo threadInfoID19 = BuildInfo(boidCount,threadCount,19);
        BoidThreadInfo threadInfoID20 = BuildInfo(boidCount,threadCount,20);
        BoidThreadInfo threadInfoID21 = BuildInfo(boidCount,threadCount,21);
        BoidThreadInfo threadInfoID22 = BuildInfo(boidCount,threadCount,22);
        BoidThreadInfo threadInfoID23 = BuildInfo(boidCount,threadCount,23);
        BoidThreadInfo threadInfoID24 = BuildInfo(boidCount,threadCount,24);
        BoidThreadInfo threadInfoID25 = BuildInfo(boidCount,threadCount,25);
        BoidThreadInfo threadInfoID26 = BuildInfo(boidCount,threadCount,26);
        BoidThreadInfo threadInfoID27 = BuildInfo(boidCount,threadCount,27);
        BoidThreadInfo threadInfoID28 = BuildInfo(boidCount,threadCount,28);
        BoidThreadInfo threadInfoID29 = BuildInfo(boidCount,threadCount,29);
        BoidThreadInfo threadInfoID30 = BuildInfo(boidCount,threadCount,30);
        BoidThreadInfo threadInfoID31 = BuildInfo(boidCount,threadCount,31);

        threadID00 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID00);
        threadID01 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID01);
        threadID02 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID02);
        threadID03 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID03);
        threadID04 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID04);
        threadID05 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID05);
        threadID06 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID06);
        threadID07 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID07);
        threadID08 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID08);
        threadID09 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID09);
        threadID10 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID10);
        threadID11 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID11);
        threadID12 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID12);
        threadID13 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID13);
        threadID14 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID14);
        threadID15 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID15);
        threadID16 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID16);
        threadID17 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID17);
        threadID18 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID18);
        threadID19 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID19);
        threadID20 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID20);
        threadID21 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID21);
        threadID22 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID22);
        threadID23 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID23);
        threadID24 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID24);
        threadID25 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID25);
        threadID26 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID26);
        threadID27 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID27);
        threadID28 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID28);
        threadID29 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID29);
        threadID30 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID30);
        threadID31 = SDL_CreateThread(BoidThreadUpdate, "BoidThreadUpdate", &threadInfoID31);

        auto view = _registry.view<const Canis::RectTransformComponent, const BoidComponent>();
        for (auto [entity, rect_transform, boid] : view.each())
        {
            Canis::QuadTree::AddPoint(*nextQuadTree, rect_transform.position, entity, boid.velocity);
        }

        int threadReturnValue00;
        SDL_WaitThread(threadID00, &threadReturnValue00);
        int threadReturnValue01;
        SDL_WaitThread(threadID01, &threadReturnValue01);
        int threadReturnValue02;
        SDL_WaitThread(threadID02, &threadReturnValue02);
        int threadReturnValue03;
        SDL_WaitThread(threadID03, &threadReturnValue03);
        int threadReturnValue04;
        SDL_WaitThread(threadID04, &threadReturnValue04);
        int threadReturnValue05;
        SDL_WaitThread(threadID05, &threadReturnValue05);
        int threadReturnValue06;
        SDL_WaitThread(threadID06, &threadReturnValue06);
        int threadReturnValue07;
        SDL_WaitThread(threadID07, &threadReturnValue07);
        int threadReturnValue08;
        SDL_WaitThread(threadID08, &threadReturnValue08);
        int threadReturnValue09;
        SDL_WaitThread(threadID09, &threadReturnValue09);
        int threadReturnValue10;
        SDL_WaitThread(threadID10, &threadReturnValue10);
        int threadReturnValue11;
        SDL_WaitThread(threadID11, &threadReturnValue11);
        int threadReturnValue12;
        SDL_WaitThread(threadID12, &threadReturnValue12);
        int threadReturnValue13;
        SDL_WaitThread(threadID13, &threadReturnValue13);
        int threadReturnValue14;
        SDL_WaitThread(threadID14, &threadReturnValue14);
        int threadReturnValue15;
        SDL_WaitThread(threadID15, &threadReturnValue15);
        int threadReturnValue16;
        SDL_WaitThread(threadID16, &threadReturnValue16);
        int threadReturnValue17;
        SDL_WaitThread(threadID17, &threadReturnValue17);
        int threadReturnValue18;
        SDL_WaitThread(threadID18, &threadReturnValue18);
        int threadReturnValue19;
        SDL_WaitThread(threadID19, &threadReturnValue19);
        int threadReturnValue20;
        SDL_WaitThread(threadID20, &threadReturnValue20);
        int threadReturnValue21;
        SDL_WaitThread(threadID21, &threadReturnValue21);
        int threadReturnValue22;
        SDL_WaitThread(threadID22, &threadReturnValue22);
        int threadReturnValue23;
        SDL_WaitThread(threadID23, &threadReturnValue23);
        int threadReturnValue24;
        SDL_WaitThread(threadID24, &threadReturnValue24);
        int threadReturnValue25;
        SDL_WaitThread(threadID25, &threadReturnValue25);
        int threadReturnValue26;
        SDL_WaitThread(threadID26, &threadReturnValue26);
        int threadReturnValue27;
        SDL_WaitThread(threadID27, &threadReturnValue27);
        int threadReturnValue28;
        SDL_WaitThread(threadID28, &threadReturnValue28);
        int threadReturnValue29;
        SDL_WaitThread(threadID29, &threadReturnValue29);
        int threadReturnValue30;
        SDL_WaitThread(threadID30, &threadReturnValue30);
        int threadReturnValue31;
        SDL_WaitThread(threadID31, &threadReturnValue31);

        threadID00 = nullptr;
        threadID01 = nullptr;
        threadID02 = nullptr;
        threadID03 = nullptr;
        threadID04 = nullptr;
        threadID05 = nullptr;
        threadID06 = nullptr;
        threadID07 = nullptr;
        threadID08 = nullptr;
        threadID09 = nullptr;
        threadID10 = nullptr;
        threadID11 = nullptr;
        threadID12 = nullptr;
        threadID13 = nullptr;
        threadID14 = nullptr;
        threadID15 = nullptr;
        threadID16 = nullptr;
        threadID17 = nullptr;
        threadID18 = nullptr;
        threadID19 = nullptr;
        threadID20 = nullptr;
        threadID21 = nullptr;
        threadID22 = nullptr;
        threadID23 = nullptr;
        threadID24 = nullptr;
        threadID25 = nullptr;
        threadID26 = nullptr;
        threadID27 = nullptr;
        threadID28 = nullptr;
        threadID29 = nullptr;
        threadID30 = nullptr;
        threadID31 = nullptr;
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
