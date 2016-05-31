///////////////////////////////////
// Internal ShankBot headers
#include "Scene.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <iostream>
#include <sstream>
#include <cassert>
#include <algorithm>
///////////////////////////////////

void Scene::clearOldTilesX(int numClears)
{
    for(size_t x = NUM_TILES_SCENE_X - numClears; x < NUM_TILES_SCENE_X; x++)
        for(size_t y = 0; y < NUM_TILES_SCENE_Y; y++)
            mTex3[x][y].clear();
}

void Scene::clearOldTilesY(int numClears)
{
    for(size_t y = NUM_TILES_SCENE_Y - numClears; y < NUM_TILES_SCENE_Y; y++)
        for(size_t x = 0; x < NUM_TILES_SCENE_X; x++)
            mTex3[x][y].clear();
}

void Scene::move(int movementX, int movementY)
{
    if(movementX < 0)
    {
        int numClears = -movementX;
        if(mLastStepX > 0)
            numClears -= 1;

        clearOldTilesX(numClears);

        for(size_t i = 0; i < -movementX; i++)
            for(size_t x = 1; x < NUM_TILES_SCENE_X; x++)
                    std::swap(mTex3[x - 1], mTex3[x]);
    }
    else if(movementX > 0)
    {
        int numClears = movementX;
        if(mLastStepX < 0)
            numClears -= 1;

        clearOldTilesX(numClears);


        for(size_t i = 0; i < movementX; i++)
            for(int x = NUM_TILES_VIEW_X; x > 0; x--)
                    std::swap(mTex3[x - 1], mTex3[x]);
    }

    if(movementY < 0)
    {
        int numClears = -movementY;
        if(mLastStepY > 0)
            numClears -= 1;

        clearOldTilesY(numClears);

        for(size_t i = 0; i < -movementY; i++)
            for(size_t y = 1; y < NUM_TILES_SCENE_Y; y++)
                for(size_t x = 0; x < NUM_TILES_SCENE_X; x++)
                    std::swap(mTex3[x][y - 1], mTex3[x][y]);
    }
    else if(movementY > 0)
    {
        int numClears = movementY;
        if(mLastStepY < 0)
            numClears -= 1;

        clearOldTilesY(numClears);

        for(size_t i = 0; i < movementY; i++)
            for(int y = NUM_TILES_VIEW_Y; y > 0; y--)
                for(size_t x = 0; x < NUM_TILES_SCENE_X; x++)
                    std::swap(mTex3[x][y - 1], mTex3[x][y]);
    }
}

void Scene::handleMovement(const FrameParser::Frame& frame)
{
    int movementX = frame.numStepsX;
    int movementY = frame.numStepsY;

    move(movementX, movementY);

    if(movementX != 0)
        mLastStepX = movementX;

    if(movementY != 0)
        mLastStepY = movementY;

    mMovementDirectionX = frame.movementDirectionX;
    mMovementDirectionY = frame.movementDirectionY;
}

void Scene::updateTex3(const FrameParser::Frame& frame)
{
    const auto CONTAINS_GROUND_DRAW = [](const std::list<FrameParser::DrawCallInfo>& tilePairings)
    {
            for(auto pairings : tilePairings)
                for(auto pair : pairings.pairings)
                    for(auto object : pair.objects)
                        if(object->type == TibiaDat::Object::Type::ITEM && object->itemInfo->isGround)
                            return true;

            return false;
    };

    for(size_t x = 0; x < NUM_TILES_VIEW_X; x++)
        for(size_t y = 0; y < NUM_TILES_VIEW_Y; y++)
        {
            auto& existingPairings = mTex3[x][y];
            const auto& newPairings = frame.tex3.view[x][y];

            if(CONTAINS_GROUND_DRAW(newPairings))
                existingPairings.clear();

            existingPairings.insert(existingPairings.end(), newPairings.begin(), newPairings.end());
        }

    for(size_t y = 0; y < NUM_TILES_VIEW_Y; y++)
    {
        auto& existingPairings = mTex3[NUM_TILES_VIEW_X][y];
        const auto& newPairings = frame.tex3.previousX[y];

        if(CONTAINS_GROUND_DRAW(newPairings))
            existingPairings.clear();

        existingPairings.insert(existingPairings.end(), newPairings.begin(), newPairings.end());
    }

    for(size_t x = 0; x < NUM_TILES_VIEW_X; x++)
    {
        auto& existingPairings = mTex3[x][NUM_TILES_VIEW_Y];
        const auto& newPairings = frame.tex3.previousY[x];

        if(CONTAINS_GROUND_DRAW(newPairings))
            existingPairings.clear();

        existingPairings.insert(existingPairings.end(), newPairings.begin(), newPairings.end());
    }
}

void Scene::updateTex1(const FrameParser::Frame& frame)
{
    for(size_t x = 0; x < NUM_TILES_VIEW_X; x++)
        for(size_t y = 0; y < NUM_TILES_VIEW_Y; y++)
        {
            auto& existingPairings = mTex1[x][y];
            const auto& newPairings = frame.tex1.view[x][y];

            existingPairings.clear();
            existingPairings.insert(existingPairings.end(), newPairings.begin(), newPairings.end());
        }

    for(size_t y = 0; y < NUM_TILES_VIEW_Y; y++)
    {
        auto& existingPairings = mTex1[NUM_TILES_VIEW_X][y];
        const auto& newPairings = frame.tex1.previousX[y];

        existingPairings.clear();
        existingPairings.insert(existingPairings.end(), newPairings.begin(), newPairings.end());
    }

    for(size_t x = 0; x < NUM_TILES_VIEW_X; x++)
    {
        auto& existingPairings = mTex1[x][NUM_TILES_VIEW_Y];
        const auto& newPairings = frame.tex1.previousY[x];

        existingPairings.clear();
        existingPairings.insert(existingPairings.end(), newPairings.begin(), newPairings.end());
    }
}

void Scene::update(const FrameParser::Frame& frame)
{
    handleMovement(frame);

    updateTex3(frame);


    if(frame.numStepsX != 0 || frame.numStepsY != 0)
        for(auto pairings : mTex3[NUM_TILES_VIEW_X / 2][NUM_TILES_VIEW_Y / 2])
            for(auto pair : pairings.pairings)
                for(auto object : pair.objects)
                {
                    if(object->type == TibiaDat::Object::Type::ITEM && object->itemInfo->isPickupable)
                    {
                        std::cout << object->id << " - " << object->itemInfo->name << std::endl;
                    }
                }


    updateTex1(frame);

    for(size_t x = 0; x < NUM_TILES_SCENE_X; x++)
        for(size_t y = 0; y < NUM_TILES_SCENE_Y; y++)
        {
            for(auto pairings : mTex1[x][y])
            {
                for(auto pair : pairings.pairings)
                {
                    for(auto object : pair.objects)
                    {
                        if(object->id == 3492)
                            std::cout << "WORM - " << x << "x" << y << std::endl;
                    }
                }
            }
        }
//
//    for(size_t x = 0; x < NUM_TILES_VIEW_X; x++)
//        for(size_t y = 0; y < NUM_TILES_VIEW_Y; y++)
//        {
//            for(auto pairings : mTex3[x][y])
//            {
//                std::set<const TibiaDat::Object*> offsetObjects;
//                for(auto pair : pairings.pairings)
//                {
//                    for(auto object : pair.objects)
//                        if(object->itemInfo->offsetX != 0 || object->itemInfo->offsetY != 0)
//                            offsetObjects.insert(object);
//                }
//                if(!offsetObjects.empty())
//                {
//                    std::cout << "Offsetted:";
//
//                    for(auto object : offsetObjects)
//                    {
//                        std::cout << " " << object->id << "-" << x << "x" << y << "-o:" << object->itemInfo->offsetX << "x" << object->itemInfo->offsetY;
//                    }
//
//                    std::cout << std::endl;
//                }
//
//
//
////                if(!pairings.isAligned)
////                    for(auto pair : pairings.pairings)
////                    {
////                        std::list<const TibiaDat::Object*> nonOffsetObjects;
////
//////                        for(auto object : pair.objects)
//////                            if(object->sprites)
////
////                        if(!pair.objects.empty())
////                        {
////                            std::cout << "Misaligned:";
////
////                            for(auto object : pair.objects)
////                            {
////                                std::cout << " " << object->id << "-" << x << "x" << y;
////                            }
////
////                            std::cout << std::endl;
////                        }
////
////
////                    }
//            }
//        }


    for(size_t x = 0; x < NUM_TILES_VIEW_X; x++)
        for(size_t y = 0; y < NUM_TILES_VIEW_Y; y++)
        {
            for(auto pairings : mTex3[x][y])
            {
//                std::set<const TibiaDat::Object*> offsetObjects;
//                for(auto pair : pairings.pairings)
//                {
//                    for(auto object : pair.objects)
//                        if(object->itemInfo->offsetX != 0 || object->itemInfo->offsetY != 0)
//                            offsetObjects.insert(object);
//                }
//                if(!offsetObjects.empty())
//                {
//                    std::cout << "Offsetted:";
//
//                    for(auto object : offsetObjects)
//                    {
//                        std::cout << " " << object->id << "-" << x << "x" << y << "-o:" << object->itemInfo->offsetX << "x" << object->itemInfo->offsetY;
//                    }
//
//                    std::cout << std::endl;
//                }

//                std::set<const TibiaDat::Object*> nonOffsetObjects;
//                for(auto pair : pairings.pairings)
//                {
//                    for(auto object : pair.objects)
//                        if(object->itemInfo->offsetX == 0 && object->itemInfo->offsetY == 0)
//                            nonOffsetObjects.insert(object);
//                        else if(object->id == 872)
//                            std::cout << "Stand: " << object->id << "-" << x + 1 << "x" << y + 1 << std::endl;
//                }
//
//
//                if(!pairings.isAligned)
//                    if(!nonOffsetObjects.empty())
//                        {
//                            std::cout << "Misaligned:";
//
//                            for(auto object : nonOffsetObjects)
//                            {
//                                std::cout << " " << object->id << "-" << x << "x" << y;
//                            }
//
//                            std::cout << std::endl;
//                        }
            }
        }
}

Scene::Object Scene::parseObjectTile(const TibiaDat::Object* object, size_t spriteId, const FrameParser::DrawCallInfo& drawInfo, char tileX, char tileY, char layer) const
{
    Object o;
    o.tileX = tileX;
    o.tileY = tileY;
    o.object = object;

    const TibiaDat::SpritesInfo* spritesInfo = nullptr;
    size_t spriteIdIndex = 0;
    size_t iSprite = 0;
    for(const TibiaDat::SpritesInfo& info : object->spritesInfos)
    {
        for(size_t i = 0; i < info.spriteIds.size(); i++)
        {
            if(spriteId == info.spriteIds[i])
            {
                spritesInfo = &info;
                spriteIdIndex = i % (spritesInfo->width * spritesInfo->height);
                iSprite = i;

                break;
            }
        }

        if(spritesInfo != nullptr)
            break;
    }

    if(spritesInfo == nullptr)
    {
        std::stringstream sstream;
        sstream << "Cannot parse object tile. Sprite id " << spriteId
                << " does not exist in object with type " << (int)object->type
                << " and id " << object->id << "." << std::endl;

        throw std::runtime_error(sstream.str());
    }
//
//
//    if(object->type == TibiaDat::Object::Type::OUTFIT)
//        std::cout << spriteIdIndex << std::endl;

    short screenX = drawInfo.x;
    short screenY = drawInfo.y;

    typedef FrameParser::DrawCallInfo::Region Region;

    if(drawInfo.region != Region::FULL && drawInfo.region != Region::TOP_LEFT)
    {
        short texUnalignmentX = drawInfo.texX % FrameParser::TILE_SIZE;
        short texUnalignmentY = drawInfo.texY % FrameParser::TILE_SIZE;

        switch(drawInfo.region)
        {
            case Region::TOP_RIGHT:
//                screenX -= FrameParser::TILE_SIZE - (texUnalignmentX + drawInfo.width);
                screenX -= texUnalignmentX;
                break;

            case Region::BOTTOM_LEFT:
//                screenY -= FrameParser::TILE_SIZE - (texUnalignmentY + drawInfo.height);
                screenY -= texUnalignmentY;
                break;

            case Region::BOTTOM_RIGHT:
                screenX -= texUnalignmentX;
                screenY -= texUnalignmentY;
//                screenX -= FrameParser::TILE_SIZE - (texUnalignmentX + drawInfo.width);
//                screenY -= FrameParser::TILE_SIZE - (texUnalignmentY + drawInfo.height);
                break;

            default:
                break;
        }
    }
//
//    if(drawInfo.width != FrameParser::TILE_SIZE || drawInfo.height != FrameParser::TILE_SIZE)
//    {
//        short unalignmentX = screenX % FrameParser::TILE_SIZE;
//        short unalignmentY = screenY % FrameParser::TILE_SIZE;
//
//        // This drawcall is drawing just a fragment of a sprite.
//        // Snap it up so that we get the top-left coordinates for
//        // the entire sprite.
//        if(unalignmentX == 0)
//            screenX -= FrameParser::TILE_SIZE - drawInfo.width;
//
//        if(unalignmentY == 0)
//            screenY -= FrameParser::TILE_SIZE - drawInfo.height;
//    }

    screenX += object->itemInfo->offsetX;
    screenY += object->itemInfo->offsetY;

    short unalignmentX = (screenX + FrameParser::TILE_SIZE * 3) % FrameParser::TILE_SIZE;
    short unalignmentY = (screenY + FrameParser::TILE_SIZE * 3) % FrameParser::TILE_SIZE;


    if(unalignmentX > 0 || unalignmentY > 0)
    {
        if(unalignmentX == unalignmentY)
        {
            // We are dealing with something lying on a stack.
            screenX += FrameParser::TILE_SIZE - unalignmentX;
            screenY += FrameParser::TILE_SIZE - unalignmentY;
            o.isOnStack = true;
        }
        else
        {
            // We are dealing with something on the move!

            if(object->type == TibiaDat::Object::Type::OUTFIT)
            {
                if(unalignmentX > 0 && unalignmentY > 0)
                {
                    o.isOnStack = true;
                }

                int direction = iSprite;
                direction %= spritesInfo->width * spritesInfo->height * spritesInfo->divX * spritesInfo->blendFrames;
                direction /= spritesInfo->divX * spritesInfo->blendFrames;

                // 0 = up
                // 1 = right
                // 2 = down
                // 3 = left
                if(direction == 0 || direction == 2)
                {
                    int stackAdjustmentX = unalignmentX > 0 ? FrameParser::TILE_SIZE - unalignmentX : 0;
                    screenX += stackAdjustmentX;
                    screenY += stackAdjustmentX;
                    assert(screenX % FrameParser::TILE_SIZE == 0);

                    unalignmentY = screenY % FrameParser::TILE_SIZE;

                    if(direction == 0)
                        screenY -= unalignmentY;
                    else
                        screenY += FrameParser::TILE_SIZE - unalignmentY;

                    assert(screenY % FrameParser::TILE_SIZE == 0);
                }
                else if(direction == 1 || direction == 3)
                {
                    int stackAdjustmentY = unalignmentY > 0 ? FrameParser::TILE_SIZE - unalignmentY : 0;
                    screenX += stackAdjustmentY;
                    screenY += stackAdjustmentY;
                    assert(screenY % FrameParser::TILE_SIZE == 0);

                    unalignmentX = screenX % FrameParser::TILE_SIZE;

                    if(direction == 3)
                        screenX -= unalignmentX;
                    else
                        screenX += FrameParser::TILE_SIZE - unalignmentX;

                    assert(screenX % FrameParser::TILE_SIZE == 0);
                }
                else
                {
                    std::stringstream sstream;
                    sstream << "Error parsing sprite id " << spriteId << " for outfit object " << object->id << ". "
                            << "Expected four directions in total, ranging from 0-3. Got " << direction << ".";

                    throw std::runtime_error(sstream.str());
                }
//
//
//                switch(direction)
//                {
//                    case 0: // Up
//                        if(unalignmentX > 0)
//                        {
//                            screenX += FrameParser::TILE_SIZE - unalignmentX;
//                            screenY += FrameParser::TILE_SIZE - unalignmentX;
//                        }
//
//
//
//
//
//                        assert(screenY % FrameParser::TILE_SIZE == 0);
//                        break;
//
//                    case 1: // Right
//                        screenX += FrameParser::TILE_SIZE - unalignmentX;
//                        screenY += FrameParser::TILE_SIZE - unalignmentX;
//                        assert(screenX % FrameParser::TILE_SIZE == 0);
//
//                        unalignmentY = screenY % FrameParser::TILE_SIZE;
//                        if(unalignmentY > 0)
//                            screenY += FrameParser::TILE_SIZE - unalignmentY;
//                        assert(screenY % FrameParser::TILE_SIZE == 0);
//                        break;
//
//                    case 2: // Down
//                        screenX += FrameParser::TILE_SIZE - unalignmentX;
//                        screenY += FrameParser::TILE_SIZE - unalignmentX;
//                        assert(screenX % FrameParser::TILE_SIZE == 0);
//
//                        unalignmentY = screenY % FrameParser::TILE_SIZE;
//                        if(unalignmentY > 0)
//
//                        assert(screenY % FrameParser::TILE_SIZE == 0);
//                        break;
//
//                    case 3: // Left
//
//                        break;
//
//                    default:
//                        std::stringstream sstream;
//                        sstream << "Error parsing sprite id " << spriteId << " for outfit object " << object->id << ". "
//                                << "Expected four directions in total, ranging from 0-3. Got " << direction << ".";
//
//                        throw std::runtime_error(sstream.str());
//                        break;
//                }
            }
            else if(object->type == TibiaDat::Object::Type::DISTANCE)
            {
                // TODO. Maybe.

//                int direction = iSprite / (spritesInfo->width * spritesInfo->height);
//                switch(direction)
//                {
//                    case 0: // Top left
//
//                        break;
//
//                    case 1: // Top
//
//                        break;
//
//                    case 2: // Top right
//
//                        break;
//
//                    case 3: // Left
//
//                        break;
//
//                    case 4: // Middle (nothing?)
//                        // Don't really know what to do here...
//                        break;
//
//                    case 5: // Right
//
//                        break;
//
//                    case 6: // Bottom left
//
//                        break;
//
//                    case 7: // Bottom
//
//                        break;
//
//                    case 8: // Bottom right
//
//                        break;
//
//                    default:
//                        std::stringstream sstream;
//                        sstream << "Error parsing sprite id " << spriteId << " for distance object " << object->id << ". "
//                                << "Expected nine directions in total, ranging from 0-8. Got " << direction << ".";
//
//                        throw std::runtime_error(sstream.str());
//                        break;
//
//                }
            }
            else
            {
                std::stringstream sstream;
                sstream << "Error parsing sprite id " << spriteId << " for object " << object->id << ". "
                        << "Expected a moving object type (outfit (" << (int)TibiaDat::Object::Type::OUTFIT << ") "
                        << " or distance (" << (int)TibiaDat::Object::Type::DISTANCE << ")), not type " << (int)object->type << ". "
                        << " Source texture: " << (int)drawInfo.drawCall.sourceTextureId << ", target texture: " << (int)drawInfo.drawCall.targetTextureId << ".";

                throw std::runtime_error(sstream.str());
            }
        }
    }


    if(screenX % FrameParser::TILE_SIZE != 0)
        int flerp = 0;

    assert(screenX % FrameParser::TILE_SIZE == 0);
    assert(screenY % FrameParser::TILE_SIZE == 0);


    int deltaTileX = screenX / FrameParser::TILE_SIZE - drawInfo.x / FrameParser::TILE_SIZE;
    int deltaTileY = screenY / FrameParser::TILE_SIZE - drawInfo.y / FrameParser::TILE_SIZE;

    tileX += deltaTileX;
    tileY += deltaTileY;

//    tileX = screenX / FrameParser::TILE_SIZE;
//    tileY = screenY / FrameParser::TILE_SIZE;


    short snapDistanceX = FrameParser::TILE_SIZE - screenX % FrameParser::TILE_SIZE;
    short snapDistanceY = FrameParser::TILE_SIZE - screenY % FrameParser::TILE_SIZE;




    if(spriteIdIndex > 0)
    {
        // We are dealing with an object that is larger than 1x1.
        // Adjust the tile coordinates to point to the object's
        // base tile (always bottom right).
        tileX += spriteIdIndex % spritesInfo->width;
        tileY += spriteIdIndex / spritesInfo->width;

        // Since this is not the base tile, we don't know
        // which layer this object belongs to.
        o.layer = -1;

    }
    else// if(tileX < NUM_TILES_VIEW_X && tileY < NUM_TILES_VIEW_Y) // If we can see its tile we know its layer
    {

//        if(drawInfo.drawCall.targetTextureId == 1)
//        {
//            if(drawInfo.region == Region::FULL || drawInfo.region == Region::BOTTOM_RIGHT)
//                o.layer = layer;
//            else
//                o.layer = -1;
//        }
//        else
            o.layer = layer;
    }
//    else
//        o.layer = -1;

//
//    if(object->itemInfo->offsetX != 0 || object->itemInfo->offsetY != 0)
//    {
//        // * If an object with offset is aligned, it means that it
//        // is so high up on a stack that it has aligned itself with the neighboring
//        // top-left tile. This can only happen with offsetted objects, since a stack
//        // is never higher than 32. This means the tile coordinates should be moved 1x1.
//        // * If an object with offset is misaligned, it may be on a stack
//        // and it may not.
//        // * Either way, am offsetted object should always be adjusted.
//
//        if
//
//        if(!isTopLeftAligned || isBottomRightAligned)
//        {
//            if(object->itemInfo->offsetX != 0)
//                tileX += 1;
//
//            if(object->itemInfo->offsetY != 0)
//                tileY += 1;
//        }
////        else
//
//
//
//        // * If the object is an outfit, it means it may be moving.
//        // This can be deduced by checking which animation frame
//        // the sprite is in. However, there are some objects that
//        // have idle animations. For these objects, there is no way
//        // to know whether it is moving by only looking at this object.
//        // We would need to know if it is standing on a stack, and run
//        // some more checks using the draw call's screen coordinates.
//        //
//        // In other words, finding out if an outfit is moving is possible,
//        // but harder to do for some outfits than others.
//        // I will leave this as a possible TODO, since it is not super important.
//
//        // * If the object is a distance (projectile), it is always moving.
//        // But do we really care about the exact position of a projectile?
//        // I will leave this as a possible TODO, since it is not super important.
//    }
//    else
//    {
//        if(!isTopLeftAligned)
//        {
//            // Object is on a stack. Move it 1x1 tiles.
//            tileX += 1;
//            tileY += 1;
//        }
//    }
//
//
//
    o.tileX = tileX;
    o.tileY = tileY;


    return o;
}



std::list<Scene::Tile> Scene::getPickupables() const
{

//    for(size_t x = 0; x < maxTileX; x++)
//        for(size_t y = 0; y < maxTileY; y++)
//        {
//            char layer = 0;
//            for(auto drawCall : mTex1[x][y])
//            {
//                for(auto pair : drawCall.pairings)
//                {
//                    for(auto object : pair.objects)
//                    {
//                        Object o = parseObjectTile(object, pair.spriteId, drawCall, x, y, layer);
//                    }
//                }
//
//                layer++;
//            }
//        }

//    std::map<TibiaDat::Object*, std::list<char, >> offsettedObjects;


    Tile tiles[MAX_VISIBLE_TILES_X][MAX_VISIBLE_TILES_Y];

    std::list<Object> stacks[MAX_VISIBLE_TILES_X][MAX_VISIBLE_TILES_Y][MAX_STACK_TILE_HEIGHT][MAX_STACK_TILE_HEIGHT];
    for(int x = 0; x < NUM_TILES_VIEW_X; x++)
        for(int y = 0; y < NUM_TILES_VIEW_Y; y++)
        {
            char layer = 0;
            auto& visibleStacks = stacks[x][y];
            for(auto drawCall : mTex3[x][y])
            {
                bool isKnownLayerObjectFound = false;
                for(auto pair : drawCall.pairings)
                {
                    for(auto object : pair.objects)
                    {
//                        if(object->itemInfo->isWall)
                        {
                            Object o = parseObjectTile(object, pair.spriteId, drawCall, x, y, layer);

                            Tile& tile = tiles[o.tileX][o.tileY];

                            if(o.layer != -1 && o.isOnStack)
                            {
                                int stackX = MAX_STACK_TILE_HEIGHT - (o.tileX - x) - 1;
                                int stackY = MAX_STACK_TILE_HEIGHT - (o.tileY - y) - 1;

                                assert(stackX < MAX_STACK_TILE_HEIGHT && stackX >= 0);
                                assert(stackY < MAX_STACK_TILE_HEIGHT && stackY >= 0);

//                                    std::list<Object>& stack = visibleStacks[stackX][stackY];
                                std::list<Object>& stack = stacks[o.tileX][o.tileY][stackX][stackY];

                                isKnownLayerObjectFound = true;
                                if(std::find(stack.begin(), stack.end(), o) == stack.end())
                                    stack.push_back(o);
                            }
                            else if(o.tileX == x && o.tileY == y) //|| (o.tileX < NUM_TILES_VIEW_X && o.tileY < NUM_TILES_VIEW_Y))
                            {
                                if(tile.stackStartLayer == -1)
                                {
                                    char height = o.object->itemInfo->height;
                                    if(height > 0)
                                        tile.stackStartLayer = layer + 1;
                                }
                                else
                                    assert(!o.isOnStack);

                                isKnownLayerObjectFound = true;

                                if(std::find(tile.knownLayerObjects.begin(), tile.knownLayerObjects.end(), o) == tile.knownLayerObjects.end())
                                    tile.knownLayerObjects.push_back(o);


//
//                                if
//                                (
//                                    std::find_if
//                                    (
//                                        tile.knownLayerObjects.begin(),
//                                        tile.knownLayerObjects.end(),
//                                        [&](const Object& other){return o.object == other.object;}
//                                    ) == tile.knownLayerObjects.end()
//                                )
//                                    tile.knownLayerObjects.push_back(o);
                            }
                            else
                            {
                                if(std::find(tile.unknownLayerObjects.begin(), tile.unknownLayerObjects.end(), o) == tile.unknownLayerObjects.end())
                                    tile.unknownLayerObjects.push_back(o);
                            }
                        }
                    }
                }

                if(isKnownLayerObjectFound)
                    layer++;
            }

            for(auto drawCall : mTex1[x][y])
            {
                bool isKnownLayerObjectFound = false;
                for(auto pair : drawCall.pairings)
                {
                    for(auto object : pair.objects)
                    {
//                        if(object->id == 9060)
//                        {
//                            std::cout << object->id << " - " << object->itemInfo->name << " - " << x << "x" << y << std::endl;
//                        }

//                        if(object->itemInfo->isWall)
                        {
                            Object o = parseObjectTile(object, pair.spriteId, drawCall, x, y, layer);

                            Tile& tile = tiles[o.tileX][o.tileY];

                            if(o.layer != -1 && o.isOnStack)
                            {
                                int stackX = MAX_STACK_TILE_HEIGHT - (o.tileX - x) - 1;
                                int stackY = MAX_STACK_TILE_HEIGHT - (o.tileY - y) - 1;

                                assert(stackX < MAX_STACK_TILE_HEIGHT && stackX >= 0);
                                assert(stackY < MAX_STACK_TILE_HEIGHT && stackY >= 0);

//                                    std::list<Object>& stack = visibleStacks[stackX][stackY];
                                std::list<Object>& stack = stacks[o.tileX][o.tileY][stackX][stackY];

                                isKnownLayerObjectFound = true;
                                if(std::find(stack.begin(), stack.end(), o) == stack.end())
                                    stack.push_back(o);
                            }
                            else if(o.tileX == x && o.tileY == y) //|| (o.tileX < NUM_TILES_VIEW_X && o.tileY < NUM_TILES_VIEW_Y))
                            {
                                if(tile.stackStartLayer == -1)
                                {
                                    char height = o.object->itemInfo->height;
                                    if(height > 0)
                                        tile.stackStartLayer = layer + 1;
                                }
                                else
                                    assert(!o.isOnStack);

                                isKnownLayerObjectFound = true;

                                if(std::find(tile.knownLayerObjects.begin(), tile.knownLayerObjects.end(), o) == tile.knownLayerObjects.end())
                                    tile.knownLayerObjects.push_back(o);


//
//                                if
//                                (
//                                    std::find_if
//                                    (
//                                        tile.knownLayerObjects.begin(),
//                                        tile.knownLayerObjects.end(),
//                                        [&](const Object& other){return o.object == other.object;}
//                                    ) == tile.knownLayerObjects.end()
//                                )
//                                    tile.knownLayerObjects.push_back(o);
                            }
                            else if(o.layer != -1 && o.object->type == TibiaDat::Object::Type::OUTFIT)
                            {
                                if(tile.stackStartLayer == -1)
                                {
                                    char height = o.object->itemInfo->height;
                                    if(height > 0)
                                        tile.stackStartLayer = layer + 1;
                                }
                                else
                                    assert(!o.isOnStack);

                                isKnownLayerObjectFound = true;
                                if(std::find(tile.knownLayerObjects.begin(), tile.knownLayerObjects.end(), o) == tile.knownLayerObjects.end())
                                    tile.knownLayerObjects.push_back(o);
                            }
                            else
                            {
                                if(std::find(tile.unknownLayerObjects.begin(), tile.unknownLayerObjects.end(), o) == tile.unknownLayerObjects.end())
                                    tile.unknownLayerObjects.push_back(o);
                            }

                        }
                    }
                }

                if(isKnownLayerObjectFound)
                    layer++;
            }

//            for(Object& object : tile.knownLayerObjects)
//                object.layer = std::abs(object.layer - (numLayers - 1));

            for(int stackX = 0; stackX < MAX_STACK_TILE_HEIGHT; stackX++)
                for(int stackY = 0; stackY < MAX_STACK_TILE_HEIGHT; stackY++)
                {
                    const std::list<Object>& stack = visibleStacks[stackX][stackY];
                    if(!stack.empty())
                    {
                        char lastLayer = stack.front().layer;
                        for(Object o : stack)
                        {
                            if(o.layer > lastLayer)
                            {
                                assert(o.layer - lastLayer == 1);
                                lastLayer = o.layer;
                            }
                        }
                    }
                }

        }

    for(int x = 0; x < NUM_TILES_VIEW_X; x++)
        for(int y = 0; y < NUM_TILES_VIEW_Y; y++)
        {
            Tile& tile = tiles[x][y];
            tile.tileX = x;
            tile.tileY = y;


            if(tile.stackStartLayer != -1)
            {
                assert(!tile.knownLayerObjects.empty());

                const std::list<Object>* maxStack = nullptr;
                int maxHeight = -1;

                const auto& tileStacks = stacks[x][y];
                for(int stackX = 0; stackX < MAX_STACK_TILE_HEIGHT; stackX++)
                    for(int stackY = 0; stackY < MAX_STACK_TILE_HEIGHT; stackY++)
                    {
                        const std::list<Object>& stack = tileStacks[stackX][stackY];
                        if(!stack.empty())
                        {
                            int minLayer = stack.front().layer;
                            int maxLayer = stack.back().layer;
                            int height = maxLayer - minLayer;
                            if(height > maxHeight)
                            {
                                maxStack = &stack;
                                maxHeight = height;
                            }
//
//                            if(x == 10 && y == 6)
//                            {
//                                std::cout << "stack:";
//                                for(auto o : stack)
//                                {
//                                    std::cout << " " << (int)o.layer << "-" << o.object->id;
//                                }
//
//                                std::cout << std::endl;
//                            }
                        }
                    }

                if(maxStack != nullptr && !maxStack->empty())
                {
//                    int stackHeight = tile.knownLayerObjects.back().layer - tile.stackStartLayer;
//                    if(stackHeight > 0 && stackHeight != maxStack->size())
//                    {
//                        auto itStackStart = std::find_if
//                        (
//                            tile.knownLayerObjects.begin(),
//                            tile.knownLayerObjects.end(),
//                            [&](const Object& other){return other.layer == tile.stackStartLayer;}
//                        );

//                        assert(itStackStart != tile.knownLayerObjects.end());

//                        tile.knownLayerObjects.erase(itStackStart, tile.knownLayerObjects.end());
                        int layerDelta = tile.stackStartLayer - maxStack->front().layer;
                        for(Object o : *maxStack)
                        {
                            o.layer += layerDelta;
                            tile.knownLayerObjects.push_back(o);
                        }
//                    }
                }


            }

            if(tile.knownLayerObjects.empty())
                tile.numLayers = 0;
            else
                tile.numLayers = tile.knownLayerObjects.back().layer + 1;

        }


    std::list<Tile> nonEmptyTiles;
    for(size_t x = 0; x < NUM_TILES_SCENE_X; x++)
        for(size_t y = 0; y < NUM_TILES_SCENE_Y; y++)
        {
            Tile tile = tiles[x][y];

            if(!tile.knownLayerObjects.empty())
            {
//                tile.knownLayerObjects.unique();
//                tile.unknownLayerObjects.unique();

                nonEmptyTiles.push_back(tile);
            }

        }

    return nonEmptyTiles;
//
//    for(size_t x = 0; x < maxTileX; x++)
//        for(size_t y = 0; y < maxTileY; y++)
//        {
//            for(auto pairings : mTex1[x][y])
//            {
//                for(auto pair : pairings.pairings)
//                {
//                    for(auto object : pair.objects)
//                    {
//                        if(object->id == 3492)
//                            std::cout << "WORM - " << x << "x" << y << std::endl;
//                    }
//                }
//            }
//        }
}
