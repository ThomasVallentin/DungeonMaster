#ifndef RESOURCE_H
#define RESOURCE_H

#include <memory>
#include <string>


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

    inline operator bool() const { return !m_resource.expired(); }

private:
    ResourceHandle(const std::string& identifier) :
            m_identifier(identifier) {}
    ResourceHandle(const std::string& identifier, const  std::weak_ptr<Resource<T>>& resource) :
            m_resource(resource), m_identifier(identifier) {}

    std::string m_identifier;
    std::weak_ptr<Resource<T>> m_resource;

    friend ResourceManager;
};


template <typename T>
std::shared_ptr<T> ResourceHandle<T>::Get() const
{
    if (auto resource = m_resource.lock())
    {
        return resource->data;
    }

    return nullptr;
}


#endif // RESOURCE_H