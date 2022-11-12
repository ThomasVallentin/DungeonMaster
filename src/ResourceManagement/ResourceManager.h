#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"

#include <filesystem>
#include <unordered_map>
#include <memory>
#include <vector>


class Mesh;
class Model;
class Texture;
class ResourceManager;

template <typename T>
struct Resource {
    std::shared_ptr<T> data;
    bool hasOwner = false;  // Define whether this ressource can be removed on its own or if an other object will take care of it.
};


template <typename T>
class ResourceHandle {
public:
    ResourceHandle() = default;
    
    std::shared_ptr<T> Get() const;
    inline const std::string& GetIdentifier() const { return m_identifier; }
    void Update();

private:
    ResourceHandle(const std::string& identifier) :
            m_identifier(identifier) {}
    ResourceHandle(const std::string& identifier, const  std::weak_ptr<Resource<T>>& resource) :
            m_resource(resource), m_identifier(identifier) {}

    std::string m_identifier;
    std::weak_ptr<Resource<T>> m_resource;

    friend ResourceManager;
};


class ResourceManager {
public:
    template <typename T>
    static ResourceHandle<T> CreateResource(const std::string& identifier, 
                                            const bool& hasOwner = false)
    {
        // TODO: Generate properly unique identifiers
        Resource<T>* resource = new Resource<T>{T::Create(), hasOwner};
        std::shared_ptr<Resource<T>> resourcePtr(resource);

        s_resources<T>.insert({ identifier, resourcePtr });

        return { identifier, resourcePtr };
    }

    template <typename T>
    static ResourceHandle<T> CreateResource(const std::string& identifier, 
                                            const std::shared_ptr<T>& data, 
                                            const bool& hasOwner = false)
    {
        // TODO: Generate properly unique identifiers
        Resource<T>* resource = new Resource<T>{data, hasOwner};
        std::shared_ptr<Resource<T>> resourcePtr(resource);

        s_resources<T>.insert({ identifier, resourcePtr });

        return { identifier, resourcePtr };
    }

    template <typename T>
    static ResourceHandle<T> GetResource(const std::string& identifier) 
    {
        auto it = s_resources<T>.find(identifier);
        if (it == s_resources<T>.end()) 
            return {identifier};
    
        return { identifier, it->second };
    }

    template <typename T>
    static void FreeResource(const std::string& identifier)
    {
        auto it = s_resources<T>.find(identifier);
        if (it != s_resources<T>.end()) 
            s_resources<T>.erase(it);
    }

    template <typename T>
    static void UpdateHandle(ResourceHandle<T>& handle)
    {
        auto it = s_resources<T>.find(handle.identifier);
        if (it != s_resources<T>.end()) 
            handle.m_resource = it->second;
    }

    static ResourceHandle<Model> LoadModel(const std::string& path);
    static ResourceHandle<Texture> LoadTexture(const std::string& path);

private:


    template <typename T>
    static std::unordered_map<std::string, std::shared_ptr<Resource<T>>> s_resources;
};


// entity.AddComponent<MeshComponent>(ResourceManager::GetResource<Mesh>("meshes/Pouet.fbx:Root/Pouet/Bidule"));
// entity.AddComponent<MeshRenderComponent>(ResourceManager::GetResource<Material>("materials/Pouet.fbx:"));

// ResourceManager::GetResource<Mesh>("Models/monmodel.fbx:Table/Plate_1");
// ResourceManager::FreeResource<Mesh>("Models/monmodel.fbx:Table/Plate_1") // -> Not working since mesh has owner;
// ResourceManager::FreeResource<Model>("Models/monmodel.fbx")              // -> Working, free all Meshes and Materials from monmodel.fbx;

#endif  // RESOURCEMANAGER_H
