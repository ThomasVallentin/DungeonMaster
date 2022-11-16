#ifndef MODELLOADER_H
#define MODELLOADER_H

#include "Resources/Resource.h"
#include "Resources/Model.h"


class ModelLoader 
{
public:
    ModelLoader() = default;
    ~ModelLoader() = default;

    ResourceHandle<Model> Load(const std::string& path);

private:
    void ProcessAssimpMesh(aiMesh* aiMesh, 
                           const glm::mat4& transform, 
                           const std::string& identifier);
    void ProcessAssimpNode(const aiNode* node, 
                           const aiScene* scene, 
                           const glm::mat4& parentTransform,
                           const std::string& parentIdentifier);

    std::string m_loadedIdentifier;
    ResourceHandle<Model> m_model;
    // std::vector<MaterialPtr> m_materials;
};


#endif  // MODELLOADER_H
