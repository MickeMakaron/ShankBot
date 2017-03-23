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
#include "monitor/ParsedFrameFile.hpp"
#include "monitor/ParsedFrame.hpp"
#include "utility/utility.hpp"
///////////////////////////////////

///////////////////////////////////
// Qt
#include "QtCore/QJsonDocument"
#include "QtCore/QJsonObject"
#include "QtCore/QJsonArray"
#include "QtCore/QFile"
///////////////////////////////////

namespace GraphicsLayer
{
namespace ParsedFrameFile
{
///////////////////////////////////
// Write
///////////////////////////////////

void write(QJsonObject& o, const std::shared_ptr<Gui::Data>& guiData);


bool write(const ParsedFrame& f, const std::string& filePath)
{
    QJsonObject o;
    write(o, f.gui);

    QFile file(QString::fromStdString(filePath + ".json"));
    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    file.write(QJsonDocument(o).toJson());
    return true;
}

QJsonValue toJson(const Gui::Rect& r)
{
    return QJsonObject(
    {
        {"x", (int)r.pos.x},
        {"y", (int)r.pos.y},
        {"width", (int)r.size.x},
        {"height", (int)r.size.y},
    });
}

void fromJson(Gui::Rect& r, const QJsonValue& json)
{
    QJsonObject o = json.toObject();
    r.pos.x = o["x"].toInt();
    r.pos.y = o["y"].toInt();
    r.size.x = o["width"].toInt();
    r.size.y = o["height"].toInt();
}


QJsonValue toJson(const Gui::SideBottomWindow::Type& t)
{
    using T = Gui::SideBottomWindow::Type;
    switch(t)
    {
        case T::BATTLE: return "battle";
        case T::CONTAINER: return "container";
        case T::NPC_TRADE: return "npcTrade";
        case T::UNJUSTIFIED_POINTS: return "unjustifiedPoints";
        case T::SKILLS: return "skills";
        case T::VIP: return "vip";

        default:
            THROW_RUNTIME_ERROR(sb::utility::stringify("Unimplemented window type: ", (int)t));
    }
}

void fromJson(Gui::SideBottomWindow::Type& t, const QJsonValue& json)
{
    std::string str = json.toString().toStdString();
    using T = Gui::SideBottomWindow::Type;
    if(str == "battle")
    {
        t = T::BATTLE;
    }
    else if(str == "container")
    {
        t = T::CONTAINER;
    }
    else if(str == "npcTrade")
    {
        t = T::NPC_TRADE;
    }
    else if(str == "unjustifiedPoints")
    {
        t = T::UNJUSTIFIED_POINTS;
    }
    else if(str == "skills")
    {
        t = T::SKILLS;
    }
    else if(str == "vip")
    {
        t = T::VIP;
    }
    else
    {
        t = T::INVALID;
    }
}

QJsonValue toJson(const std::set<size_t>& ids)
{
    QJsonArray array;
    for(const size_t& id : ids)
    {
        array.push_back((int)id);
    }

    return array;
}


void fromJson(std::set<size_t>& ids, const QJsonValue& json)
{
    for(QJsonValueRef v : json.toArray())
    {
        ids.insert(v.toInt());
    }
}

QJsonValue toJson(const Gui::SideBottomWindow& w)
{
    QJsonObject json(
    {
        {"clientArea", toJson(w.clientArea)},
        {"exitButton", toJson(w.exitButton)},
        {"isMinimized", w.isMinimized},
        {"minMaxButton", toJson(w.minMaxButton)},
        {"type", toJson(w.type)},
    });

    using T = Gui::SideBottomWindow::Type;
    switch(w.type)
    {
        case T::BATTLE:
            json["expandButton"] = toJson(w.expandButton);
            break;
        case T::CONTAINER:
            json["upButton"] = toJson(w.containerUpButton);
            break;

        default:
            break;
    }

    if(w.hasResizer)
    {
        json["resizer"] = toJson(w.resizer);
    }

    return json;
}


void fromJson(Gui::SideBottomWindow& w, const QJsonValue& json)
{
    QJsonObject o = json.toObject();
    fromJson(w.clientArea, o["clientArea"]);
    fromJson(w.exitButton, o["exitButton"]);
    w.isMinimized = o["isMinimized"].toBool();
    fromJson(w.minMaxButton, o["minMaxButton"]);
    fromJson(w.type, o["type"]);

    using T = Gui::SideBottomWindow::Type;
    switch(w.type)
    {
        case T::BATTLE:
            fromJson(w.expandButton, o["expandButton"]);
            break;
        case T::CONTAINER:
            fromJson(w.containerUpButton, o["upButton"]);
            break;

        default:
            break;
    }

    w.hasResizer = o.contains("resizer");

    if(w.hasResizer)
    {
        fromJson(w.resizer, o["resizer"]);
    }
}


QJsonValue toJson(const Gui::State& s)
{
    using S = Gui::State;
    switch(s)
    {
        case S::MAIN_MENU: return "mainMenu";
        case S::GAME: return "game";

        default:
            THROW_RUNTIME_ERROR(sb::utility::stringify("Unimplemented GUI state: ", (int)s));
    }
}


void fromJson(Gui::State& s, const QJsonValue& json)
{
    std::string str = json.toString().toStdString();
    using S = Gui::State;
    if(str == "mainMenu")
    {
        s = S::MAIN_MENU;
    }
    else if(str == "game")
    {
        s = S::GAME;
    }
    else
    {
        s = S::UNDEFINED;
    }
}

QJsonValue toJson(const Gui::NpcTradeWindow::Offer& o)
{
    return QJsonObject(
    {
        {"cost", (int)o.cost},
        {"isAffordable", o.isAffordable},
        {"weight", o.weight},
        {"name", QString::fromStdString(o.name)},
        {"objectIds", toJson(o.objects)},
    });
}

void fromJson(Gui::NpcTradeWindow::Offer& o, const QJsonValue& json)
{
    QJsonObject obj = json.toObject();
    o.cost = obj["cost"].toInt();
    o.isAffordable = obj["isAffordable"].toBool();
    o.weight = obj["weight"].toDouble();
    o.name = obj["name"].toString().toStdString();
    fromJson(o.objects, obj["objectIds"]);
}

QJsonValue toJson(const Gui::NpcTradeWindow::Tab& t)
{
    using Tab = Gui::NpcTradeWindow::Tab;
    switch(t)
    {
        case Tab::BUY: return "buy";
        case Tab::SELL: return "sell";

        default:
            THROW_RUNTIME_ERROR(sb::utility::stringify("Unimplemented trade tab: ", (int)t));
    }
}

void fromJson(Gui::NpcTradeWindow::Tab& t, const QJsonValue& json)
{
    std::string str = json.toString().toStdString();
    using Tab = Gui::NpcTradeWindow::Tab;
    if(str == "buy")
    {
        t = Tab::BUY;
    }
    else if(str == "sell")
    {
        t = Tab::SELL;
    }
    else
    {
        t = Tab::INVALID;
    }
}


QJsonValue toJson(const Gui::Button& b)
{
    return QJsonObject(
    {
        {"isDown", b.isDown},
        {"text", QString::fromStdString(b.text)},
        {"x", b.x},
        {"y", b.y},
        {"width", b.width},
        {"height", b.height},
    });
}

void fromJson(Gui::Button& b, const QJsonValue& json)
{
    QJsonObject o = json.toObject();
    b.isDown = o["isDown"].toBool();
    b.text = o["text"].toString().toStdString();
    b.x = o["x"].toInt();
    b.y = o["y"].toInt();
    b.width = o["width"].toInt();
    b.height = o["height"].toInt();
}

QJsonValue toJson(const Gui::NpcTradeWindow& w)
{
    QJsonObject json(
    {
        {"amount", (int)w.amount},
        {"availableMoney", (int)w.availableMoney},
        {"totalPrice", (int)w.totalPrice},
        {"selectedOfferIndex", (int)w.selectedOfferIndex},

    });

    QJsonArray offers;
    for(const Gui::NpcTradeWindow::Offer& o : w.visibleOffers)
    {
        offers.push_back(toJson(o));
    }
    json["visibleOffers"] = offers;

    if(w.selectedOfferIndex < w.visibleOffers.size())
    {
        json["selectedOffer"] = toJson(w.visibleOffers[w.selectedOfferIndex]);
    }

    json["tab"] = toJson(w.currentTab);

    json["window"] = w.window == nullptr ? QJsonObject() : toJson(*w.window);
    json["buyButton"] = w.buyButton == nullptr ? QJsonObject() : toJson(*w.buyButton);
    json["sellButton"] = w.sellButton == nullptr ? QJsonObject() : toJson(*w.sellButton);
    json["okButton"] = w.okButton == nullptr ? QJsonObject() : toJson(*w.okButton);

    return json;
}

void fromJson(Gui::NpcTradeWindow& w, const QJsonValue& json)
{
    QJsonObject o = json.toObject();

    w.amount = o["amount"].toInt();
    w.availableMoney = o["availableMoney"].toInt();
    w.totalPrice = o["totalPrice"].toInt();
    w.selectedOfferIndex = o["selectedOfferIndex"].toInt();

    for(QJsonValueRef v : o["visibleOffers"].toArray())
    {
        w.visibleOffers.emplace_back();
        fromJson(w.visibleOffers.back(), v);
    }

    fromJson(w.currentTab, o["tab"]);

    w.window = nullptr;
    w.buyButton = nullptr;
    w.sellButton = nullptr;
    w.okButton = nullptr;
    QJsonObject window = o["window"].toObject();
    QJsonObject buyButton = o["buyButton"].toObject();
    QJsonObject sellButton = o["sellButton"].toObject();
    QJsonObject okButton = o["okButton"].toObject();

    if(!window.empty())
    {
        w.window.reset(new Gui::SideBottomWindow());
        fromJson(*w.window, window);
    }
    if(!buyButton.empty())
    {
        w.buyButton.reset(new Gui::Button());
        fromJson(*w.buyButton, buyButton);
    }
    if(!sellButton.empty())
    {
        w.sellButton.reset(new Gui::Button());
        fromJson(*w.sellButton, sellButton);
    }
    if(!okButton.empty())
    {
        w.okButton.reset(new Gui::Button());
        fromJson(*w.okButton, okButton);
    }
}

QJsonValue toJson(const Gui::Container& c)
{
    QJsonArray items;
    for(const std::pair<unsigned short, std::set<size_t>>& item : c.items)
    {
        items.push_back(QJsonObject(
        {
            {"count", (int)item.first},
            {"objectIds", toJson(item.second)},
        }));
    }


    return QJsonObject(
    {
        {"name", QString::fromStdString(c.name)},
        {"cap", (int)c.capacity},
        {"items", items},
        {"scroll", c.scroll},
        {"window", c.window == nullptr ? QJsonObject() : toJson(*c.window)},
    });
}

void fromJson(Gui::Container& c, const QJsonValue& json)
{
    QJsonObject o = json.toObject();
    c.name = o["name"].toString().toStdString();
    c.capacity = o["cap"].toInt();

    for(QJsonValueRef v : o["items"].toArray())
    {
        QJsonObject itemObject = v.toObject();

        c.items.emplace_back();
        c.items.back().first = itemObject["count"].toInt();
        fromJson(c.items.back().second, itemObject["objectIds"]);
    }

    c.scroll = o["scroll"].toDouble();

    c.window = nullptr;
    QJsonObject window = o["window"].toObject();
    if(!window.empty())
    {
        c.window.reset(new Gui::SideBottomWindow());
        fromJson(*c.window, window);
    }
}

QJsonValue toJson(const Gui::BattleWindow::Outfit& o)
{
    return QJsonObject(
    {
        {"name", QString::fromStdString(o.name)},
        {"hpPercent", o.hpPercent},
        {"objectIds", toJson(o.objects)},
    });
}

void fromJson(Gui::BattleWindow::Outfit& o, const QJsonValue& json)
{
    QJsonObject obj = json.toObject();
    o.name = obj["name"].toString().toStdString();
    o.hpPercent = obj["hpPercent"].toDouble();
    fromJson(o.objects, obj["objectIds"]);
}

QJsonValue toJson(const Gui::BattleWindow& w)
{
    QJsonArray outfits;
    for(const std::shared_ptr<Gui::BattleWindow::Outfit>& o : w.outfits)
    {
        if(o != nullptr)
        {
            outfits.push_back(toJson(*o));
        }
    }

    return QJsonObject(
    {
        {"outfits", outfits},
        {"selectedOutfit", w.selectedOutfit == nullptr ? QJsonObject() : toJson(*w.selectedOutfit)},
    });
}

void fromJson(Gui::BattleWindow& w, const QJsonValue& json)
{
    QJsonObject o = json.toObject();
    for(QJsonValueRef v : o["outfits"].toArray())
    {
        w.outfits.emplace_back(new Gui::BattleWindow::Outfit());
        fromJson(*w.outfits.back(), v);
    }

    w.selectedOutfit = nullptr;
    QJsonObject selectedOutfit = o["selectedOutfit"].toObject();
    if(!selectedOutfit.empty())
    {
        w.selectedOutfit.reset(new Gui::BattleWindow::Outfit());
        fromJson(*w.selectedOutfit, selectedOutfit);
    }
}

void write(QJsonObject& o, const std::shared_ptr<Gui::Data>& guiData)
{
    if(guiData == nullptr)
    {
        o["gui"] = "";
        return;
    }

    QJsonObject gui;

    gui["attributes"] = QJsonObject(
    {
        {"cap", guiData->cap},
        {"soul", guiData->soul},
        {"mana", guiData->mana},
        {"hp", guiData->hp},
        {"hpLevel", guiData->hpLevel},
        {"manaLevel", guiData->manaLevel},
        {"level", guiData->level},
        {"experience", (int)guiData->experience},
        {"xpGainRate", guiData->xpGainRate},
        {"speed", guiData->speed},
        {"foodMinutes", guiData->foodMinutes},
        {"staminaMinutes", guiData->staminaMinutes},
        {"offlineTrainingMinutes", guiData->offlineTrainingMinutes},
        {"magicLevel", guiData->magicLevel},
        {"fistLevel", guiData->fistLevel},
        {"clubLevel", guiData->clubLevel},
        {"swordLevel", guiData->swordLevel},
        {"axeLevel", guiData->axeLevel},
        {"distanceLevel", guiData->distanceLevel},
        {"shieldingLevel", guiData->shieldingLevel},
        {"fishingLevel", guiData->fishingLevel},
        {"critChance", guiData->critChance},
        {"critDamage", guiData->critDamage},
        {"hpLeechChance", guiData->hpLeechChance},
        {"hpLeechAmount", guiData->hpLeechAmount},
        {"manaLeechChance", guiData->manaLeechChance},
        {"manaLeechAmount", guiData->manaLeechAmount},
    });

    gui["state"] = toJson(guiData->state);

    QJsonArray buttons;
    for(const std::shared_ptr<Gui::Button>& b : guiData->buttons)
    {
        buttons.push_back(b == nullptr ? QJsonObject() : toJson(*b));
    }
    gui["buttons"] = buttons;

    gui["equipment"] = QJsonObject(
    {
        {"hand1", (int)guiData->equipment[Gui::EqType::HAND1]},
        {"hand2", (int)guiData->equipment[Gui::EqType::HAND2]},
        {"neck", (int)guiData->equipment[Gui::EqType::NECK]},
        {"finger", (int)guiData->equipment[Gui::EqType::FINGER]},
        {"head", (int)guiData->equipment[Gui::EqType::HEAD]},
        {"torso", (int)guiData->equipment[Gui::EqType::TORSO]},
        {"legs", (int)guiData->equipment[Gui::EqType::LEGS]},
        {"feet", (int)guiData->equipment[Gui::EqType::FEET]},
        {"back", (int)guiData->equipment[Gui::EqType::BACK]},
        {"hip", (int)guiData->equipment[Gui::EqType::HIP]},
    });

    QJsonArray onlineVips;
    QJsonArray offlineVips;
    for(const std::string& name : guiData->onlineVips)
    {
        onlineVips.push_back(QString::fromStdString(name));
    }
    for(const std::string& name : guiData->offlineVips)
    {
        offlineVips.push_back(QString::fromStdString(name));
    }

    gui["onlineVips"] = onlineVips;
    gui["offlineVips"] = offlineVips;

    QJsonArray containers;
    for(const Gui::Container& c : guiData->containers)
    {
        containers.push_back(toJson(c));
    }
    gui["containers"] = containers;
    gui["trade"] = guiData->npcTradeWindow == nullptr ? QJsonObject() : toJson(*guiData->npcTradeWindow);
    gui["battle"] = guiData->battleWindow == nullptr ? QJsonObject() : toJson(*guiData->battleWindow);






    o["gui"] = gui;
}


///////////////////////////////////
// Read
///////////////////////////////////
void read(const QJsonObject& o, std::shared_ptr<Gui::Data>& guiData);
void read(const QJsonObject& o, std::shared_ptr<RawImage>& screenPixels);

bool read(ParsedFrame& f, const std::string& filePath)
{
    QFile file(QString::fromStdString(filePath));
    if(!file.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QJsonObject o(QJsonDocument::fromJson(file.readAll()).object());

    read(o, f.gui);

    return true;
}



void read(const QJsonObject& o, std::shared_ptr<Gui::Data>& guiData)
{
    guiData = nullptr;

    QJsonObject gui = o["gui"].toObject();
    if(gui.empty())
    {
        return;
    }

    guiData.reset(new Gui::Data());

    QJsonObject attributes = gui["attributes"].toObject();
    guiData->cap = attributes["cap"].toInt();
    guiData->soul = attributes["soul"].toInt();
    guiData->mana = attributes["mana"].toInt();
    guiData->hp = attributes["hp"].toInt();
    guiData->hpLevel = attributes["hpLevel"].toDouble();
    guiData->manaLevel = attributes["manaLevel"].toDouble();
    guiData->level = attributes["level"].toInt();
    guiData->experience = attributes["experience"].toInt();
    guiData->xpGainRate = attributes["xpGainRate"].toInt();
    guiData->speed = attributes["speed"].toInt();
    guiData->foodMinutes = attributes["foodMinutes"].toInt();
    guiData->staminaMinutes = attributes["staminaMinutes"].toInt();
    guiData->offlineTrainingMinutes = attributes["offlineTrainingMinutes"].toInt();
    guiData->magicLevel = attributes["magicLevel"].toInt();
    guiData->fistLevel = attributes["fistLevel"].toInt();
    guiData->clubLevel = attributes["clubLevel"].toInt();
    guiData->swordLevel = attributes["swordLevel"].toInt();
    guiData->axeLevel = attributes["axeLevel"].toInt();
    guiData->distanceLevel = attributes["distanceLevel"].toInt();
    guiData->shieldingLevel = attributes["shieldingLevel"].toInt();
    guiData->fishingLevel = attributes["fishingLevel"].toInt();
    guiData->critChance = attributes["critChance"].toInt();
    guiData->critDamage = attributes["critDamage"].toInt();
    guiData->hpLeechChance = attributes["hpLeechChance"].toInt();
    guiData->hpLeechAmount = attributes["hpLeechAmount"].toInt();
    guiData->manaLeechChance = attributes["manaLeechChance"].toInt();
    guiData->manaLeechAmount = attributes["manaLeechAmount"].toInt();

    fromJson(guiData->state, gui["state"]);

    for(QJsonValueRef v : gui["buttons"].toArray())
    {
        std::shared_ptr<Gui::Button> bPtr;
        QJsonObject bObj = v.toObject();
        if(!bObj.empty())
        {
            bPtr.reset(new Gui::Button());
            fromJson(*bPtr, bObj);
        }

        guiData->buttons.push_back(bPtr);
    }

    QJsonObject eq = gui["equipment"].toObject();
    guiData->equipment =
    {
        {Gui::EqType::HAND1, eq["hand1"].toInt()},
        {Gui::EqType::HAND2, eq["hand2"].toInt()},
        {Gui::EqType::NECK, eq["neck"].toInt()},
        {Gui::EqType::FINGER, eq["finger"].toInt()},
        {Gui::EqType::HEAD, eq["head"].toInt()},
        {Gui::EqType::TORSO, eq["torso"].toInt()},
        {Gui::EqType::LEGS, eq["legs"].toInt()},
        {Gui::EqType::FEET, eq["feet"].toInt()},
        {Gui::EqType::BACK, eq["back"].toInt()},
        {Gui::EqType::HIP, eq["hip"].toInt()},
    };

    for(QJsonValueRef v : gui["onlineVips"].toArray())
    {
        guiData->onlineVips.push_back(v.toString().toStdString());
    }
    for(QJsonValueRef v : gui["offlineVips"].toArray())
    {
        guiData->offlineVips.push_back(v.toString().toStdString());
    }

    for(QJsonValueRef v : gui["containers"].toArray())
    {
        guiData->containers.emplace_back();
        fromJson(guiData->containers.back(), v);
    }

    guiData->npcTradeWindow = nullptr;
    guiData->battleWindow = nullptr;
    QJsonObject npcTradeWindow = gui["trade"].toObject();
    QJsonObject battleWindow = gui["battle"].toObject();
    if(!npcTradeWindow.empty())
    {
        guiData->npcTradeWindow.reset(new Gui::NpcTradeWindow());
        fromJson(*guiData->npcTradeWindow, npcTradeWindow);
    }
    if(!battleWindow.empty())
    {
        guiData->battleWindow.reset(new Gui::BattleWindow());
        fromJson(*guiData->battleWindow, battleWindow);
    }
}

}
}
