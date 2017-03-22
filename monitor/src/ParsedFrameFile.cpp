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
#include "monitor/Frame.hpp"
#include "monitor/TibiaContext.hpp"
#include "utility/utility.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


///////////////////////////////////
// Qt
#include "QtGui/QImage"
#include "QtCore/QJsonDocument"
#include "QtCore/QJsonObject"
#include "QtCore/QJsonArray"
#include "QtCore/QFile"
#include "QtCore/QFileInfo"
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <fstream>
#include <iostream>
///////////////////////////////////

ParsedFrameFile::ParsedFrameFile(const TibiaContext& context)
: mContext(context)
{

}

ParsedFrameFile::ParsedFrameFile(const TibiaContext& context, const Frame& f)
: mContext(context)
{
    parse(f);
}

bool ParsedFrameFile::write(const std::string& filePath) const
{
    QJsonObject o;
    writeScreenPixels(o, filePath);
    writeGui(o);

    QFile file(QString::fromStdString(filePath + ".json"));
    if(!file.open(QIODevice::WriteOnly))
    {
        return false;
    }

    file.write(QJsonDocument(o).toJson());
    return true;
}

bool ParsedFrameFile::read(const std::string& filePath) const
{

}

void ParsedFrameFile::parse(const Frame& f)
{
    mScreenPixels = f.screenPixels;

    Gui gui(mContext);
    gui.update(f);
    mGui.reset(new Gui::Data(gui.getData()));
}

void ParsedFrameFile::writeScreenPixels(QJsonObject& o, const std::string& filePath) const
{
    if(mScreenPixels == nullptr)
    {
        o["screenPixels"] = "";
        return;
    }

    const RawImage& i = *mScreenPixels;
    QImage img(i.pixels.data(), i.width, i.height, QImage::Format_RGB888);
    img = img.mirrored(false, true);
    QString imgPath = QString::fromStdString(filePath + ".png");
    img.save(imgPath);

    o["screenPixels"] = QFileInfo(imgPath).absoluteFilePath();
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

QJsonValue toJson(const std::set<size_t>& ids)
{
    QJsonArray array;
    for(const size_t& id : ids)
    {
        array.push_back((int)id);
    }

    return array;
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

struct BattleWindow
            {
                struct Outfit
                {
                    std::string name;
                    float hpPercent;
                    std::set<size_t> objects;
                };

                std::vector<std::shared_ptr<Outfit>> outfits;
                std::shared_ptr<Outfit> selectedOutfit;
            };

QJsonValue toJson(const Gui::BattleWindow::Outfit& o)
{
    return QJsonObject(
    {
        {"name", QString::fromStdString(o.name)},
        {"hpPercent", o.hpPercent},
        {"objectIds", toJson(o.objects)},
    });
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

void ParsedFrameFile::writeGui(QJsonObject& o) const
{
    if(mGui == nullptr)
    {
        o["gui"] = "";
        return;
    }

    QJsonObject gui;

    gui["attributes"] = QJsonObject(
    {
        {"cap", mGui->cap},
        {"soul", mGui->soul},
        {"mana", mGui->mana},
        {"hp", mGui->hp},
        {"hpLevel", mGui->hpLevel},
        {"manaLevel", mGui->manaLevel},
        {"level", mGui->level},
        {"experience", (int)mGui->experience},
        {"xpGainRate", mGui->xpGainRate},
        {"speed", mGui->speed},
        {"foodMinutes", mGui->foodMinutes},
        {"staminaMinutes", mGui->staminaMinutes},
        {"offlineTrainingMinutes", mGui->offlineTrainingMinutes},
        {"magicLevel", mGui->magicLevel},
        {"fistLevel", mGui->fistLevel},
        {"clubLevel", mGui->clubLevel},
        {"swordLevel", mGui->swordLevel},
        {"axeLevel", mGui->axeLevel},
        {"distanceLevel", mGui->distanceLevel},
        {"shieldingLevel", mGui->shieldingLevel},
        {"fishingLevel", mGui->fishingLevel},
        {"critChance", mGui->critChance},
        {"critDamage", mGui->critDamage},
        {"hpLeechChance", mGui->hpLeechChance},
        {"hpLeechAmount", mGui->hpLeechAmount},
        {"manaLeechChance", mGui->manaLeechChance},
        {"manaLeechAmount", mGui->manaLeechAmount},
    });

    gui["state"] = toJson(mGui->state);

    QJsonArray buttons;
    for(const std::shared_ptr<Gui::Button>& b : mGui->buttons)
    {
        buttons.push_back(b == nullptr ? QJsonObject() : toJson(*b));
    }
    gui["buttons"] = buttons;

    gui["equipment"] = QJsonObject(
    {
        {"hand1", (int)mGui->equipment[Gui::EqType::HAND1]},
        {"hand2", (int)mGui->equipment[Gui::EqType::HAND2]},
        {"neck", (int)mGui->equipment[Gui::EqType::NECK]},
        {"finger", (int)mGui->equipment[Gui::EqType::FINGER]},
        {"head", (int)mGui->equipment[Gui::EqType::HEAD]},
        {"torso", (int)mGui->equipment[Gui::EqType::TORSO]},
        {"legs", (int)mGui->equipment[Gui::EqType::LEGS]},
        {"feet", (int)mGui->equipment[Gui::EqType::FEET]},
        {"back", (int)mGui->equipment[Gui::EqType::BACK]},
        {"hip", (int)mGui->equipment[Gui::EqType::HIP]},
    });

    QJsonArray onlineVips;
    QJsonArray offlineVips;
    for(const std::string& name : mGui->onlineVips)
    {
        onlineVips.push_back(QString::fromStdString(name));
    }
    for(const std::string& name : mGui->offlineVips)
    {
        offlineVips.push_back(QString::fromStdString(name));
    }

    gui["onlineVips"] = onlineVips;
    gui["offlineVips"] = offlineVips;

    QJsonArray containers;
    for(const Gui::Container& c : mGui->containers)
    {
        containers.push_back(toJson(c));
    }
    gui["containers"] = containers;
    gui["trade"] = mGui->npcTradeWindow == nullptr ? QJsonObject() : toJson(*mGui->npcTradeWindow);
    gui["battle"] = mGui->battleWindow == nullptr ? QJsonObject() : toJson(*mGui->battleWindow);






    o["gui"] = gui;
}
