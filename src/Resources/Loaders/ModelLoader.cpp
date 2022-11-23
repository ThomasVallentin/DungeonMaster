#include "ModelLoader.h"

#include "Core/Resolver.h"

#include "Scene/Components/Basics.h"

#include "Resources/Model.h"
#include "Renderer/Mesh.h"
#include "Resources/Manager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

// Model Loader

ResourceHandle<Prefab> ModelLoader::Load(const std::string &path)
{
    const auto &resolver = Resolver::Get();
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(
        resolver.Resolve(path),
        aiProcess_Triangulate | aiProcess_GenSmoothNormals);

    if (!scene || !scene->mRootNode)
    {
        LOG_ERROR("Could not load model %s : %s", path.c_str(), importer.GetErrorString());
        return {};
    }
    if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        LOG_ERROR("Model %s is empty, not loading anything...", path.c_str());
        return {};
    }

    m_loadedIdentifier = resolver.AsIdentifier(path);
    m_prefab = ResourceManager::CreateResource<Prefab>(m_loadedIdentifier);

    ProcessMaterials(scene);
    ProcessNode(scene->mRootNode, scene, m_prefab.Get()->GetRootEntity(), m_loadedIdentifier + ":");

    return m_prefab;
}

void ModelLoader::ProcessMesh(const aiMesh *aiMesh,
                              const Entity &entity,
                              const std::string &identifier)
{
    ResourceHandle<Mesh> meshHandle = ResourceManager::CreateResource<Mesh>(identifier);

    std::vector<Vertex> vertices;
    vertices.reserve(aiMesh->mNumVertices);
    for (size_t i = 0; i < aiMesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.position = {aiMesh->mVertices[i].x,
                           aiMesh->mVertices[i].y,
                           aiMesh->mVertices[i].z};

        if (aiMesh->HasNormals())
        {
            vertex.normal = {aiMesh->mNormals[i].x,
                             aiMesh->mNormals[i].y,
                             aiMesh->mNormals[i].z};
            vertex.normal = glm::normalize(vertex.normal);
        }
        else
            vertex.normal = {0.0f, 0.0f, 0.0f};

        if (aiMesh->HasTextureCoords(0))
            vertex.texCoords = {aiMesh->mTextureCoords[0][i].x,
                                aiMesh->mTextureCoords[0][i].y};
        else
            vertex.texCoords = {0.0f, 0.0f};

        vertices.push_back(vertex);
    }

    std::vector<uint32_t> indices;
    for (size_t f = 0; f < aiMesh->mNumFaces; f++)
    {
        for (size_t i = 0; i < aiMesh->mFaces[f].mNumIndices; i++)
        {
            indices.push_back(aiMesh->mFaces[f].mIndices[i]);
        }
    }

    MeshPtr mesh = meshHandle.Get();
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);
    entity.EmplaceComponent<Components::Mesh>(meshHandle);
    entity.EmplaceComponent<Components::RenderMesh>(m_materials[aiMesh->mMaterialIndex]);
}

void ModelLoader::ProcessNode(const aiNode *node,
                              const aiScene *scene,
                              const Entity &entity,
                              const std::string &identifier)
{
    std::string currentIdentifier = identifier + "/" + node->mName.C_Str();
    entity.EmplaceComponent<Components::Transform>(glm::transpose(glm::make_mat4(&node->mTransformation.a1)));

    // One mesh, adding its info to the current Entity
    if (node->mNumMeshes == 1)
    {
        ProcessMesh(scene->mMeshes[node->mMeshes[0]],
                    entity,
                    currentIdentifier);
    }
    // Multiple meshes, adding them as children
    else if (node->mNumMeshes)
    {
        std::string meshName = entity.GetName() + "_msh";
        Entity child = entity.AddChild(meshName);
        ProcessMesh(scene->mMeshes[node->mMeshes[0]],
            child,
            currentIdentifier);

        // TODO: We're loading other meshes with a suffix. These should potentially be loaded as "sub meshes" (sub parts of a Mesh)
        //       to load them into a single resource instead of one resource per mesh.
        for (size_t i = 1; i < node->mNumMeshes; i++)
        {
            child = entity.AddChild(meshName + std::to_string(i));
            ProcessMesh(scene->mMeshes[node->mMeshes[i]],
                        child,
                        currentIdentifier + "_" + std::to_string(i));
        }
    }

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        Entity child = entity.AddChild(node->mChildren[i]->mName.C_Str());
        ProcessNode(node->mChildren[i],
                    scene,
                    child,
                    currentIdentifier);
    }
}

void ModelLoader::ProcessMaterials(const aiScene *scene)
{
    const auto &resolver = Resolver::Get();

    m_materials.reserve(scene->mNumMaterials);
    for (size_t i = 0; i < scene->mNumMaterials; i++)
    {
        auto aiMaterial = scene->mMaterials[i];
        MaterialPtr material = Material::Create(Shader::Open(resolver.Resolve("Shaders/default.vert"),
                                                             resolver.Resolve("Shaders/default.frag")));
        aiColor3D color;
        if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color) != AI_FAILURE)
            material->SetInputValue("diffuseColor", glm::vec3(color.r, color.g, color.b));
        else
            material->SetInputValue("diffuseColor", glm::vec3(0.0, 1.0, 0.0));
        
        if (aiMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color) != AI_FAILURE)
            material->SetInputValue("ambientColor", glm::vec3(color.r, color.g, color.b));

        ResourceHandle<Material> handle = ResourceManager::CreateResource<Material>(
            m_loadedIdentifier + ":" + aiMaterial->GetName().C_Str(),
            material);

        m_materials.push_back(handle);
    }
}
