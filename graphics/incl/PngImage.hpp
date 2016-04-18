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
}


#endif // GRAPHICS_LAYER_PNG_IMAGE
