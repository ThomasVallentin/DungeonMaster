#include "EntityIndex.h"

#include "Core/Logging.h"

#include <random>

// Very (very) basic UUID system based on a random seed and a random increment each time an entity is created.
static std::random_device uuidRandomGenerator;
static std::uniform_int_distribution<uint32_t> nextUuidDistrib(0, 1024);


EntityIndex::EntityIndex()
{
    std::uniform_int_distribution<uint32_t> dist(1, UINT32_MAX);
    m_last_uuid = dist(uuidRandomGenerator);

    LOG_INFO("index seed %u", m_last_uuid);
}

EntityIndex::~EntityIndex()
{
    Clear();
}

uint32_t EntityIndex::Create()
{
    m_last_uuid += nextUuidDistrib(uuidRandomGenerator);
    m_data.insert({m_last_uuid, {}});

    return m_last_uuid;
}

void EntityIndex::Remove(const uint32_t& entity)
{
    auto it = m_data.find(entity);
    if (it != m_data.end()) {
        m_data.erase(it);
    }
}

void EntityIndex::Clear() {
    m_data.clear();
}
