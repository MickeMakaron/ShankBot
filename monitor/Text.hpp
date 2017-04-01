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
#ifndef GRAPHICS_LAYER_TEXT_HPP
#define GRAPHICS_LAYER_TEXT_HPP

///////////////////////////////////
// STD C++
#include <string>
///////////////////////////////////

namespace GraphicsLayer
{
    struct Text
    {
        enum class Type : int
        {
            UNKNOWN,
            GUI,
            DISABLED_LIST_ENTRY,
            TOOLTIP,
            SELECTED_TEXT,
            NAME,
            NAME_OBSCURED,
            CHAT_TAB,
            CONTEXT_MENU,
            SERVER_BROADCAST,
            BROADCAST,
            GREEN_TEXT,
            INFO_TEXT,
            SAY,
            PM,
            SELF_PM,
            CLICKABLE_NPC_TEXT,
            SOUND,
            ITEM_STACK_COUNT,
            HOTKEY_TEXT,
            HOTKEY_USE_ON_SELF,
            HOTKEY_USE_ON_TARGET,
            HOTKEY_USE_WITH_CROSSHAIR,
            HOTKEY_EQUIP,
            STORE_NEW,
            VIP_OFFLINE,
            VIP_ONLINE,
            VIP_GROUP,
            ADD_NEW_HOTKEY_ENTRY,
            NAME_BATTLE_WINDOW_HIGHLIGHTED
        };

        float x = 0;
        float y = 0;
        float localX = 0;
        float localY = 0;
        float width = 0;
        float height = 0;
        std::string string;
        Type type;
    };
}

#endif // GRAPHICS_LAYER_TEXT_HPP
