#ifdef _WIN32
#include <windows.h>
#endif

#include <SDL.h>

#include <Canis/App.hpp>
#include <Canis/ECS/Decode.hpp>
#include <Canis/Canis.hpp>
#include <Canis/SceneManager.hpp>
#include <Canis/ECS/Systems/RenderHUDSystem.hpp>
#include <Canis/ECS/Systems/RenderTextSystem.hpp>
#include <Canis/ECS/Systems/SpriteRenderer2DSystem.hpp>
#include <Canis/ECS/Systems/SpriteAnimationSystem.hpp>
#include <Canis/ECS/Systems/CollisionSystem2D.hpp>
#include <Canis/ECS/Systems/ButtonSystem.hpp>

#include "ECS/ScriptableEntities/DebugCamera2D.hpp"
#include "ECS/ScriptableEntities/BeachBall.hpp"
#include "ECS/ScriptableEntities/GameOfLifeLoader.hpp"
#include "ECS/ScriptableEntities/FPSCounter.hpp"

#include "ECS/Systems/GameOfLifeSystem.hpp"
#include "ECS/Systems/BoidSystem.hpp"

#include <iostream>

int ThreadFunction(void* data)
{
    for (int i = 0; i < 100; i++)
    {
        std::cout << "Thread: " << *(int*)data << " " << i << std::endl;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    int t1Number = 1;
    SDL_Thread *t1 = SDL_CreateThread(ThreadFunction, "ThreadFunction", &t1Number); 

    int t2Number = 2;
    SDL_Thread *t2 = SDL_CreateThread(ThreadFunction, "ThreadFunction", &t2Number); 

    SDL_WaitThread(t1, NULL);
    SDL_WaitThread(t2, NULL);

    int mainProcessNumber = 0;
    ThreadFunction(&mainProcessNumber);

    return 0;

    Canis::App app;

    // decode system
    app.AddDecodeSystem(Canis::DecodeButtonSystem);
    app.AddDecodeSystem(Canis::DecodeCollisionSystem2D);
	app.AddDecodeSystem(Canis::DecodeSpriteAnimationSystem);
    app.AddDecodeSystem(DecodeGameOfLifeSystem);
    app.AddDecodeSystem(DecodeBoidSystem);

    // decode render system
    app.AddDecodeRenderSystem(Canis::DecodeRenderHUDSystem);
    app.AddDecodeRenderSystem(Canis::DecodeRenderTextSystem);
    app.AddDecodeRenderSystem(Canis::DecodeSpriteRenderer2DSystem);

    // decode scriptable entities
    app.AddDecodeScriptableEntity(DecodeDebugCamera2D);
    app.AddDecodeScriptableEntity(DecodeBeachBall);
    app.AddDecodeScriptableEntity(DecodeGameOfLifeLoader);
    app.AddDecodeScriptableEntity(DecodeFPSCounter);

    // decode component
    app.AddDecodeComponent(Canis::DecodeTagComponent);
    app.AddDecodeComponent(Canis::DecodeCamera2DComponent);
    app.AddDecodeComponent(Canis::DecodeRectTransformComponent);
    app.AddDecodeComponent(Canis::DecodeColorComponent);
    app.AddDecodeComponent(Canis::DecodeTextComponent);
    app.AddDecodeComponent(Canis::DecodeSprite2DComponent);
    app.AddDecodeComponent(Canis::DecodeUIImageComponent);
    app.AddDecodeComponent(Canis::DecodeUISliderComponent);
    app.AddDecodeComponent(Canis::DecodeSpriteAnimationComponent);
    app.AddDecodeComponent(Canis::DecodeCircleColliderComponent);

    app.AddScene(new Canis::Scene("sprite_demo", "assets/scenes/sprite_demo.scene"));
    app.AddScene(new Canis::Scene("game_of_life", "assets/scenes/game_of_life.scene"));
    app.AddScene(new Canis::Scene("boid_demo", "assets/scenes/boid_demo.scene"));

    app.Run("Canis Demos", "boid_demo");

    return 0;
}