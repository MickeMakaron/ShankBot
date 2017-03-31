// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016-2017 Mikael Hernvall
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
* Contact:
*       mikael.hernvall@gmail.com
*
****************************************************************
****************************************************************/
// {SHANK_BOT_LICENSE_END}
///////////////////////////////////
// Internal ShankBot headers
#include "monitor/MiniMap.hpp"
#include "monitor/Constants.hpp"
#include "utility/file.hpp"
#include "utility/utility.hpp"
#include "monitor/Input.hpp"
#include "monitor/GraphicsMonitorReader.hpp"
using namespace GraphicsLayer;
using namespace sb::utility;
using namespace sb::utility::file;
using namespace sb::tibiaassets;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cassert>
#include <sstream>
#include <limits>
#include <cmath>
///////////////////////////////////

///////////////////////////////////
// STD C
///////////////////////////////////

///////////////////////////////////
// Qt
#include "QtGui/QImage"
///////////////////////////////////

const uint32_t MiniMap::M_MINI_MAP_BLACK = 0xff000000;
const size_t MiniMap::M_MINI_MAP_TILE_PIXEL_SIZE = Constants::MINI_MAP_PIXEL_WIDTH * Constants::MINI_MAP_PIXEL_HEIGHT;
const size_t MiniMap::M_MINI_MAP_TILE_BYTE_SIZE = M_MINI_MAP_TILE_PIXEL_SIZE * BYTES_PER_PIXEL_RGBA;
const std::string MiniMap::M_MINI_MAP_COLOR_FILE_PREFIX = "Minimap_Color_";
const std::vector<uint32_t> MiniMap::M_BLACK_TILE(M_MINI_MAP_TILE_PIXEL_SIZE, M_MINI_MAP_BLACK);

const std::map<int, Object::MiniMapColor> MiniMap::RGBA_TO_MINI_MAP_COLOR =
{
    {Color({0, 0, 0, 255}).packed, Object::MiniMapColor::BLACK},
    {Color({255, 255, 0, 255}).packed, Object::MiniMapColor::YELLOW},
    {Color({153, 153, 153, 255}).packed, Object::MiniMapColor::GRAY},
    {Color({255, 51, 0, 255}).packed, Object::MiniMapColor::RED},
    {Color({102, 102, 102, 255}).packed, Object::MiniMapColor::DARK_GRAY},
    {Color({0, 204, 0, 255}).packed, Object::MiniMapColor::GREEN},
    {Color({51, 102, 153, 255}).packed, Object::MiniMapColor::LIGHT_BLUE},
    {Color({0, 102, 0, 255}).packed, Object::MiniMapColor::DARK_GREEN},
    {Color({153, 102, 51, 255}).packed, Object::MiniMapColor::LIGHT_BROWN},
    {Color({153, 51, 0, 255}).packed, Object::MiniMapColor::BROWN},
};

MiniMap::MiniMap(const Input& input, GraphicsMonitorReader& reader)
: mInput(input)
, mReader(reader)
{
    mPixels.reset(new std::vector<unsigned char>());
    using namespace Constants;
    mPixels->resize(M_MINI_MAP_TILE_BYTE_SIZE);
    std::fill((uint32_t*)mPixels->data(), (uint32_t*)(mPixels->data() + mPixels->size()), M_MINI_MAP_BLACK);
}

bool MiniMap::parseMiniMapColorFileName(const std::string& fileName, unsigned int& x, unsigned int& y, unsigned char& level)
{
    static const size_t X_INDEX = M_MINI_MAP_COLOR_FILE_PREFIX.size();

    std::string base = basename(fileName);
    if(base.find(M_MINI_MAP_COLOR_FILE_PREFIX) != 0)
        return false;

    size_t separatorIndex = base.find(M_MINI_MAP_FILE_SEPARATOR, X_INDEX);
    assert(separatorIndex != base.npos);
    std::string xStr(&base[X_INDEX], &base[separatorIndex]);
    assert(isNumeric(xStr));

    size_t yIndex = separatorIndex + 1;
    separatorIndex = base.find(M_MINI_MAP_FILE_SEPARATOR, yIndex);
    assert(separatorIndex != base.npos);
    std::string yStr(&base[yIndex], &base[separatorIndex]);
    assert(isNumeric(yStr));

    size_t levelIndex = separatorIndex + 1;
    separatorIndex = base.find('.', levelIndex);
    assert(separatorIndex != base.npos);
    std::string levelStr(&base[levelIndex], &base[separatorIndex]);
    assert(isNumeric(levelStr));

    x = strToInt(xStr);
    y = strToInt(yStr);
    level = strToInt(levelStr);
    return true;
}

std::string MiniMap::createMiniMapColorFileName(unsigned int x, unsigned int y, unsigned char level)
{

    std::stringstream sstream;
    sstream << M_MINI_MAP_COLOR_FILE_PREFIX << x << M_MINI_MAP_FILE_SEPARATOR << y << M_MINI_MAP_FILE_SEPARATOR << (int)level << ".png";
    return sstream.str();
}

std::list<MiniMap::Tile> MiniMap::findPosAbsolute()
{
    static const std::string MINI_MAP_DIR = "C:/Users/Vendrii/Documents/programming/projects/ShankBot/monitor/tibia/packages/Tibia/minimap";
    uint64_t latestModifiedTime = getLatestModifiedFileTime(MINI_MAP_DIR);

    mInput.sendKeyCombination({VK_MENU, VK_NEXT});
    mReader.getNewFrame();
    mReader.getNewFrame();
    mReader.getNewFrame();
    mInput.sendKeyCombination({VK_MENU, VK_PRIOR});
    mReader.getNewFrame();
    mReader.getNewFrame();
    mReader.getNewFrame();
    mInput.sendKeyCombination({VK_MENU, VK_PRIOR});
    mReader.getNewFrame();
    mReader.getNewFrame();
    mReader.getNewFrame();
    mInput.sendKeyCombination({VK_MENU, VK_NEXT});
    mReader.getNewFrame();
    mReader.getNewFrame();
    mCurrentFrame = mReader.getNewFrame();

    Sleep(300);

    assert(getLatestModifiedFileTime(MINI_MAP_DIR) > latestModifiedTime);



    std::list<std::pair<uint64_t, std::string>> files = getLatestModifiedFiles(MINI_MAP_DIR, 16);
    files.remove_if([&latestModifiedTime](const std::pair<uint64_t, std::string>& pair)
    {
        return pair.first <= latestModifiedTime || pair.second.find("Minimap_Color_") == std::string::npos;
    });

    std::list<Tile> tiles;
    for(const auto& pair : files)
    {
        tiles.emplace_back();
        Tile& t = tiles.back();
        unsigned int x;
        unsigned int y;
        if(!parseMiniMapColorFileName(pair.second, x, y, t.level))
            SB_THROW("Failed to parse mini map color file name: " + pair.second);

        t.x = x;
        t.y = y;
        t.width = Constants::MINI_MAP_PIXEL_WIDTH;
        t.height = Constants::MINI_MAP_PIXEL_HEIGHT;
    }

    return tiles;
}

bool MiniMap::isTileBlack(const std::vector<unsigned char>& pixels)
{
    assert(pixels.size() == M_MINI_MAP_TILE_BYTE_SIZE);

    return memcmp(pixels.data(), M_BLACK_TILE.data(), M_MINI_MAP_TILE_BYTE_SIZE) == 0;
}


bool MiniMap::isTileMatch(const unsigned char* previous, const unsigned char* current, size_t size)
{
    assert(size == M_MINI_MAP_TILE_BYTE_SIZE);

    const uint32_t* prev = (const uint32_t*)previous;
    const uint32_t* curr = (const uint32_t*)current;


    bool isAllBlack = true;
    for(size_t i = 0; i < M_MINI_MAP_TILE_PIXEL_SIZE; i++)
    {
        if(prev[i] != M_MINI_MAP_BLACK)
        {
            isAllBlack = false;
            if(prev[i] != curr[i])
                return false;
        }
    }

    return !isAllBlack;
}

bool MiniMap::isTileMatch(const std::vector<unsigned char>& previous, const std::vector<unsigned char>& current)
{
    assert(previous.size() == current.size());
    return isTileMatch(previous.data(), current.data(), current.size());
}


unsigned int MiniMap::getX() const
{
    return mGlobalX;
}
unsigned int MiniMap::getY() const
{
    return mGlobalY;
}

unsigned char MiniMap::getLevel() const
{
    return mLevel;
}

void MiniMap::getMiniMapScreenTopLeft(float halfFrameWidth, float halfFrameHeight, const MiniMapDraw& d, unsigned short& x, unsigned short& y)
{
    float screenOffsetX;
    float screenOffsetY;
    d.getScreenCoords(halfFrameWidth, halfFrameHeight, screenOffsetX, screenOffsetY, 0, 0);
    x = screenOffsetX + 0.5f;
    y = screenOffsetY + 0.5f;
}

#include <iostream>
void MiniMap::initialize()
{
    updatePosAbsolute();
    mUpdatePosFunc = &updatePosRelative;
}

bool MiniMap::getCrosshairMiddle(const std::vector<GuiDraw>& guiDraws, float& crosshairMiddleX, float& crosshairMiddleY)
{
    for(const GuiDraw& d : guiDraws)
    {
        if(d.name.find("automap-crosshair-") != d.name.npos)
        {
            crosshairMiddleX = d.topLeft.x + (d.botRight.x - d.topLeft.x) / 2.f;
            crosshairMiddleY = d.topLeft.y + (d.botRight.y - d.topLeft.y) / 2.f;
            return true;
        }
    }

    return false;
}

void MiniMap::updatePosAbsolute()
{
    std::list<Tile> tiles;
    unsigned char level = -1;
    for(size_t i = 0; i < 10; i++)
    {
        tiles = findPosAbsolute();
        std::list<unsigned char> levels;

        for(const Tile& t : tiles)
            levels.push_back(t.level);

        levels.sort();
        levels.unique();

        assert(levels.size() <= 2);
        if(levels.size() == 1)
        {
            unsigned char otherLevel = levels.front();
            static const unsigned char MIN_LEVEL = 0;
            static const unsigned char MAX_LEVEL = 15;
//            assert(otherLevel == MIN_LEVEL + 1 || otherLevel == MAX_LEVEL - 1);
            if(otherLevel == MIN_LEVEL + 1)
            {
                level = MIN_LEVEL;
                break;
            }
            else if(otherLevel == MAX_LEVEL - 1)
            {
                level = MAX_LEVEL;
                break;
            }
            else
            {
//                SB_THROW("Unexpected level: ", (int)otherLevel);
            }
        }
        else if(levels.size() == 2)
        {
            unsigned char dLevel = levels.back() - levels.front();
            if(dLevel != 2)
            {
//                SB_THROW("Unepected level delta: ", (int)dLevel);
            }
            else
            {
                level = levels.front() + 1;
                break;
            }
        }
        else
        {
//            SB_THROW("Unexpected levels count: ", levels.size());

        }

        centreAndUpdate();
    }
    if(level == -1)
    {
        SB_THROW("Failed to determine position by minimap. Probably because a user is screwing around with the window. Please stop.");
    }

    std::list<Tile> drawnTiles = getDrawnTiles();
    assert(!drawnTiles.empty());

    auto currentTileIt = getCurrentTile(drawnTiles);
    assert(currentTileIt != drawnTiles.end());

    Tile currentTile = *currentTileIt;

    int globalLeft = std::numeric_limits<int>::max();
    int globalTop = std::numeric_limits<int>::max();
    for(const Tile& t : tiles)
    {
        if(t.x < globalLeft)
            globalLeft = t.x;
        if(t.y < globalTop)
            globalTop = t.y;
    }
    int drawLeft = std::numeric_limits<int>::max();
    int drawTop = std::numeric_limits<int>::max();
    for(const Tile& t : drawnTiles)
    {
        if(t.x < drawLeft)
            drawLeft = t.x;
        if(t.y < drawTop)
            drawTop = t.y;
    }

    mGlobalLeft = currentTile.x + (globalLeft - drawLeft);
    mGlobalTop = currentTile.y + (globalTop - drawTop);
    mGlobalX = mGlobalLeft + currentTile.dCrosshairX;
    mGlobalY = mGlobalTop + currentTile.dCrosshairY;
    mLevel = level;
    mPixels = currentTile.pixels;

    mCurrentTile = currentTile;
    tiles.erase(currentTileIt);
    mOtherTiles = tiles;
}

std::list<MiniMap::Tile>::iterator MiniMap::getCurrentTile(std::list<Tile>& tiles)
{
    auto it = tiles.begin();
    while(it != tiles.end())
    {
        if(it->dCrosshairX >= 0 && it->dCrosshairY >= 0 && it->dCrosshairX < it->widthInTiles && it->dCrosshairY < it->heightInTiles)
            return it;

        it++;
    }

    return it;
}

bool MiniMap::handleLevelChange2()
{
    static const std::vector<std::pair<int, int>> POSSIBLE_TILE_OFFSETS =
    {
        {0, 0},
        {Constants::MINI_MAP_PIXEL_WIDTH, 0},
        {-Constants::MINI_MAP_PIXEL_WIDTH, 0},
        {0, Constants::MINI_MAP_PIXEL_HEIGHT},
        {0, -Constants::MINI_MAP_PIXEL_HEIGHT},
        {Constants::MINI_MAP_PIXEL_WIDTH, Constants::MINI_MAP_PIXEL_HEIGHT},
        {-Constants::MINI_MAP_PIXEL_WIDTH, Constants::MINI_MAP_PIXEL_HEIGHT},
        {Constants::MINI_MAP_PIXEL_WIDTH, -Constants::MINI_MAP_PIXEL_HEIGHT},
        {-Constants::MINI_MAP_PIXEL_WIDTH, -Constants::MINI_MAP_PIXEL_HEIGHT}
    };


    mCurrentFrame = mReader.getNewFrame();
    mCurrentFrame = mReader.getNewFrame();
    mCurrentFrame = mReader.getNewFrame();
    std::list<Tile> tiles = getDrawnTiles();
    assert(!tiles.empty());

    auto currentTileIt = getCurrentTile(tiles);
    assert(currentTileIt != tiles.end());

    Tile currentTile = *currentTileIt;
    tiles.erase(currentTileIt);


    for(const auto& pair : POSSIBLE_TILE_OFFSETS)
    {
        for(unsigned char level = mLevel - 1; level <= mLevel + 1; level += 2)
        {
            unsigned int x = mGlobalLeft + pair.first;
            unsigned int y = mGlobalTop + pair.second;
            static const std::string DIRECTORY = "C:/Users/Vendrii/Documents/programming/projects/ShankBot/monitor/tibia/packages/Tibia/minimap/";
            std::string path = DIRECTORY + createMiniMapColorFileName(x, y, level);
            QImage img(QString::fromStdString(path));
            if(!img.isNull())
            {
                assert(img.depth() == 8);
                assert(img.format() == QImage::Format_Indexed8);
                assert(img.byteCount() == M_MINI_MAP_TILE_PIXEL_SIZE);

                img = img.convertToFormat(QImage::Format_ARGB32);

                assert(img.byteCount() == currentTile.pixels->size());
                if(isTileMatch(img.constBits(), currentTile.pixels->data(), currentTile.pixels->size()))
                {
                    mGlobalLeft = x;
                    mGlobalTop = y;
                    mLevel = level;
                    mGlobalX = mGlobalLeft + currentTile.dCrosshairX;
                    mGlobalY = mGlobalTop + currentTile.dCrosshairY;
                    mPixels = currentTile.pixels;

                    mCurrentTile = currentTile;
                    mOtherTiles = tiles;
                    return true;
                }
            }

        }
    }
    return false;
}

bool MiniMap::handleLevelChange(const Tile& currentTile)
{
    mReader.getNewFrame();
    mReader.getNewFrame();
    mInput.sendKeyCombination({VK_MENU, VK_NEXT});
    mReader.getNewFrame();
    mReader.getNewFrame();
    mReader.getNewFrame();
    mCurrentFrame = mReader.getNewFrame();
    mInput.sendKeyCombination({VK_MENU, VK_PRIOR});
    mReader.getNewFrame();
    mReader.getNewFrame();

    std::list<Tile> tiles = getDrawnTiles();
    auto currentTileIt = getCurrentTile(tiles);
    assert(currentTileIt != tiles.end());
    Tile currTile = *currentTileIt;
    tiles.erase(currentTileIt);

    if(handleMatch(tiles, currTile))
    {
        mPixels = currentTile.pixels;
        mLevel--;
        return true;
    }


    mInput.sendKeyCombination({VK_MENU, VK_PRIOR});
    mReader.getNewFrame();
    mReader.getNewFrame();
    mReader.getNewFrame();
    mCurrentFrame = mReader.getNewFrame();
    mInput.sendKeyCombination({VK_MENU, VK_NEXT});
    mReader.getNewFrame();
    mReader.getNewFrame();

    tiles = getDrawnTiles();
    currentTileIt = getCurrentTile(tiles);
    assert(currentTileIt != tiles.end());
    currTile = *currentTileIt;
    tiles.erase(currentTileIt);

    if(handleMatch(tiles, currTile))
    {
        mPixels = currentTile.pixels;
        mLevel++;
        return true;
    }




    return false;
}

std::list<MiniMap::Tile> MiniMap::getDrawnTiles() const
{
    std::list<Tile> tiles;
    if(mCurrentFrame.miniMapDraws->empty())
        return tiles;

    const auto& miniMapDraws = *mCurrentFrame.miniMapDraws;

    unsigned int tileWidth = miniMapDraws.front().botRight.x - miniMapDraws.front().topLeft.x;
    float tilesPerPixel = float(Constants::MINI_MAP_PIXEL_WIDTH) / float(tileWidth);
    float crosshairX = mCrosshairDrawX;
    float crosshairY = mCrosshairDrawY;
    for(const MiniMapDraw& d : miniMapDraws)
    {
        #ifndef NDEBUG
            unsigned short screenX;
            unsigned short screenY;
            getMiniMapScreenTopLeft(float(mCurrentFrame.width) / 2.f, float(mCurrentFrame.height) / 2.f, d, screenX, screenY);
            assert(mScreenX == screenX);
            assert(mScreenY == screenY);
        #endif // NDEBUG

        tiles.emplace_back();
        Tile& t = tiles.back();
        t.x = d.topLeft.x;
        t.y = d.topLeft.y;
        assert(tileWidth == d.botRight.x - d.topLeft.x);
        assert(tileWidth == d.botRight.y - d.topLeft.y);
        t.width = tileWidth;
        t.height = tileWidth;
        t.widthInTiles = t.width * tilesPerPixel;
        t.heightInTiles = t.height * tilesPerPixel;
        t.dCrosshairX = std::ceil((crosshairX - d.topLeft.x) * tilesPerPixel - 1.f);
        t.dCrosshairY = std::ceil((crosshairY - d.topLeft.y) * tilesPerPixel - 1.f);
        t.pixels = d.pixels;
    }

    return tiles;
}

bool MiniMap::handleMatch(const std::list<Tile>& otherTiles, const Tile& currentTile)
{
    if(isTileMatch(*mPixels, *currentTile.pixels))
    {
        mPixels = currentTile.pixels;
        mGlobalX = mGlobalLeft + currentTile.dCrosshairX;
        mGlobalY = mGlobalTop + currentTile.dCrosshairY;

        mCurrentTile = currentTile;
        mOtherTiles = otherTiles;
        return true;
    }


    for(const Tile& t : otherTiles)
    {
        assert(t.pixels != nullptr);
        if(isTileMatch(*mPixels, *t.pixels))
        {
            mGlobalLeft += currentTile.x - t.x;
            mGlobalTop += currentTile.y - t.y;
            mGlobalX = mGlobalLeft + t.dCrosshairX;
            mGlobalY = mGlobalTop + t.dCrosshairY;
            mPixels = t.pixels;

            mCurrentTile = currentTile;
            mOtherTiles = otherTiles;
            return true;
        }
    }

    return false;
}
void MiniMap::update(const Frame& frame)
{
    if(frame.miniMapDraws->empty())
        return;
    mCurrentFrame = frame;
    getMiniMapScreenTopLeft(float(mCurrentFrame.width) / 2.f, float(mCurrentFrame.height) / 2.f, mCurrentFrame.miniMapDraws->front(), mScreenX, mScreenY);

    float crosshairX;
    float crosshairY;
    mIsCrosshairDrawn = getCrosshairMiddle(*mCurrentFrame.guiDraws, crosshairX, crosshairY);

    if(!mIsCrosshairDrawn)
        return;

    mCrosshairDrawX = crosshairX;
    mCrosshairDrawY = crosshairY;


    float tileWidth = mCurrentFrame.miniMapDraws->front().botRight.x - mCurrentFrame.miniMapDraws->front().topLeft.x;
    float tileHeight = mCurrentFrame.miniMapDraws->front().botRight.y - mCurrentFrame.miniMapDraws->front().topLeft.y;
    assert(tileWidth == tileHeight);
    if(tileWidth == 768)
    {
        mDefaultCrosshairX = M_DEFAULT_CROSSHAIR_MIDDLE_X_MAX_ZOOM;
        mDefaultCrosshairY = M_DEFAULT_CROSSHAIR_MIDDLE_Y_MAX_ZOOM;
    }
    else
    {
        mDefaultCrosshairX = M_DEFAULT_CROSSHAIR_MIDDLE_X;
        mDefaultCrosshairY = M_DEFAULT_CROSSHAIR_MIDDLE_Y;
    }

    mPixelsPerTile = tileWidth / float(Constants::MINI_MAP_PIXEL_WIDTH);


    (this->*mUpdatePosFunc)();

}

void MiniMap::updatePosRelative()
{
    if(mCrosshairDrawX != mDefaultCrosshairX || mCrosshairDrawY != mDefaultCrosshairY)
//    (!(
//        (mCrosshairDrawX == M_DEFAULT_CROSSHAIR_MIDDLE_X && mCrosshairDrawY == M_DEFAULT_CROSSHAIR_MIDDLE_Y) ||
//        (mCrosshairDrawX == M_DEFAULT_CROSSHAIR_MIDDLE_X_MAX_ZOOM && mCrosshairDrawY == M_DEFAULT_CROSSHAIR_MIDDLE_Y_MAX_ZOOM)
//    ))
    {
        // Map is being moved around.
        // This means we aren't moving.
        // Do nothing this frame.
        return;
    }

    std::list<Tile> tiles = getDrawnTiles();
    assert(!tiles.empty());


    auto currentTileIt = getCurrentTile(tiles);
    Tile currentTile = *currentTileIt;

    assert(currentTileIt != tiles.end());
    if(currentTileIt->pixels == mPixels)
        return;

    if(isTileBlack(*currentTileIt->pixels))
        return;

    tiles.erase(currentTileIt);


    if(handleMatch(tiles, currentTile))
        return;


    if(handleLevelChange2())
        return;


    if(handleLevelChange(currentTile))
        return;



    updatePosAbsolute();



    QImage img(currentTile.pixels->data(), Constants::MINI_MAP_PIXEL_WIDTH, Constants::MINI_MAP_PIXEL_HEIGHT, QImage::Format_ARGB32);
    static size_t count = 0;
    img.save(QString::fromStdString(stringify("miniMapDebug/", mGlobalLeft, "_", mGlobalTop, "_", (int)mLevel, "_", count, ".png")));
}
#include <iostream>

void MiniMap::centreAndUpdate()
{
    for(size_t i = 0; i < 10; i++)
    {
        mInput.sendKeyCombination({VK_MENU, 'C'});
        bool doBreak = false;
        for(size_t j = 0; j < 10; j++)
        {
            update(mReader.getNewFrame());
            if(mIsCrosshairDrawn)
            {
                doBreak = true;
                break;
            }
        }
        if(doBreak)
            break;
    }

    float prevPixPerTile = mPixelsPerTile;
    char levelSwitchDirection = mPixelsPerTile < 1 ? VK_END : VK_HOME;
    for(size_t i = 0; i < 10; i++)
    {
        if(mPixelsPerTile == 1)
            return;

        mInput.sendKeyCombination({VK_MENU, levelSwitchDirection});
        for(size_t j = 0; j < 10; j++)
        {
            update(mReader.getNewFrame());
            if(prevPixPerTile != mPixelsPerTile)
            {
                prevPixPerTile = mPixelsPerTile;
                break;
            }
        }
    }


    std::cout << "Failed to centre and update. :-(" << std::endl;
}


void MiniMap::globalCoordsToLocalDrawCoords(unsigned int x, unsigned int y, int& drawX, int& drawY)
{
    if(!mIsCrosshairDrawn || mPixelsPerTile != 1)
        centreAndUpdate();

    int dX = int(x) - int(mGlobalX);
    int dY = int(y) - int(mGlobalY);

    drawX = mCrosshairDrawX + std::ceil(float(dX) * mPixelsPerTile - 1.f);
    drawY = mCrosshairDrawY + std::ceil(float(dY) * mPixelsPerTile - 1.f);
}



Object::MiniMapColor MiniMap::getColor(unsigned int x, unsigned int y)
{
    int drawX;
    int drawY;
    globalCoordsToLocalDrawCoords(x, y, drawX, drawY);
    drawX -= (mCrosshairDrawX - mDefaultCrosshairX);
    drawY -= (mCrosshairDrawY - mDefaultCrosshairY);
    static auto tileContainsPoint = [](int px, int py, const MiniMapDraw& d)
    {
        return px >= d.topLeft.x && py >= d.topLeft.y && px < d.botRight.x && py < d.botRight.y;
    };

    int dDrawX;
    int dDrawY;
    const MiniMapDraw* tile = nullptr;

    for(const MiniMapDraw& d : *mCurrentFrame.miniMapDraws)
    {
        if(tileContainsPoint(drawX, drawY, d))
        {
            dDrawX = drawX - d.topLeft.x;
            dDrawY = drawY - d.topLeft.y;
            tile = &d;
            break;
        }
    }

    if(!tile)
        return Object::MiniMapColor::INVALID;

    assert(dDrawX >= 0 && dDrawY >= 0);

    size_t index = float(dDrawX) / mPixelsPerTile + (float(dDrawY) / mPixelsPerTile) * Constants::MINI_MAP_PIXEL_WIDTH;
    index *= BYTES_PER_PIXEL_RGBA;
    assert(index < tile->pixels->size() - (BYTES_PER_PIXEL_RGBA - 1));

    GraphicsLayer::Color color;
    const auto& pixels = *tile->pixels;
    color.r = pixels[index + 2];
    color.g = pixels[index + 1];
    color.b = pixels[index];
    color.a = pixels[index + 3];

    auto colorIt = RGBA_TO_MINI_MAP_COLOR.find(color.packed);
    if(colorIt == RGBA_TO_MINI_MAP_COLOR.end())
        std::cout << "Warning: " << (stringify("Failed to find mini map color in map: (", (int)color.r, ", ", (int)color.g, ", ", (int)color.b, ", ", (int)color.a, ") at ", x, "x", y));
    return colorIt->second;
}

void MiniMap::getScreenCoords(unsigned int x, unsigned int y, unsigned short& screenX, unsigned short& screenY)
{
    int drawX;
    int drawY;
    globalCoordsToLocalDrawCoords(x, y, drawX, drawY);
    screenX = drawX + mScreenX;
    screenY = drawY + mScreenY;
}

void MiniMap::goTo(unsigned int x, unsigned int y)
{
    int drawX;
    int drawY;
    globalCoordsToLocalDrawCoords(x, y, drawX, drawY);

    int dX = 0;
    if(drawX < 0)
        dX = drawX;
    else if(drawX >= M_MINI_MAP_SCREEN_WIDTH)
        dX = drawX - (M_MINI_MAP_SCREEN_WIDTH - 1);

    int dY = 0;
    if(drawY < 0)
        dY = drawY;
    else if(drawY >= M_MINI_MAP_SCREEN_HEIGHT)
        dY = drawY - (M_MINI_MAP_SCREEN_HEIGHT - 1);

    if(dX == 0 && dY == 0)
    {
        mInput.sendMouseClick(VK_LBUTTON, drawX + mScreenX, drawY + mScreenY);
        return;
    }

    if(dX < 0)
        mInput.sendKeyCombination({VK_MENU, VK_LEFT});
    else if(dX > 0)
        mInput.sendKeyCombination({VK_MENU, VK_RIGHT});
    else if(dY < 0)
        mInput.sendKeyCombination({VK_MENU, VK_UP});
    else
        mInput.sendKeyCombination({VK_MENU, VK_DOWN});


    update(mReader.getNewFrame());
    goTo(x, y);
}
