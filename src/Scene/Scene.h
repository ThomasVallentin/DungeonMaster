#ifndef SCENE_H
#define SCENE_H

#include "EntityIndex.h"

#include "Core/Foundations.h"


class Scene;
class Entity;

DECLARE_PTR_TYPE(Scene);





// We are keeping a extra class between the Scene and the iterator to differenciate 
// more clearly the various iterators that will be returned by the Scene later on.
// The EntityView is responsible for iterating the Entities of the Scene.
class EntityView 
{
public:
    class iterator
    {
    public:
        typedef std::forward_iterator_tag iterator_category;
        typedef std::ptrdiff_t            difference_type;
        typedef uint32_t                  value_type;
        typedef Entity                    pointer;
        typedef Entity                    reference;
        
        explicit iterator(const uint32_t& entityId, Scene* scene) : 
            m_id(entityId), m_scene(scene) {}
        
        reference operator->();
        pointer operator*() const;

        iterator& operator++();
        iterator operator++(int) { iterator _tmp = *this; ++(*this); return _tmp; }

        friend bool operator==(const iterator& it, const iterator& other) {return it.m_id == other.m_id && it.m_scene == other.m_scene; }
        friend bool operator!=(const iterator& it, const iterator& other) {return it.m_id != other.m_id || it.m_scene != other.m_scene; }

    private:
        uint32_t m_id;
        Scene* m_scene;
    };

    EntityView(const Entity& entity);

    iterator begin() { return m_begin; }
    iterator end()   { return m_end; }

private:
    EntityView(const uint32_t& beginId, const uint32_t& endId, Scene* scene);

    iterator m_begin;
    iterator m_end;
};


class Scene
{
public:
    ~Scene() = default;

    Entity CreateEntity();
    Entity CreateEntity(const std::string& name);
    Entity CreateEntity(const std::string& name, const Entity& parent);

    Entity CopyEntity(const Entity& entity);

    const Entity& GetRootEntity() const;
    Entity FindByName(const std::string& name);
    
    void RemoveEntity(Entity& entity);
    void Clear();

    EntityView Traverse();

    static ScenePtr Create();

private:
    Scene();
    Entity CreateEntity(const std::string& name, const uint32_t& parent);
    const std::string& GetEntityName(const uint32_t& id);
    uint32_t GetEntityParent(const uint32_t& id);

    EntityIndex m_index;
    uint32_t m_rootId;

    friend Entity;
    friend EntityView;
};

#endif  // SCENE_H