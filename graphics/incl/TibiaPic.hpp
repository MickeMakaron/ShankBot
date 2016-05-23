#ifndef GRAPHICS_LAYER_TIBIA_PIC
#define GRAPHICS_LAYER_TIBIA_PIC

///////////////////////////////////
// STD C++
#include <vector>
#include <string>
///////////////////////////////////

namespace GraphicsLayer
{
    class TibiaPic
    {
        public:
            class TileSheet
            {
                public:
                    explicit TileSheet(unsigned char width, unsigned char height, std::vector<std::vector<unsigned char>> sprites);

                    unsigned char* mergeSprites() const;
                    const std::vector<unsigned char>& getSprite(size_t x, size_t y) const;

                    unsigned char getWidth() const;
                    unsigned char getHeight() const;

                public:
                    static const size_t TILE_WIDTH = 32;
                    static const size_t TILE_HEIGHT = 32;
                    static const size_t BYTES_PER_PIXEL = 4;

                private:
                    const unsigned char M_WIDTH;
                    const unsigned char M_HEIGHT;
                    const std::vector<std::vector<unsigned char>> M_SPRITES;

                    static const size_t BYTES_PER_TILE_ROW = TILE_WIDTH * BYTES_PER_PIXEL;
            };

        public:
            explicit TibiaPic(std::string picFilePath);

            const std::vector<TileSheet>& getTileSheets() const;
            static unsigned int getVersion(std::string picFilePath);

        private:
            void read(std::string picFilePath);

        private:
            unsigned int mVersion;
            std::vector<TileSheet> mSheets;
    };
}


#endif // GRAPHICS_LAYER_TIBIA_PIC
