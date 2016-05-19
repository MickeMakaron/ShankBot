///////////////////////////////////
// Internal ShankBot headers
#include "FrameParser.hpp"
#include "utility.hpp"
using namespace GraphicsLayer;
using namespace SharedMemoryProtocol;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <algorithm>
#include <iostream>
///////////////////////////////////


FrameParser::FrameParser(const TibiaContext& context)
: mContext(context)
{

}

void FrameParser::updateTileBuffers(const PixelData* texxes, size_t numTexxes)
{
    for(size_t i = 0; i < numTexxes; i++)
    {
        const PixelData& data = texxes[i];
        const unsigned char* pixels = data.pixels;
        if(data.targetTextureId == 0)
            mGuiCache.remove(data.targetTextureId, data.texX, data.texX);
        else
        {
            std::vector<unsigned char> opaquePixels = rgbaToColorTreeSprite(pixels, SharedMemoryProtocol::TILE_SIZE);

            std::list<SpriteObjectPairing> pairings;
            if(opaquePixels.size() > 0)
            {
                std::list<size_t> ids;
                if(mContext.getSpriteColorTree().find(opaquePixels, ids))
                {
                    std::vector<unsigned char> transparency = rgbaToTransparencyTreeSprite(pixels, SharedMemoryProtocol::TILE_SIZE);

                    std::list<size_t> tIds;
                    mContext.getSpriteTransparencyTree().find(transparency, tIds);

                    std::list<size_t> matchingIds;
                    for(size_t id : ids)
                        if(std::find(tIds.begin(), tIds.end(), id) != tIds.end())
                            matchingIds.push_back(id);

                    ids = matchingIds;
                    ids.unique();
                    for(size_t spriteId : ids)
                    {
                        SpriteObjectPairing pairing;
                        pairing.spriteId = spriteId;

                        std::list<const TibiaDat::Object*> objects = mContext.getSpriteObjectBindings().getObjects(spriteId);
                        if(!objects.empty())
                        {
                            for(auto object : objects)
                                pairing.objects.insert(object);

                            pairings.push_back(pairing);
                        }
                    }

                }
            }

            size_t texX = data.texX - data.texX % 32;
            size_t texY = data.texY - data.texY % 32;
            mSpriteCache.set(data.targetTextureId, texX, texY, pairings);
        }
    }
}

void FrameParser::parse(const DrawCall* drawCalls, size_t numDrawCalls)
{
    mObjectParser.clear();
    for(size_t i = 0; i < numDrawCalls; i++)
    {
        const DrawCall& d = drawCalls[i];

        if(d.targetTextureId == 1 && d.sourceTextureId == 3)
        {
            unsigned short texTileX = d.texX / 32;
            unsigned short texTileY = d.texY / 32;
            unsigned short screenTileX = d.screenX / 32;
            unsigned short screenTileY = d.screenY / 32;

            size_t screenIndex = screenTileX + screenTileY * 16;
            size_t texIndex = texTileX + texTileY * 16;
            mTex3TileIndexToTex1TileIndexMap[texIndex] = screenIndex;
        }
    }

    for(size_t i = 0; i < numDrawCalls; i++)
    {
        const DrawCall& d = drawCalls[i];

        if(d.targetTextureId == 0) // GUI
        {
            if(d.sourceTextureId == 1)
            {
//                mGameViewWidth = d.texWidth;
//                mGameViewHeight = d.texHeight;
            }
        }
        else if(d.sourceTextureId != 3)
        {
            size_t texX = d.texX - d.texX % 32;
            size_t texY = d.texY - d.texY % 32;

            std::list<SpriteObjectPairing> pairings;
            mSpriteCache.get(d.sourceTextureId, texX, texY, pairings);

            unsigned short screenTileX = d.screenX / 32;
            unsigned short screenTileY = d.screenY / 32;

//            for(SpriteObjectPairing& p : pairings)
            if(!pairings.empty())
            {
                SpriteObjectPairing& p = pairings.front();
                if(d.targetTextureId == 1)
                {
                     mObjectParser.parse(p.objects, screenTileX, screenTileY, d.width, d.height);
                }
                else if(d.targetTextureId == 3)
                {
                    size_t index = mTex3TileIndexToTex1TileIndexMap[screenTileX + screenTileY * 16];
                    mObjectParser.parse(p.objects, index % 16, index / 16, d.width, d.height);
                }
            }
        }
    }
//
//    for(GroundTile tile : mObjectParser.getGroundTiles())
//        std::cout << tile.x / 32 << "x" << tile.y / 32 << " - " << tile.id << std::endl;

    for(Item item : mObjectParser.getItems())
    {
        std::cout << item.x << "x" << item.y << " - " << item.id << " - " << item.object->itemInfo->name << std::endl;
    }

}
