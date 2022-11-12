#ifndef CAMERA_H
#define CAMERA_H


#include "Core/Foundations.h"

#include <glm/glm.hpp>

class Camera;

DECLARE_PTR_TYPE(Camera);


struct ProjectionSpecs {
    float fov = 35.0f;
    float aspectRatio = 1.778f;
    float nearClip = 1.0f, farClip = 10000.0f;
};


class Camera
{
public:
    ~Camera() = default;

    // Projection specs 

    inline const float& GetFov() const { return m_projSpecs.fov; } 
    void SetFov(const float& fov);

    inline const float& GetAspectRatio() const { return m_projSpecs.aspectRatio; } 
    void SetAspectRatio(const float& aspectRatio);

    inline const float& GetNearClip() const { return m_projSpecs.nearClip; } 
    void SetNearClip(const float& nearClip);

    inline const float& GetFarClip() const { return m_projSpecs.farClip; } 
    void SetFarClip(const float& farClip);

    inline ProjectionSpecs GetProjectionSpecs() const { return m_projSpecs; }
    void SetProjectionSpecs(const ProjectionSpecs& specs);

    // View Matrix

    inline const glm::mat4& GetViewMatrix() const { return m_viewMatrix; };
    inline void SetViewMatrix(const glm::mat4& viewMatrix) { m_viewMatrix = viewMatrix; }

    // Proj Matrix

    inline const glm::mat4& GetProjMatrix() const { return m_projMatrix; };
    inline glm::mat4 GetViewProjMatrix() const { return m_projMatrix * m_viewMatrix; };

    // Creators

    static CameraPtr Create(const ProjectionSpecs& specs);
    static CameraPtr Create(const glm::mat4& viewMatrix, const ProjectionSpecs& specs);

private:
    Camera(const ProjectionSpecs& specs);
    Camera(const glm::mat4& viewMatrix, const ProjectionSpecs& specs);

    void UpdateProjMatrix();

    ProjectionSpecs m_projSpecs;

    glm::mat4 m_projMatrix;
    glm::mat4 m_viewMatrix;  
};

#endif  // CAMERA_H