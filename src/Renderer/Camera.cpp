#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const ProjectionSpecs& specs) : m_projSpecs(specs)
{
    UpdateProjMatrix();
}

Camera::Camera(const glm::mat4& viewMatrix, const ProjectionSpecs& specs) :
        m_viewMatrix(viewMatrix), m_projSpecs(specs)
{
    UpdateProjMatrix();
}

void Camera::UpdateProjMatrix()
{
    m_projMatrix = glm::perspective(glm::radians(m_projSpecs.fov), 
                                m_projSpecs.aspectRatio, 
                                m_projSpecs.nearClip,
                                m_projSpecs.farClip);
}

void Camera::SetProjectionSpecs(const ProjectionSpecs& specs)
{
    m_projSpecs = specs;
    UpdateProjMatrix();
}


CameraPtr Camera::Create(const ProjectionSpecs& specs)
{
    return CameraPtr(new Camera(specs));
}

CameraPtr Camera::Create(const glm::mat4& viewMatrix, const ProjectionSpecs& specs)
{
    return CameraPtr(new Camera(viewMatrix, specs));
}

void Camera::SetFov(const float& fov)
{
    m_projSpecs.fov = fov;
    UpdateProjMatrix();
}

void Camera::SetAspectRatio(const float& aspectRatio)
{
    m_projSpecs.aspectRatio = aspectRatio;
    UpdateProjMatrix();
}

void Camera::SetNearClip(const float& nearClip)
{
    m_projSpecs.nearClip = nearClip;
    UpdateProjMatrix();
}

void Camera::SetFarClip(const float& farClip)
{
    m_projSpecs.farClip = farClip;
    UpdateProjMatrix();
}
