#ifndef NAVIGATIONENGINE_H
#define NAVIGATIONENGINE_H

#include "Agent.h"

#include "Core/Image.h"

#include "Utils/TypeUtils.h"

#include <vector>


enum NavCellFilters
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


struct NavCell
{
    glm::vec2 pos;
    uint32_t gCost;
    uint32_t fCost;
    NavCell* previous = nullptr;

    friend bool operator<(const NavCell& first, const NavCell& second)
    {
        return first.fCost < second.fCost;
    }
};


class NavigationEngine
{
public:
    static NavigationEngine& Init();
    inline static NavigationEngine& Get() { return s_instance; }

    void SetNavMap(const ImagePtr& navMap);

    void OnUpdate();

    std::vector<glm::vec2> FindPath(const glm::vec2& startPos, const glm::vec2& endPos, 
                                    const NavCellFilters& filter=NavCellFilters::Default) const;
    bool IsWalkableCell(const glm::vec2& cell, const NavCellFilters& filter=NavCellFilters::Default) const;

    NavAgentPtr CreateAgent();
    void RemoveAgent(const NavAgentPtr& agent);

private:
    NavigationEngine() = default;
    ~NavigationEngine() = default;
    
    void ComputeAgentPath(const NavAgentPtr& agent);
    uint32_t GetCell(const uint32_t& x, const uint32_t& y) const;

    std::vector<glm::vec2> ReconstructPath(const NavCell& end) const;

    std::vector<uint32_t> m_navMap;
    uint32_t m_navWidth;
    uint32_t m_navHeight;
    bool m_navMapHasChanged = false;

    std::vector<NavAgentPtr> m_agents;

    static NavigationEngine s_instance;
};


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