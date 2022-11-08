#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include "Foundations.h"
#include "Utils/TypeUtils.h"

#include "glm/glm.hpp"

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>


class Image;


DECLARE_PTR_TYPE(Image);
DECLARE_CONST_PTR_TYPE(Image);


// Pseudo color management
enum class ColorSpace
{
    Raw = 0,
    sRGB
};


struct ImageSpecs {
    std::string filePath;
    ColorSpace inputColorSpace;

    inline bool operator==(const ImageSpecs& other) const
    {
        return (filePath == other.filePath && 
                inputColorSpace == other.inputColorSpace);
    }
};

template <>
class std::hash<ImageSpecs> {
public:
    size_t operator()(const ImageSpecs& specs) const
    {
        size_t hash = 0;
        HashCombine(hash, specs.filePath);
        HashCombine(hash, specs.inputColorSpace);
        
        return hash;
    }
};


class ImageRegistry;


class Image
{
public:
    ~Image() = default;

    inline uint32_t GetWidth() const { return m_width; }
    inline uint32_t GetHeight() const { return m_height; }

    inline const glm::vec4* GetPixels() const { return m_pixels.data(); }
    inline glm::vec4* GetPixels() { return m_pixels.data(); }

    static ImageConstWeakPtr Read(const ImageSpecs& specs);
    
private:
    Image(const uint32_t& width, const uint32_t& height) : 
            m_width(width), m_height(height), m_pixels(width * height) {}

    std::vector<glm::vec4> m_pixels;
    uint32_t m_width = 0;
    uint32_t m_height = 0;

    friend ImageRegistry;
};


typedef std::unordered_map<ImageSpecs, ImagePtr> ImageMap;

class ImageRegistry {
public:
    static ImageConstWeakPtr ReadImage(const ImageSpecs& specs);
    static ImageConstWeakPtr FindImage(const ImageSpecs& specs);

private:

    static ImageMap s_registry;
};


#endif // IMAGEUTILS_H
