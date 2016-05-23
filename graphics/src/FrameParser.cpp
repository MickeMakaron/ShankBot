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

    frame.isMoving = mMovementMonitor.getDeltaX() != 0 || mMovementMonitor.getDeltaY() != 0;
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

void FrameParser::getAdjustedScreenTileCoordinates(int snapDistanceX, int snapDistanceY, int edgeAdjustX, int edgeAdjustY, const DrawCall& d, int& x, int& y) const
{
    int screenTileX = (int)d.screenX + snapDistanceX;
    int screenTileY = (int)d.screenY + snapDistanceY;

    if(d.screenX == 0)
        screenTileX += edgeAdjustX + mMovementMonitor.getDeltaX();

    if(d.screenY == 0)
        screenTileY += edgeAdjustY + mMovementMonitor.getDeltaY();

    screenTileX /= TILE_SIZE;
    screenTileY /= TILE_SIZE;

    x = screenTileX;
    y = screenTileY;
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

void FrameParser::parseTex3(const std::list<const DrawCall*>& drawCalls, Layer& layer) const
{
    for(const DrawCall* drawCall : drawCalls)
    {
        const DrawCall& d = *drawCall;

        unsigned short screenTileX = d.screenX / TILE_SIZE;
        unsigned short screenTileY = d.screenY / TILE_SIZE;
        auto screenCoords = mTex3TileIndexToTex1TileIndexMap[screenTileX][screenTileY];
        short tileX = screenCoords.first;
        short tileY = screenCoords.second;

        auto pairings = getPairings(d);

        if(tileX == -1 || tileX == Layer::NUM_TILES_VIEW_X)
            layer.previousX[tileY].push_back(pairings);
        else if(tileY == -1 || tileY == Layer::NUM_TILES_VIEW_Y)
            layer.previousY[tileX].push_back(pairings);
        else
            layer.view[tileX][tileY].push_back(pairings);
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
        getAdjustedScreenTileCoordinates
        (
            snapDistanceX,
            snapDistanceY,
            edgeAdjustX,
            edgeAdjustY,
            *d,
            tileX,
            tileY
        );

        auto pairings = getPairings(*d);
        if(tileX == -1 || tileX == Layer::NUM_TILES_VIEW_X)
            layer.previousX[tileY].push_back(pairings);
        else if(tileY == -1 || tileY == Layer::NUM_TILES_VIEW_Y)
            layer.previousY[tileX].push_back(pairings);
        else
            layer.view[tileX][tileY].push_back(pairings);
    }
}
