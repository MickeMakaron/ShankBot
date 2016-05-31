///////////////////////////////////
// Internal ShankBot headers
#include "TibiaDat.hpp"
#include "utility.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

TibiaDat::TibiaDat(std::string filePath)
: mPath(filePath)
{
    readDat(filePath);
}

unsigned int TibiaDat::getVersion(std::string datFilePath)
{
    std::ifstream dat(datFilePath, std::ios::binary);

    unsigned int version;
    readStream(version, dat);

    dat.close();

    return version;
}

void TibiaDat::readDat(std::string filePath)
{
    std::ifstream dat(filePath, std::ios::binary);

    readStream(mVersion, dat);
    readStream(mNumItems, dat);
    mNumItems -= 99;
    readStream(mNumOutfits, dat);
    readStream(mNumEffects, dat);
    readStream(mNumDistances, dat);


    mObjects.resize(mNumItems + mNumOutfits + mNumEffects + mNumDistances);
    readItems(dat);
    readOutfits(dat);
    readEffectsAndDistances(dat);

    dat.close();
}
#include <iostream>
void TibiaDat::readItemInfo(ItemInfo& out, std::istream& dat) const
{
    unsigned char option;
    readStream(option, dat);
    while(option != 0xff)
    {
        switch(option)
        {
            case 0x00: // Ground tile
                out.isGround = true;
                readStream(out.walkSpeed, dat);
                break;

            case 0x01: // Rendered third last
                out.topOrder = 1;
                break;

            case 0x02: // Rendered second last
                out.topOrder = 2;
                break;

            case 0x03: // Rendered last (always on top)
                out.topOrder = 3;
                break;

            case 0x04: // Container
                out.isContainer = true;
                break;

            case 0x05: // Stackable (gold, spears, etc.)
                out.isStackable = true;
                break;

            case 0x06: // N/A
                break;

            case 0x07: // Usable. I.e. keys, runes.
                out.isUsable = true;
                break;

            case 0x08: // Readable and writable
                out.isReadable = true;
                out.isWritable = true;
                readStream(out.maxCharacters, dat);
                break;

            case 0x09: // Readable
                out.isReadable = true;
                readStream(out.maxCharacters, dat);
                break;

            case 0x0a: // Fluid containers (including filled ones, like "vial of water")
                out.isFluid = true;
                break;

            case 0x0b: // A splash on the ground (retrievable with a fluid container)
                out.isSplash = true;
                break;

            case 0x0c: // Blocking. Player cannot move here.
                out.isBlocking = true;
                break;

            case 0x0d: // Immovable
                out.isMovable = false;
                break;

            case 0x0e: // Blocks ranged attacks
                out.isBlockingRangedAttack = true;
                break;

            case 0x0f: // The pathfinding will not walk on this (fire fields, for example)
                out.isPathBlocking = true;
                break;

            case 0x10: // Blocking 10.00
                out.isBlockingRangedAttack = true;
                break;

            case 0x11: // Pickupable
                out.isPickupable = true;
                break;

            case 0x12: // Hangable on a wall
                out.isHangable = true;
                break;

            case 0x13: // Horizontal wall
                out.isWall = true;
                break;

            case 0x14: // Vertical wall
                out.isWall = true;
                break;

            case 0x15: // Rotatable
                out.isRotatable = true;
                break;

            case 0x16: // Light data
                readStream(out.lightBrightness, dat);
                readStream(out.lightColor, dat);
                break;

            case 0x17: // N/A
                break;

            case 0x18: // Floor change tile
                out.isFloorChange = true;
                break;

            case 0x19: // Offset
                {
                    readStream(out.offsetX, dat);
                    readStream(out.offsetY, dat);
                }
                break;

            case 0x1a: // Height. For example, if you put something on a parcel, that thing will be rendered {height} higher than normal.
                readStream(out.height, dat);
                break;

            case 0x1b: // N/A
                break;

            case 0x1c: // Idle animation
                out.hasIdleAnimation = true;
                break;

            case 0x1d: // Color on the minimap
                readStream(out.minimapColor, dat);
                break;

            case 0x1e: // Help ???
                {
                    unsigned short op;
                    readStream(op, dat);
                    if(op == 1112)
                        out.isReadable = true;
                }
                break;

            case 0x1f: // Full tile ???

                break;

            case 0x20: // Transparent
                out.isTransparent = true;
                break;

            case 0x21: // N/A
                {
                    unsigned short unknown;
                    readStream(unknown, dat);
                }
                break;

            case 0x22: // Market data
                out.hasMarketInfo = true;
                readStream(out.category, dat);
                readStream(out.marketId1, dat);
                readStream(out.marketId2, dat);
                {
                    unsigned short nameLength;
                    readStream(nameLength, dat);

                    char* name = new char[nameLength];
                    dat.read(name, nameLength);
                    out.name.assign(name, nameLength);
                }

                readStream(out.profession, dat);
                readStream(out.level, dat);
                break;

            case 0x23: // Actionable ???
                readStream(out.defaultAction, dat);
                break;

            case 0xfe: // Usable ???
                break;

        }

        readStream(option, dat);
    }
}

void TibiaDat::readObjectSprites(Object& out, std::istream& dat) const
{
    SpritesInfo info;

    readStream(info.width, dat);
    readStream(info.height, dat);

    if(info.width != 1 || info.height != 1)
    {
        unsigned char garbage;
        readStream(garbage, dat);
    }

    readStream(info.blendFrames, dat);
    readStream(info.divX, dat);
    readStream(info.divY, dat);
    readStream(info.divZ, dat);
    readStream(info.animationLength, dat);

    if(info.animationLength > 1)
    {
        dat.seekg((int)dat.tellg() + 6 + info.animationLength * 2 * 4);
    }

    size_t numSprites = info.width * info.height * info.blendFrames * info.divX * info.divY * info.divZ * info.animationLength;
    for(size_t j = 0; j < numSprites; j++)
    {
        unsigned int spriteId;
        readStream(spriteId, dat);
        info.spriteIds.push_back(spriteId);
    }

    out.spritesInfos.push_back(info);
}

std::string TibiaDat::getPath() const
{
    return mPath;
}

unsigned int TibiaDat::getVersion() const
{
    return mVersion;
}

void TibiaDat::readItems(std::istream& dat)
{
    for(size_t i = 0, itemId = 100; i < mNumItems; i++, itemId++)
    {
        Object& item = mObjects[i];

        item.id = itemId;
        item.type = Object::Type::ITEM;

        item.itemInfo = std::unique_ptr<ItemInfo>(new ItemInfo());
        readItemInfo(*item.itemInfo.get(), dat);

        readObjectSprites(item, dat);
    }
}

//void TibiaDat::skipItemInfo(std::istream& dat) const
//{
//    unsigned char option;
//    readStream(option, dat);
//    while(option != 0xff)
//        readStream(option, dat);
//}

void TibiaDat::readOutfits(std::istream& dat)
{
    for(size_t i = mNumItems, outfitId = 1; i < mNumItems + mNumOutfits; i++, outfitId++)
    {
        Object& outfit = mObjects[i];

        outfit.id = outfitId;
        outfit.type = Object::Type::OUTFIT;

        outfit.itemInfo = std::unique_ptr<ItemInfo>(new ItemInfo());
        readItemInfo(*outfit.itemInfo.get(), dat);

        unsigned short op;
        readStream(op, dat);
        bool hasOutfitThingy = (op == 2);

        readObjectSprites(outfit, dat);

        if(hasOutfitThingy)
        {
            unsigned char unknown;
            readStream(unknown, dat);
            readObjectSprites(outfit, dat);
        }
    }

}

void TibiaDat::readEffectsAndDistances(std::istream& dat)
{
    for(size_t i = mNumItems + mNumOutfits, effectId = 1; i <  mNumItems + mNumOutfits + mNumEffects; i++, effectId++)
    {
        Object& effect = mObjects[i];
        effect.id = effectId;
        effect.type = Object::Type::EFFECT;

        effect.itemInfo = std::unique_ptr<ItemInfo>(new ItemInfo());
        readItemInfo(*effect.itemInfo.get(), dat);
        readObjectSprites(effect, dat);
    }

    for(size_t i = mNumItems + mNumOutfits + mNumEffects, distanceId = 1; i <  mNumItems + mNumOutfits + mNumEffects + mNumDistances; i++, distanceId++)
    {
        Object& distance = mObjects[i];
        distance.id = distanceId;
        distance.type = Object::Type::DISTANCE;

        distance.itemInfo = std::unique_ptr<ItemInfo>(new ItemInfo());
        readItemInfo(*distance.itemInfo.get(), dat);
        readObjectSprites(distance, dat);
    }
}

const std::vector<TibiaDat::Object>& TibiaDat::getObjects() const
{
    return mObjects;
}
