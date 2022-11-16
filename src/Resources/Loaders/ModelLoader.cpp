#include "ModelLoader.h"

#include "Core/Resolver.h"

#include "Resources/Model.h"
#include "Renderer/Mesh.h"
#include "Resources/Manager.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Model Loader

ResourceHandle<Model> ModelLoader::Load(const std::string& path)
{
    auto& resolver = Resolver::Get();
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        resolver.Resolve(path), 
        aiProcess_Triangulate | aiProcess_GenSmoothNormals
    );

    if (!scene || !scene->mRootNode) {
        LOG_ERROR("Could not load model %s : %s", path.c_str(), importer.GetErrorString());
        return {};
    }
    if (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        LOG_ERROR("Model %s is empty, not loading anything...", path.c_str());
        return {};
    }

    m_loadedIdentifier = resolver.AsIdentifier(path);
    m_model = ResourceManager::CreateResource<Model>(m_loadedIdentifier);

    ProcessAssimpNode(scene->mRootNode, scene, glm::mat4(1.0f), "");

    return m_model;
}

void ModelLoader::ProcessAssimpMesh(aiMesh* aiMesh, 
                                    const glm::mat4& transform, 
                                    const std::string& identifier) 
{
    ResourceHandle<Mesh> meshHandle = ResourceManager::CreateResource<Mesh>(identifier);

    std::vector<Vertex> vertices;
    vertices.reserve(aiMesh->mNumVertices);
    for (size_t i=0 ; i < aiMesh->mNumVertices ; i++) 
    {
        Vertex vertex;
        vertex.position = transform * glm::vec4(aiMesh->mVertices[i].x, 
                                                aiMesh->mVertices[i].y, 
                                                aiMesh->mVertices[i].z, 1.0f);

        if (aiMesh->HasNormals())
        {
            vertex.normal = transform * glm::vec4(aiMesh->mNormals[i].x, 
                                                  aiMesh->mNormals[i].y, 
                                                  aiMesh->mNormals[i].z, 0.0f);
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
    for (size_t f=0 ; f < aiMesh->mNumFaces ; f++)
    {
        for (size_t i=0 ; i < aiMesh->mFaces[f].mNumIndices ; i++)
        {
            indices.push_back(aiMesh->mFaces[f].mIndices[i]);
        }
    }

    MeshPtr mesh = meshHandle.Get();
    mesh->SetVertices(vertices);
    mesh->SetIndices(indices);
    m_model.Get()->AddMesh(meshHandle);
}

void ModelLoader::ProcessAssimpNode(const aiNode* node, 
                                    const aiScene* scene, 
                                    const glm::mat4& parentTransform, 
                                    const std::string& parentIdentifier)
    {
        std::string currentIdentifier = parentIdentifier + "/" + node->mName.C_Str();
        glm::mat4 transform = parentTransform * glm::transpose(glm::make_mat4(&node->mTransformation.a1));

        // Loading the first mesh
        if (node->mNumMeshes) 
        {
            ProcessAssimpMesh(scene->mMeshes[node->mMeshes[0]], 
                              transform, 
                              currentIdentifier);

            // TODO: We're loading other meshes with a suffix. These should potentially be loaded as "sub meshes" (sub parts of a Mesh)
            //       to load them into a single resource instead of one resource per mesh.
            for (size_t i=1 ; i < node->mNumMeshes ; i++) 
            {
                ProcessAssimpMesh(scene->mMeshes[node->mMeshes[i]], 
                                  transform, 
                                  currentIdentifier + "_" + std::to_string(i));
            }
        }

        for (size_t i=0 ; i < node->mNumChildren ; i++) 
        {
            ProcessAssimpNode(node->mChildren[i], 
                              scene,
                              transform, 
                              currentIdentifier);
        }
    }