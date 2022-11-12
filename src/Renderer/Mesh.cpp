#include "Mesh.h"

Mesh::Mesh() 
{
    CreateVertexArray();
}

Mesh::Mesh(const std::vector<Vertex>& vertices, 
           const std::vector<uint32_t>& indices) :
        m_vertices(vertices), m_indices(indices)
{
    CreateVertexArray();
}

void Mesh::SetVertices(const std::vector<Vertex> &vertices) 
{
    m_vertices = vertices;
    m_vertexArray->GetVertexBuffers()[0]->SetData(m_vertices.data(), m_vertices.size() * sizeof(Vertex));
}

void Mesh::SetIndices(const std::vector<uint32_t>& indices) 
{
    m_indices = indices;
    m_vertexArray->GetIndexBuffer()->SetData(indices.data(), indices.size());
}

void Mesh::CreateVertexArray() 
{
    VertexBufferPtr vBuffer = VertexBuffer::Create(m_vertices.data(), m_vertices.size() * sizeof(Vertex));
    vBuffer->SetLayout({{"aPosition",  3, GL_FLOAT, false},
                        {"aNormal",    3, GL_FLOAT, false},
                        {"aTexCoords", 2, GL_FLOAT, false}
                       });

    IndexBufferPtr iBuffer = IndexBuffer::Create(m_indices.data(), m_indices.size());
    
    m_vertexArray = VertexArray::Create();
    m_vertexArray->AddVertexBuffer(vBuffer);
    m_vertexArray->SetIndexBuffer(iBuffer);
    m_vertexArray->Unbind();
}

MeshPtr Mesh::Create() 
{
    return MeshPtr(new Mesh);
}


MeshPtr Mesh::Create(const std::vector<Vertex>& vertices, 
                     const std::vector<uint32_t>& indices) 
{
    return MeshPtr(new Mesh(vertices, indices));
}
