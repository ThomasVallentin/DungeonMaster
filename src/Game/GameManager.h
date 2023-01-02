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

    inline std::string GetCurrentLevel() const { return m_currentLevel; }
    void SetNextLevel(const std::string& levelIdentifier);
    void SetNextFloor(const uint32_t& floor);

    void StartGame();
    void RestartGame();
    void ShowLooseScreen() const;

private:
    GameManager() = default;
    ~GameManager() = default;

    void LoadLevel(const std::string& levelIdentifier, const uint32_t& floor);

    static GameManager* s_instance;

    Entity m_player;
    std::vector<Entity> m_monsters;

    std::string m_currentLevel;
    std::string m_nextLevel;
    int m_currentFloor = 0;
    int m_nextFloor = -1;
};


#endif