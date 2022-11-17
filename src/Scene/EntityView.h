#ifndef ENTITYVIEW_H
#define ENTITYVIEW_H

#include <algorithm>
#include <stdint.h>

class Scene;
class Entity;

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

#endif  // ENTITYVIEW_H