#ifndef MODELLOADER_H
#define MODELLOADER_H

#include "Resources/Resource.h"
#include "Resources/Prefab.h"

class Material;

class aiScene;
class aiNode;
class aiMesh;


class ModelLoader 
{
public:
    ModelLoader() = default;
    ~ModelLoader() = default;

    ResourceHandle<Prefab> Load(const std::string& path);

private:
    void ProcessNode(const aiNode* node, 
                           const aiScene* scene, 
                           const Entity& entity,
                           const std::string& identifier);
    void ProcessMesh(const aiMesh* aiMesh, 
                           const Entity& entity,
                           const std::string& identifier);
    void ProcessMaterials(const aiScene* scene);

    std::string m_loadedIdentifier;
    ResourceHandle<Prefab> m_prefab;
    std::vector<ResourceHandle<Material>> m_materials;
    // std::vector<MaterialPtr> m_materials;
};


#endif  // MODELLOADER_H
