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
#include "monitor/tibiaIoUtility.hpp"
#include "monitor/GraphicsMonitorReader.hpp"
#include "monitor/TibiaClient.hpp"
#include "monitor/Input.hpp"
#include "monitor/TextBuilder.hpp"

///////////////////////////////////

///////////////////////////////////
// STD C++
#include <algorithm>
#include <iostream>
#include <cassert>
///////////////////////////////////

namespace GraphicsLayer
{

std::vector<std::string> getContextMenu(GraphicsMonitorReader& reader, unsigned short screenX, unsigned short screenY)
{
    const Input& input = reader.getClient().getInput();
    input.sendMouseClick(MK_RBUTTON, screenX, screenY);
    std::cout << "Sent ctx menu request at " << screenX << "x" << screenY << std::endl;

    GraphicsLayer::Frame frame = reader.getNewFrame();
    frame = reader.getNewFrame();
//    const std::list<std::string> KNOWN_CONTEXT_MENU_ITEMS =
//    {
//       "Look",
//       "Use",
//       "Browse Field",
//       "Copy Name",
//       "Attack",
//       "Follow",
//       "Message to",
//       "to VIP list",
//       "Ignore",
//       "to party",
//       "Report Name",
//       "Report Bot/Macro",
//       "Set Outfit",
//       "Mount"
//    };
    std::vector<std::string> contextMenu;
    std::for_each(frame.textDraws->begin(), frame.textDraws->end(), [&](const TextDraw& d)
    {
        TextBuilder builder(d, frame.width, frame.height);
        if(builder.getTextType() == Text::Type::CONTEXT_MENU)
        {
            for(const Text& t : builder.getText())
                contextMenu.push_back(t.string);
        }
//        if(d.r == 247 && d.g == 247 && d.b == 247)
//        {
//            auto match = std::find_if(KNOWN_CONTEXT_MENU_ITEMS.begin(), KNOWN_CONTEXT_MENU_ITEMS.end(), [&d](const std::string& str)
//            {
//                return d.string.find(str) != d.string.npos;
//            });
//
//            if(match != KNOWN_CONTEXT_MENU_ITEMS.end())
//            {
//                contextMenu.push_back(*match);
//            }
//        }
    });
    assert(!contextMenu.empty());

    input.sendMouseClick(MK_LBUTTON, screenX, screenY);





//     Context menu:
//     Ground:
//          Look, Use, Browse Field
//     IF ON SAME LEVEL:
//          Outfit:
//              (Ground) + Copy Name
//          NPC:
//              (Outfit) + Attack, Follow
//          Other player:
//              (Outfit) + Message to {NAME}, Add {NAME} to VIP list, Ignore {NAME}, Invite {NAME} to party,
//                         Report Name, Report Bot/Macro
//          Creature:
//              (NPC)
//     ELSE
//          *:
//              (Ground)
//
//     Self:
//          (Outfit) + Set Outfit, Mount
//
//     GREEN TEXT:
//          Self:
//              You see yourself. You are a/an {VOC}.
//          Other player:
//              You see {NAME} (Level {LVL}). He/She is a/an {VOC}.
//          Creature/NPC:
//              You see  // Nothing useful

    return contextMenu;
}


std::string assembleGreenText(const Frame& frame)
{
    std::string greenText;
    for(const TextDraw& draw : *frame.textDraws)
    {
        TextBuilder builder(draw, frame.width, frame.height);
        if(builder.getTextType() == Text::Type::GREEN_TEXT)
        {
            std::vector<Text> texts = builder.getText();
            for(const Text& text : texts)
                greenText.append(text.string);
        }
    }

    return greenText;
}

void clearGreenText(const Input& input, GraphicsMonitorReader& reader)
{
    std::string greenText;
    do
    {
        input.sendKeyCombination({VK_CONTROL, 'W'});
        greenText = assembleGreenText(reader.getNewFrame());

    } while(greenText != "");
}

std::string getGreenText(GraphicsMonitorReader& reader, unsigned short screenX, unsigned short screenY)
{
    const Input& input = reader.getClient().getInput();
    clearGreenText(input, reader);

    while(true)
    {
        input.sendMouseClick(MK_LBUTTON, screenX, screenY, MK_SHIFT);
        reader.getNewFrame();
        for(size_t numTries = 0; numTries < 100; numTries++)
        {
            Sleep(50);
            std::string greenText = assembleGreenText(reader.getNewFrame());
            if(greenText != "")
                return greenText;
        }
    }
    THROW_RUNTIME_ERROR("Green text request timed out. No green text appeared.");

}

}
