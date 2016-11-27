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
#ifndef GRAPHICS_LAYER_DYN_QIMAGE_HPP
#define GRAPHICS_LAYER_DYN_QIMAGE_HPP

#include "DynamicClass.hpp"
#include "QtGui/QImage"
#include "DynQString.hpp"

namespace GraphicsLayer
{
    class DynQImage : public DynamicClass<QImage>
    {
        public:
            DynQImage(const unsigned char* data, int width, int height, QImage::Format format, QImageCleanupFunction cleanupFunction = Q_NULLPTR, void* cleanupInfo = Q_NULLPTR);
            bool save(const DynQString& fileName, const char* format = Q_NULLPTR, int quality = -1);
    };
}
#endif // GRAPHICS_LAYER_DYN_QIMAGE_HPP
