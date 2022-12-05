#ifndef NAVIGATIONENGINE_H
#define NAVIGATIONENGINE_H

#include "Agent.h"

#include "Core/Image.h"

#include "Utils/TypeUtils.h"

#include <vector>


namespace Navigation {


enum CellFilters
{
    None = 0,
    Floor = 1 << 0,
    Doors = 1 << 1,
    Water = 1 << 2,
    Walls = 1 << 3,

    Default = Floor | Doors,
    Flying = Default | Water,
    All = Floor | Doors | Water | Walls
};


struct Cell
{
    glm::vec2 pos;
    uint32_t gCost;
    uint32_t fCost;
    Cell* previous = nullptr;

    friend bool operator<(const Cell& first, const Cell& second)
    {
        return first.fCost < second.fCost;
    }
};


class Engine
{
public:
    static Engine& Init();
    inline static Engine& Get() { return *s_instance; }

    void SetNavMap(const ImagePtr& navMap);

    void OnUpdate();

    std::vector<glm::vec2> FindPath(const glm::vec2& startPos, const glm::vec2& endPos, 
                                    const CellFilters& filter=CellFilters::Default) const;
    bool IsWalkableCell(const glm::vec2& cell, const CellFilters& filter=CellFilters::Default) const;

    AgentPtr CreateAgent();
    void RemoveAgent(const AgentPtr& agent);

private:
    Engine() = default;
    Engine(const Engine&) = delete;
    ~Engine() = default;
    
    void ComputeAgentPath(const AgentPtr& agent);
    uint32_t GetCell(const uint32_t& x, const uint32_t& y) const;

    std::vector<glm::vec2> ReconstructPath(const Cell& end) const;

    std::vector<uint32_t> m_navMap;
    uint32_t m_navWidth;
    uint32_t m_navHeight;
    bool m_navMapHasChanged = false;

    std::vector<AgentPtr> m_agents;

    static Engine* s_instance;
};


} // Namespace Navigation

// Mandatory to define unordered_maps with glm::vec2 as keys
template <>
struct std::hash<glm::vec2>
{
    std::size_t operator()(const glm::vec2& vec) const
    {
        size_t hash = 0;
        HashCombine(hash, vec.x);
        HashCombine(hash, vec.y);

        return hash;
    }
};


#endif  // NAVIGATIONENGINE_H