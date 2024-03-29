#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include "VertexBuffer.h"

#include <glad/glad.h>

#include <vector>
#include <memory>


class VertexArray;


DECLARE_PTR_TYPE(VertexArray);


// == VertexArray ==

class VertexArray
{
    typedef std::vector<VertexBufferPtr> VertexBufferVector;

public:
    virtual ~VertexArray();

    void Bind() const;
    void Unbind() const;
    bool IsValid() const;

    void AddVertexBuffer(std::shared_ptr<VertexBuffer> &vertexBuffer);
    inline const VertexBufferVector& GetVertexBuffers() const { return m_vertexBuffers; }

    void SetIndexBuffer(std::shared_ptr<IndexBuffer> &indexBuffer);
    inline const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_indexBuffer; }

    static VertexArrayPtr Create();

private:
    VertexArray();


    VertexBufferVector m_vertexBuffers;
    std::shared_ptr<IndexBuffer> m_indexBuffer;

    GLuint m_id = 0;
};


#endif  // VERTEXARRAY_H
