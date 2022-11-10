#include "EntityRegistry.h"

#include "Core/Logging.h"

#include <random>

// Very (very) basic UUID system based on a random seed and a random increment each time an entity is created.
static std::random_device uuidRandomGenerator;
static std::uniform_int_distribution<uint32_t> nextUuidDistrib(0, 1024);


EntityRegistry::EntityRegistry()
{
    std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
    m_last_uuid = dist(uuidRandomGenerator);

    LOG_INFO("registry seed %u", m_last_uuid);
}

EntityRegistry::~EntityRegistry()
{
    
}

uint32_t EntityRegistry::Create()
{
    m_last_uuid += nextUuidDistrib(uuidRandomGenerator);
    m_registry.insert({m_last_uuid, {}});

    return m_last_uuid;
}

void EntityRegistry::Remove(const uint32_t& entity)
{
    auto it = m_registry.find(entity);
    if (it != m_registry.end()) {
        m_registry.erase(it);
    }
}

void EntityRegistry::Clear() {
    m_registry.clear();
}
