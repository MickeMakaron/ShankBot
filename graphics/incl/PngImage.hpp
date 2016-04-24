#ifndef GRAPHICS_LAYER_PNG_IMAGE
#define GRAPHICS_LAYER_PNG_IMAGE


///////////////////////////////////
// STD C++
#include <string>
///////////////////////////////////

namespace GraphicsLayer
{
    struct PngImage
    {
        size_t width;
        size_t height;
        unsigned char* pixelData;
        unsigned char bytesPerPixel;
    };

    PngImage readPng(std::string filePath);
    void writePng(std::string filePath, unsigned char* pixels, size_t width, size_t height);
}


#endif // GRAPHICS_LAYER_PNG_IMAGE
