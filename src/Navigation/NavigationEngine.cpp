#include "NavigationEngine.h"

#include "Resources/Loaders/LevelLoader.h"

#include "Core/Logging.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <vector>
#include <unordered_map>


NavigationEngine NavigationEngine::s_instance;


NavigationEngine& NavigationEngine::Init()
{
    s_instance = NavigationEngine();
    return s_instance;
}

void NavigationEngine::SetNavMap(const ImagePtr& navMap)
{
    m_navMap = navMap;
    m_navMapHasChanged = true;
}

void NavigationEngine::OnUpdate()
{
    if (!m_navMap)
    {
        return;
    }

    // Update disabled agents
    for (const auto& agent : m_agents)
    {
        if (agent->IsMoving())
            continue;

        const auto& dest = agent->GetDestination();
        const auto& destPix = m_navMap->GetPixel(dest.x, dest.z);
        if (destPix == LevelCells::Floor)
        {
            agent->m_animation = {{{0.0f, agent->GetPosition()},
                                   {1.0f, dest}},
                                  InterpolationType::Smooth,
                                  2.5f};
            agent->m_animation.Start();
        }
            
    }
    
    m_navMapHasChanged = false;
}

AgentPtr NavigationEngine::CreateAgent()
{
    AgentPtr agent(new Agent);
    m_agents.push_back(agent);

    return agent;
}

std::vector<glm::vec2> NavigationEngine::ReconstructPath(const NavCell& end) const
{
    std::vector<glm::vec2> result {end.pos};
    const NavCell* cell = end.previous;
    while (cell)
    {
        result.insert(result.begin(), cell->pos);
        cell = cell->previous;
    }
    return result;
}

uint32_t CostHeuristic(const glm::vec2& start, const glm::vec2& end) 
{ 
    return abs(end.x - start.x) + abs(end.y - start.y); 
}

std::vector<glm::vec2> NavigationEngine::FindPath(const glm::vec2& startPos, const glm::vec2& endPos) const
{
    if (m_navMap->GetPixel(startPos.x, startPos.y) == LevelCells::Wall)
    {
        return {};
    }

    std::unordered_map<glm::vec2, NavCell*> openedCells;
    openedCells.insert({startPos, new NavCell{startPos, 0, CostHeuristic(startPos, endPos)}});

    std::unordered_map<glm::vec2, NavCell*> closedCells;

    while (!openedCells.empty())
    {
        auto currentIt = std::min_element(openedCells.begin(), openedCells.end(),
                                            [](const auto& l, const auto& r) { return *l.second < *r.second; });

        if (currentIt->first == endPos)
        {
            // Yeah !
            LOG_INFO("FOUND !");
            return ReconstructPath(*currentIt->second);
        }

        NavCell* currentCell = currentIt->second;
        openedCells.erase(currentIt);
        
        std::array<glm::vec2, 4> neighbours {glm::vec2(-1, 0), 
                                             glm::vec2(1, 0), 
                                             glm::vec2(0, -1), 
                                             glm::vec2(0, 1)};
        for (const auto& neighbour : neighbours)
        {
            glm::vec2 pos = currentCell->pos + neighbour;
            // Skipping walls
            if (m_navMap->GetPixel(pos.x, pos.y) == LevelCells::Wall)
            {
                continue;
            }

            // Computing costs
            uint32_t gCost = currentCell->gCost + 1;
            uint32_t fCost = gCost + CostHeuristic(pos, endPos);

            // Trying to find better paths that could invalidate the current one
            auto it = openedCells.find(pos);
            if (it != openedCells.end() && it->second->fCost < fCost)
                continue;

            it = closedCells.find(pos);
            if (it != closedCells.end() && it->second->fCost < fCost)
                continue;

            // This is the best path to this cell so far, adding it to the queue
            openedCells[pos] = new NavCell{pos, gCost, fCost, currentCell};
        }

        closedCells[currentCell->pos] = currentCell;
    }

    return {};
}

