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
#include "DynQImage.hpp"


DEFINE_DYNAMIC_CLASS_PROPERTIES
(
    QImage,
    "Qt5Gui.dll",
    "??1QImage@@UAE@XZ",

    QImage* (*qImageCtor)(const unsigned char* data, int width, int height, QImage::Format format, QImageCleanupFunction cleanupFunction, void* cleanupInfo) = nullptr;
    std::string qImageCtorSymbol = "??0QImage@@QAE@PBEHHW4Format@0@P6AXPAX@Z2@Z";
    bool (*qSave)(const QString& fileName, const char* format, int quality) = nullptr;
    std::string qSaveSymbol = "?save@QImage@@QBE_NABVQString@@PBDH@Z";
);

using namespace GraphicsLayer;

DynQImage::DynQImage(const unsigned char* data, int width, int height, QImage::Format format, QImageCleanupFunction cleanupFunction, void* cleanupInfo)
{
    CALL_DYNAMIC_CLASS_CTOR(mInstance, QImage, DYNM.qImageCtor(data, width, height, format, cleanupFunction, cleanupInfo));
}

bool DynQImage::save(const DynQString& fileName, const char* format, int quality)
{
    auto& i = fileName.getInstance();
    CALL_DYNAMIC_CLASS_METHOD(mInstance, return DYNM.qSave(i, format, quality));
}
