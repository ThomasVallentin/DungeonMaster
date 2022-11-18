#include "Manager.h"

#include "Loaders/ModelLoader.h"
#include "Loaders/LevelLoader.h"

#include "Core/Application.h"
#include "Core/Resolver.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>


ResourceHandle<Prefab> ResourceManager::LoadModel(const std::string& path) 
{
    auto& resolver = Resolver::Get();

    std::string identifier = resolver.AsIdentifier(path);
    if (auto handle = GetResource<Prefab>(identifier)) 
    {
        return handle;
    }

    ModelLoader loader;
    return loader.Load(identifier);
}

ResourceHandle<Texture> ResourceManager::LoadTexture(const std::string& path) 
{
    auto& resolver = Resolver::Get();

    std::string identifier = resolver.AsIdentifier(path);
    if (auto handle = GetResource<Texture>(identifier)) 
    {
        return handle;
    }

    ImagePtr image = Image::Read(resolver.Resolve(path));
    if (!image)
    {
        return ResourceHandle<Texture>();
    }

    return CreateResource<Texture>(identifier, Texture::FromImage(image));
}


ResourceHandle<Prefab> ResourceManager::LoadScene(const std::string& path) 
{
    auto& resolver = Resolver::Get();

    std::string identifier = resolver.AsIdentifier(path);
    if (auto handle = GetResource<Prefab>(identifier)) 
    {
        return handle;
    }

    LevelLoader loader;
    return loader.Load(identifier);
}
