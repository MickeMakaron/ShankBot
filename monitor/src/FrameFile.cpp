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
#include "monitor/FrameFile.hpp"
#include "monitor/Frame.hpp"
#include "monitor/TibiaContext.hpp"
#include "utility/utility.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


///////////////////////////////////
// Qt
#include "QtGui/QImage"
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <fstream>
///////////////////////////////////

FrameFile::FrameFile(const TibiaContext& context)
: mContext(context)
{

}

FrameFile::FrameFile(const TibiaContext& context, const Frame& f)
: mContext(context)
{
    parse(f);
}

bool FrameFile::write(const std::string& filePath) const
{
    if(mScreenPixels != nullptr)
    {
        const RawImage& i = *mScreenPixels;
        QImage img(i.pixels.data(), i.width, i.height, QImage::Format_RGB888);
        img = img.mirrored(false, true);
        img.save(QString::fromStdString(filePath + ".png"));
    }
    if(mGui != nullptr)
    {
        std::ofstream file(filePath + "gui.txt");
        sb::utility::writeStream(*mGui, file);
    }
}

bool FrameFile::read(const std::string& filePath) const
{

}

void FrameFile::parse(const Frame& f)
{
    mScreenPixels = f.screenPixels;

    Gui gui(mContext);
    gui.update(f);
    mGui.reset(new Gui::Data(gui.getData()));
}
