#include "EntityIndex.h"

#include "Core/Logging.h"

#include <random>

// Very (very) basic UUID system based on a random seed and a random increment each time an entity is created.
static std::random_device uuidRandomGenerator;
static std::uniform_int_distribution<uint32_t> nextUuidDistrib(1, 1024);


EntityIndex::EntityIndex()
{
    std::uniform_int_distribution<uint32_t> dist(1, UINT32_MAX);
    m_last_uuid = dist(uuidRandomGenerator);
}

EntityIndex::~EntityIndex()
{
    Clear();
}

uint32_t EntityIndex::CreateId()
{
    m_last_uuid += nextUuidDistrib(uuidRandomGenerator);
    m_dataMap.insert({m_last_uuid, {}});

    return m_last_uuid;
}

void EntityIndex::RemoveId(const uint32_t& entity)
{
    auto it = m_dataMap.find(entity);
    if (it != m_dataMap.end()) {
        m_dataMap.erase(it);
    }
}

bool EntityIndex::ContainsId(const uint32_t& entity) const
{
    const auto it = m_dataMap.find(entity);
    return it != m_dataMap.end();
}

void EntityIndex::Clear() {
    m_dataMap.clear();
}

void EntityIndex::SetData(const uint32_t& entity, const EntityData& data)
{
    auto it = m_dataMap.find(entity);
    if (it != m_dataMap.end()) {
        it->second = data;
    }
}
