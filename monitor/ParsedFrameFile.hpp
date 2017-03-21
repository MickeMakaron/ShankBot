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
#ifndef GRAPHICS_LAYER_PARSED_FRAME_FILE_HPP
#define GRAPHICS_LAYER_PARSED_FRAME_FILE_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "monitor/Gui.hpp"
#include "monitor/RawImage.hpp"
namespace GraphicsLayer
{
    struct Frame;
    class TibiaContext;
}
///////////////////////////////////


///////////////////////////////////
// Qt
class QJsonObject;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <string>
///////////////////////////////////

namespace GraphicsLayer
{
    class ParsedFrameFile
    {
        public:
            ParsedFrameFile(const TibiaContext& context);
            ParsedFrameFile(const TibiaContext& context, const Frame& f);

            bool write(const std::string& filePath) const;
            bool read(const std::string& filePath) const;

        private:
            void parse(const Frame& f);
            void writeScreenPixels(QJsonObject& o, const std::string& filePath) const;
            void writeGui(QJsonObject& o) const;

        private:
            const TibiaContext& mContext;
            std::shared_ptr<Gui::Data> mGui;
            std::shared_ptr<RawImage> mScreenPixels;
    };
}


#endif // GRAPHICS_LAYER_PARSED_FRAME_FILE_HPP
