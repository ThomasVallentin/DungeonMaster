#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include "Resources/Resource.h"
#include "Resources/Prefab.h"


class LevelLoader
{
public:
    LevelLoader() = default;
    ~LevelLoader() = default;

    ResourceHandle<Prefab> Load(const std::string& path);

private:
    ResourceHandle<Prefab> m_prefab;
};

#endif  // LEVELLOADER_H
