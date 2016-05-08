#ifndef GRAPHICS_LAYER_TIBIA_SPR
#define GRAPHICS_LAYER_TIBIA_SPR

///////////////////////////////////
// STD C++
#include <vector>
#include <string>
#include <istream>
///////////////////////////////////

namespace GraphicsLayer
{
    class TibiaSpr
    {
        public:
            TibiaSpr(std::string sprFilePath);

            static unsigned int getVersion(std::string sprFilePath);

            const std::vector<std::vector<unsigned char>>& getSprites() const;
            const std::vector<size_t>& getSpriteIds() const;

        private:
            void readSprites(std::string sprFilePath);
            std::vector<unsigned char> readSpritePixels(std::istream& spr) const;


        private:
            unsigned int mVersion;
            unsigned int mNumSprites;
            std::vector<std::vector<unsigned char>> mSprites;
            std::vector<size_t> mIds;
    };
}


#endif // GRAPHICS_LAYER_TIBIA_SPR
