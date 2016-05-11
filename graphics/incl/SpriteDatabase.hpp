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
                explicit Sprite(std::string name, unsigned char* pixels);
                ~Sprite();

                std::string name;
                unsigned char* pixels;

            };

            static const size_t NUM_BUCKETS_PER_BYTE = 4;
            static constexpr unsigned char BUCKET_RANGES[NUM_BUCKETS_PER_BYTE] =
            {
                63,
                127,
                191,
                255
            };

//            static const size_t NUM_BUCKETS_PER_BYTE = 10;
//            static constexpr unsigned char BUCKET_RANGES[NUM_BUCKETS_PER_BYTE] =
//            {
//                25,
//                50,
//                75,
//                100,
//                125,
//                150,
//                175,
//                200,
//                225,
//                255
//            };

//            static const size_t NUM_BUCKETS_PER_BYTE = 8;
//            static constexpr short BUCKET_RANGES[NUM_BUCKETS_PER_BYTE] =
//            {
//                -120,
//                -50,
//                -5,
//                0,
//                5,
//                50,
//                120,
//                255
//            };


            struct HistogramEntry
            {
                std::string name;
                float red[NUM_BUCKETS_PER_BYTE];
                float green[NUM_BUCKETS_PER_BYTE];
                float blue[NUM_BUCKETS_PER_BYTE];
                float alpha;
            };


            typedef std::unique_ptr<Sprite> SpritePtr;

        public:
            explicit SpriteDatabase(std::string directory);

            bool getName(size_t width, size_t height, unsigned char* pixels, std::string& name) const;


        private:
            void loadImages(std::string directory);
            void loadImage(std::string path, std::string folderName);

            void insert(std::string name, size_t width, size_t height, unsigned char* pixels);

            bool findSimilarSprite(size_t size, unsigned char* pixels, HistogramEntry& similarSprite) const;

            void insertHistogramEntry(std::string name, size_t size, unsigned char* pixels);
            bool createHistogramEntry(std::string name, size_t size, unsigned char* pixels, HistogramEntry& entry) const;
            bool createHistogramEntry2(std::string name, size_t size, unsigned char* pixels, HistogramEntry& entry) const;

        private:
            std::map<size_t, std::vector<SpritePtr>> mSpriteMap;
            std::map<size_t, std::vector<HistogramEntry>> mHistogramMap;
            std::list<std::string> mNames;
    };
}


#endif // GRAPHICS_LAYER_SPRITE_DATABASE
