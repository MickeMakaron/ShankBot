///////////////////////////////////
// Internal ShankBot headers
#include "TibiaDat.hpp"
#include "utility.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <sstream>
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
    mItemInfos.resize(numItems - 99);

    unsigned short numOutfits;
    readStream(numOutfits, dat);
    mOutfits.resize(numOutfits);

    unsigned short numEffects;
    readStream(numEffects, dat);
    mEffects.resize(numEffects);

    unsigned short numDistances;
    readStream(numDistances, dat);
    mDistances.resize(numDistances);

    mSpritesInfos.resize(mItems.size() + mOutfits.size() + mEffects.size() + mDistances.size());

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

                    if(nameLength + 1 > ItemInfo::MAX_NAME_LENGTH)
                    {
                        std::stringstream sstream;
                        sstream << "Error reading Tibia.dat: The name length of an item (" << nameLength << ") exceeds the "
                                << "preset buffer size (" << ItemInfo::MAX_NAME_LENGTH << "). This means the buffer size should be increased.";

                        throw std::runtime_error(sstream.str());
                    }
                    dat.read(out.name, nameLength);
                    out.name[nameLength] = '\0';
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

TibiaDat::SpritesInfo TibiaDat::readObjectSprites(Object& out, std::istream& dat) const
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
        out.spriteIds.insert(spriteId);
    }

    return info;
}

void TibiaDat::readItems(std::istream& dat)
{
    for(size_t i = 0; i < mItems.size(); i++)
    {
        mItems[i].id = i + 100;
        readItemInfo(mItemInfos[i], dat);
        mSpritesInfos[i] = readObjectSprites(mItems[i], dat);
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
        mOutfits[i].id = i + 1;

        skipItemInfo(dat);

        unsigned short op;
        readStream(op, dat);
        bool hasOutfitThingy = (op == 2);

        mSpritesInfos[i + mItems.size()] = readObjectSprites(mOutfits[i], dat);


        if(hasOutfitThingy)
        {
            unsigned char unknown;
            readStream(unknown, dat);
            readObjectSprites(mOutfits[i], dat);
        }
    }
}

void TibiaDat::readEffectsAndDistances(std::istream& dat)
{
    for(size_t i = 0; i < mEffects.size(); i++)
    {
        mEffects[i].id = i + 1;
        skipItemInfo(dat);
        mSpritesInfos[i + mItems.size() + mOutfits.size()] = readObjectSprites(mEffects[i], dat);
    }

    for(size_t i = 0; i < mDistances.size(); i++)
    {
        mDistances[i].id = i + 1;
        skipItemInfo(dat);
        mSpritesInfos[i + mItems.size() + mOutfits.size() + mEffects.size()] = readObjectSprites(mDistances[i], dat);
    }
}

const std::vector<TibiaDat::Object>& TibiaDat::getItems() const
{
    return mItems;
}

const std::vector<TibiaDat::Object>& TibiaDat::getOutfits() const
{
    return mOutfits;
}

const std::vector<TibiaDat::Object>& TibiaDat::getEffects() const
{
    return mEffects;
}

const std::vector<TibiaDat::Object>& TibiaDat::getDistances() const
{
    return mDistances;
}

const std::vector<TibiaDat::ItemInfo>& TibiaDat::getItemInfos() const
{
    return mItemInfos;
}

const std::vector<TibiaDat::SpritesInfo>& TibiaDat::getSpritesInfos() const
{
    return mSpritesInfos;
}
