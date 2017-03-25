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
#include "monitor/FrameFile.hpp"
#include "monitor/Frame.hpp"
#include "utility/utility.hpp"
#include "monitor/TibiaContext.hpp"
#include "tibiaassets/CatalogContent.hpp"
#include "test/utility.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

////////////////////////////////////////
// Google Test
#include "gtest/gtest.h"
////////////////////////////////////////

////////////////////////////////////////
// FrameParseTest
////////////////////////////////////////
class FrameParseTest : public ::testing::Test
{
public:
    FrameParseTest()
    {
        FrameFile::read(frame, framePath);
        ParsedFrameFile::read(expectFrame, expectPath);


        std::unique_ptr<SpriteObjectBindings> bindings;
        std::unique_ptr<SequenceTree> tree;
        std::unique_ptr<std::vector<std::string>> graphicsResourceNames;
        std::unique_ptr<std::vector<FontSample::Glyph>> glyphs;

        sb::tibiaassets::CatalogContent cat("../monitor/tibia/packages/Tibia/assets/catalog-content.json");
        const std::list<sb::tibiaassets::CatalogContent::Appearances>& appearanceses = cat.getAppearances();
        assert(appearanceses.size() == 1);

        sb::tibiaassets::AppearancesReader appearances(appearanceses.front().path);
        auto objects = std::make_unique<std::vector<sb::tibiaassets::Object>>(appearances.getObjects());

        auto spriteInfo = std::make_unique<SpriteInfo>(cat.getSpriteSheets());
        TibiaContext context
        (
            objects,
            bindings,
            tree,
            tree,
            spriteInfo,
            graphicsResourceNames,
            glyphs
        );

        Gui gui(context);
        gui.update(frame);
        parsedFrame.gui.reset(new Gui::Data(gui.getData()));

        Scene scene(context);
        scene.update(frame);
        parsedFrame.scene.reset(new Scene::Data(scene.getData()));
    }


    Frame frame;
    ParsedFrame parsedFrame;
    ParsedFrame expectFrame;
    std::string framePath = "frames/d21";
    std::string expectPath = "frames/d21e.json";
};

TEST_F(FrameParseTest, ParsedFrameMatchesExpectFrame)
{
    sb::test::expectEq(parsedFrame, expectFrame);
}
