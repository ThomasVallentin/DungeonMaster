#ifndef MESH_H
#define MESH_H


#include "Core/Foundations.h"

#include <glm/glm.hpp>

#include <vector>
#include <memory>


DECLARE_PTR_TYPE(Mesh);


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};


class Mesh
{
public:
    ~Mesh();

    static MeshPtr Create() { return nullptr; };

private:
    Mesh();

    std::vector<Vertex> m_vertices;

};

#endif  // MESH_H