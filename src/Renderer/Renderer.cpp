#include "Renderer.h"

#include "Scene/Components/Basics.h"

#include "Mesh.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Material.h"

#include "Core/Resolver.h"
#include "Core/Time.h"

#include <glad/glad.h>


Renderer* Renderer::s_instance = nullptr;


Renderer& Renderer::Init()
{
    s_instance = new Renderer;
    return *s_instance;
}

Renderer::Renderer()
{
    Resolver& resolver = Resolver::Get();

    // Full screen render (Blit) utils
    m_blitTextureArray = VertexArray::Create();
    m_blitTextureShader = Shader::Open(resolver.Resolve("Shaders/fullScreen.vert"), 
                                       resolver.Resolve("Shaders/sprite.frag"));

}

void Renderer::SetRenderBuffer(const FrameBufferPtr& renderBuffer)
{
    m_renderBuffer = renderBuffer;
    if (m_renderBuffer)
    {
        FrameBufferSpecs specs = m_renderBuffer->GetSpecs();
        specs.samples = 1;
        m_postProcessBuffer = FrameBuffer::Create(specs);
    }
    else 
    {
        m_postProcessBuffer = nullptr;
    }
}

void Renderer::RenderScene(const ScenePtr& scene, const Entity& cameraEntity)
{
    if (!m_renderBuffer)
    {
        return;
    }
    // Clearing all the FrameBuffers
    m_postProcessBuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    m_renderBuffer->Bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glm::mat4 viewMatrix(1.0f);
    glm::mat4 camModelMatrix(1.0f);
    glm::mat4 projMatrix(1.0f);

    auto* camera = cameraEntity.FindComponent<Components::Camera>();
    if (camera)
    {
        camModelMatrix = Components::Transform::ComputeWorldMatrix(cameraEntity);
        viewMatrix = glm::inverse(camModelMatrix);
        projMatrix = camera->camera.GetProjMatrix();
    }

    glm::mat4 viewProjMatrix = projMatrix * viewMatrix;

    // Rendering the scene
    for (Entity entity : scene->Traverse())
    {
        auto* meshRenderComp = entity.FindComponent<Components::RenderMesh>();
        if (meshRenderComp)
        {
            auto* meshComp = entity.FindComponent<Components::Mesh>();
            if (!meshComp)
            {
                continue;
            }
            
            glm::mat4 modelMatrix = Components::Transform::ComputeWorldMatrix(entity);

            auto material = meshRenderComp->material.Get();
            auto mesh = meshComp->mesh.Get();

            // TODO: Insert culling here

            double time = Time::GetTime();
            material->Bind();
            material->ApplyUniforms();
            material->GetShader()->SetMat4("uModelMatrix", modelMatrix);
            material->GetShader()->SetMat4("uViewMatrix", viewMatrix);
            material->GetShader()->SetMat4("uCameraModelMatrix", camModelMatrix);
            material->GetShader()->SetMat3("uNormalMatrix", glm::transpose(glm::inverse(glm::mat3(modelMatrix))));
            material->GetShader()->SetMat4("uMVPMatrix", viewProjMatrix * modelMatrix);
            material->GetShader()->SetVec3("uPointLights[0].position", glm::vec3(glm::inverse(viewMatrix) * glm::vec4(0, 0, 0, 1)));
            material->GetShader()->SetVec3("uPointLights[0].color", glm::vec3(0.8 + (std::abs(sin(time * 2.3)) * 2 + sin(0.5 + time * 7.7)) * 0.3) * 10.0f);  // Flicking torch effect
            material->GetShader()->SetFloat("uPointLights[0].decay", 2.0f);
            material->GetShader()->SetInt("uDoubleSided", meshRenderComp->doubleSided); 
            material->GetShader()->SetFloat("uTime", time); 
            mesh->Bind();

            glDrawElements(GL_TRIANGLES, 
                           mesh->GetElementCount(),
                           GL_UNSIGNED_INT,
                           nullptr);

            mesh->Unbind();
            material->Unbind();
            continue;
        }

        auto* renderImage = entity.FindComponent<Components::RenderImage>();
        if (renderImage)
        {
            VertexArrayPtr varray = VertexArray::Create();
            varray->Bind();

            m_blitTextureShader->Bind();
            renderImage->image.Get()->Bind(0);
            m_blitTextureShader->SetInt("uTexture", 0);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            varray->Unbind();
            continue;
        }
    }
}

void Renderer::SetPostProcessShader(const ShaderPtr& shader)
{
    m_postProcessShader = shader;
}

void Renderer::BlitRenderToBuffer(const uint32_t& frameBufferId) const
{
    if (!m_postProcessShader)
    {
        m_renderBuffer->Bind();
        m_renderBuffer->Blit(frameBufferId, m_renderBuffer->GetWidth(), m_renderBuffer->GetHeight());
        m_renderBuffer->Unbind();
    }
    else
    {
        // Blit the render to the "post processing" buffer (non-multisampled)
        m_renderBuffer->Blit(m_postProcessBuffer->GetId(), m_renderBuffer->GetWidth(), m_renderBuffer->GetHeight());
        FrameBuffer::BindFromId(frameBufferId);

        // Draw the post process into the destination frame buffer
        VertexArrayPtr varray = VertexArray::Create();
        varray->Bind();

        m_postProcessShader->Bind();
        Texture::BindFromId(m_postProcessBuffer->GetColorAttachmentId(0), 0);
        Texture::BindFromId(m_postProcessBuffer->GetDepthAttachmentId(), 1);
        m_postProcessShader->SetInt("uBeauty", 0);
        m_postProcessShader->SetInt("uDepth", 1);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        varray->Unbind();
    }
}

void Renderer::SetClearColor(const glm::vec3& color)
{
    glClearColor(color.x, color.y, color.z, 1.0f);
}

void Renderer::ClearBuffer(const uint32_t& frameBuffer)
{
    FrameBuffer::BindFromId(frameBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
