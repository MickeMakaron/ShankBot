///////////////////////////////////
// Internal ShankBot headers
#include "Scene.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <iostream>
///////////////////////////////////


void Scene::handleMovement(const FrameParser::Frame& frame)
{
    int movementX = frame.numStepsX;
    int movementY = frame.numStepsY;

    if(movementX < 0)
    {
        int numClears = -movementX;
        if(mLastStepX > 0)
            numClears -= 1;

        for(size_t x = NUM_TILES_SCENE_X - numClears; x < NUM_TILES_SCENE_X; x++)
            for(size_t y = 0; y < NUM_TILES_SCENE_Y; y++)
                mScene[x][y].clear();

        for(size_t i = 0; i < -movementX; i++)
            for(size_t x = 1; x < NUM_TILES_SCENE_X; x++)
                    std::swap(mScene[x - 1], mScene[x]);
    }
    else if(movementX > 0)
    {
        int numClears = movementX;
        if(mLastStepX < 0)
            numClears -= 1;

        for(size_t x = NUM_TILES_SCENE_X - numClears; x < NUM_TILES_SCENE_X; x++)
            for(size_t y = 0; y < NUM_TILES_SCENE_Y; y++)
                mScene[x][y].clear();

        for(size_t i = 0; i < movementX; i++)
            for(int x = NUM_TILES_VIEW_X; x > 0; x--)
                    std::swap(mScene[x - 1], mScene[x]);
    }

    if(movementY < 0)
    {
        int numClears = -movementY;
        if(mLastStepY > 0)
            numClears -= 1;

        for(size_t y = NUM_TILES_SCENE_Y - numClears; y < NUM_TILES_SCENE_Y; y++)
            for(size_t x = 0; x < NUM_TILES_SCENE_X; x++)
                mScene[x][y].clear();

        for(size_t i = 0; i < -movementY; i++)
            for(size_t y = 1; y < NUM_TILES_SCENE_Y; y++)
                for(size_t x = 0; x < NUM_TILES_SCENE_X; x++)
                    std::swap(mScene[x][y - 1], mScene[x][y]);
    }
    else if(movementY > 0)
    {
        int numClears = movementY;
        if(mLastStepY < 0)
            numClears -= 1;


        for(size_t y = NUM_TILES_SCENE_Y - numClears; y < NUM_TILES_SCENE_Y; y++)
            for(size_t x = 0; x < NUM_TILES_SCENE_X; x++)
                mScene[x][y].clear();

        for(size_t i = 0; i < movementY; i++)
            for(int y = NUM_TILES_VIEW_Y; y > 0; y--)
                for(size_t x = 0; x < NUM_TILES_SCENE_X; x++)
                    std::swap(mScene[x][y - 1], mScene[x][y]);
    }

    if(movementX != 0)
        mLastStepX = movementX;

    if(movementY != 0)
        mLastStepY = movementY;
}

void Scene::update(const FrameParser::Frame& frame)
{
    handleMovement(frame);

    for(size_t x = 0; x < NUM_TILES_VIEW_X; x++)
        for(size_t y = 0; y < NUM_TILES_VIEW_Y; y++)
        {
            for(auto pairings : frame.tex3.view[x][y])
            {
                bool doBreak = false;
                for(auto pair : pairings)
                {
                    for(auto object : pair.objects)
                    {
                        if(object->type == TibiaDat::Object::Type::ITEM && object->itemInfo->isGround)
                        {
                            mScene[x][y].clear();
                            doBreak = true;
                            break;
                        }
                    }
                    if(doBreak)
                        break;
                }
                if(doBreak)
                    break;
            }


            mScene[x][y].insert(mScene[x][y].end(), frame.tex3.view[x][y].begin(), frame.tex3.view[x][y].end());
        }

    for(size_t y = 0; y < NUM_TILES_VIEW_Y; y++)
    {
        for(auto pairings : frame.tex3.previousX[y])
        {
            bool doBreak = false;
            for(auto pair : pairings)
            {
                for(auto object : pair.objects)
                {
                    if(object->type == TibiaDat::Object::Type::ITEM && object->itemInfo->isGround)
                    {
                        mScene[NUM_TILES_VIEW_X][y].clear();
                        doBreak = true;
                        break;
                    }
                }
                if(doBreak)
                    break;
            }
            if(doBreak)
                break;
        }

        mScene[NUM_TILES_VIEW_X][y].insert(mScene[NUM_TILES_VIEW_X][y].end(), frame.tex3.previousX[y].begin(), frame.tex3.previousX[y].end());
    }

    for(size_t x = 0; x < NUM_TILES_VIEW_X; x++)
    {
        for(auto pairings : frame.tex3.previousY[x])
        {
            bool doBreak = false;
            for(auto pair : pairings)
            {
                for(auto object : pair.objects)
                {
                    if(object->type == TibiaDat::Object::Type::ITEM && object->itemInfo->isGround)
                    {
                        mScene[x][NUM_TILES_VIEW_Y].clear();
                        doBreak = true;
                        break;
                    }
                }
                if(doBreak)
                    break;
            }
            if(doBreak)
                break;
        }

        mScene[x][NUM_TILES_VIEW_Y].insert(mScene[x][NUM_TILES_VIEW_Y].end(), frame.tex3.previousY[x].begin(), frame.tex3.previousY[x].end());
    }

    if(frame.numStepsX != 0 || frame.numStepsY != 0)
        for(auto pairings : mScene[NUM_TILES_VIEW_X / 2][NUM_TILES_VIEW_Y / 2])
            for(auto pair : pairings)
                for(auto object : pair.objects)
                {
                    if(object->type == TibiaDat::Object::Type::ITEM && object->itemInfo->isPickupable)
                    {
                        std::cout << object->id << " - " << object->itemInfo->name << std::endl;
                    }
                }



    for(size_t x = 0; x < NUM_TILES_VIEW_X; x++)
        for(size_t y = 0; y < NUM_TILES_VIEW_Y; y++)
        {
            for(auto pairings : frame.tex1.view[x][y])
            {
                bool doBreak = false;
                for(auto pair : pairings)
                {
                    for(auto object : pair.objects)
                    {
                        if(object->id == 3492)
                            std::cout << "WORM - " << x << "x" << y << std::endl;
                    }
                    if(doBreak)
                        break;
                }
                if(doBreak)
                    break;
            }
        }
}
