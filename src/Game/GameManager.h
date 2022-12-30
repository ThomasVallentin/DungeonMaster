#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Components.h"

#include "Scene/Entity.h"


class GameManager
{
public:
    static GameManager& Init();
    inline static GameManager& Get() { return *s_instance; }

    void SetPlayer(const Entity& player);
    inline Entity GetPlayer(const Entity& player) { return m_player; }

    void AddMonster(const Entity& entity);
    void RemoveMonster(const Entity& entity);
    inline const std::vector<Entity>& GetMonsters() const { return m_monsters; }
    
    void Clear();

private:
    GameManager() = default;
    ~GameManager() = default;

    static GameManager* s_instance;

    Entity m_player;
    std::vector<Entity> m_monsters;
};


#endif