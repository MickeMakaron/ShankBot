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
            unsigned short walkSpeed = 0;
            unsigned char topOrder = 0;
            bool isContainer = false;
            bool isStackable = false;
            bool isUsable = false;
            bool isReadable = false;
            bool isWritable = false;
            unsigned short maxCharacters = 0;
            bool isFluid = false;
            bool isSplash = false;
            bool isBlocking = false;
            bool isMovable = false;
            bool isBlockingRangedAttack = false;
            bool isPathBlocking = false;
            bool isPickupable = false;
            bool isHangable = false;
            bool isWall = false;
            bool isRotatable = false;
            bool isFloorChange = false;
            unsigned short lightBrightness = 0;
            unsigned short lightColor = 0;
            unsigned short height = 0;
            unsigned short minimapColor = 0;
            bool isTransparent = false;
            bool hasMarketInfo = false;
            unsigned short category = 0;
            unsigned short marketId1 = 0;
            unsigned short marketId2 = 0;
            char name[MAX_NAME_LENGTH];
            unsigned short profession = 0;
            unsigned short level = 0;
            unsigned short defaultAction = 0;
        };

    public:
        explicit TibiaDat(std::string filePath);

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
