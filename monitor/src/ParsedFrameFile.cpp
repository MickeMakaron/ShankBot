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
#include "QtCore/QFile"
#include "QtCore/QFileInfo"
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <fstream>
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
        o["screen-pixels"] = "";
        return;
    }

    const RawImage& i = *mScreenPixels;
    QImage img(i.pixels.data(), i.width, i.height, QImage::Format_RGB888);
    img = img.mirrored(false, true);
    QString imgPath = QString::fromStdString(filePath + ".png");
    img.save(imgPath);

    o["screen-pixels"] = QFileInfo(imgPath).absoluteFilePath();
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
    });

    gui["state"] = (int)mGui->state;
    gui["cap"] = mGui->cap;
    gui["soul"] = mGui->soul;
    gui["mana"] = mGui->mana;
    gui["hp"] = mGui->hp;
    gui["hpLevel"] = mGui->hpLevel;
    gui["manaLevel"] = mGui->manaLevel;
    gui["level"] = mGui->level;
    gui["experience"] = (int)mGui->experience;
    gui["xpGainRate"] = mGui->xpGainRate;
    gui["speed"] = mGui->speed;
    gui["foodMinutes"] = mGui->foodMinutes;
    gui["staminaMinutes"] = mGui->staminaMinutes;
    gui["offlineTrainingMinutes"] = mGui->offlineTrainingMinutes;
    gui["magicLevel"] = mGui->magicLevel;
    gui["fistLevel"] = mGui->fistLevel;
    gui["clubLevel"] = mGui->clubLevel;
    gui["swordLevel"] = mGui->swordLevel;
    gui["axeLevel"] = mGui->axeLevel;
    gui["distanceLevel"] = mGui->distanceLevel;
    gui["shieldingLevel"] = mGui->shieldingLevel;
    gui["fishingLevel"] = mGui->fishingLevel;
    gui["critChance"] = mGui->critChance;
    gui["critDamage"] = mGui->critDamage;
    gui["hpLeechChance"] = mGui->hpLeechChance;
    gui["hpLeechAmount"] = mGui->hpLeechAmount;
    gui["manaLeechChance"] = mGui->manaLeechChance;
    gui["manaLeechAmount"] = mGui->manaLeechAmount;

//
//    std::vector<std::shared_ptr<Button>> buttons;
//
//    std::map<EqType, size_t> equipment;
//    std::vector<Container> containers;
//
//    std::vector<std::shared_ptr<SideBottomWindow>> sideBottomWindows;
//    std::shared_ptr<NpcTradeWindow> npcTradeWindow;
//    std::shared_ptr<BattleWindow> battleWindow;
//    std::vector<std::string> onlineVips;
//    std::vector<std::string> offlineVips;




    o["gui"] = gui;
}
