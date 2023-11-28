#pragma once
#include <string>
#include <SDL_keyboard.h>
#include <Canis/SceneManager.hpp>
#include <Canis/ScriptableEntity.hpp>
#include <Canis/ECS/Components/TextComponent.hpp>

class FPSCounter : public Canis::ScriptableEntity
{
private:
    float m_time = 0.0f;
    float m_maxTime = 0.1f;
public:
    void OnCreate()
    {
        
    }

    void OnReady()
    {

    }
    
    void OnDestroy()
    {

    }

    void OnUpdate(float _dt)
    {
        m_time -= _dt;

        if ( m_time > 0.0f )
            return;
        
        m_time = m_maxTime;

        Canis::RectTransformComponent &rectComponent = GetComponent<Canis::RectTransformComponent>();
        Canis::TextComponent &textComponent = GetComponent<Canis::TextComponent>();
        Canis::Text::Set(textComponent, rectComponent, "FPS : " + std::to_string((int)m_Entity.scene->window->fps));
        
        //auto *sceneManager = (Canis::SceneManager*)(m_Entity.scene->sceneManager);
        //Canis::Text::Set(textComponent, rectComponent,
        //     "UT : " + std::to_string((float)sceneManager->updateTime) +
        //     " DT : " + std::to_string((float)sceneManager->drawTime));
    }
};

bool DecodeFPSCounter(const std::string &_name, Canis::Entity &_entity)
{
    if (_name == "FPSCounter")
    {
        Canis::ScriptComponent scriptComponent = {};
        scriptComponent.Bind<FPSCounter>();
        _entity.AddComponent<Canis::ScriptComponent>(scriptComponent);
        return true;
    }
    return false;
}