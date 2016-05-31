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
#include <cassert>
#include <iostream>
///////////////////////////////////


FrameParser::FrameParser(const TibiaContext& context)
: mContext(context)
, mMovementMonitor(MIDDLE_SCREEN_X, MIDDLE_SCREEN_Y, TILE_SIZE)
{

}
#include <sstream>
#include "PngImage.hpp"
size_t herpaherpa = 0;
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
//            if(data.targetTextureId > 16)
//            {
//                std::stringstream sstream;
//                sstream << "texxes/" << herpaherpa++;
//                writePng(sstream.str(), (unsigned char*)pixels, 32, 32);
//            }


            std::vector<unsigned char> opaquePixels = rgbaToColorTreeSprite(pixels, SharedMemoryProtocol::TILE_SIZE);

            std::list<SpriteObjectPairing> pairings;
            if(opaquePixels.size() > 0)
            {
                std::vector<unsigned char> transparency = rgbaToTransparencyTreeSprite(pixels, SharedMemoryProtocol::TILE_SIZE);

                std::list<size_t> ids;
//                std::list<size_t> tIds;
//                mContext.getSpriteColorTree().find(opaquePixels, ids);
//                mContext.getSpriteTransparencyTree().find(transparency, tIds);
//
//                if(!ids.empty() && !tIds.empty())
//                {
////                    std::vector<unsigned char> transparency = rgbaToTransparencyTreeSprite(pixels, SharedMemoryProtocol::TILE_SIZE);
//
////                    std::list<size_t> tIds;
////                    mContext.getSpriteTransparencyTree().find(transparency, tIds);
//
//                    std::list<size_t> matchingIds;
//                    for(size_t id : ids)
//                        if(std::find(tIds.begin(), tIds.end(), id) != tIds.end())
//                            matchingIds.push_back(id);
//
//                    if(matchingIds.empty())
//                    {
//
//                    }
//
//                    ids = matchingIds;
//                    ids.unique();
//                    for(size_t spriteId : ids)
//                    {
//                        SpriteObjectPairing pairing;
//                        pairing.spriteId = spriteId;
//
//                        std::list<const TibiaDat::Object*> objects = mContext.getSpriteObjectBindings().getObjects(spriteId);
//                        if(!objects.empty())
//                        {
//                            for(auto object : objects)
//                                pairing.objects.insert(object);
//
//                            pairings.push_back(pairing);
//                        }
//                    }
//
//                }
//                else // should only happen for sprites using blending
//                {
//                    std::vector<unsigned char> transparency = rgbaToTransparencyTreeSprite(pixels, SharedMemoryProtocol::TILE_SIZE);
//                    ids.clear();
//                    if(mContext.getSpriteTransparencyTree().find(transparency, ids) && ids.size() < 10)
//                    {
//                        std::cout << "blend?-" << (int)data.targetTextureId << "-" << data.texX - data.texX % TILE_SIZE << "x" << data.texY - data.texY % TILE_SIZE;
//                        for(size_t id : ids)
//                        {
//                            std::cout << " " << id;
//                        }
////                        std::cout << ids.size() << " num";
//                        std::cout << std::endl;
//                    }
//                }


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
                else // should only happen for sprites using blending (outfits)
                {
                    std::vector<unsigned char> transparency = rgbaToTransparencyTreeSprite(pixels, SharedMemoryProtocol::TILE_SIZE);
                    ids.clear();
                    if(mContext.getSpriteTransparencyTree().find(transparency, ids) && ids.size() < 10)
                    {
//                        std::cout << "blend?-" << (int)data.targetTextureId << "-" << data.texX - data.texX % TILE_SIZE << "x" << data.texY - data.texY % TILE_SIZE;
                        bool isBlend = true;


                        std::list<SpriteObjectPairing> tmpPairings;

                        for(size_t spriteId : ids)
                        {
//                            std::cout << " " << id;
                            SpriteObjectPairing pairing;
                            pairing.spriteId = spriteId;


                            std::list<const TibiaDat::Object*> objects = mContext.getSpriteObjectBindings().getObjects(spriteId);
                            if(!objects.empty())
                            {
                                for(auto object : objects)
                                    pairing.objects.insert(object);

                                tmpPairings.push_back(pairing);
                            }

                            if(pairing.objects.empty())
                            {
                                isBlend = false;
                                break;
                            }

                            for(auto object : pairing.objects)
                            {
                                if(object->type != TibiaDat::Object::Type::OUTFIT)
                                {
                                    isBlend = false;
                                    break;
                                }
                            }

                            if(!isBlend)
                                break;
                        }

                        if(isBlend)
                        {
//                            std::cout << "blend?-" << (int)data.targetTextureId << "-" << data.texX - data.texX % TILE_SIZE << "x" << data.texY - data.texY % TILE_SIZE;
//                            for(size_t id : ids)
//                            {
//                                std::cout << " " << id;
//                            }
//
//                            std::cout << std::endl;

                            pairings.insert(pairings.end(), tmpPairings.begin(), tmpPairings.end());
                        }
//                        std::cout << ids.size() << " num";
//                        std::cout << std::endl;
                    }
                }
            }

            size_t texX = data.texX - data.texX % TILE_SIZE;
            size_t texY = data.texY - data.texY % TILE_SIZE;
            mSpriteCache.set(data.targetTextureId, texX, texY, pairings);
        }
    }
}


void FrameParser::handleMovement(const std::list<const DrawCall*>& tex3ToTex1DrawCalls, Frame& frame)
{
    if(tex3ToTex1DrawCalls.empty())
        return;

    std::list<std::pair<size_t, size_t>> newMappings;
    int newMiddleTexIndex = mCenterTexIndex;
    for(const DrawCall* drawCall : tex3ToTex1DrawCalls)
    {
        const DrawCall& d = *drawCall;

        unsigned short texTileX = d.texX / TILE_SIZE;
        unsigned short texTileY = d.texY / TILE_SIZE;

        size_t texIndex = texTileX + texTileY * NUM_TILES_X;

        if(texIndex == mCenterTexIndex)
            mMovementMonitor.update(d.screenX, d.screenY);

        if(d.screenX == MIDDLE_SCREEN_X && d.screenY == MIDDLE_SCREEN_Y)
            newMiddleTexIndex = texIndex;

        newMappings.emplace(newMappings.end(), texTileX, texTileY);
    }

    int movementX = mMovementMonitor.getNumStepsX();
    int movementY = mMovementMonitor.getNumStepsY();

    if(mCenterTexIndex != newMiddleTexIndex)
    {
        auto itMapping = newMappings.begin();
        auto itDrawCall = tex3ToTex1DrawCalls.begin();

        while(itMapping != newMappings.end())
        {
            auto texCoords = *itMapping;
            const DrawCall& d = **itDrawCall;

            int screenTileX = d.screenX / TILE_SIZE;
            int screenTileY = d.screenY / TILE_SIZE;

            mTex3TileIndexToTex1TileIndexMap[texCoords.first][texCoords.second] = std::make_pair(screenTileX, screenTileY);

            itMapping++;
            itDrawCall++;
        }
    }
    else if(movementX != 0 || movementY != 0)
    {
        int snapDistanceX;
        int snapDistanceY;
        int edgeAdjustX;
        int edgeAdjustY;
        getTileAdjustmentData(snapDistanceX, snapDistanceY, edgeAdjustX, edgeAdjustY);

        auto itMapping = newMappings.begin();
        auto itDrawCall = tex3ToTex1DrawCalls.begin();
        while(itMapping != newMappings.end())
        {
            int screenTileX;
            int screenTileY;
            getAdjustedScreenTileCoordinates
            (
                snapDistanceX,
                snapDistanceY,
                edgeAdjustX,
                edgeAdjustY,
                **itDrawCall,
                screenTileX,
                screenTileY
            );

            auto texCoords = *itMapping;
            mTex3TileIndexToTex1TileIndexMap[texCoords.first][texCoords.second] = std::make_pair(screenTileX, screenTileY);

            itMapping++;
            itDrawCall++;
        }
    }


    if(mMovementMonitor.getDeltaX() < 0)
        frame.movementDirectionX = -1;
    else if(mMovementMonitor.getDeltaX() > 0)
        frame.movementDirectionX = 1;

    if(mMovementMonitor.getDeltaY() < 0)
        frame.movementDirectionY = -1;
    else if(mMovementMonitor.getDeltaY() > 0)
        frame.movementDirectionY = 1;

    frame.numStepsX = movementX;
    frame.numStepsY = movementY;

    if(mCenterTexIndex != newMiddleTexIndex)
        mMovementMonitor.reset();
    mCenterTexIndex = newMiddleTexIndex;

}

void FrameParser::getTileAdjustmentData(int& snapDistanceX, int& snapDistanceY, int& edgeAdjustX, int& edgeAdjustY) const
{
    snapDistanceX = 0;
    snapDistanceY = 0;
    edgeAdjustX = 0;
    edgeAdjustY = 0;

    if(mMovementMonitor.getDeltaX() != 0)
    {
        snapDistanceX = TILE_SIZE - std::abs(mMovementMonitor.getDeltaX());

        if(mMovementMonitor.getDeltaX() < 0)
            snapDistanceX = -snapDistanceX;
        else
            edgeAdjustX = -TILE_SIZE;
    }

    if(mMovementMonitor.getDeltaY() != 0)
    {
        snapDistanceY = TILE_SIZE - std::abs(mMovementMonitor.getDeltaY());

        if(mMovementMonitor.getDeltaY() < 0)
            snapDistanceY = -snapDistanceY;
        else
            edgeAdjustY = -TILE_SIZE;
    }
}

void FrameParser::getAdjustedScreenTileCoordinates(int snapDistanceX, int snapDistanceY, int edgeAdjustX, int edgeAdjustY, const DrawCall& d, int& tileX, int& tileY, int* x, int* y) const
{
    int screenTileX = (int)d.screenX + snapDistanceX;
    int screenTileY = (int)d.screenY + snapDistanceY;



    int newScreenTileX = screenTileX;
    int newScreenTileY = screenTileY;

    if(newScreenTileX < 0 && newScreenTileX > -TILE_SIZE)
        newScreenTileX = (newScreenTileX - TILE_SIZE) / TILE_SIZE;
    else
        newScreenTileX /= TILE_SIZE;

    if(newScreenTileY < 0 && newScreenTileY > -TILE_SIZE)
        newScreenTileY = (newScreenTileY - TILE_SIZE) / TILE_SIZE;
    else
        newScreenTileY /= TILE_SIZE;


//    if(d.screenX == 0)
//        screenTileX += edgeAdjustX + mMovementMonitor.getDeltaX();
//
//    if(d.screenY == 0)
//        screenTileY += edgeAdjustY + mMovementMonitor.getDeltaY();
//


    if(x)
        *x = screenTileX;

    if(y)
        *y = screenTileY;

//    screenTileX /= TILE_SIZE;
//    screenTileY /= TILE_SIZE;


    tileX = newScreenTileX;
    tileY = newScreenTileY;
}



FrameParser::Frame FrameParser::parse(const DrawCall* drawCalls, size_t numDrawCalls)
{
    std::list<const DrawCall*> tex3ToTex1DrawCalls;
    std::list<const DrawCall*> uiDrawCalls;
    std::list<const DrawCall*> sceneDrawCalls;
    std::list<const DrawCall*> tex3DrawCalls;
    std::list<const DrawCall*> minimapDrawCalls;

    for(size_t i = 0; i < numDrawCalls; i++)
    {
        const DrawCall& d = drawCalls[i];
        switch(d.targetTextureId)
        {
            case 0:
                if(d.sourceTextureId == 4)
                    minimapDrawCalls.push_back(&d);
                else if(d.sourceTextureId > 4)
                    uiDrawCalls.push_back(&d);
                break;

            case 1:
                if(d.sourceTextureId == 3)
                    tex3ToTex1DrawCalls.push_back(&d);
                else if(d.sourceTextureId > 4)
                    sceneDrawCalls.push_back(&d);
                break;

            case 3:
                if(d.sourceTextureId > 4)
                    tex3DrawCalls.push_back(&d);
                break;

            default:
                break;

        }
    }

    Frame frame;
    handleMovement(tex3ToTex1DrawCalls, frame);

    parseTex3(tex3DrawCalls, frame.tex3);
    parseTex1(sceneDrawCalls, frame.tex1);

    return frame;
}

std::list<FrameParser::SpriteObjectPairing> FrameParser::getPairings(const DrawCall& d) const
{
    size_t texX = d.texX - d.texX % TILE_SIZE;
    size_t texY = d.texY - d.texY % TILE_SIZE;

    std::list<SpriteObjectPairing> pairings;
    mSpriteCache.get(d.sourceTextureId, texX, texY, pairings);

    return pairings;
}

FrameParser::DrawCallInfo FrameParser::getDrawCallInfo(const DrawCall& d) const
{
    DrawCallInfo drawInfo;
    drawInfo.pairings = getPairings(d);
    drawInfo.x = d.screenX;
    drawInfo.y = d.screenY;
    drawInfo.width = d.width;
    drawInfo.height = d.height;
    drawInfo.texX = d.texX;
    drawInfo.texY = d.texY;


    char region;
    char full = 0;
    char topLeft = 1;
    char topRight = 2;
    char bottomRight = 3;
    char bottomLeft = 4;
    if(d.texWidth == TILE_SIZE && d.texHeight == TILE_SIZE)
    {
        drawInfo.region = DrawCallInfo::Region::FULL;
    }
    else
    {
        if(d.texX % TILE_SIZE == 0)
        {
            if(d.texY % TILE_SIZE == 0)
            {
                drawInfo.region = DrawCallInfo::Region::TOP_LEFT;
            }
            else
            {
                drawInfo.region = DrawCallInfo::Region::BOTTOM_LEFT;
            }
        }
        else if(d.texY % TILE_SIZE == 0)
        {
            drawInfo.region = DrawCallInfo::Region::TOP_RIGHT;
        }
        else
            drawInfo.region = DrawCallInfo::Region::BOTTOM_RIGHT;
    }

    drawInfo.drawCall = d;

    return drawInfo;
}

void FrameParser::parseTex3(const std::list<const DrawCall*>& drawCalls, Layer& layer) const
{
    for(const DrawCall* d : drawCalls)
    {
        unsigned short screenTileX = d->screenX / TILE_SIZE;
        unsigned short screenTileY = d->screenY / TILE_SIZE;
        auto screenCoords = mTex3TileIndexToTex1TileIndexMap[screenTileX][screenTileY];
        short tileX = screenCoords.first;
        short tileY = screenCoords.second;

        DrawCallInfo drawInfo = getDrawCallInfo(*d);
        if(tileX == -1 || tileX == Layer::NUM_TILES_VIEW_X)
            layer.previousX[tileY].push_back(drawInfo);
        else if(tileY == -1 || tileY == Layer::NUM_TILES_VIEW_Y)
            layer.previousY[tileX].push_back(drawInfo);
        else
            layer.view[tileX][tileY].push_back(drawInfo);
    }
}

void FrameParser::parseTex1(const std::list<const DrawCall*>& drawCalls, Layer& layer) const
{
    int snapDistanceX;
    int snapDistanceY;
    int edgeAdjustX;
    int edgeAdjustY;
    getTileAdjustmentData(snapDistanceX, snapDistanceY, edgeAdjustX, edgeAdjustY);

    for(const DrawCall* d : drawCalls)
    {
        int tileX;
        int tileY;
        int x;
        int y;
        getAdjustedScreenTileCoordinates
        (
            snapDistanceX,
            snapDistanceY,
            edgeAdjustX,
            edgeAdjustY,
            *d,
            tileX,
            tileY,
            &x,
            &y
        );

        DrawCallInfo drawInfo = getDrawCallInfo(*d);
        drawInfo.x = x;
        drawInfo.y = y;
        if(tileX == -1 || tileX == Layer::NUM_TILES_VIEW_X)
            layer.previousX[tileY].push_back(drawInfo);
        else if(tileY == -1 || tileY == Layer::NUM_TILES_VIEW_Y)
            layer.previousY[tileX].push_back(drawInfo);
        else
            layer.view[tileX][tileY].push_back(drawInfo);
    }
}
