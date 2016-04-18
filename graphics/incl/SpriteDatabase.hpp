#ifndef GRAPHICS_LAYER_SPRITE_DATABASE
#define GRAPHICS_LAYER_SPRITE_DATABASE

///////////////////////////////////
// STD C++
#include <string>
#include <map>
#include <vector>
#include <list>
#include <memory>
///////////////////////////////////

namespace GraphicsLayer
{
    class SpriteDatabase
    {
        private:
            struct Sprite
            {
                Sprite(std::string name, unsigned char* pixels);
                ~Sprite();

                std::string name;
                unsigned char* pixels;

            };

            typedef std::unique_ptr<Sprite> SpritePtr;

        public:
            SpriteDatabase(std::string directory);

            bool getName(size_t width, size_t height, unsigned char* pixels, std::string& name) const;


        private:
            void loadImages(std::string directory);
            void loadImage(std::string path, std::string folderName);

            void insert(std::string name, size_t width, size_t height, unsigned char* pixels);

            bool compareSprites(size_t size, unsigned char* s1, unsigned char* s2) const;

        private:
            std::map<size_t, std::vector<SpritePtr>> mSpriteMap;
            std::list<std::string> mNames;
    };
}


#endif // GRAPHICS_LAYER_SPRITE_DATABASE
