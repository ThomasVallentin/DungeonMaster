#include "Resolver.h"

#include "Logging.h"


Resolver* Resolver::s_instance = nullptr;


Resolver& Resolver::Init(const std::filesystem::path& rootPath) 
{
    if (Resolver::s_instance) {
        LOG_WARNING("Resolver already exists, cannot Init() it twice.");
        return *s_instance;
    }

    s_instance = new Resolver(rootPath);
    return *s_instance;
}

std::string Resolver::Resolve(const std::string& identifier) const
{
    std::filesystem::path normPath = std::filesystem::path(identifier).lexically_normal();
    if (normPath.is_relative())
    {
        return GetResourcesPath() / normPath;
    }

    return identifier;
}

// Identifiers

std::string Resolver::AsIdentifier(const std::string& path) const
{
    std::filesystem::path normPath = std::filesystem::path(path).lexically_normal();
    if (normPath.is_relative())
    {
        return normPath;
    }

    std::filesystem::path resources = GetResourcesPath();
    for (auto it=normPath.begin(), otherIt=resources.begin() ; otherIt != resources.end() ; it++, otherIt++)
    {
        if (it == normPath.end() || *it != *otherIt)
        {
            return normPath;
        }
    }

    return normPath.lexically_relative(resources);
}


std::string Resolver::GetResourcesPath() const
{
    return m_rootPath / "resources";
}
