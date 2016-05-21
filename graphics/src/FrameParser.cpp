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
#include <sstream>
///////////////////////////////////


FrameParser::FrameParser(const TibiaContext& context)
: mContext(context)
, mMovementMonitor(MIDDLE_SCREEN_X, MIDDLE_SCREEN_Y, TILE_SIZE)
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

            size_t texX = data.texX - data.texX % TILE_SIZE;
            size_t texY = data.texY - data.texY % TILE_SIZE;
            mSpriteCache.set(data.targetTextureId, texX, texY, pairings);
        }
    }
}


void FrameParser::parse(const DrawCall* drawCalls, size_t numDrawCalls)
{
    mObjectParser.clear();
    int newMiddleTexIndex = mCenterTexIndex;
    for(size_t i = 0; i < numDrawCalls; i++)
    {
        const DrawCall& d = drawCalls[i];
        if(d.targetTextureId == 1 && d.sourceTextureId == 3)
        {
            unsigned short texTileX = d.texX / TILE_SIZE;
            unsigned short texTileY = d.texY / TILE_SIZE;
            unsigned short screenTileX = d.screenX / TILE_SIZE;
            unsigned short screenTileY = d.screenY / TILE_SIZE;

            size_t screenIndex = screenTileX + screenTileY * NUM_TILES_X;
            size_t texIndex = texTileX + texTileY * NUM_TILES_X;

            if(texIndex == mCenterTexIndex)
                mMovementMonitor.update(d.screenX, d.screenY);

            if(d.screenX == MIDDLE_SCREEN_X && d.screenY == MIDDLE_SCREEN_Y)
                newMiddleTexIndex = texIndex;

            mTex3TileIndexToTex1TileIndexMap[texIndex] = screenIndex;
        }
    }

    int movementX = mMovementMonitor.getNumStepsX();
    int movementY = mMovementMonitor.getNumStepsY();

    if(mCenterTexIndex != newMiddleTexIndex)
        mMovementMonitor.reset();
    mCenterTexIndex = newMiddleTexIndex;

    if(movementX != 0 || movementY != 0)
        std::cout << "Moved (" << movementX << "x" << movementY << ")" << std::endl;


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
            size_t texX = d.texX - d.texX % TILE_SIZE;
            size_t texY = d.texY - d.texY % TILE_SIZE;

            if(d.screenX % TILE_SIZE != 0 || d.screenY % TILE_SIZE != 0)
            {
                // Sprites that are not aligned with the tile grid.
                // By what is known at the moment, the only case where this happens
                // is when objects are stacked (crates, barrels etc) and
                // when a creature is moving or facing a certain direction.

                // Ignore these for now.

                std::list<SpriteObjectPairing> pairings;
                mSpriteCache.get(d.sourceTextureId, texX, texY, pairings);
//
//                for(auto pair : pairings)
//                    std::cout << pair.spriteId << " ";
//
//                if(!pairings.empty())
//                {
//                    std::cout   << " - " << (int)d.targetTextureId << "<-" << (int)d.sourceTextureId
//                                << " - " << d.screenX << "x" << d.screenY << "<-" << d.texX << "x" << d.texY
//                                << " - " << d.width << "x" << d.height << "<-" << d.texWidth << "x" << d.texHeight << std::endl;
//                }

            }
            else if(d.targetTextureId == 1 || d.targetTextureId == 3)
            {
                std::list<SpriteObjectPairing> pairings;
                mSpriteCache.get(d.sourceTextureId, texX, texY, pairings);

                unsigned short screenTileX = d.screenX / TILE_SIZE;
                unsigned short screenTileY = d.screenY / TILE_SIZE;

                unsigned short tileX;
                unsigned short tileY;

                if(d.targetTextureId == 1)
                {
                    tileX = screenTileX;
                    tileY = screenTileY;
                }
                else if(d.targetTextureId == 3)
                {
                    size_t index = mTex3TileIndexToTex1TileIndexMap[screenTileX + screenTileY * NUM_TILES_X];
                    tileX = index % NUM_TILES_X;
                    tileY = index / NUM_TILES_X;
                }

                mPairingsMap[tileX][tileY].push_back(pairings);

                if(pairings.size() == 1)
                {
                    const SpriteObjectPairing& pairing = pairings.front();

                    if(pairing.objects.size() == 1)
                    {
                        const TibiaDat::Object* object = *pairings.front().objects.begin();
                    }
                }
            }

        }
    }

    std::list<std::set<const TibiaDat::Object*>> parsedFrame[NUM_TILES_X][NUM_TILES_Y];
    for(size_t x = 0; x < NUM_TILES_X; x++)
        for(size_t y = 0; y < NUM_TILES_Y; y++)
        {
            std::list<std::list<SpriteObjectPairing>>& drawCalls = mPairingsMap[x][y];

            for(auto it = drawCalls.begin(); it != drawCalls.end();)
            {
                std::set<const TibiaDat::Object*> completeObjects;
                std::set<std::list<std::list<SpriteObjectPairing>>::iterator> completeObjectParts;
                for(const SpriteObjectPairing& pair : *it)
                {
                    for(const TibiaDat::Object* object : pair.objects)
                    {
                        if(object->spritesInfos.size() == 1)
                        {
                            const TibiaDat::SpritesInfo& info = object->spritesInfos.front();
                            size_t numSprites = info.width * info.height * info.blendFrames * info.divX * info.divY * info.divZ * info.animationLength;
                            if(info.width == 1 && info.height == 1)
                            {
//                                std::cout << object->id << std::endl;
                                completeObjects.insert(object);
                            }
                        }
                    }
                }


                if(completeObjects.empty())
                    it++;
                else
                {
                    parsedFrame[x][y].push_back(completeObjects);
                    drawCalls.erase(it++);
                }

            }

        }


    if(movementX < 0)
    {
        int numClears = -movementX;
        if(mMovementMonitor.getLastStepX() > 0)
            numClears -= 1;

        for(size_t x = NUM_TILES_SCENE_X - numClears; x < NUM_TILES_SCENE_X; x++)
            for(size_t y = 0; y < NUM_TILES_SCENE_Y; y++)
                mCurrentScene[x][y].clear();

        for(size_t i = 0; i < -movementX; i++)
            for(size_t x = 1; x < NUM_TILES_SCENE_X; x++)
                    std::swap(mCurrentScene[x - 1], mCurrentScene[x]);
    }
    else if(movementX > 0)
    {
        int numClears = movementX;
        if(mMovementMonitor.getLastStepX() < 0)
            numClears -= 1;

        for(size_t x = NUM_TILES_SCENE_X - numClears; x < NUM_TILES_SCENE_X; x++)
            for(size_t y = 0; y < NUM_TILES_SCENE_Y; y++)
                mCurrentScene[x][y].clear();

        for(size_t i = 0; i < movementX; i++)
            for(int x = (int)NUM_TILES_SCENE_X - 1; x > 0; x--)
                    std::swap(mCurrentScene[x - 1], mCurrentScene[x]);
    }

    if(movementY < 0)
    {
        int numClears = -movementY;
        if(mMovementMonitor.getLastStepY() > 0)
            numClears -= 1;

        for(size_t y = NUM_TILES_SCENE_Y - numClears; y < NUM_TILES_SCENE_Y; y++)
            for(size_t x = 0; x < NUM_TILES_SCENE_X; x++)
                mCurrentScene[x][y].clear();

        for(size_t i = 0; i < -movementY; i++)
            for(size_t y = 1; y < NUM_TILES_SCENE_Y; y++)
                for(size_t x = 0; x < NUM_TILES_SCENE_X; x++)
                    std::swap(mCurrentScene[x][y - 1], mCurrentScene[x][y]);
    }
    else if(movementY > 0)
    {
        int numClears = movementY;
        if(mMovementMonitor.getLastStepY() < 0)
            numClears -= 1;

        for(size_t y = NUM_TILES_SCENE_Y - numClears; y < NUM_TILES_SCENE_Y; y++)
            for(size_t x = 0; x < NUM_TILES_SCENE_X; x++)
                mCurrentScene[x][y].clear();

        for(size_t i = 0; i < movementY; i++)
            for(int y = (int)NUM_TILES_SCENE_Y - 1; y > 0; y--)
                for(size_t x = 0; x < NUM_TILES_SCENE_X; x++)
                    std::swap(mCurrentScene[x][y - 1], mCurrentScene[x][y]);
    }


    for(size_t x = 0; x < NUM_TILES_SCENE_X; x++)
        for(size_t y = 0; y < NUM_TILES_SCENE_Y; y++)
            mCurrentScene[x][y].insert(mCurrentScene[x][y].end(), parsedFrame[x][y].begin(), parsedFrame[x][y].end());

    if(movementX != 0 || movementY != 0)
    {
        std::list<std::set<const TibiaDat::Object*>>& objectsInMiddle = mCurrentScene[NUM_TILES_VIEW_X / 2][NUM_TILES_VIEW_Y / 2];
        for(auto objects : objectsInMiddle)
        {
            for(auto object : objects)
                std::cout << object->id << " ";

            if(!objects.empty())
                std::cout << std::endl;
        }
    }
}
