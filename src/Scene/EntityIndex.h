#ifndef ENTITYREGISTRY_H
#define ENTITYREGISTRY_H

#include <stdint.h>
#include <unordered_map>
#include <vector>
#include <any>
#include <stdexcept>


typedef std::vector<std::any> EntityData;
typedef std::unordered_map<uint32_t, EntityData> EntityDataMap;

class EntityIndex
{
public:
    EntityIndex();
    ~EntityIndex();

    // Entity management

    uint32_t CreateId();
    void RemoveId(const uint32_t& entity);
    bool ContainsId(const uint32_t& entity) const;
    void Clear();
    
    // ComponentType management

    template<typename ComponentType, typename... Args>
    ComponentType& EmplaceComponent(const uint32_t& entity, Args&&... args)
    {
        auto it = m_dataMap.find(entity);
        if (it == m_dataMap.end()) 
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
        auto it = m_dataMap.find(entity);
        if (it == m_dataMap.end()) 
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

        throw std::runtime_error("Access was made to a non-existing component !");
    }

    template<typename ComponentType>
    ComponentType* FindComponent(const uint32_t& entity) 
    {
        auto it = m_dataMap.find(entity);
        if (it == m_dataMap.end()) 
        {
            return nullptr;
        }

        for (auto& cmp : it->second) 
        {
            if (cmp.type() == typeid(ComponentType)) 
            {
                return &std::any_cast<ComponentType&>(cmp);
            }
        }

        return nullptr;
    }

    template<typename ComponentType>
    void RemoveComponent(const uint32_t& entity) 
    {
        auto it = m_dataMap.find(entity);
        if (it == m_dataMap.end()) 
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

    bool HasData(const uint32_t& entity) { return m_dataMap.find(entity) != m_dataMap.end(); }
    EntityData& GetData(const uint32_t& entity) { return m_dataMap.find(entity)->second; }
    void SetData(const uint32_t& entity, const EntityData& data);

    EntityDataMap& GetDataMap() { return m_dataMap; }

private:
    uint32_t m_last_uuid = 0;
    EntityDataMap m_dataMap;
};


#endif  // ENTITYREGISTRY_H
