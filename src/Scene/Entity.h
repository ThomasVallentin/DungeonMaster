#ifndef ENTITY_H
#define ENTITY_H

#include "Scene.h"
#include "Core/Foundations.h"

#include "Utils/TypeUtils.h"

#include <stdint.h>
#include <string>


class Entity
{
public:
    Entity() = default;
    ~Entity() = default;

    std::string GetName() const;

    Entity GetParent() const;
    std::vector<Entity> GetChildren() const;
    Entity FindChild(const std::string& name) const;
    Entity AddChild(const std::string& name) const;

    void Remove();

    template<typename ComponentType, typename... Args>
    ComponentType& EmplaceComponent(Args&&... args) const {
        return m_scene->m_index.EmplaceComponent<ComponentType>(m_id, std::forward<Args>(args)...);
    }

    template<typename ComponentType>
    ComponentType& GetComponent() const {
        return m_scene->m_index.GetComponent<ComponentType>(m_id);
    }

    template<typename ComponentType>
    ComponentType* FindComponent() const {
        return m_scene->m_index.FindComponent<ComponentType>(m_id);
    }

    bool IsValid() const;
    bool IsRoot() const;
    
    inline Scene* GetScene() const { return m_scene; } 

    inline operator bool() { return IsValid(); }
    inline bool operator ==(const Entity& other) const { return m_id == other.m_id && m_scene == other.m_scene; }
    inline bool operator !=(const Entity& other) const { return m_id != other.m_id || m_scene != other.m_scene; }

private:
    Entity(const uint32_t& id, Scene* scene) : m_id(id), m_scene(scene) {}

    uint32_t m_id = 0;
    Scene* m_scene = nullptr;
    // TODO: Entities could be speed up by holding a pointer to their data 
    //       instead of querying the map each time they require a Component

    friend Scene;
    friend class EntityView;
    friend std::hash<Entity>;
};


// Mandatory to define unordered_maps with Entity as keys
template <>
struct std::hash<Entity>
{
    std::size_t operator()(const Entity& entity) const
    {
        size_t hash = 0;
        HashCombine(hash, entity.m_id);

        return hash;
    }
};

#endif  // ENTITY_H
