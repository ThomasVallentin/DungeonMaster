#ifndef TEXTURE_H
#define TEXTURE_H

#include "Core/Image.h"

#include "Core/Foundations.h"

#include <glad/glad.h>

class Texture;

DECLARE_PTR_TYPE(Texture);


class Texture
{
public:
    ~Texture();

    void Bind(const GLuint& unit) const;
    void Unbind() const;
    bool IsValid() const;

    void SetData(void* data, const uint32_t& size, const GLenum& dataType = GL_FLOAT) const;
    void SetData(const uint32_t& width, const uint32_t& height, void* data,
                 const GLenum& internalFormat = GL_RGBA8, 
                 const GLenum& dataFormat = GL_RGBA, 
                 const GLenum& dataType = GL_FLOAT);

    static TexturePtr Create(const uint32_t &width, const uint32_t &height,
                             const GLenum& internalFormat, 
                             const GLenum& dataFormat);
    static TexturePtr Open(const ImageSpecs& specs, const GLenum& internalFormat = GL_RGBA8);
    static TexturePtr FromImage(const ImageConstWeakPtr& image, const GLenum& internalFormat = GL_RGBA8);
    
private:
    Texture();
    Texture(const uint32_t &width, const uint32_t &height,
            const GLenum& internalFormat, 
            const GLenum& dataFormat);

    GLuint m_id = 0;
    
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    GLenum m_internalFormat; 
    GLenum m_dataFormat; 
};

#endif  // TEXTURE_H
