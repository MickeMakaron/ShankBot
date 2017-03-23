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
using namespace GraphicsLayer;
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

    void expectEq(const Vertex& v1, const Vertex& v2) const
    {
        EXPECT_FLOAT_EQ(v1.x, v2.x);
        EXPECT_FLOAT_EQ(v1.y, v2.y);
    }

    void expectEq(const Gui::Container& c1, const Gui::Container& c2) const
    {
        EXPECT_EQ(c1.name, c2.name);
        EXPECT_EQ(c1.capacity, c2.capacity);
        EXPECT_EQ(c1.items, c2.items);
        EXPECT_FLOAT_EQ(c1.scroll, c2.scroll);
        expectEq(c1.window, c2.window);
    }

    void expectEq(const Gui::NpcTradeWindow::Offer& o1, const Gui::NpcTradeWindow::Offer& o2) const
    {
        EXPECT_EQ(o1.name, o2.name);
        EXPECT_EQ(o1.objects, o2.objects);
        EXPECT_EQ(o1.cost, o2.cost);
        EXPECT_FLOAT_EQ(o1.weight, o2.weight);
        EXPECT_EQ(o1.isAffordable, o2.isAffordable);
    }

    void expectEq(const Gui::NpcTradeWindow& w1, const Gui::NpcTradeWindow& w2) const
    {
        expectEq(w1.visibleOffers, w2.visibleOffers);

        EXPECT_EQ(w1.selectedOfferIndex, w2.selectedOfferIndex);
        EXPECT_EQ(w1.amount, w2.amount);
        EXPECT_EQ(w1.totalPrice, w2.totalPrice);
        EXPECT_EQ(w1.availableMoney, w2.availableMoney);
        expectEq(w1.window, w2.window);
        expectEq(w1.buyButton, w2.buyButton);
        expectEq(w1.sellButton, w2.sellButton);
        expectEq(w1.okButton, w2.okButton);
        EXPECT_EQ(w1.currentTab, w2.currentTab);
    }

    void expectEq(const Gui::BattleWindow::Outfit& o1, const Gui::BattleWindow::Outfit& o2) const
    {
        EXPECT_FLOAT_EQ(o1.hpPercent, o2.hpPercent);
        EXPECT_EQ(o1.name, o2.name);
        EXPECT_EQ(o1.objects, o2.objects);
    }

    void expectEq(const Gui::BattleWindow& w1, const Gui::BattleWindow& w2) const
    {
        expectEq(w1.outfits, w2.outfits);
        expectEq(w1.selectedOutfit, w2.selectedOutfit);
    }

    void expectEq(const Gui::Rect& r1, const Gui::Rect& r2) const
    {
        expectEq(r1.pos, r2.pos);
        expectEq(r1.size, r2.size);
    }

    void expectEq(const Gui::SideBottomWindow& w1, const Gui::SideBottomWindow& w2) const
    {
        EXPECT_EQ(w1.type, w2.type);
        EXPECT_EQ(w1.hasResizer, w2.hasResizer);
        EXPECT_EQ(w1.isMinimized, w2.isMinimized);
        expectEq(w1.clientArea, w2.clientArea);
        expectEq(w1.exitButton, w2.exitButton);
        expectEq(w1.minMaxButton, w2.minMaxButton);

        if(w1.hasResizer)
        {
            expectEq(w1.resizer, w2.resizer);
        }

        using T = Gui::SideBottomWindow::Type;
        switch(w1.type)
        {
            case T::CONTAINER:
                expectEq(w1.containerUpButton, w2.containerUpButton);
                break;
            case T::BATTLE:
                expectEq(w1.expandButton, w2.expandButton);
                break;

            default:
                break;
        }
    }

    void expectEq(const Gui::Button& b1, const Gui::Button& b2) const
    {
        EXPECT_EQ(b1.x, b2.x);
        EXPECT_EQ(b1.y, b2.y);
        EXPECT_EQ(b1.width, b2.width);
        EXPECT_EQ(b1.height, b2.height);
        EXPECT_EQ(b1.isDown, b2.isDown);
        EXPECT_EQ(b1.text, b2.text);
    }

    template<typename T>
    void expectEq(const std::shared_ptr<T>& p1, const std::shared_ptr<T>& p2) const
    {
        EXPECT_EQ(p1 == nullptr, p2 == nullptr);
        if(p1 != nullptr)
        {
            expectEq(*p1, *p2);
        }
    }

    template<typename T>
    void expectEq(const std::list<T>& p1, const std::list<T>& p2) const
    {
        EXPECT_EQ(p1.size(), p2.size());
        auto it1 = p1.begin();
        auto it2 = p2.begin();
        while(it1 != p1.end())
        {
            expectEq(*it1, *it2);

            it1++;
            it2++;
        }
    }

    template<typename T>
    void expectEq(const std::vector<T>& p1, const std::vector<T>& p2) const
    {
        EXPECT_EQ(p1.size(), p2.size());
        if(p1.size() == p2.size())
        {
            for(size_t i = 0; i < p1.size(); i++)
            {
                expectEq(p1[i], p2[i]);
            }
        }
        else
        {
            int flerp = 0;
        }
    }

    void expectEq(const Gui::Data& f1, const Gui::Data& f2) const
    {
        EXPECT_EQ(f1.state, f2.state);
        EXPECT_EQ(f1.cap, f2.cap);
        EXPECT_EQ(f1.soul, f2.soul);
        EXPECT_EQ(f1.mana, f2.mana);
        EXPECT_EQ(f1.hp, f2.hp);
        EXPECT_FLOAT_EQ(f1.hpLevel, f2.hpLevel);
        EXPECT_FLOAT_EQ(f1.manaLevel, f2.manaLevel);
        EXPECT_EQ(f1.level, f2.level);
        EXPECT_EQ(f1.experience, f2.experience);
        EXPECT_EQ(f1.xpGainRate, f2.xpGainRate);
        EXPECT_EQ(f1.speed, f2.speed);
        EXPECT_EQ(f1.foodMinutes, f2.foodMinutes);
        EXPECT_EQ(f1.staminaMinutes, f2.staminaMinutes);
        EXPECT_EQ(f1.offlineTrainingMinutes, f2.offlineTrainingMinutes);
        EXPECT_EQ(f1.magicLevel, f2.magicLevel);
        EXPECT_EQ(f1.fistLevel, f2.fistLevel);
        EXPECT_EQ(f1.clubLevel, f2.clubLevel);
        EXPECT_EQ(f1.swordLevel, f2.swordLevel);
        EXPECT_EQ(f1.axeLevel, f2.axeLevel);
        EXPECT_EQ(f1.distanceLevel, f2.distanceLevel);
        EXPECT_EQ(f1.shieldingLevel, f2.shieldingLevel);
        EXPECT_EQ(f1.fishingLevel, f2.fishingLevel);
        EXPECT_EQ(f1.critChance, f2.critChance);
        EXPECT_EQ(f1.critDamage, f2.critDamage);
        EXPECT_EQ(f1.hpLeechChance, f2.hpLeechChance);
        EXPECT_EQ(f1.hpLeechAmount, f2.hpLeechAmount);
        EXPECT_EQ(f1.manaLeechChance, f2.manaLeechChance);
        EXPECT_EQ(f1.manaLeechAmount, f2.manaLeechAmount);

        EXPECT_EQ(f1.equipment, f2.equipment);
        EXPECT_EQ(f1.onlineVips, f2.onlineVips);
        EXPECT_EQ(f1.offlineVips, f2.offlineVips);


        expectEq(f1.containers, f2.containers);
//        expectEq(f1.sideBottomWindows, f2.sideBottomWindows);
        expectEq(f1.npcTradeWindow, f2.npcTradeWindow);
        expectEq(f1.battleWindow, f2.battleWindow);
    }

    void expectEq(const ParsedFrame& f1, const ParsedFrame& f2) const
    {
        expectEq(f1.gui, f2.gui);
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
