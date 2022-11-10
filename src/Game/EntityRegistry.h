#ifndef ENTITYREGISTRY_H
#define ENTITYREGISTRY_H

#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <any>
#include <stdexcept>


class EntityRegistry
{
public:
    EntityRegistry();
    ~EntityRegistry();

    // Entity management

    uint32_t Create();
    void Remove(const uint32_t& entity);
    void Clear();
    
    // ComponentType management

    template<typename ComponentType, typename... Args>
    ComponentType& EmplaceComponent(const uint32_t& entity, Args&&... args)
    {
        auto it = m_registry.find(entity);
        if (it == m_registry.end()) 
        {
            throw std::runtime_error("Access was made to a non-existing entity !");
        }

        for (auto& cmp : it->second) 
        {
            if (cmp.type() == typeid(ComponentType)) 
            {
                return std::any_cast<ComponentType&>(cmp);
            }
        }

        it->second.push_back(std::make_any<ComponentType>(std::forward<Args>(args)...));
        return std::any_cast<ComponentType&>(it->second.back());
    }

    template<typename ComponentType>
    ComponentType& GetComponent(const uint32_t& entity) 
    {
        auto it = m_registry.find(entity);
        if (it == m_registry.end()) 
        {
            throw std::runtime_error("Access was made to a non-existing entity !");
        }

        for (auto& cmp : it->second) 
        {
            if (cmp.type() == typeid(ComponentType)) 
            {
                return std::any_cast<ComponentType&>(cmp);
            }
        }

        throw std::runtime_error("Access was made to a non-existing component of entity !");
    }

    template<typename ComponentType>
    ComponentType* FindComponent(const uint32_t& entity) const 
    {
        auto it = m_registry.find(entity);
        if (it == m_registry.end()) 
        {
            return nullptr;
        }

        for (const auto& cmp : it->second) 
        {
            if (cmp.type() == typeid(ComponentType)) 
            {
                return &std::any_cast<ComponentType>(cmp);
            }
        }

        return nullptr;
    }

    template<typename ComponentType>
    void RemoveComponent(const uint32_t& entity) 
    {
        auto it = m_registry.find(entity);
        if (it == m_registry.end()) 
        {
            return;
        }

        auto& cmpVector = it->second;
        for (auto cmpIt = cmpVector.begin() ; cmpIt != cmpVector.end() ; )
        {
            if (cmpIt->type() == typeid(ComponentType)) 
            {
                cmpVector.erase(cmpIt);
            }
            else
            {
                cmpIt++;
            }
        }
    }

private:
    uint32_t m_last_uuid = 0;
    std::unordered_map<uint32_t, std::vector<std::any>> m_registry;
};


#endif  // ENTITYREGISTRY_H