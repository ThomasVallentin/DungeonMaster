#ifndef NAVIGATIONENGINE_H
#define NAVIGATIONENGINE_H

#include "Agent.h"

#include "Core/Image.h"

#include "Utils/TypeUtils.h"

#include <vector>


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

    ImagePtr GetNavMap() const { return m_navMap; }
    void SetNavMap(const ImagePtr& navMap);

    void OnUpdate();

    std::vector<glm::vec2> FindPath(const glm::vec2& startPos, const glm::vec2& endPos) const;

    AgentPtr CreateAgent();
    void RemoveAgent(const AgentPtr& agent);

private:
    NavigationEngine() = default;
    ~NavigationEngine() = default;
    
    void UpdateAgent();

    std::vector<glm::vec2> ReconstructPath(const NavCell& end) const;

    ImagePtr m_navMap;
    bool m_navMapHasChanged = false;
    std::vector<AgentPtr> m_agents;

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