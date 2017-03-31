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
#include "monitor/FrameAssembler.hpp"
#include "monitor/Frame.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


///////////////////////////////////
// STD C++
///////////////////////////////////

#include "monitor/FrameFile.hpp"
#include "monitor/FrameFileJson.hpp"
#include "monitor/ParsedFrameFile.hpp"
#include "monitor/ParsedFrame.hpp"
#include "utility/utility.hpp"

FrameAssembler::FrameAssembler(const TibiaContext& context)
: mContext(context)
, mGui()
, mScene(context)
{

}


void FrameAssembler::update(const Frame& frame)
{
    mCurrentFrame = frame;
    mIsFrameAssembled = false;

    assembleCurrentFrame();
}

void FrameAssembler::assembleCurrentFrame()
{
    if(mIsFrameAssembled)
    {
        return;
    }

    static size_t id = 54;
    static std::string filePath = "frameDumps/d" + std::to_string(id);

    Frame frame;
    GraphicsLayer::FrameFile::read(frame, filePath);
    if(!FrameFileJson::write(frame, filePath + "frame", mContext))
    {
        SB_THROW("Failed to write frame JSON file.");
    }

    mGui.parse(frame);
    mText.parse(frame);
//    mScene.update(frame);

//    ParsedFrame pFrame;
//    pFrame.gui.reset(new Gui::Data(mGui.getData()));
//    pFrame.scene.reset(new Scene::Data(mScene.getData()));
//    pFrame.context = &mContext;
//
//    if(!ParsedFrameFile::write(pFrame, filePath + "p"))
//    {
//        SB_THROW("Failed to write parsed frame file.");
//    }
    exit(0);

    mIsFrameAssembled = true;
}
