#include "Game.h"

#include "Core/Event.h"
#include "Core/Inputs.h"
#include "Core/Time.h"
#include "Core/Animation.h"

#include "Navigation/NavigationEngine.h"


namespace Components {


struct CharacterControllerData
{
    Animation<glm::mat4> moveAnimation;
    Animation<glm::mat4> attackAnimation;
    float speed = 2.0f;
    NavCellFilters navFilter = NavCellFilters::Default;
};


Scriptable CreateCharacterController(const Entity& entity)
{
    return Scriptable(
        "CharacterController",
        entity,

// CharacterController::OnCreate
[](const Entity& entity, std::any& dataBlock)
{
    dataBlock = std::make_any<CharacterControllerData>();
},

// CharacterController::OnUpdate
[](const Entity& entity, std::any& dataBlock)
{
    auto* transform = entity.FindComponent<Transform>();
    if (!transform)
    {
        return;
    }

    CharacterControllerData& data = std::any_cast<CharacterControllerData&>(dataBlock);
    NavigationEngine& navEngine = NavigationEngine::Get();

    bool shouldSampleInput = true;

    // The character is currently moving, evaluate the move animation
    if (!data.moveAnimation.ended)
    {
        transform->transform = data.moveAnimation.Evaluate(Time::GetDeltaTime());
        shouldSampleInput = false;
    }

    // The character is currently attacking, evaluate the move animation
    if (!data.attackAnimation.ended)
    {
        auto* childTransform = entity.FindChild("Weapon").FindComponent<Transform>();
        if (childTransform)
        {
            childTransform->transform = data.attackAnimation.Evaluate(Time::GetDeltaTime());
        }
        shouldSampleInput = false;
    }

    if (!shouldSampleInput)
    {
        return;
    }

    // Sample keyboard inputs
    if (Inputs::IsKeyPressed(KeyCode::Up))
    {
        glm::mat4 nextTransform = glm::translate(transform->transform, glm::vec3(0, 0, -1.0f));
        glm::vec2 nextCell = glm::vec2(nextTransform[3].x, -nextTransform[3].z);
        LOG_INFO("%f, %f, %d", nextCell.x, nextCell.y, navEngine.IsWalkableCell(nextCell, data.navFilter));
        if (navEngine.IsWalkableCell(nextCell, data.navFilter))
        {
            data.moveAnimation = {{{0.0f, transform->transform},
                                   {1.0f, nextTransform}},
                                  InterpolationType::Smooth,
                                  data.speed};
            data.moveAnimation.Start();    
        }
    }
    else if (Inputs::IsKeyPressed(KeyCode::Down))
    {
        glm::mat4 nextTransform = glm::translate(transform->transform, glm::vec3(0, 0, 1.0f));
        glm::vec2 nextCell = glm::vec2(nextTransform[3].x, -nextTransform[3].z);
        LOG_INFO("%f, %f, %f, %d", nextCell.x, nextCell.y, navEngine.IsWalkableCell(nextCell, data.navFilter));
        if (navEngine.IsWalkableCell(nextCell, data.navFilter))
        {
            data.moveAnimation = {{{0.0f, transform->transform},
                                   {1.0f, nextTransform}},
                                  InterpolationType::Smooth,
                                  data.speed};
            data.moveAnimation.Start();
        }
    }
    else if (Inputs::IsKeyPressed(KeyCode::Left))
    {
        data.moveAnimation = {{{0.0f, transform->transform},
                               {1.0f, glm::rotate(transform->transform, (float)M_PI_2, glm::vec3(0, 1, 0))}},
                              InterpolationType::Smooth,
                              data.speed};
        data.moveAnimation.Start();
    }
    else if (Inputs::IsKeyPressed(KeyCode::Right))
    {
        data.moveAnimation = {{{0.0f, transform->transform},
                               {1.0f, glm::rotate(transform->transform, -(float)M_PI_2, glm::vec3(0, 1, 0))}},
                              InterpolationType::Smooth,
                              data.speed};
        data.moveAnimation.Start();
    }

    // Sampling mouse inputs
    else if (Inputs::IsMouseButtonPressed(MouseButton::Left))
    {
        auto* childTransform = entity.FindChild("Weapon").FindComponent<Transform>();
        if (childTransform)
        {
            data.attackAnimation = {{
                    {0.0f, childTransform->transform},
                    {0.2f, glm::rotate(childTransform->transform, -(float)M_PI / 8.0f, glm::vec3(0, 1, 0))},
                    {0.4f, glm::rotate(childTransform->transform, (float)M_PI / 4.0f, glm::vec3(0, 1, 0))},
                    {1.0f, childTransform->transform}
                },
                InterpolationType::Smooth, 
                data.speed
            };
            data.attackAnimation.Start();
        }
    }
},

// CharacterController::OnEvent
[](Event* event, const Entity& entity, std::any& dataBlock) {},
nullptr);

} 


// == NavAgent ==

NavAgent::NavAgent(const Entity& entity) :
        Scripted("NavAgent", entity),
        m_agent(NavigationEngine::Get().CreateAgent())
{
    
}

NavAgent::NavAgent(const NavAgent& other) :
        Scripted(other),
        m_agent(NavigationEngine::Get().CreateAgent())
{

}

NavAgent::~NavAgent()
{
    NavigationEngine::Get().RemoveAgent(m_agent);
}

void NavAgent::OnUpdate()
{
    if (!m_agent->IsMoving())
    {
        auto transform = GetEntity().FindComponent<Transform>();
        if (transform)
        {
            m_agent->SetPosition(transform->transform[3]);
        }
        
        return;
    }
    
    auto transform = GetEntity().FindComponent<Transform>();
    if (transform)
    {
        auto nextPos = m_agent->GetNextPosition();

        transform->transform[3] = glm::vec4(nextPos, 1.0);
        m_agent->SetPosition(nextPos);

        m_agent->SetAdvanced();
    }
}


} // Namespace Components
