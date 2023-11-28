#pragma once
#include <string>
#include <vector>
#include <Canis/ScriptableEntity.hpp>
#include <Canis/ECS/Components/RectTransformComponent.hpp>
#include <Canis/ECS/Components/ColorComponent.hpp>
#include <Canis/ECS/Components/Sprite2DComponent.hpp>

#include <Canis/AssetManager.hpp>

#include "../Components/GameOfLifeComponent.hpp"

class GameOfLifeLoader : public Canis::ScriptableEntity
{
private:
    unsigned int numberOfRows = 30;
    unsigned int numberOfColumns = 30;
public:
    std::vector<std::vector<Canis::Entity>> cells;
    float cellSize = 12.0f;

    void OnCreate()
    {
        numberOfColumns = GetWindow().GetScreenWidth() / cellSize;
        numberOfRows = GetWindow().GetScreenHeight() / cellSize;

        cells = std::vector(numberOfRows, std::vector<Canis::Entity>(numberOfColumns));
    }

    void OnReady()
    {
        for(int x = 0; x < numberOfColumns; x++)
        {
            for(int y = 0; y < numberOfRows; y++)
            {
                Canis::RectTransformComponent transform = {};
                transform.position = glm::vec2(cellSize*x, cellSize*y);
                transform.depth = -0.1;
                transform.size = glm::vec2(cellSize);
                Canis::ColorComponent color = {};
                Canis::Sprite2DComponent sprite = {};
                sprite.uv = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
                sprite.texture = Canis::AssetManager::GetTexture("assets/textures/box.png")->GetTexture();

                GameOfLifeComponent gameOfLife = {};
                gameOfLife.currentState = false;

                Canis::Entity square = CreateEntity();
                square.AddComponent<Canis::RectTransformComponent>(transform);
                square.AddComponent<Canis::ColorComponent>(color);
                square.AddComponent<Canis::Sprite2DComponent>(sprite);
                square.AddComponent<GameOfLifeComponent>(gameOfLife);

                cells[y][x] = square;
            }
        }
    }
    
    void OnDestroy()
    {

    }

    void OnUpdate(float _dt)
    {
        
    }
};

bool DecodeGameOfLifeLoader(const std::string &_name, Canis::Entity &_entity)
{
    if (_name == "GameOfLifeLoader")
    {
        Canis::ScriptComponent scriptComponent = {};
        scriptComponent.Bind<GameOfLifeLoader>();
        _entity.AddComponent<Canis::ScriptComponent>(scriptComponent);
        return true;
    }
    return false;
}