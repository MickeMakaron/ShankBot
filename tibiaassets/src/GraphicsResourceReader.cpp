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
#include "tibiaassets/GraphicsResourceReader.hpp"
#include "utility/utility.hpp"
#include "utility/file.hpp"
#include "tibiaassets/CatalogContent.hpp"
#include "tibiaassets/constants.hpp"
using namespace sb::tibiaassets;
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <fstream>
#include <cassert>
#include <iostream> // DEBUGGING PURPOSES
///////////////////////////////////

///////////////////////////////////
// Qt
#include "QtCore/QtCore"
#include "QtGui/QImage"
#include "QtGui/QImageReader"
///////////////////////////////////

///////////////////////////////////
// Lzma
#include "lzma/LzmaLib.h"
///////////////////////////////////

const std::map<std::string, GraphicsResourceReader::SpriteSheet> GraphicsResourceReader::SPRITE_SHEETS =
{
    {"creature-speech-flags.png", SpriteSheet(18, 18, {"talk", "quest", "trade", "directions"})},
    {"speech.png", SpriteSheet(18, 18, {"talk", "quest", "trade", "directions"})},
    {"creature-state-flags.png", SpriteSheet(11, 11,
        {
            "party-none-unknown", "party-inviter", "party-invitee", "party-member", "party-leader", "party-member-shared-xp", "party-leader-shared-xp", "party-member-shared-xp-fail", "party-leader-shared-xp-fail", "party-gray-unknown",
            "skull-none-unknown", "skull-yellow", "skull-green", "skull-white", "skull-red", "skull-black", "skull-orange", "skull-none-unknown", "skull-none-unknown", "skull-none-unknown",
            "guild-none-unknown", "guild-war-green", "guild-war-red", "guild-war-blue", "guild-yellow", "guild-white", "guild-none-unknown", "guild-none-unknown", "guild-none-unknown", "guild-none-unknown",
            "summon-none-unknown", "summon-green", "summon-red", "summon-none-unknown", "summon-none-unknown", "summon-none-unknown", "summon-none-unknown", "summon-none-unknown", "summon-none-unknown", "summon-none-unknown",
            "lightning-none-unknown", "lightning-red"
        })},
    {"guild.png", SpriteSheet(11, 11, {"war-green", "war-red", "war-blue", "yellow", "white"})},
    {"party.png", SpriteSheet(11, 11, {"inviter", "invitee", "member", "leader", "member-shared-xp", "leader-shared-xp", "member-shared-xp-fail", "leader-shared-xp-fail", "leader-shared-xp-fail", "leader-shared-xp-fail","gray-unknown"})},
    {"playerkiller.png", SpriteSheet(11, 11, {"yellow", "green", "white", "red", "black", "orange"})},
    {"player-state-playerkiller-flags.png", SpriteSheet(9, 9, {"yellow", "green", "white", "red", "black", "orange"})},
    {"player-state-flags.png", SpriteSheet(9, 9, {"poisoned", "burning", "electrified", "drunk", "magic-shield", "slowed", "haste", "logout-block", "drowning", "freezing", "dazzled", "cursed", "strengthened", "protection-zone-block", "protection-zone", "bleeding", "hungry"})},
    {"summon.png", SpriteSheet(9, 9, {"green", "red"})},
    {"markers.png", SpriteSheet(11, 11, {"checkmark", "questionmark", "exclamationmark", "star", "crossmark", "cross", "mouth", "brush", "sword", "flag", "lock", "bag", "skull", "dollar", "red-up", "red-down", "red-right", "red-left", "green-up", "green-down"})},
    {"vip-icons.png", SpriteSheet(12, 12, {"none-unknown", "heart", "skull", "lightning", "crosshair", "star", "yinyang", "triangle", "cross", "dollar", "balkenkreuz"})},
    {"premium-icons.png", SpriteSheet(20, 20, {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16"})},
    {"spellgroup-icons-20x20.png", SpriteSheet(20, 20, {"attack", "healing", "support", "special"})},
//    {"spell-icons.png", SpriteSheet(20, 20, {"1", "2", "3", "4"})}, // 138 st... // TODO:
};


GraphicsResourceReader::GraphicsResourceReader(std::string path)
{
    readResources(path);
}

GraphicsResourceReader::~GraphicsResourceReader()
{
    for(GraphicsResource& res : mGraphicsResources)
        delete[] res.pixels;
}

std::vector<std::string> GraphicsResourceReader::readNames(const std::string& path)
{
    QString resourcePath = QString::fromStdString(path);
    QResource::registerResource(resourcePath);

    std::vector<std::string> names;
    std::function<void(const QString& path)> readDir =
    [&](const QString& path)
    {
        QDir dir(path);
        for(const QFileInfo& file : dir.entryInfoList())
        {
            if(file.isDir())
                readDir(file.absoluteFilePath());
            else
            {

                QImageReader reader(file.absoluteFilePath());
                if(!reader.read().isNull())
                {
                    std::string name = file.absoluteFilePath().toStdString();

                    auto nameIt = SPRITE_SHEETS.find(sb::utility::file::basename(name));
                    if(nameIt != SPRITE_SHEETS.end())
                    {
                        size_t nameInsertionPos = name.rfind(".");
                        assert(nameInsertionPos != name.npos);

                        for(const std::string& suffix : nameIt->second.names)
                        {
                            std::string partName = name;
                            partName.insert(nameInsertionPos, "-" + suffix);
                            names.push_back(partName);
                        }

                    }
                    else
                    {
                        names.push_back(name);
                    }


                }
            }
        }
    };
    readDir(":/");

    QResource::unregisterResource(resourcePath);

    return names;
}

void GraphicsResourceReader::readResources(std::string path)
{
    QString resourcePath = QString::fromStdString(path);
    QResource::registerResource(resourcePath);

    std::function<void(const QString& path)> readDir =
    [&](const QString& path)
    {
        QDir dir(path);

        for(const QFileInfo& file : dir.entryInfoList())
        {
            if(file.isDir())
                readDir(file.absoluteFilePath());
            else
            {
                QImage resource(file.absoluteFilePath());

                if(!resource.isNull())
                {
                    QImage::Format format = resource.format();
                    resource = resource.convertToFormat(QImage::Format_RGBA8888);
                    const unsigned char* pixels = resource.constBits();
                    resource.save("graphicsResourcesOutTest/" + file.baseName() + ".png");

                    GraphicsResource r;
                    r.name = file.absoluteFilePath().toStdString();
                    auto spriteSheetIt = SPRITE_SHEETS.find(sb::utility::file::basename(r.name));
                    if(spriteSheetIt != SPRITE_SHEETS.end())
                    {
                        const SpriteSheet& s = spriteSheetIt->second;
                        size_t spriteRowSize = s.spriteWidth * sb::utility::BYTES_PER_PIXEL_RGBA;
                        size_t spriteSize = spriteRowSize * s.spriteHeight;
                        size_t spriteSheetRowSize = resource.bytesPerLine();
                        size_t spriteSheetSize = resource.byteCount();
                        assert(spriteSize * s.names.size() <= spriteSheetSize);

                        r.width = s.spriteWidth;
                        r.height = s.spriteHeight;
                        size_t nameInsertionPos = r.name.rfind(".");
                        assert(nameInsertionPos != r.name.npos);
                        std::string fullName = r.name;
                        const unsigned char* p = pixels;
                        size_t x = 0;
                        size_t y = 0;
                        size_t spritesPerRow = spriteSheetRowSize / spriteRowSize;
                        for(const std::string& name : s.names)
                        {
                            assert(p + spriteSheetRowSize * (s.spriteHeight - 1) + spriteRowSize - 1 < pixels + spriteSheetSize);
                            r.name = fullName;
                            r.name.insert(nameInsertionPos, "-" + name);
                            r.pixels = new unsigned char[spriteSize];
                            unsigned char* dest = r.pixels;
                            const unsigned char* src = p;
                            while(dest < r.pixels + spriteSize)
                            {
                                memcpy(dest, src, spriteRowSize);
                                dest += spriteRowSize;
                                src += spriteSheetRowSize;
                            }

                            mGraphicsResources.push_back(r);
//                            QImage img(r.pixels, r.width, r.height, QImage::Format_RGBA8888);
//                            img.save(QString::fromStdString("graphicsResourceSplitOut/" + basename(r.name)));


                            x = (x + 1) % spritesPerRow;
                            if(x == 0)
                                y++;

                            p = pixels + y * s.spriteHeight * spriteSheetRowSize + x * spriteRowSize;
                        }


                    }
                    else
                    {
    //                    if(r.name.find("cipsoft-logo.png") != r.name.npos)
    //                    {
    //                        std::cout << (int)format << ": " << r.name << std::endl;
    //                    }
    //                    if(r.name.find("dragon") != r.name.npos)
    //                    {
    //////                        std::cout << (int)format << ": " << r.name << std::endl;
    //////                        for(size_t i = 0; i < resource.width() * 4; i++)
    //////                            std::cout << (int)pixels[i] << " ";
    //////                        std::cout << std::endl;
    ////
    ////                        std::ofstream f("dragonpre-" + basename(r.name) + ".bin", std::ios::binary);
    ////                        f.write((const char*)pixels, resource.width() * resource.height() * 4);
    ////                        f.close();
    ////
    //////                        std::cout << r.name << " " << resource.width() << "x" << resource.height() << std::endl;
    //                    }
                        r.width = resource.width();
                        r.height = resource.height();
                        size_t size = r.width * r.height * sb::utility::BYTES_PER_PIXEL_RGBA;
                        r.pixels = new unsigned char[size];
                        memcpy(r.pixels, pixels, size);

                        mGraphicsResources.push_back(r);
                    }
                }
            }
        }
    };
    readDir(":/");

    QResource::unregisterResource(resourcePath);
}


const std::vector<GraphicsResourceReader::GraphicsResource>& GraphicsResourceReader::getGraphicsResources() const
{
    return mGraphicsResources;
}


void GraphicsResourceReader::printResources(std::string path) const
{
    QString resourcePath = QString::fromStdString(path);
    QResource::registerResource(resourcePath);

    std::function<void(const QString& path, size_t indentation)> readDir =
    [&](const QString& path, size_t indentation)
    {
        QDir dir(path);
        for(size_t i = 0; i < indentation; i++)
            std::cout << "\t";
        std::cout << path.toStdString() << std::endl;
        for(const QFileInfo& file : dir.entryInfoList())
        {
            if(file.isDir())
                readDir(file.absoluteFilePath(), indentation + 1);
            else
            {
                for(size_t i = 0; i < indentation; i++)
                    std::cout << "\t";

                std::cout << file.fileName().toStdString() << std::endl;
            }
        }
    };
    readDir(":/", 0);

    QResource::unregisterResource(resourcePath);
}



