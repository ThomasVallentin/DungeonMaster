#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "Resource.h"
#include "Prefab.h"

#include "Renderer/Material.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"

#include <filesystem>
#include <unordered_map>
#include <memory>
#include <vector>


class Level;

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

    static ResourceHandle<Prefab> LoadModel(const std::string& path);
    static ResourceHandle<Texture> LoadTexture(const std::string& path);
    static ResourceHandle<Level> LoadLevel(const std::string& path);

private:

    template <typename T>
    static std::unordered_map<std::string, std::shared_ptr<Resource<T>>> s_resources;
};

template<typename T>
std::unordered_map<std::string, std::shared_ptr<Resource<T>>> ResourceManager::s_resources;


#endif  // RESOURCEMANAGER_H
