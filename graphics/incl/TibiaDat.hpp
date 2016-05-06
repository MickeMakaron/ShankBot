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

            struct Item : public Object
            {
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
                std::string name;
                unsigned short profession;
                unsigned short level;
                unsigned short defaultAction;
            };

        public:
            TibiaDat(std::string filePath);

            std::map<size_t, std::set<size_t>> getSpriteItemBindings() const;
            std::map<size_t, std::set<size_t>> getSpriteOutfitBindings() const;
            std::map<size_t, std::set<size_t>> getSpriteEffectBindings() const;
            std::map<size_t, std::set<size_t>> getSpriteDistanceBindings() const;

        private:
            void readDat(std::string filePath);
            void readItems(std::istream& dat);
            void readOutfits(std::istream& dat);
            void readEffectsAndDistances(std::istream& dat);

            void readItemInfo(Item& out, std::istream& dat) const;
            void skipItemInfo(std::istream& dat) const;
            void readObjectSprites(Object& out, std::istream& dat) const;

            template<typename ObjectType>
            std::map<size_t, std::set<size_t>> getSpriteObjectBindings(const std::vector<ObjectType>& objects) const
            {
                std::map<size_t, std::set<size_t>> bindings;
                for(const ObjectType& object : objects)
                {
                    for(size_t spriteId : object.spriteIds)
                    {
                        if(spriteId != 0)
                        {
                            auto it = bindings.insert(std::make_pair(spriteId, std::set<size_t>()));
                            it.first->second.insert(object.id);
                        }
                    }
                }

                return bindings;
            }


        private:
            unsigned int mVersion;

            std::vector<Item> mItems;
            std::vector<Object> mOutfits;
            std::vector<Object> mEffects;
            std::vector<Object> mDistances;
    };
}


#endif // GRAPHICS_LAYER_TIBIA_DAT
