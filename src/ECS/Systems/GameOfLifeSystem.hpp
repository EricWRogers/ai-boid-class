#pragma once

#include <SDL_keyboard.h>

#include <glm/glm.hpp>

#include <Canis/ECS/Systems/System.hpp>
#include <Canis/ECS/Components/RectTransformComponent.hpp>
#include <Canis/ECS/Components/ColorComponent.hpp>
#include <Canis/ECS/Components/ScriptComponent.hpp>
#include <Canis/ECS/Components/TextComponent.hpp>

#include "../Components/GameOfLifeComponent.hpp"

#include "../ScriptableEntities/GameOfLifeLoader.hpp"

class GameOfLifeSystem : public Canis::System
{
private:
    bool m_runRulesUpdate = false;
    float m_resetTime = 0.25f;
    float m_countDown = 0.0f;
public:

    GameOfLifeSystem() : Canis::System() {

    }

    ~GameOfLifeSystem() {

    }

    void Create()
    {

    }

    void Ready()
    {

    }

    void Update(entt::registry &_registry, float _deltaTime)
    {
        // get all
        auto view = _registry.view<Canis::RectTransformComponent, Canis::ColorComponent, GameOfLifeComponent>();

        // find the entity in the scene with a tag Boards
        Canis::Entity e = GetScene().FindEntityWithTag("Boards");

        // get the scriptable entity off of it
        GameOfLifeLoader* loader = static_cast<GameOfLifeLoader*>(e.GetComponent<Canis::ScriptComponent>().Instance);

        // on click change state
        for(auto[entity, rectTransform, color, gameOfLife] : view.each())
        {
            // turn on cell
            if (GetInputManager().GetLeftClick())
            {
                if (GetInputManager().mouse.x > rectTransform.position.x &&
                    GetInputManager().mouse.x < rectTransform.position.x + rectTransform.size.x)
                {
                    if (GetInputManager().mouse.y > rectTransform.position.y &&
                        GetInputManager().mouse.y < rectTransform.position.y + rectTransform.size.y)
                    {
                        gameOfLife.currentState = true;
                    }
                }
            }

            // turn off cell
            if (GetInputManager().GetRightClick())
            {
                if (GetInputManager().mouse.x > rectTransform.position.x &&
                    GetInputManager().mouse.x < rectTransform.position.x + rectTransform.size.x)
                {
                    if (GetInputManager().mouse.y > rectTransform.position.y &&
                        GetInputManager().mouse.y < rectTransform.position.y + rectTransform.size.y)
                    {
                        gameOfLife.currentState = false;
                    }
                }
            }
        }

        // toggel running
        if (GetInputManager().JustPressedKey(SDLK_SPACE))
        {
            m_runRulesUpdate = !m_runRulesUpdate;

            Canis::Entity titleText = GetScene().FindEntityWithTag("TITLE");

            Canis::RectTransformComponent& rect = titleText.GetComponent<Canis::RectTransformComponent>();
            Canis::TextComponent& text = titleText.GetComponent<Canis::TextComponent>();

            if (m_runRulesUpdate)
                Canis::Text::Set(text, rect, "Game of Life Demo | Running");
            else
                Canis::Text::Set(text, rect, "Game of Life Demo | Paused");
        }

        // rules loop
        if (m_runRulesUpdate)
        {
            m_countDown -= _deltaTime;

            if (m_countDown < 0.0f)
            {
                m_countDown = m_resetTime;

                for(auto[entity, rectTransform, color, gameOfLife] : view.each())
                {
                    // find index
                    int xIndex = (static_cast<int>(rectTransform.position.x + loader->cellSize) / loader->cellSize) - 1;
                    int yIndex = (static_cast<int>(rectTransform.position.y + loader->cellSize) / loader->cellSize) - 1;
                    
                    int count = 0;
                    int sizeX = loader->cells[0].size();
                    int sizeY = loader->cells.size();
                    
                    // rap the index around the screen
                    int right = ((xIndex + 1) >= sizeX) ? 0 : xIndex + 1;
                    int left = ((xIndex - 1) < 0) ? sizeX - 1 : xIndex - 1;
                    int up = ((yIndex + 1) >= sizeY) ? 0 : yIndex + 1;
                    int down = ((yIndex - 1) < 0) ? sizeY - 1 : yIndex - 1;

                    // check neighbors
                    count += (loader->cells[up][right].GetComponent<GameOfLifeComponent>().currentState == true); // top right
                    count += (loader->cells[yIndex][right].GetComponent<GameOfLifeComponent>().currentState == true); // right
                    count += (loader->cells[down][right].GetComponent<GameOfLifeComponent>().currentState == true); // bottom right
                    count += (loader->cells[up][xIndex].GetComponent<GameOfLifeComponent>().currentState == true); // top center
                    count += (loader->cells[down][xIndex].GetComponent<GameOfLifeComponent>().currentState == true); // bottom center
                    count += (loader->cells[up][left].GetComponent<GameOfLifeComponent>().currentState == true); // top left
                    count += (loader->cells[yIndex][left].GetComponent<GameOfLifeComponent>().currentState == true); // left
                    count += (loader->cells[down][left].GetComponent<GameOfLifeComponent>().currentState == true); // bottom left

                    // rule one
                    if (count < 2 && gameOfLife.currentState == true)
                    {
                        gameOfLife.nextState = false;
                        gameOfLife.update = true;
                    }

                    // rule two
                    if (count > 3 && gameOfLife.currentState == true)
                    {
                        gameOfLife.nextState = false;
                        gameOfLife.update = true;
                    }

                    // rule three


                    // rule four
                    if (count == 3 && gameOfLife.currentState == false)
                    {
                        gameOfLife.nextState = true;
                        gameOfLife.update = true;
                    }
                }
            }
        }

        // set color based on state
        for(auto[entity, rectTransform, color, gameOfLife] : view.each())
        {
            // update current state
            if (gameOfLife.update == true)
            {
                gameOfLife.currentState = gameOfLife.nextState;
                gameOfLife.update = false;
            }
            // clear
            if (GetInputManager().JustPressedKey(SDLK_c))
            {
                gameOfLife.currentState = false;
            }
            // update color
            if (gameOfLife.currentState == true)
            {
                color.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            }
            else{
                color.color = glm::vec4(1.0f);
            }
        }
    }
};

bool DecodeGameOfLifeSystem(const std::string &_name, Canis::Scene *_scene)
{
    if (_name == "GameOfLifeSystem")
    {
        _scene->CreateSystem<GameOfLifeSystem>();
        return true;
    }
    return false;
}
