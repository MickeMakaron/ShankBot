///////////////////////////////////
// Internal ShankBot headers
#include "TibiaDat.hpp"
#include "utility.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


TibiaDat::TibiaDat(std::string filePath)
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

    unsigned short numItems;
    readStream(numItems, dat);
    mItems.resize(numItems - 99);

    unsigned short numOutfits;
    readStream(numOutfits, dat);
    mOutfits.resize(numOutfits);

    unsigned short numEffects;
    readStream(numEffects, dat);
    mEffects.resize(numEffects);

    unsigned short numDistances;
    readStream(numDistances, dat);
    mDistances.resize(numDistances);

    readItems(dat);
    readOutfits(dat);
    readEffectsAndDistances(dat);

    dat.close();
}

void TibiaDat::readItemInfo(Item& out, std::istream& dat) const
{
    unsigned char option;
    readStream(option, dat);
    while(option != 0xff)
    {
        switch(option)
        {
            case 0x00: // Ground tile
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

            case 0x0f: // The pathfinding will not walk on this (fire fields, for example)
                out.isPathBlocking = true;
                break;

            case 0x10: // No mode animation ???
                break;

            case 0x11: // BLocks path of creatures ???
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

            case 0x19: // N/A
                {
                    unsigned int unknown;
                    readStream(unknown, dat);
                }
                break;

            case 0x1a: // Height. For example, if you put something on a parcel, that thing will be rendered {height} higher than normal.
                readStream(out.height, dat);
                break;

            case 0x1b: // N/A
                break;

            case 0x1c: // N/A
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
    unsigned char width;
    readStream(width, dat);

    unsigned char height;
    readStream(height, dat);

    if(width != 1 || height != 1)
    {
        unsigned char garbage;
        readStream(garbage, dat);
    }

    unsigned char blendFrames;
    readStream(blendFrames, dat);

    unsigned char divX;
    readStream(divX, dat);

    unsigned char divY;
    readStream(divY, dat);

    unsigned char divZ;
    readStream(divZ, dat);

    unsigned char animationLength;
    readStream(animationLength, dat);

    if(animationLength > 1)
    {
        dat.seekg((size_t)dat.tellg() + 6 + animationLength * 2 * 4);
    }

    size_t numSprites = width * height * blendFrames * divX * divY * divZ * animationLength;
    for(size_t j = 0; j < numSprites; j++)
    {
        unsigned int spriteId;
        readStream(spriteId, dat);
        out.spriteIds.insert(spriteId);
    }
}

void TibiaDat::readItems(std::istream& dat)
{
    for(size_t i = 0; i < mItems.size(); i++)
    {
        Item item;
        item.id = i + 100;
        readItemInfo(item, dat);
        readObjectSprites(item, dat);

        mItems[i] = item;
    }
}

void TibiaDat::skipItemInfo(std::istream& dat) const
{
    unsigned char option;
    readStream(option, dat);
    while(option != 0xff)
        readStream(option, dat);
}

void TibiaDat::readOutfits(std::istream& dat)
{
    for(size_t i = 0; i < mOutfits.size(); i++)
    {
        Object object;
        object.id = i + 1;
        skipItemInfo(dat);

        unsigned short op;
        readStream(op, dat);
        bool hasOutfitThingy = (op == 2);

        readObjectSprites(object, dat);

        if(hasOutfitThingy)
        {
            unsigned char unknown;
            readStream(unknown, dat);
            readObjectSprites(object, dat);
        }

        mOutfits[i] = object;
    }
}

void TibiaDat::readEffectsAndDistances(std::istream& dat)
{
    for(size_t i = 0; i < mEffects.size(); i++)
    {
        Object object;
        object.id = i + 1;
        skipItemInfo(dat);
        readObjectSprites(object, dat);

        mEffects[i] = object;
    }

    for(size_t i = 0; i < mDistances.size(); i++)
    {
        Object object;
        object.id = i + 1;
        skipItemInfo(dat);
        readObjectSprites(object, dat);



        mDistances[i] = object;
    }
}


std::map<size_t, std::set<size_t>> TibiaDat::getSpriteItemBindings() const
{
    return getSpriteObjectBindings(mItems);
}

std::map<size_t, std::set<size_t>> TibiaDat::getSpriteOutfitBindings() const
{
    return getSpriteObjectBindings(mOutfits);
}

std::map<size_t, std::set<size_t>> TibiaDat::getSpriteEffectBindings() const
{
    return getSpriteObjectBindings(mEffects);
}

std::map<size_t, std::set<size_t>> TibiaDat::getSpriteDistanceBindings() const
{
    return getSpriteObjectBindings(mDistances);
}
