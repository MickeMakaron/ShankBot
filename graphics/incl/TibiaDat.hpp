#ifndef GRAPHICS_LAYER_TIBIA_DAT
#define GRAPHICS_LAYER_TIBIA_DAT

///////////////////////////////////
// STD C++
#include <set>
#include <map>
#include <vector>
#include <memory>
#include <list>
#include <fstream>
///////////////////////////////////

namespace GraphicsLayer
{
class TibiaDat
{
    public:
        struct SpritesInfo
        {
            unsigned char width;
            unsigned char height;
            unsigned char blendFrames;
            unsigned char divX;
            unsigned char divY;
            unsigned char divZ;
            unsigned char animationLength;

            std::vector<size_t> spriteIds;
        };

        struct ItemInfo
        {
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
            std::string name;
            unsigned short profession = 0;
            unsigned short level = 0;
            unsigned short defaultAction = 0;
        };

        struct Object
        {
            enum class Type : char
            {
                ITEM,
                OUTFIT,
                EFFECT,
                DISTANCE
            };

            Type type;
            size_t id;
            std::list<SpritesInfo> spritesInfos;
            std::unique_ptr<ItemInfo> itemInfo;
        };





    public:
        explicit TibiaDat(std::string filePath);

        std::string getPath() const;
        unsigned int getVersion() const;

        static unsigned int getVersion(std::string datFilePath);

        const std::vector<Object>& getObjects() const;

    private:
        void readDat(std::string filePath);
        void readItems(std::istream& dat);
        void readOutfits(std::istream& dat);
        void readEffectsAndDistances(std::istream& dat);

        void readItemInfo(ItemInfo& out, std::istream& dat) const;
        void skipItemInfo(std::istream& dat) const;
        void readObjectSprites(Object& out, std::istream& dat) const;


    private:
        std::string mPath;
        unsigned int mVersion;
        unsigned short mNumItems;
        unsigned short mNumOutfits;
        unsigned short mNumEffects;
        unsigned short mNumDistances;

        std::vector<Object> mObjects;
};
}


#endif // GRAPHICS_LAYER_TIBIA_DAT
