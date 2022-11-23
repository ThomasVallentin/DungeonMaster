#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(const float& fov, const float& aspectRatio, 
               const float& nearClip, const float& farClip) :
        m_fov(fov),
        m_aspectRatio(aspectRatio),
        m_nearClip(nearClip),
        m_farClip(farClip) 
{
    UpdateProjection();
}

void Camera::UpdateProjection()
{
    m_projMatrix = glm::perspective(glm::radians(m_fov), 
                                    m_aspectRatio, 
                                    m_nearClip,
                                    m_farClip);
}

void Camera::SetFov(const float& fov)
{
    m_fov = fov;
    UpdateProjection();
}

void Camera::SetAspectRatio(const float& aspectRatio)
{
    m_aspectRatio = aspectRatio;
    UpdateProjection();
}

void Camera::SetNearClip(const float& nearClip)
{
    m_nearClip = nearClip;
    UpdateProjection();
}

void Camera::SetFarClip(const float& farClip)
{
    m_farClip = farClip;
    UpdateProjection();
}
