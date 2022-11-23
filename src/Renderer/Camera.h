#ifndef CAMERA_H
#define CAMERA_H


#include "Core/Foundations.h"

#include <glm/glm.hpp>


class Camera
{
public:
    Camera() = default;
    Camera(const float& fov, const float& aspectRatio, 
           const float& nearClip, const float& farClip);
    ~Camera() = default;

    // Projection specs 

    inline const float& GetFov() const { return m_fov; } 
    void SetFov(const float& fov);

    inline const float& GetAspectRatio() const { return m_aspectRatio; } 
    void SetAspectRatio(const float& aspectRatio);

    inline const float& GetNearClip() const { return m_nearClip; } 
    void SetNearClip(const float& nearClip);

    inline const float& GetFarClip() const { return m_farClip; } 
    void SetFarClip(const float& farClip);

    // Proj Matrix

    inline const glm::mat4& GetProjMatrix() const { return m_projMatrix; };

private:
    void UpdateProjection();
    
    glm::mat4 m_projMatrix;

    float m_fov = 35.0f;
    float m_aspectRatio = 1.778f;
    float m_nearClip = 0.1f, m_farClip = 1000.0f;
};

#endif  // CAMERA_H