#ifndef RESOLVER_H
#define RESOLVER_H

#include <filesystem>

class Resolver
{
public:
    static Resolver& Init(const std::filesystem::path& rootPath);
    inline static Resolver& Get() { return *s_instance; }


    // Identifier utils
    std::string Resolve(const std::string& identifier) const;
    std::string AsIdentifier(const std::string& path) const;

private:
    Resolver(const std::filesystem::path& rootPath) : m_rootPath(rootPath) {}
    ~Resolver() = default;

    std::string GetResourcesPath() const;
    
    std::filesystem::path m_rootPath;

    static Resolver* s_instance;
};

#endif  // RESOLVER_H
