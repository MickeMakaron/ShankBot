#ifndef GRAPHICS_LAYER_UTILITY
#define GRAPHICS_LAYER_UTILITY


///////////////////////////////////
// STD C++
#include <string>
#include <list>
#include <istream>
#include "PngImage.hpp"
///////////////////////////////////


namespace GraphicsLayer
{
    void writePixelsToFile(std::string fileName, const unsigned char* pixels, size_t width, size_t height);
    unsigned char* rgbaToRgb(const unsigned char* rgba, size_t width, size_t height);
    void testLoadImages(std::string directory, std::list<PngImage>& images);
    void readPpm(std::string filePath, unsigned char** data, size_t* width, size_t* height);

    template<typename T>
    void readStream(T& out, std::istream& stream)
    {
        stream.read((char*)&out, sizeof(T));
    }
}

#endif // GRAPHICS_LAYER_UTILITY
