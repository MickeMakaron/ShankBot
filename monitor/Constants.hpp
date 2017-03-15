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
#ifndef GRAPHICS_LAYER_CONSTANTS_HPP
#define GRAPHICS_LAYER_CONSTANTS_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "tibiaassets/constants.hpp"
#include "utility/Color.hpp"
///////////////////////////////////


namespace GraphicsLayer
{
    namespace Constants
    {
        const unsigned char TILE_PIXEL_WIDTH = sb::tibiaassets::constants::TILE_PIXEL_WIDTH;
        const unsigned char TILE_PIXEL_HEIGHT = sb::tibiaassets::constants::TILE_PIXEL_HEIGHT;

        const unsigned char CONTAINER_SEPARATOR_PIXEL_WIDTH = 5;
        const unsigned char CONTAINER_WIDTH = 4;


        const int SIDE_BOTTOM_WINDOW_CLIENT_SCREEN_LEFT_OFFSET_X = 172; // Offset from the right side of the windows' client area.
        const int SIDE_BOTTOM_WINDOW_CLIENT_SCREEN_RIGHT_OFFSET_X = 4;
        const int SIDE_BOTTOM_WINDOW_CLIENT_WIDTH = SIDE_BOTTOM_WINDOW_CLIENT_SCREEN_LEFT_OFFSET_X - SIDE_BOTTOM_WINDOW_CLIENT_SCREEN_RIGHT_OFFSET_X + 1;
        const int SIDE_BOTTOM_WINDOW_ICON_LOCAL_X = 4;
        const int BATTLE_WINDOW_SPRITE_LOCAL_X = 1;
        const int BATTLE_WINDOW_NAME_LOCAL_X = 21;

        const int CONTAINER_FIRST_SLOT_LOCAL_X = 8;
        const int CONTAINER_FIRST_SLOT_LOCAL_Y = 5;
        const int NPC_TRADE_LIST_SLOT_LOCAL_X = 3;
        const int NPC_TRADE_LIST_FIRST_SLOT_LOCAL_Y = 2;
        const int NPC_TRADE_LIST_SEPARATOR_PIXEL_WIDTH = CONTAINER_SEPARATOR_PIXEL_WIDTH;
        const int NPC_TRADE_CURRENT_SLOT_LOCAL_X = 130;

        const unsigned char NUM_EQ_SLOTS = 10;

        const int NUM_TILES_VIEW_X = 15;
        const int NUM_TILES_VIEW_Y = 11;
        const unsigned short VIEW_PIXEL_WIDTH = TILE_PIXEL_WIDTH * NUM_TILES_VIEW_X;
        const unsigned short VIEW_PIXEL_HEIGHT = TILE_PIXEL_HEIGHT * NUM_TILES_VIEW_Y;
        const int MIDDLE_TILE_X = NUM_TILES_VIEW_X / 2;
        const int MIDDLE_TILE_Y = NUM_TILES_VIEW_Y / 2;

        const unsigned int MINI_MAP_PIXEL_WIDTH = 256;
        const unsigned int MINI_MAP_PIXEL_HEIGHT = 256;

        const unsigned int SPRITE_ID_START = 0;
        const unsigned int SPRITE_ID_END = SPRITE_ID_START + 1000000;
        const unsigned int GRAPHICS_RESOURCE_ID_START = SPRITE_ID_END + 1;
        const unsigned int GRAPHICS_RESOURCE_ID_END = GRAPHICS_RESOURCE_ID_START + 10000;
        const unsigned int COMBAT_SQUARE_ID_START = GRAPHICS_RESOURCE_ID_END + 1;
        const unsigned int COMBAT_SQUARE_ID_END = COMBAT_SQUARE_ID_START + 20;

        enum class TextColor : int
        {
            SERVER_BROADCAST = Color({248, 96, 96, 255}).packed,
            BROADCAST = Color({240, 240, 240, 255}).packed,
            GREEN_TEXT = Color({0, 240, 0, 255}).packed,
            CHAT_TAB_INACTIVE = Color({127, 127, 127, 255}).packed,
            CHAT_TAB_ACTIVE = Color({223, 223, 223, 255}).packed,
            CHAT_TAB_UNREAD = Color({247, 95, 95, 255}).packed,
            CHAT_TAB_NEW_MESSAGE = Color({247, 247, 247, 255}).packed,
            INFO_TEXT = Color({255, 255, 255, 255}).packed,
            SAY = Color({240, 240, 0, 255}).packed,
            PM = Color({96, 248, 248, 255}).packed,
            SELF_PM = Color({160, 160, 255, 255}).packed,
            SELF_PM_INPUT = Color({159, 159, 254, 255}).packed,
            CLICKABLE_NPC_TEXT = Color({32, 160, 255, 255}).packed,
            SOUND = Color({255, 102, 0, 255}).packed,
            ITEM_STACK_COUNT = Color({191, 191, 191, 255}).packed,
            HP_GRAY = Color({192, 192, 192, 255}).packed,
            HP_FULL = Color({0, 192, 0, 255}).packed,
            HP_LIGHT_GREEN = Color({96, 192, 96, 255}).packed,
            HP_YELLOW = Color({192, 192, 0, 255}).packed,
            HP_LIGHT_RED = Color({192, 48, 48, 255}).packed,
            HP_RED = Color({192, 0, 0, 255}).packed,
            HP_DARK_RED = Color({96, 0, 0, 255}).packed,
            GUI = Color({144, 144, 144, 255}).packed,
            DISABLED_LIST_ENTRY = Color({112, 112, 112, 255}).packed,
            SELECTED_TEXT = Color({244, 244, 244, 255}).packed,
            XP_GAIN_RATE_FULL = Color({68, 173, 37, 255}).packed,
            TOOLTIP = Color({63, 63, 63, 255}).packed,
            HOTKEY_TEXT = Color({175, 175, 175, 255}).packed,
            HOTKEY_USE_ON_SELF = Color({176, 255, 176, 255}).packed,
            HOTKEY_USE_ON_TARGET = Color({255, 176, 176, 255}).packed,
            HOTKEY_USE_WITH_CROSSHAIR = Color({200, 125, 125, 255}).packed,
            HOTKEY_EQUIP = Color({191, 191, 0, 255}).packed,
            STORE_NEW = Color({247, 175, 72, 255}).packed,
            VIP_OFFLINE = Color({248, 96, 96, 255}).packed,
            VIP_ONLINE = Color({96, 248, 96, 255}).packed,
            ADD_NEW_HOTKEY_ENTRY = Color({255, 0, 0, 255}).packed,
            CONTEXT_MENU = CHAT_TAB_NEW_MESSAGE,
            NAME_BATTLE_WINDOW_HIGHLIGHTED = CHAT_TAB_NEW_MESSAGE,
        };

        enum class RectColor : int
        {
            HP_GRAY = (int)TextColor::HP_GRAY,
            HP_FULL = (int)TextColor::HP_FULL,
            HP_LIGHT_GREEN = (int)TextColor::HP_LIGHT_GREEN,
            HP_YELLOW = (int)TextColor::HP_YELLOW,
            HP_LIGHT_RED = (int)TextColor::HP_LIGHT_RED,
            HP_RED = (int)TextColor::HP_RED,
            HP_DARK_RED = (int)TextColor::HP_DARK_RED,
            TEXT_INPUT_FIELD = Color({54, 54, 54, 255}).packed,
            TEXT_SELECTION_BOX = Color({128, 128, 128, 255}).packed,
            LIST_MENU_BACKGROUND = Color({64, 64, 64, 255}).packed,
            LIST_MENU_SELECTED_ENTRY_BOX = Color({88, 88, 88, 255}).packed,
            SKILL_BAR_GREEN_FULL = Color({0, 255, 0, 255}).packed,
            HP_BACKGROUND = Color({0, 0, 0, 255}).packed,
            TEXT_CURSOR_CHAT = (int)TextColor::SELECTED_TEXT,
            HP_FULL_BATTLE_LIST = HP_FULL,
            HP_LIGHT_GREEN_BATTLE_LIST = HP_LIGHT_GREEN,
            HP_YELLOW_BATTLE_LIST = HP_YELLOW,
            HP_LIGHT_RED_BATTLE_LIST = HP_LIGHT_RED,
            HP_RED_BATTLE_LIST = HP_RED,
            HP_DARK_RED_BATTLE_LIST = HP_DARK_RED,
            SCENE_CLEAR_COLOR = HP_BACKGROUND,
            GUI_TEXT_UNDERLINE = HP_GRAY,
            TEXT_CURSOR_MENUS = HP_GRAY,
            SKILL_BAR_RED = HP_RED,
            SKILL_BAR_GREEN = HP_FULL,
            LIST_MENU_UNSELECTED_ENTRY_BOX = LIST_MENU_BACKGROUND,
        };
    }
}

#endif // GRAPHICS_LAYER_CONSTANTS_HPP
