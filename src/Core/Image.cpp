#include "Image.h"

#include "Logging.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


ImageMap ImageRegistry::s_registry;


ImageConstWeakPtr Image::Read(const ImageSpecs& specs) {
    return ImageRegistry::ReadImage(specs);
}


ImageConstWeakPtr ImageRegistry::FindImage(const ImageSpecs& specs) {
    auto it = s_registry.find(specs);
    if (it != s_registry.end()) {
        return ImageConstWeakPtr(it->second);
    }

    return ImageConstWeakPtr();
}


ImageConstWeakPtr ImageRegistry::ReadImage(const ImageSpecs& specs) {
    // Try to find the image in the registry
    ImageConstWeakPtr result = FindImage(specs);
    if (!result.expired()) {
        return result;
    }

    // Simple gamma handling based on the input colorspace of the image. 
    // Further work should be achieved if we want more advanced color spaces
    switch (specs.inputColorSpace) {
        case ColorSpace::Raw:
            stbi_ldr_to_hdr_gamma(1.0f);
            break;
        
        case ColorSpace::sRGB:
            stbi_ldr_to_hdr_gamma(2.2f);
            break;
    }
    stbi_set_flip_vertically_on_load(true); 

    // Image doesn't exist, opening it
    int width, height, channels;
    float *img = stbi_loadf(specs.filePath.c_str(), &width, &height, &channels, 0);
    if(img == nullptr) {
        LOG_ERROR("Could not open image %s...", specs.filePath.c_str());
        return ImageConstWeakPtr();
    }

    ImagePtr image = std::shared_ptr<Image>(new Image(width, height));
    auto pixels = image->GetPixels();

    switch (channels) {
        case 4:
        {
            for (size_t i = 0 ; i < width * height ; i++) {
                pixels[i] = glm::vec4(img[i * channels],
                                      img[i * channels + 1],
                                      img[i * channels + 2],
                                      img[i * channels + 3]);
            }
            break;
        }

        case 3:
        {
            for (size_t i = 0 ; i < width * height ; i++) {
                pixels[i] = glm::vec4(img[i * channels],
                                      img[i * channels + 1],
                                      img[i * channels + 2],
                                      1.0f);
            }
            break;
        }

        default:
            LOG_ERROR("Invalid image channel count : %d", channels);
            return ImageConstWeakPtr();
    }

    stbi_image_free(img);

    s_registry.insert( { specs, image } );
    
    return ImageConstWeakPtr(image);
}
