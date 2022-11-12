#ifndef MODEL_H
#define MODEL_H

#include "ResourceManager.h"

#include "Core/Foundations.h"
#include "Renderer/Mesh.h"

#include <vector>
#include <unordered_map>

class Model;

DECLARE_PTR_TYPE(Model);


// This class is roughly equivalent to a prefab, and should be adapted 
// to support all the usual features of such object
class Model
{
public:
    ~Model() = default;

    void AddMesh(const ResourceHandle<Mesh>& meshHandle);
    inline const std::vector<ResourceHandle<Mesh>>& GetMeshes() const { return m_meshes; }

    static ModelPtr Create();

private:
    Model() = default;

    std::vector<ResourceHandle<Mesh>> m_meshes;
    // std::vector<ResourceHandle<Material>> m_materialIdentifiers;
};

#endif  // MODEL_H