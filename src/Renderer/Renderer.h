#ifndef RENDERER_H
#define RENDERER_H

#include "FrameBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#include "Scene/Scene.h"


class Renderer
{
public:
    static Renderer& Init();
    inline static Renderer& Get() { return *s_instance; };

    inline FrameBufferPtr GetRenderBuffer() const { return m_renderBuffer; };
    void SetRenderBuffer(const FrameBufferPtr& renderBuffer);

    void RenderScene(const ScenePtr& scene, const Entity& camera);
    void BlitRenderToBuffer(const uint32_t& frameBufferId) const;

    inline ShaderPtr GetPostProcessShader() const { return m_postProcessShader; }
    void SetPostProcessShader(const ShaderPtr& shader);

    void SetClearColor(const glm::vec3& color);
    void ClearBuffer(const uint32_t& frameBuffer);

private:
    Renderer();
    ~Renderer() = default;
    Renderer(const Renderer&) = delete;

    FrameBufferPtr m_renderBuffer;
    FrameBufferPtr m_postProcessBuffer;

    ShaderPtr m_postProcessShader;

    ShaderPtr m_blitTextureShader;
    VertexArrayPtr m_blitTextureArray;

    static Renderer* s_instance;
};

#endif