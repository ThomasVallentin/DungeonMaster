#include "VertexBuffer.h"


// == VertexBuffer ==

VertexBuffer::VertexBuffer()
{
    glGenBuffers(1, &m_id);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_id);
    m_id = 0;
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool VertexBuffer::IsValid() const
{
    return m_id;
}

VertexBufferLayout VertexBuffer::GetLayout() const
{
    return m_layout;
}

void VertexBuffer::SetLayout(const VertexBufferLayout& layout)
{
    m_layout = layout;
}

void VertexBuffer::SetData(void* data, const GLuint& size) const
{
    Bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    Unbind();
}

VertexBufferPtr VertexBuffer::Create()
{
    VertexBuffer* buffer = new VertexBuffer();
    return VertexBufferPtr(buffer);
}

VertexBufferPtr VertexBuffer::Create(void* data, const GLuint& size)
{
    VertexBuffer* buffer = new VertexBuffer();
    buffer->SetData(data, size);
    
    return VertexBufferPtr(buffer);
}


// == IndexBuffer ==

IndexBuffer::IndexBuffer()
{
    glGenBuffers(1, &m_id);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &m_id);
    m_id = 0;
}

void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool IndexBuffer::IsValid() const
{
    return m_id;
}

void IndexBuffer::SetData(GLuint* indices, const GLuint& count) {
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), indices, GL_STATIC_DRAW);
    m_count = count;
    Unbind();
}

IndexBufferPtr IndexBuffer::Create()
{
    IndexBuffer* buffer = new IndexBuffer();
    return IndexBufferPtr(buffer);
}

IndexBufferPtr IndexBuffer::Create(GLuint* indices, const GLuint& count)
{
    IndexBuffer* buffer = new IndexBuffer();
    buffer->SetData(indices, count);
    
    return IndexBufferPtr(buffer);
}
