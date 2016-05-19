#ifndef GRAPHICS_LAYER_UTILITY
#define GRAPHICS_LAYER_UTILITY


///////////////////////////////////
// STD C++
#include <string>
#include <list>
#include <istream>
#include <vector>
class PngImage;
///////////////////////////////////


namespace GraphicsLayer
{
    void writePixelsToFile(std::string fileName, const unsigned char* pixels, size_t width, size_t height);
    unsigned char* rgbaToRgb(const unsigned char* rgba, size_t width, size_t height);
    void testLoadImages(std::string directory, std::list<PngImage>& images);
    void readPpm(std::string filePath, unsigned char** data, size_t* width, size_t* height);

    std::vector<unsigned char> rgbaToColorTreeSprite(const std::vector<unsigned char>& rgba);
    std::vector<unsigned char> rgbaToTransparencyTreeSprite(const std::vector<unsigned char>& rgba);

    std::vector<unsigned char> rgbaToColorTreeSprite(const unsigned char* rgba, size_t numPixels);
    std::vector<unsigned char> rgbaToTransparencyTreeSprite(const unsigned char* rgba, size_t numPixels);


    template<typename T>
    void readStream(T& out, std::istream& stream)
    {
        stream.read((char*)&out, sizeof(T));
    }

    std::string getDateTime();
}

#endif // GRAPHICS_LAYER_UTILITY
