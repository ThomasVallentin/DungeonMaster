#ifndef SCENE_H
#define SCENE_H

#include "EntityIndex.h"

#include "Core/Foundations.h"


class Scene;
class Entity;

DECLARE_PTR_TYPE(Scene);


struct BaseComponent {
    BaseComponent() = default;
    BaseComponent(const std::string& name) : name(name) {}

    std::string name;
};


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
        
        explicit iterator(const EntityDataMap::iterator& it, Scene* scene) : 
            m_it(it), m_scene(scene) {}
        
        reference operator->();
        pointer operator*() const;

        iterator& operator++() { m_it++; return *this; }
        iterator operator++(int) { iterator _tmp = *this; ++(*this); return _tmp; }

        friend bool operator==(const iterator& it, const iterator& other) {return it.m_it == other.m_it; }
        friend bool operator!=(const iterator& it, const iterator& other) {return it.m_it != other.m_it; }

    private:
        EntityDataMap::iterator m_it;
        Scene* m_scene;
    };

    // EntityView(const Entity& entity);

    iterator begin() { return iterator(m_begin, m_scene); }
    iterator end()   { return iterator(m_end, nullptr); }

private:
    EntityView(const EntityDataMap::iterator& begin, const EntityDataMap::iterator& end, Scene* scene) :
            m_begin(begin), m_end(end), m_scene(scene) {}

    EntityDataMap::iterator m_begin;
    EntityDataMap::iterator m_end;
    Scene* m_scene;
    
    friend Scene;
};


class Scene
{
public:
    ~Scene() = default;

    Entity CreateEntity();
    Entity CreateEntity(const std::string& name);
    void RemoveEntity(Entity& entity);

    Entity FindByName(const std::string& name);
    void Clear();

    EntityView Traverse();

    static ScenePtr Create();

private:
    Scene() = default;

    EntityIndex m_index;

    friend Entity;
};

#endif  // SCENE_H