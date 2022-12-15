#ifndef SCRIPTINGCOMPONENTS_H
#define SCRIPTINGCOMPONENTS_H

#include "Scene/Entity.h"

#include <string>
#include <functional>
#include <any>


class Event;


typedef std::function<void(Entity, std::any&)>         OnCreateFn;
typedef std::function<void(Entity, std::any&)>         OnUpdateFn;
typedef std::function<void(Event*, Entity, std::any&)> OnEventFn;
typedef std::function<void(Entity, std::any&)>         OnDestroyFn;


namespace Components
{

class Scripted
{
public:
    Scripted(const std::string& name, const Entity& entity);
    Scripted(const Scripted& other);
    ~Scripted();

    inline const std::string& GetName() const { return m_name; };

    virtual void OnUpdate() {};
    virtual void OnEvent(Event* event) {};

    inline Entity GetEntity() const { return m_entity; };
    inline void SetEntity(const Entity& entity) { m_entity = entity; };

private:
    std::string m_name;
    Entity m_entity;
};


class Scriptable final : public Scripted 
{
public:
    Scriptable() = default;
    Scriptable(const Scriptable& other);
    Scriptable(const std::string& name,
               const Entity& entity,
               const OnCreateFn& onCreate,
               const OnUpdateFn& onUpdate,
               const OnEventFn& onEvent,
               const OnDestroyFn& onDestroy);
    ~Scriptable();
    
    void OnCreate();
    void OnUpdate() override;
    void OnEvent(Event* event) override;
    void OnDestroy();

    template <typename DataType>
    DataType& GetDataBlock()
    {
        return std::any_cast<DataType&>(m_dataBlock);
    }

private:
    std::any m_dataBlock;

    OnCreateFn m_onCreateFn;
    OnUpdateFn m_onUpdateFn;
    OnEventFn m_onEventFn;
    OnDestroyFn m_onDestroyFn;
};

} // Namespace Components


#endif  // SCRIPTINGCOMPONENTS_H