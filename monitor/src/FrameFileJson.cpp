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
#include "monitor/FrameFileJson.hpp"
#include "monitor/Frame.hpp"
#include "utility/utility.hpp"
#include "monitor/TextBuilder.hpp"
#include "monitor/TibiaContext.hpp"
using namespace sb::utility;
///////////////////////////////////

///////////////////////////////////
// Qt
#include "QtCore/QJsonDocument"
#include "QtCore/QJsonObject"
#include "QtCore/QJsonArray"
#include "QtCore/QFile"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cassert>
///////////////////////////////////

namespace GraphicsLayer
{
namespace FrameFileJson
{

QJsonValue toJson(Text::Type t)
{
    using T = Text::Type;
    switch(t)
    {
        case T::UNKNOWN: return "UNKNOWN";
        case T::GUI: return "GUI";
        case T::DISABLED_LIST_ENTRY: return "DISABLED_LIST_ENTRY";
        case T::TOOLTIP: return "TOOLTIP";
        case T::SELECTED_TEXT: return "SELECTED_TEXT";
        case T::NAME: return "NAME";
        case T::NAME_OBSCURED: return "NAME_OBSCURED";
        case T::CHAT_TAB: return "CHAT_TAB";
        case T::CONTEXT_MENU: return "CONTEXT_MENU";
        case T::SERVER_BROADCAST: return "SERVER_BROADCAST";
        case T::BROADCAST: return "BROADCAST";
        case T::GREEN_TEXT: return "GREEN_TEXT";
        case T::INFO_TEXT: return "INFO_TEXT";
        case T::SAY: return "SAY";
        case T::PM: return "PM";
        case T::SELF_PM: return "SELF_PM";
        case T::CLICKABLE_NPC_TEXT: return "CLICKABLE_NPC_TEXT";
        case T::SOUND: return "SOUND";
        case T::ITEM_STACK_COUNT: return "ITEM_STACK_COUNT";
        case T::HOTKEY_TEXT: return "HOTKEY_TEXT";
        case T::HOTKEY_USE_ON_SELF: return "HOTKEY_USE_ON_SELF";
        case T::HOTKEY_USE_ON_TARGET: return "HOTKEY_USE_ON_TARGET";
        case T::HOTKEY_USE_WITH_CROSSHAIR: return "HOTKEY_USE_WITH_CROSSHAIR";
        case T::HOTKEY_EQUIP: return "HOTKEY_EQUIP";
        case T::STORE_NEW: return "STORE_NEW";
        case T::VIP_OFFLINE: return "VIP_OFFLINE";
        case T::VIP_ONLINE: return "VIP_ONLINE";
        case T::ADD_NEW_HOTKEY_ENTRY: return "ADD_NEW_HOTKEY_ENTRY";
        case T::NAME_BATTLE_WINDOW_HIGHLIGHTED: return "NAME_BATTLE_WINDOW_HIGHLIGHTED";

        default:
            THROW_RUNTIME_ERROR(sb::utility::stringify("Unimplemented text type: ", (int)t));
    }
}

QJsonValue toJson(const std::list<Text>& t)
{
    QJsonArray a;
    for(const Text& text : t)
    {
        a.push_back(QJsonObject(
        {
            {"width", text.width},
            {"height", text.height},
            {"x", text.x},
            {"y", text.y},
            {"localX", text.localX},
            {"localY", text.localY},
            {"string", QString::fromStdString(text.string)},
        }));
    }

    return a;
}


void toJson(QJsonObject& out, const Draw& d, const Frame& f)
{
    Vertex topLeft = d.topLeft;
    Vertex botRight = d.botRight;

    if(d.transform != nullptr)
    {
        d.getScreenCoords(f.width / 2.f, f.height / 2.f, topLeft.x, topLeft.y, d.topLeft.x, d.topLeft.y);
        d.getScreenCoords(f.width / 2.f, f.height / 2.f, botRight.x, botRight.y, d.botRight.x, d.botRight.y);
    }

    out["x"] = (int)(topLeft.x + 0.5f);
    out["y"] = (int)(topLeft.y + 0.5f);
    out["width"] = (int)(botRight.x - topLeft.x + 0.5f);
    out["height"] = (int)(botRight.y - topLeft.y + 0.5f);
}

QJsonValue toJson(const GlyphDraw& d, const Frame& f)
{
    QJsonObject o;
    toJson(o, d, f);

    o["character"] = QString(d.character);

    return o;
}

QJsonValue toJson(const TextDraw& d, const Frame& f)
{
    TextBuilder builder(d, f.width, f.height);

    QJsonArray glyphs;
    if(d.glyphDraws != nullptr)
    {
        for(const GlyphDraw& g : *d.glyphDraws)
        {
            glyphs.push_back(toJson(g, f));
        }
    }

    return QJsonObject(
    {
        {"type", toJson(builder.getTextType())},
        {"text", toJson(builder.getText())},
        {"glyphs", glyphs},
    });
}


const sb::tibiaassets::Object* getNamedObject(const SpriteDraw& d, const TibiaContext& c)
{
    for(const SpriteDraw::SpriteObjectPairing& p : d.pairings)
    {
        for(size_t objectId : p.objects)
        {
            const sb::tibiaassets::Object& object = c.getObjects()[objectId];
            if(object.itemInfo.hasMarketInfo)
            {
                return &object;
            }
        }
    }

    return nullptr;
}

QJsonValue toJson(const SpriteDraw& d, const TibiaContext& c, const Frame& f)
{
    QJsonObject o;
    o["x"] = (int)(d.topLeft.x + 0.5f);
    o["y"] = (int)(d.botRight.y + 0.5f);

    const sb::tibiaassets::Object* object = getNamedObject(d, c);
    QJsonObject objectJson;
    if(!object)
    {
        assert(!d.pairings.empty());
        assert(!d.pairings.front().objects.empty());
        object = &c.getObjects()[d.pairings.front().objects.front()];
    }
    else
    {
        objectJson["name"] = QString::fromStdString(object->itemInfo.marketInfo.name);
    }

    objectJson["id"] = (int)object->id;

    o["object"] = objectJson;
    return o;
}

QJsonValue toJson(const GuiDraw& d, const Frame& f)
{
    QJsonObject o;
    toJson(o, d, f);

    o["name"] = QString::fromStdString(d.name);
    return o;
}

QJsonValue toJson(Constants::RectColor c)
{
    using C = Constants::RectColor;
    switch(c)
    {
        case C::HP_GRAY: return "HP_GRAY/GUI_TEXT_UNDERLINE/TEXT_CURSOR_MENUS";

        case C::HP_FULL: return "HP_FULL/HP_FULL_BATTLE_LIST/SKILL_BAR_GREEN";

        case C::HP_LIGHT_GREEN: return "HP_LIGHT_GREEN/HP_LIGHT_GREEN_BATTLE_LIST";
        case C::HP_YELLOW: return "HP_YELLOW/HP_YELLOW_BATTLE_LIST";
        case C::HP_LIGHT_RED: return "HP_LIGHT_RED/HP_LIGHT_RED_BATTLE_LIST";
        case C::HP_RED: return "HP_RED/HP_RED_BATTLE_LIST/SKILL_BAR_RED";
        case C::HP_DARK_RED: return "HP_DARK_RED/HP_DARK_RED_BATTLE_LIST";
        case C::TEXT_INPUT_FIELD: return "TEXT_INPUT_FIELD";
        case C::TEXT_SELECTION_BOX: return "TEXT_SELECTION_BOX";
        case C::LIST_MENU_BACKGROUND: return "LIST_MENU_BACKGROUND/LIST_MENU_UNSELECTED_ENTRY_BOX";
        case C::LIST_MENU_SELECTED_ENTRY_BOX: return "LIST_MENU_SELECTED_ENTRY_BOX";
        case C::SKILL_BAR_GREEN_FULL: return "SKILL_BAR_GREEN_FULL";
        case C::HP_BACKGROUND: return "HP_BACKGROUND/SCENE_CLEAR_COLOR";
        case C::TEXT_CURSOR_CHAT: return "TEXT_CURSOR_CHAT";

        default:
            THROW_RUNTIME_ERROR(sb::utility::stringify("Unimplemented rect type: ", (int)c));
    };
}

QJsonValue toJson(const RectDraw& d, const Frame& f)
{
    QJsonObject o;
    toJson(o, d, f);

    o["type"] = toJson((Constants::RectColor)d.color.packed);
    return o;
}


template<typename T, typename... OtherArgs>
QJsonValue toJson(const std::shared_ptr<std::vector<T>>& vecPtr, const OtherArgs&... otherArgs)
{
    if(vecPtr == nullptr)
    {
        return QJsonValue();
    }

    QJsonArray a;
    for(const T& element : *vecPtr)
    {
        a.push_back(toJson(element, otherArgs...));
    }

    return a;
}

bool write(const Frame& f, const std::string& filePath, const TibiaContext& context)
{
    QJsonObject o(
    {
        {"hasMiniMapMoved", f.hasMiniMapMoved},
        {"miniMapX", (int)f.miniMapX},
        {"miniMapY", (int)f.miniMapY},
        {"miniMapScreenX", (int)f.miniMapScreenX},
        {"miniMapScreenY", (int)f.miniMapScreenY},
        {"miniMapScreenWidth", (int)f.miniMapScreenWidth},
        {"miniMapScreenHeight", (int)f.miniMapScreenHeight},
        {"hasViewUpdated", f.hasViewUpdated},
        {"viewX", (int)f.viewX},
        {"viewY", (int)f.viewY},
        {"viewWidth", (int)f.viewWidth},
        {"viewHeight", (int)f.viewHeight},
        {"width", (int)f.width},
        {"height", (int)f.height},
    });

    o["text"] = toJson(f.textDraws, f);

    o["spriteDraws"] = toJson(f.spriteDraws, context, f);
    o["guiSpriteDraws"] = toJson(f.guiSpriteDraws, context, f);
    o["guiDraws"] = toJson(f.guiDraws, f);
    o["rectDraws"] = toJson(f.rectDraws, f);

    QFile file(QString::fromStdString(filePath + ".json"));
    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QByteArray json = QJsonDocument(o).toJson();
    return file.write(json) == json.size();
}

}
}
