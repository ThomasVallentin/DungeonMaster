#ifndef LEVEL_H
#define LEVEL_H

#include "Renderer/Mesh.h"

#include <string>

class Level
{
public:
    Level();
    ~Level();

    void Load(const std::string& path);

    MeshPtr m_mesh;
};

#endif  // LEVEL_H
