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
    m_navWidth = navMap->GetWidth();
    m_navHeight = navMap->GetHeight();

    uint32_t pixelCount = m_navWidth * m_navHeight;
    if (pixelCount >= m_navMap.size())
        m_navMap.reserve(pixelCount);
    else 
        m_navMap.resize(pixelCount);
    
    auto pixels = navMap->GetPixels();
    for (size_t i=0 ; i < pixelCount ; ++i)
    {
        const auto& pixel = pixels[i];
        if (pixel == LevelCell::Wall)
            m_navMap[i] = NavCellFilters::Walls;

        else if (pixel == LevelCell::Floor || 
                 pixel == LevelCell::Entrance || 
                 pixel == LevelCell::Exit)
            m_navMap[i] = NavCellFilters::Floor;

        else if (pixel == LevelCell::Water)
            m_navMap[i] = NavCellFilters::Water;

        else if (pixel == LevelCell::Door)
            m_navMap[i] = NavCellFilters::Doors;

        else 
        {
            std::cout << pixel.r <<" " << pixel.g << " " << pixel.b << std::endl;
            m_navMap[i] = NavCellFilters::None;
        }
    }

    for (size_t y=0 ; y < m_navHeight ; ++y)
    {
        std::cout << y << " : ";
        for (size_t x=0 ; x < m_navWidth ; ++x)
            std::cout << m_navMap[y * m_navWidth + x];
        std::cout << std::endl;
    }

    m_navMapHasChanged = true;
}

void NavigationEngine::OnUpdate()
{
    if (m_navMap.empty())
    {
        return;
    }

    // Update disabled agents
    for (const auto& agent : m_agents)
    {
        if (agent->IsMoving())
        {
            if (agent->HasAdvanced())
            {
                agent->MakeProgress(0.03f); // TODO: Use deltaTime here
            }
        }
        else if (agent->NeedsNewPath())
        {
            ComputeAgentPath(agent);
            agent->MakeProgress(0.03f); // TODO: Use deltaTime here
        }
    }
    
    m_navMapHasChanged = false;
}

void NavigationEngine::ComputeAgentPath(const NavAgentPtr& agent)
{
    glm::vec3 pos = agent->GetPosition();
    glm::vec3 dest = agent->GetDestination();
    auto path = FindPath(glm::vec2(pos.x, pos.z), glm::vec2(dest.x, dest.z));
}

NavAgentPtr NavigationEngine::CreateAgent()
{
    NavAgentPtr agent(new NavAgent);
    m_agents.push_back(agent);

    return agent;
}

void NavigationEngine::RemoveAgent(const NavAgentPtr& agent)
{
    const auto it = std::find(m_agents.begin(), m_agents.end(), agent);
    if (it != m_agents.end())
    {
        m_agents.erase(it);
    }
}

uint32_t NavigationEngine::GetCell(const uint32_t& x, const uint32_t& y) const
{
    return m_navMap[y * m_navWidth + x];
}

bool NavigationEngine::IsWalkableCell(const glm::vec2& cell, const NavCellFilters& filter) const
{
    uint32_t a = GetCell(cell.x, cell.y);
    return a & filter;
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

std::vector<glm::vec2> NavigationEngine::FindPath(const glm::vec2& startPos, const glm::vec2& endPos,
                                                  const NavCellFilters& filter) const
{
    if (!IsWalkableCell(startPos, filter))
    {
        return {};
    }

    std::unordered_map<glm::vec2, NavCell*> openedCells;
    openedCells.insert({startPos, new NavCell{startPos, 0, CostHeuristic(startPos, endPos)}});

    std::unordered_map<glm::vec2, NavCell*> closedCells;

    std::vector<glm::vec2> result;
    while (!openedCells.empty())
    {
        auto currentIt = std::min_element(openedCells.begin(), openedCells.end(),
                                            [](const auto& l, const auto& r) { return *l.second < *r.second; });

        if (currentIt->first == endPos)
        {
            // Yeah !
            LOG_INFO("FOUND !");
            result = ReconstructPath(*currentIt->second); 
            break; 
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
            if (!IsWalkableCell(pos, filter))
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

    // Cleanup temporary data
    for (const auto &it : closedCells)
    {
        delete it.second;
    }

    return result;
}



