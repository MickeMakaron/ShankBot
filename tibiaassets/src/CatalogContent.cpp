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
#include "tibiaassets/CatalogContent.hpp"
#include "utility/utility.hpp"
#include "utility/file.hpp"
using namespace sb::tibiaassets;
///////////////////////////////////

///////////////////////////////////
// Qt
#include "QtCore/QtCore"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <fstream>
#include <cassert>
///////////////////////////////////


CatalogContent::CatalogContent(std::string filePath)
{
    read(filePath);
}

void CatalogContent::read(std::string filePath)
{
    using namespace sb::utility::file;
    if(!fileExists(filePath))
        SB_THROW("Cannot open catalog content at '" + filePath + "'. File does not exist.");


    QFile file(QString::fromStdString(filePath));
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString content = file.readAll();
    file.close();
    QJsonDocument catalogContent = QJsonDocument::fromJson(content.toUtf8());

    if(catalogContent.isNull())
        SB_THROW("Failed to read catalog content at '" + filePath + "'.");

    QJsonArray array = catalogContent.array();
    std::string dir = getPath(filePath);
    for(const QJsonValue& val : array)
    {
        QJsonObject obj = val.toObject();
        QString type = obj["type"].toString();

        if(type == "appearances")
        {
            Appearances a;
            a.path = dir + "/" + obj["file"].toString().toStdString();
            mAppearances.push_back(a);
        }
        else if(type == "sprite")
        {
            SpriteSheet s;
            s.path = dir + "/" + obj["file"].toString().toStdString();
            s.spriteSize = (SpriteSize)obj["spritetype"].toInt();
            s.firstSpriteId = obj["firstspriteid"].toInt();
            s.lastSpriteId = obj["lastspriteid"].toInt();
            s.area = obj["area"].toInt();
            mSpriteSheets.push_back(s);
        }
        else
        {
            SB_THROW("Error reading catalog content. Unimplemented type.");
        }
    }
}

const std::list<CatalogContent::Appearances>& CatalogContent::getAppearances() const
{
    return mAppearances;
}


const std::list<CatalogContent::SpriteSheet>& CatalogContent::getSpriteSheets() const
{
    return mSpriteSheets;
}

void CatalogContent::getSpriteTileSize(SpriteSize size, unsigned char& width, unsigned char& height)
{
        switch(size)
        {
            case SpriteSize::_1x1:
                width = 1;
                height = 1;
                return;

            case SpriteSize::_1x2:
                width = 1;
                height = 2;
                return;

            case SpriteSize::_2x1:
                width = 2;
                height = 1;
                return;

            case SpriteSize::_2x2:
                width = 2;
                height = 2;
                return;

            default:
                std::stringstream sstream;
                sstream << "Unimplemented sprite size: " << (int)size << std::endl;
                SB_THROW(sstream.str());
        }
}
