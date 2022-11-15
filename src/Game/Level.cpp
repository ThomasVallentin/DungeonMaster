#include "Level.h"

#include "Core/Image.h"
#include "Core/Logging.h"

#include "Renderer/Mesh.h"

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

Level::Level()
{

}

Level::~Level()
{

}

const uint32_t width = 128;
const uint32_t height = 128;


glm::vec4 getPixel(const glm::vec4* map, const int&x, const int&y) {
    if (x < 0 || x >= width || y < 0 || y >= height)
        return glm::vec4(0, 0, 0, 1);

    return map[y * width + x];
}

bool pointIsFull(const glm::vec4* map, const int&x, const int&y){
    glm::vec4 val = getPixel(map, x/2, y/2);
    if (val != getPixel(map, x / 2 - 1, y / 2))
        return true;
    if (val != getPixel(map, x / 2, y / 2 - 1))
        return true;
    if (val != getPixel(map, x / 2 - 1, y / 2 - 1))
        return true;

    return false;
}

bool hEdgeIsFull(const glm::vec4* map, const int&x, const int&y){
    return getPixel(map, x/2, y/2) != getPixel(map, x/2, y/2-1);
}

bool vEdgeIsFull(const glm::vec4* map, const int&x, const int&y){
    return getPixel(map, x/2, y/2) != getPixel(map, x/2-1, y/2);
}


void pushVertex(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, const Vertex& vertex)
{
    const auto it = std::find(vertices.begin(), vertices.end(), vertex);
    if (it != vertices.end())
    {
        indices.push_back(it - vertices.begin());
    }
    else
    {
        indices.push_back(vertices.size());
        vertices.push_back(vertex);
    }
}

void Level::Load(const std::string& path)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    ImagePtr image = Image::Read(path);

    const glm::vec4* pixels = image->GetPixels();

    bool evenY = true;
    for (int y=0 ; y <= height * 2 ; y++)
    {
        bool evenX = true;
        for (size_t x=0 ; x <= width * 2 ; x++)
        {
            if (evenY)
            {
                if (evenX)
                {
                    if (hEdgeIsFull(pixels, x, y))
                    {
                        pushVertex(vertices, indices, {{x/2,     0, y/2}, {0, 0, 1}, {0, 0}});
                        pushVertex(vertices, indices, {{x/2 + 1, 0, y/2}, {0, 0, 1}, {1, 0}});
                        pushVertex(vertices, indices, {{x/2 + 1, 1, y/2}, {0, 0, 1}, {1, 1}});

                        pushVertex(vertices, indices, {{x/2 + 1, 1, y/2}, {0, 0, 1}, {1, 1}});
                        pushVertex(vertices, indices, {{x/2,     1, y/2}, {0, 0, 1}, {0, 1}});
                        pushVertex(vertices, indices, {{x/2,     0, y/2}, {0, 0, 1}, {0, 0}});
                    }
                }
            }
            else 
            {
                if (evenX)
                {
                    if (vEdgeIsFull(pixels, x, y))
                    {
                        pushVertex(vertices, indices, {{x/2, 0, y/2}, {1, 0, 0}, {0, 0}});
                        pushVertex(vertices, indices, {{x/2, 0, y/2 + 1}, {1, 0, 0}, {1, 0}});
                        pushVertex(vertices, indices, {{x/2, 1, y/2 + 1}, {1, 0, 0}, {1, 1}});

                        pushVertex(vertices, indices, {{x/2, 1, y/2 + 1}, {1, 0, 0}, {1, 1}});
                        pushVertex(vertices, indices, {{x/2, 1, y/2}, {1, 0, 0}, {0, 1}});
                        pushVertex(vertices, indices, {{x/2, 0, y/2}, {1, 0, 0}, {0, 0}});
                    }
                }
                else
                {
                    if (getPixel(pixels, x/2, y/2).x)
                    {
                        pushVertex(vertices, indices, {{x/2,     0, y/2}, {0, 1, 0}, {0, 0}});
                        pushVertex(vertices, indices, {{x/2 + 1, 0, y/2}, {0, 1, 0}, {1, 0}});
                        pushVertex(vertices, indices, {{x/2 + 1, 0, y/2 + 1}, {0, 1, 0}, {1, 1}});

                        pushVertex(vertices, indices, {{x/2 + 1, 0, y/2 + 1}, {0, 1, 0}, {1, 1}});
                        pushVertex(vertices, indices, {{x/2,     0, y/2 + 1}, {0, 1, 0}, {0, 1}});
                        pushVertex(vertices, indices, {{x/2,     0, y/2}, {0, 1, 0}, {0, 0}});
                    }
                }
            }

            evenX = !evenX;
        }

        evenY = !evenY;
    }
    
    m_mesh = Mesh::Create(vertices, indices);
}
