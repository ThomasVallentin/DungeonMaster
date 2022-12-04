#ifndef BASICCOMPONENTS_H
#define BASICCOMPONENTS_H


#include "Scene/Entity.h"
#include "Scene/ScriptEngine.h"

#include "Core/Event.h"
#include "Core/Logging.h"

#include "Resources/Manager.h"

#include "Renderer/Camera.h"

#include <functional>
#include <any>


namespace Components {


struct Transform 
{
    Transform() = default;
    Transform(const glm::mat4& transform) : transform(transform) {}

    static glm::mat4 ComputeWorldMatrix(const Entity& entity);

    glm::mat4 transform{1.0f};
};


struct Camera
{
    Camera() = default;
    Camera(const ::Camera& camera) : camera(camera) {}

    ::Camera camera;
};

struct Mesh
{
    Mesh() = default;
    Mesh(const ResourceHandle<::Mesh>& mesh) : mesh(mesh) {}

    ResourceHandle<::Mesh> mesh;
};

struct RenderMesh
{
    RenderMesh() = default;
    RenderMesh(const ResourceHandle<Material>& material) : material(material) {}

    ResourceHandle<Material> material;
    // bool castShadows;
    // bool receiveShadows;
};


typedef std::function<void(const Entity&, std::any&)>         OnCreateFn;
typedef std::function<void(const Entity&, std::any&)>         OnUpdateFn;
typedef std::function<void(Event*, const Entity&, std::any&)> OnEventFn;
typedef std::function<void(const Entity&, std::any&)>         OnDestroyFn;


class Scripted
{
public:
    Scripted(const std::string& name);
    Scripted(const Scripted& other);
    ~Scripted();

    inline const std::string& GetName() const { return m_name; };

    virtual void OnUpdate() {};
    virtual void OnEvent(Event* event) {};

private:
    std::string m_name;
};


class Scriptable final : Scripted 
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

private:
    std::any m_dataBlock;
    Entity m_entity;

    OnCreateFn m_onCreateFn;
    OnUpdateFn m_onUpdateFn;
    OnEventFn m_onEventFn;
    OnDestroyFn m_onDestroyFn;
};


} // Namespace Components::


#endif  // BASICCOMPONENTS_H
