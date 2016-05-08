#ifndef GRAPHICS_LAYER_TIBIA_DAT
#define GRAPHICS_LAYER_TIBIA_DAT

///////////////////////////////////
// STD C++
#include <set>
#include <map>
#include <vector>
#include <fstream>
///////////////////////////////////

namespace GraphicsLayer
{
    class TibiaDat
    {
        public:
            struct Object
            {
                size_t id;
                std::set<size_t> spriteIds;
            };

            struct SpritesInfo
            {
                unsigned char width;
                unsigned char height;
                unsigned char blendFrames;
                unsigned char divX;
                unsigned char divY;
                unsigned char divZ;
                unsigned char animationLength;
            };

            struct ItemInfo
            {
                static const int MAX_NAME_LENGTH = 64;
                unsigned short walkSpeed;
                unsigned char topOrder;
                bool isContainer;
                bool isStackable;
                bool isUsable;
                bool isReadable;
                bool isWritable;
                unsigned short maxCharacters;
                bool isFluid;
                bool isSplash;
                bool isBlocking;
                bool isMovable;
                bool isPathBlocking;
                bool isHangable;
                bool isWall;
                bool isRotatable;
                bool isFloorChange;
                unsigned short lightBrightness;
                unsigned short lightColor;
                unsigned short height;
                unsigned short minimapColor;
                bool isTransparent;
                bool hasMarketInfo;
                unsigned short category;
                unsigned short marketId1;
                unsigned short marketId2;
                char name[MAX_NAME_LENGTH];
                unsigned short profession;
                unsigned short level;
                unsigned short defaultAction;
            };

        public:
            TibiaDat(std::string filePath);

            static unsigned int getVersion(std::string datFilePath);

            const std::vector<Object>& getItems() const;
            const std::vector<ItemInfo>& getItemInfos() const;
            const std::vector<Object>& getOutfits() const;
            const std::vector<Object>& getEffects() const;
            const std::vector<Object>& getDistances() const;

            const std::vector<SpritesInfo>& getSpritesInfos() const;



        private:
            void readDat(std::string filePath);
            void readItems(std::istream& dat);
            void readOutfits(std::istream& dat);
            void readEffectsAndDistances(std::istream& dat);

            void readItemInfo(ItemInfo& out, std::istream& dat) const;
            void skipItemInfo(std::istream& dat) const;
            SpritesInfo readObjectSprites(Object& out, std::istream& dat) const;


        private:
            unsigned int mVersion;

            std::vector<Object> mItems;
            std::vector<ItemInfo> mItemInfos;
            std::vector<Object> mOutfits;
            std::vector<Object> mEffects;
            std::vector<Object> mDistances;

            std::vector<SpritesInfo> mSpritesInfos;
    };
}


#endif // GRAPHICS_LAYER_TIBIA_DAT
