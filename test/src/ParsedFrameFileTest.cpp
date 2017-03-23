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
#include "utility/utility.hpp"
#include "test/utility.hpp"
using namespace GraphicsLayer;
using namespace sb::test;
///////////////////////////////////

////////////////////////////////////////
// Google Test
#include "gtest/gtest.h"
////////////////////////////////////////

////////////////////////////////////////
// ParsedFrameFileTest
////////////////////////////////////////
class ParsedFrameFileTest : public ::testing::Test
{
public:
    ParsedFrameFileTest()
    {
        srand(0);
        frame.gui.reset(new Gui::Data());
        Gui::Data* gui = frame.gui.get();

        gui->state = (Gui::State)(rand() % (size_t)Gui::State::UNDEFINED);
        gui->cap = rand();
        gui->soul = rand();
        gui->mana = rand();
        gui->hp = rand();
        gui->hpLevel = rand();
        gui->manaLevel = rand();
        gui->level = rand();
        gui->experience = rand();
        gui->xpGainRate = rand();
        gui->speed = rand();
        gui->foodMinutes = rand();
        gui->staminaMinutes = rand();
        gui->offlineTrainingMinutes = rand();
        gui->magicLevel = rand();
        gui->fistLevel = rand();
        gui->clubLevel = rand();
        gui->swordLevel = rand();
        gui->axeLevel = rand();
        gui->distanceLevel = rand();
        gui->shieldingLevel = rand();
        gui->fishingLevel = rand();
        gui->critChance = rand();
        gui->critDamage = rand();
        gui->hpLeechChance = rand();
        gui->hpLeechAmount = rand();
        gui->manaLeechChance = rand();
        gui->manaLeechAmount = rand();

        for(size_t i = 0; i < (size_t)Gui::EqType::NUM_TYPES; i++)
        {
            gui->equipment[(Gui::EqType)i] = rand();
        }

        size_t numButtons = 3 + rand() % 10;
        for(size_t i = 0; i < numButtons; i++)
        {
            gui->buttons.emplace_back(new Gui::Button(genButton()));
        }

        size_t numContainers = 3 + rand() % 10;
        for(size_t i = 0; i < numContainers; i++)
        {
            gui->containers.push_back(genContainer());
        }


        size_t numSideBottomWindows = 3 + rand() % 10;
        for(size_t i = 0; i < numSideBottomWindows; i++)
        {
            gui->sideBottomWindows.emplace_back(new Gui::SideBottomWindow(genSideBottomWindow()));
        }

        gui->npcTradeWindow.reset(new Gui::NpcTradeWindow(genNpcTradeWindow()));
        gui->battleWindow.reset(new Gui::BattleWindow(genBattleWindow()));

        size_t numOnlineVips = 5 + rand() % 3;
        for(size_t i = 0; i < numOnlineVips; i++)
        {
            gui->onlineVips.push_back(sb::utility::randStr(5 + rand() % 10));
        }

        size_t numOfflineVips = 5 + rand() % 3;
        for(size_t i = 0; i < numOfflineVips; i++)
        {
            gui->offlineVips.push_back(sb::utility::randStr(5 + rand() % 10));
        }

        emptyFrame.gui.reset(new Gui::Data());
        gui = emptyFrame.gui.get();

        gui->state = (Gui::State)(rand() % (size_t)Gui::State::UNDEFINED);
        gui->cap = rand();
        gui->soul = rand();
        gui->mana = rand();
        gui->hp = rand();
        gui->hpLevel = rand();
        gui->manaLevel = rand();
        gui->level = rand();
        gui->experience = rand();
        gui->xpGainRate = rand();
        gui->speed = rand();
        gui->foodMinutes = rand();
        gui->staminaMinutes = rand();
        gui->offlineTrainingMinutes = rand();
        gui->magicLevel = rand();
        gui->fistLevel = rand();
        gui->clubLevel = rand();
        gui->swordLevel = rand();
        gui->axeLevel = rand();
        gui->distanceLevel = rand();
        gui->shieldingLevel = rand();
        gui->fishingLevel = rand();
        gui->critChance = rand();
        gui->critDamage = rand();
        gui->hpLeechChance = rand();
        gui->hpLeechAmount = rand();
        gui->manaLeechChance = rand();
        gui->manaLeechAmount = rand();
    }

    ~ParsedFrameFileTest()
    {
        std::remove(std::string(filePath + ".json").c_str());
    }

    static Gui::Button genButton()
    {
        Gui::Button b;
        b.isDown = rand() % 2;
        b.text = sb::utility::randStr(5 + rand() % 10);
        b.x = rand();
        b.y = rand();
        b.width = rand();
        b.height = rand();
        return b;
    }

    static Gui::Container genContainer()
    {
        Gui::Container c;
        c.name = sb::utility::randStr(5 + rand() % 10);
        c.capacity = rand();

        size_t numItems = 5 + rand() % 6;
        for(size_t i = 0; i < numItems; i++)
        {
            std::set<size_t> objectIds;
            size_t numIds = 5 + rand() % 2;
            for(size_t j = 0; j < numIds; j++)
            {
                objectIds.insert(rand());
            }
            c.items.emplace_back(rand(), objectIds);
        }

        c.scroll = rand();

        c.window.reset(new Gui::SideBottomWindow(genSideBottomWindow()));

        return c;
    }

    static Gui::Rect genRect()
    {
        Gui::Rect r;
        r.pos.x = rand();
        r.pos.y = rand();
        r.size.x = rand();
        r.size.y = rand();

        return r;
    }

    static Gui::SideBottomWindow genSideBottomWindow()
    {
        Gui::SideBottomWindow w;
        w.clientArea = genRect();
        w.containerUpButton = genRect();
        w.exitButton = genRect();
        w.expandButton = genRect();
        w.hasResizer = rand() % 2;
        w.isMinimized = rand() % 2;
        w.minMaxButton = genRect();
        w.resizer = genRect();
        w.type = (Gui::SideBottomWindow::Type)(rand() % ((size_t)Gui::SideBottomWindow::Type::INVALID));

        return w;
    }

    static Gui::NpcTradeWindow::Offer genOffer()
    {
        Gui::NpcTradeWindow::Offer o;
        o.cost = rand();
        o.isAffordable = rand() % 2;
        o.name = sb::utility::randStr(5 + rand() % 10);
        o.weight = rand();

        size_t numObjects = 5 + rand() % 5;
        for(size_t i = 0; i < numObjects; i++)
        {
            o.objects.insert(rand());
        }

        return o;
    }

    static Gui::NpcTradeWindow genNpcTradeWindow()
    {
        Gui::NpcTradeWindow w;

        size_t numOffers = 5 + rand() % 3;
        for(size_t i = 0; i < numOffers; i++)
        {
            w.visibleOffers.push_back(genOffer());
        }

        w.selectedOfferIndex = rand() % (w.visibleOffers.size() + 1) - 1;
        w.amount = rand();
        w.totalPrice = rand();
        w.availableMoney = rand();
        w.window.reset(new Gui::SideBottomWindow(genSideBottomWindow()));
        w.buyButton.reset(new Gui::Button(genButton()));
        w.sellButton.reset(new Gui::Button(genButton()));
        w.okButton.reset(new Gui::Button(genButton()));

        w.currentTab = (Gui::NpcTradeWindow::Tab)(rand() % ((size_t)Gui::NpcTradeWindow::Tab::INVALID));
        return w;
    }

    static Gui::BattleWindow genBattleWindow()
    {
        Gui::BattleWindow w;

        size_t numOutfits = 5 + rand() % 10;
        for(size_t i = 0; i < numOutfits; i++)
        {
            Gui::BattleWindow::Outfit o;
            o.hpPercent = rand();
            o.name = sb::utility::randStr(5 + rand() % 10);
            size_t numIds = 5 + rand() % 3;
            for(size_t j = 0; j < numIds; j++)
            {
                o.objects.insert(rand());
            }

            w.outfits.emplace_back(new Gui::BattleWindow::Outfit(o));
        }

        size_t selectedOutfitIndex = rand() % (w.outfits.size() + 3);
        if(selectedOutfitIndex < w.outfits.size())
        {
            w.selectedOutfit = w.outfits[selectedOutfitIndex];
        }

        return w;
    }


    void expectEqAfterWriteRead(const ParsedFrame& f) const
    {
        EXPECT_TRUE(ParsedFrameFile::write(f, filePath));
        ParsedFrame readFrame;
        EXPECT_TRUE(ParsedFrameFile::read(readFrame, filePath + ".json"));

        expectEq(f, readFrame);
    }

    void expectWriteReadTrue(ParsedFrame& f) const
    {
        EXPECT_TRUE(ParsedFrameFile::write(f, filePath));
        EXPECT_TRUE(ParsedFrameFile::read(f, filePath + ".json"));
    }


    ParsedFrame nullFrame = {0};
    ParsedFrame frame;
    ParsedFrame emptyFrame;
    std::string filePath = "frameDump";
};

TEST_F(ParsedFrameFileTest, WriteReadNullFrame)
{
    expectWriteReadTrue(nullFrame);
}

TEST_F(ParsedFrameFileTest, WriteReadFrame)
{
    expectWriteReadTrue(frame);
}


TEST_F(ParsedFrameFileTest, WriteReadEmptyFrame)
{
    expectWriteReadTrue(emptyFrame);
}

TEST_F(ParsedFrameFileTest, SameDataAfterReadNullFrame)
{
    expectEqAfterWriteRead(nullFrame);
}

TEST_F(ParsedFrameFileTest, SameDataAfterReadFrame)
{
    expectEqAfterWriteRead(frame);
}

TEST_F(ParsedFrameFileTest, SameDataAfterReadEmptyFrame)
{
    expectEqAfterWriteRead(emptyFrame);
}
