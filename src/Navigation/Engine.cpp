#include "Engine.h"

#include "Resources/Loaders/LevelLoader.h"

#include "Core/Logging.h"
#include "Core/Time.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <vector>
#include <unordered_map>


namespace Navigation {


Engine* Engine::s_instance = nullptr;


Engine& Engine::Init()
{
    s_instance = new Engine();
    return *s_instance;
}

void Engine::SetNavMap(const ImagePtr& navMap)
{
    m_navWidth = navMap->GetWidth();
    m_navHeight = navMap->GetHeight();

    uint32_t pixelCount = m_navWidth * m_navHeight;
    m_navMap.resize(pixelCount);
    
    auto pixels = navMap->GetPixels();
    for (size_t i=0 ; i < pixelCount ; ++i)
    {
        const auto& pixel = pixels[i];
        if (pixel == LevelCell::Wall)
            m_navMap[i] = CellFilters::Walls;

        else if (pixel == LevelCell::Floor || 
                 pixel == LevelCell::Entrance || 
                 pixel == LevelCell::Exit)
            m_navMap[i] = CellFilters::Floor;

        else if (pixel == LevelCell::Water)
            m_navMap[i] = CellFilters::Water;

        else if (pixel == LevelCell::Door)
            m_navMap[i] = CellFilters::Doors;

        else 
        {
            m_navMap[i] = CellFilters::None;
        }
    }

    m_navMapHasChanged = true;
}

uint32_t Engine::GetCell(const int& x, const int& y) const
{
    if (x < 0 || x >= m_navWidth || -y < 0 || -y >= m_navHeight)
        return CellFilters::None;

    return m_navMap[-y * m_navWidth + x];
}

void Engine::SetCell(const int& x, const int& y, const CellFilters& value)
{
    m_navMap[-y * m_navWidth + x] = value;
}

void Engine::OnUpdate()
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
                agent->MakeProgress(Time::GetDeltaTime()); // TODO: Use deltaTime here
            }
        }
        else if (agent->NeedsNewPath())  // If a more recent path has been set, compute it
        {
            ComputeAgentPath(agent);
            agent->MakeProgress(Time::GetDeltaTime()); // TODO: Use deltaTime here
        }
        else if (agent->HasPath()) // Keep on moving on the current path otherwise
        {
            if (agent->HasAdvanced())
            {
                agent->MakeProgress(Time::GetDeltaTime()); // TODO: Use deltaTime here
            }
        }
    }
    
    m_navMapHasChanged = false;
}

void Engine::ComputeAgentPath(const AgentPtr& agent)
{
    glm::vec3 pos = agent->GetTransform()[3];
    glm::vec3 dest = agent->GetDestination();
    agent->SetPath(FindPath(glm::vec2(round(pos.x), round(pos.z)), glm::vec2(round(dest.x), round(dest.z))));
}

AgentPtr Engine::CreateAgent()
{
    AgentPtr agent(new Agent);
    m_agents.push_back(agent);

    return agent;
}

void Engine::RemoveAgent(const AgentPtr& agent)
{
    const auto it = std::find(m_agents.begin(), m_agents.end(), agent);
    if (it != m_agents.end())
    {
        m_agents.erase(it);
    }
}

bool Engine::CellIsEmpty(const glm::vec2& cell, 
                         const CellFilters& filter) const
{
    uint32_t a = GetCell(cell.x, cell.y);
    return (a & filter);
}

bool Engine::CellContainsAgent(const glm::vec2& cell)
{
    for (const auto& agent : m_agents)
    {
        const glm::vec4& position = agent->GetTransform()[3];
        if (cell == glm::vec2(round(position.x), round(position.z)))
        {
            return true;
        }
    }

    return false;
}

bool Engine::CanSeeCell(glm::vec2 source, 
                        glm::vec2 target, 
                        const CellFilters& filter)
{
    glm::vec2 toTargetDir = glm::normalize(target - source);

    if (std::abs(toTargetDir.x) > std::abs(toTargetDir.y))
    {
        // Normalizing the direction by its x
        toTargetDir.y /= toTargetDir.x;
        toTargetDir.x = 1.0f;

        // Make sure that pos has a smaller x than searchPos
        if (source.x > target.x)
        {
            std::swap(target, source);
            toTargetDir *= -1.0f;
        }

        int ySign = toTargetDir.y > 0 ? 1 : -1;
        float e = 0.0f;
        while (source.x <= target.x)
        {
            if (std::abs(e) >= 0.5f)
            {
                if (!CellIsEmpty(source, filter))
                    return false;
                source.y += ySign;
                e = ySign - e;
            }

            if (!CellIsEmpty(source, filter))
                return false;
            e -= toTargetDir.y;
            source.x++;
        }
    }
    else
    {
        // Normalizing the direction by its y
        toTargetDir.x /= toTargetDir.y;
        toTargetDir.y = 1.0f;

        // Make sure that pos has a smaller x than searchPos
        if (source.y > target.y)
        {
            std::swap(target, source);
            toTargetDir *= -1.0;
        }

        int xSign = toTargetDir.x > 0 ? 1 : -1;
        float e = 0;
        while (source.y <= target.y)
        {
            if (std::abs(e) >= 0.5)
            {
                if (!CellIsEmpty(source, filter))
                    return false;
                source.x += xSign;
                e = xSign - e;
            }

            if (!CellIsEmpty(source, filter))
                return false;
            source.y++;
            e -= toTargetDir.x;
        }
    }

    return true;
}

std::vector<glm::vec2> Engine::ReconstructPath(const Cell& end) const
{
    std::vector<glm::vec2> result {end.pos};
    const Cell* cell = end.previous;
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

std::vector<glm::vec2> Engine::FindPath(const glm::vec2& startPos, const glm::vec2& endPos,
                                        const CellFilters& filter) const
{
    if (startPos == endPos)
    {
        return {};
    }

    std::unordered_map<glm::vec2, Cell*> openedCells;
    openedCells.insert({startPos, new Cell{startPos, 0, CostHeuristic(startPos, endPos)}});

    std::unordered_map<glm::vec2, Cell*> closedCells;

    std::vector<glm::vec2> result;
    while (!openedCells.empty())
    {
        auto currentIt = std::min_element(openedCells.begin(), openedCells.end(),
                                          [](const auto& l, const auto& r) { return *l.second < *r.second; });

        if (currentIt->first == endPos)
        {
            result = ReconstructPath(*currentIt->second); 
            break;
        }

        Cell* currentCell = currentIt->second;
        openedCells.erase(currentIt);
        
        std::array<glm::vec2, 4> neighbours {glm::vec2(-1, 0), 
                                             glm::vec2(1, 0), 
                                             glm::vec2(0, -1), 
                                             glm::vec2(0, 1)};
        for (const auto& neighbour : neighbours)
        {
            glm::vec2 pos = currentCell->pos + neighbour;
            // Skipping walls
            if (!CellIsEmpty(pos, filter))
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
            openedCells[pos] = new Cell{pos, gCost, fCost, currentCell};
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


} // Namespace Navigation

