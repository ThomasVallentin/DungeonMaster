#ifndef ENTITY_H
#define ENTITY_H

#include "Scene.h"
#include "Core/Foundations.h"

#include <stdint.h>
#include <string>


class Entity
{
public:
    ~Entity() = default;

    const std::string& GetName() const;
    Entity GetParent() const;
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
    Entity() : m_id(0), m_scene(nullptr) {}
    Entity(const uint32_t& id, Scene* scene) : m_id(id), m_scene(scene) {}

    uint32_t m_id;
    Scene* m_scene;

    friend Scene;
    friend class EntityView;
};

#endif  // ENTITY_H
