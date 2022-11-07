#include "VertexArray.h"


VertexArray::VertexArray() : m_id(0)
{
    glGenVertexArrays(1, &m_id);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_id);
    m_id = 0;
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_id);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}

bool VertexArray::IsValid() const
{
    return m_id;
}

void VertexArray::AddVertexBuffer(std::shared_ptr<VertexBuffer> &vertexBuffer)
{
    Bind();
    vertexBuffer->Bind();

    GLuint i = 0;
    const auto& layout = vertexBuffer->GetLayout();
    for (const auto& attribute : layout) {
        glVertexAttribPointer(i, 
                              attribute.dimension, 
                              attribute.type, 
                              attribute.normalized, 
                              layout.GetStride(), 
                              (const void*)attribute.offset);
        glEnableVertexAttribArray(i);

        i++;
    }

    m_vertexBuffers.push_back(vertexBuffer);
    Unbind();
}

void VertexArray::SetIndexBuffer(std::shared_ptr<IndexBuffer> &indexBuffer)
{
    Bind();
    indexBuffer->Bind();

    m_indexBuffer = indexBuffer;
}

VertexArrayPtr VertexArray::Create()
{
    VertexArray* vtxArray = new VertexArray();
    return VertexArrayPtr(vtxArray);
}
