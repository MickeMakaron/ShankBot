// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016 Mikael Hernvall
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
#ifndef GRAPHICS_LAYER_MINI_MAP_HPP
#define GRAPHICS_LAYER_MINI_MAP_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "Frame.hpp"
namespace GraphicsLayer
{
    class Input;
    class GraphicsMonitorReader;
}
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <memory>
#include <map>
///////////////////////////////////

namespace GraphicsLayer
{
    class MiniMap
    {
        private:
            struct Tile
            {
                int x = 0;
                int y = 0;
                unsigned char level = 0;
                unsigned int width = 0;
                unsigned int height = 0;
                unsigned int widthInTiles = 0;
                unsigned int heightInTiles = 0;
                std::shared_ptr<std::vector<unsigned char>> pixels;
                int dCrosshairX = 0;
                int dCrosshairY = 0;
            };

        public:
            explicit MiniMap(const Input& input, GraphicsMonitorReader& reader);

            void update(const Frame& frame);

            unsigned int getX() const;
            unsigned int getY() const;
            unsigned char getLevel() const;

            Object::MiniMapColor getColor(unsigned int x, unsigned int y);
            void getScreenCoords(unsigned int x, unsigned int y, unsigned short& screenX, unsigned short& screenY);
            void goTo(unsigned int x, unsigned int y);


        private:
            static bool parseMiniMapColorFileName(const std::string& fileName, unsigned int& x, unsigned int& y, unsigned char& level);
            static std::string createMiniMapColorFileName(unsigned int x, unsigned int y, unsigned char level);
            static bool getCrosshairMiddle(const std::vector<GuiDraw>& guiDraws, float& crosshairMiddleX, float& crosshairMiddleY);
            static bool isTileMatch(const unsigned char* previous, const unsigned char* current, size_t size);
            static bool isTileMatch(const std::vector<unsigned char>& previous, const std::vector<unsigned char>& current);
            static bool isTileBlack(const std::vector<unsigned char>& pixels);
            static void getMiniMapScreenTopLeft(float halfFrameWidth, float halfFrameHeight, const MiniMapDraw& d, unsigned short& x, unsigned short& y);
            std::list<Tile> getDrawnTiles() const;
            static std::list<Tile>::iterator getCurrentTile(std::list<Tile>& tiles);
            bool handleLevelChange2();
            bool handleLevelChange(const Tile& currentTile);
            bool handleMatch(const std::list<Tile>& otherTiles, const Tile& currentTile);
            void updatePosRelative();
            void updatePosAbsolute();
            void initialize();
            std::list<Tile> findPosAbsolute();
            void globalCoordsToLocalDrawCoords(unsigned int x, unsigned int y, int& drawX, int& drawY);
            void centreAndUpdate();

        private:
            const Input& mInput;
            GraphicsMonitorReader& mReader;
            std::shared_ptr<std::vector<unsigned char>> mPixels;
            unsigned int mGlobalLeft;
            unsigned int mGlobalTop;
            unsigned int mGlobalX;
            unsigned int mGlobalY;
            unsigned char mLevel;

            unsigned short mScreenX;
            unsigned short mScreenY;
            int mCrosshairDrawX;
            int mCrosshairDrawY;
            int mDefaultCrosshairX;
            int mDefaultCrosshairY;
            bool mIsCrosshairDrawn = false;
            Tile mCurrentTile;
            std::list<Tile> mOtherTiles;

            Frame mCurrentFrame;
            float mPixelsPerTile;

            void (MiniMap::* mUpdatePosFunc)() = &initialize;


            static const uint32_t M_MINI_MAP_BLACK;
            static const size_t M_MINI_MAP_TILE_PIXEL_SIZE;
            static const size_t M_MINI_MAP_TILE_BYTE_SIZE;
            static const std::string M_MINI_MAP_COLOR_FILE_PREFIX;
            static const char M_MINI_MAP_FILE_SEPARATOR = '_';
            static const int M_DEFAULT_CROSSHAIR_MIDDLE_X = 53;
            static const int M_DEFAULT_CROSSHAIR_MIDDLE_Y = 53;
            static const int M_DEFAULT_CROSSHAIR_MIDDLE_X_MAX_ZOOM = 54;
            static const int M_DEFAULT_CROSSHAIR_MIDDLE_Y_MAX_ZOOM = 54;
            static const size_t M_MINI_MAP_SCREEN_WIDTH = 105;
            static const size_t M_MINI_MAP_SCREEN_HEIGHT = 105;
            static const std::vector<uint32_t> M_BLACK_TILE;
            static const std::map<int, Object::MiniMapColor> RGBA_TO_MINI_MAP_COLOR;
    };
}


#endif // GRAPHICS_LAYER_MINI_MAP_HPP
