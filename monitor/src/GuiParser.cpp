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
#include "monitor/GuiParser.hpp"
#include "monitor/TextBuilder.hpp"
#include "utility/utility.hpp"
#include "utility/file.hpp"
#include "monitor/TibiaContext.hpp"
using namespace sb::tibiaassets;
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <algorithm>
#include <iostream>
#include <cassert>
///////////////////////////////////

GuiParser::GuiParser()
: mGuiDrawHandlers(initGuiDrawHandlers())
{
}

void GuiParser::parse(const Frame& frame)
{
    mData = Data();
    pass1 = Pass1();
    mDraws = frame.guiDraws;
    mBaseNames.clear();

    mHalfFrameWidth = frame.width / 2.f;
    mHalfFrameHeight = frame.height / 2.f;

    mBaseNames.resize(mDraws->size());
    std::transform(mDraws->begin(), mDraws->end(), mBaseNames.begin(), [](const GuiDraw& d)
    {
        return sb::utility::file::basenameNoExt(d.name);
    });

    for(size_t i = 0; i < mDraws->size(); i++)
    {
        auto foundIt = mGuiDrawHandlers.find(mBaseNames[i]);
        if(foundIt != mGuiDrawHandlers.end())
        {
            try
            {
                foundIt->second(i);
            }
            catch(const std::runtime_error& e)
            {
                std::cout << "Failed to handle GUI draw: \"" << e.what() << "\"." << std::endl;
            }
        }
        else
        {
            std::cout << "Unhandled: " << mBaseNames[i] << std::endl;
        }
    }

    try
    {
        parsePass1();
    }
    catch(const std::runtime_error& e)
    {
        std::cout << "Failed to parse pass 1: \"" << e.what() << "\"." << std::endl;
    }
}

void GuiParser::parsePass1()
{
    if(pass1.hpManaBorders.empty())
    {
        return;
    }

    SB_EXPECT(pass1.hpManaBorders.size(), ==, 2);
    SB_EXPECT(pass1.manaFill.draw, !=, nullptr);
    SB_EXPECT(pass1.hpFill.draw, !=, nullptr);

    const GuiDraw* border = pass1.hpManaBorders[0].draw;
    float borderWidth = border->botRight.x - border->topLeft.x;

    const GuiDraw* mana = pass1.manaFill.draw;
    const GuiDraw* hp = pass1.hpFill.draw;

    DefaultSideBar::StatusBar& sb = mData.game.defaultSideBar.statusBar;
    sb.manaPercent = (mana ? mana->botRight.x - mana->topLeft.x : 0.f) / borderWidth;
    sb.hpPercent = (hp ? hp->botRight.x - hp->topLeft.x : 0.f) / borderWidth;

}

const GuiParser::Data& GuiParser::getData()
{
    return mData;
}


IRect GuiParser::getRect(Vertex topLeft, Vertex botRight)
{
    if(botRight.x < topLeft.x || botRight.y < topLeft.y)
    {
        std::swap(topLeft, botRight);
    }

    IRect ir;
    ir.x = topLeft.x + 0.5f;
    ir.y = topLeft.y + 0.5f;
    ir.width = botRight.x - topLeft.x + 0.5f;
    ir.height = botRight.y - topLeft.y + 0.5f;

    return ir;
}

IRect GuiParser::getScreenRect(const Draw& d)
{
    Vertex topLeft;
    Vertex botRight;
    d.getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, topLeft.x, topLeft.y, d.topLeft.x, d.topLeft.y);
    d.getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, botRight.x, botRight.y, d.botRight.x, d.botRight.y);

    return getRect(topLeft, botRight);
}

DrawRect GuiParser::getDrawRect(const Draw& d)
{
    DrawRect r;
    r.local = getRect(d.topLeft, d.botRight);
    r.screen = getScreenRect(d);
    return r;
}


std::map<std::string, std::function<void(size_t&)>> GuiParser::initGuiDrawHandlers()
{


    auto createButtonHandlers = [this]
    (
        const std::vector<std::pair<std::string, Button&>>& buttons,
        std::map<std::string, std::function<void(size_t&)>>& handlers,
        const std::string& down,
        const std::string& up
    )
    {
        for(auto& pair : buttons)
        {
            Button& b = pair.second;
            handlers[pair.first + up] = [&b, this](size_t& i)
            {
                b = Button((*mDraws)[i], false);
            };
            handlers[pair.first + down] = [&b, this](size_t& i)
            {
                b = Button((*mDraws)[i], true);
            };
        }
    };

    auto createButtonVecHandlers = [this]
    (
        const std::vector<std::pair<std::string, std::vector<Button>&>>& buttons,
        std::map<std::string, std::function<void(size_t&)>>& handlers,
        const std::string& down,
        const std::string& up
    )
    {
        for(auto& pair : buttons)
        {
            std::vector<Button>& b = pair.second;
            handlers[pair.first + up] = [&b, this](size_t& i)
            {
                b.emplace_back((*mDraws)[i], false);
            };
            handlers[pair.first + down] = [&b, this](size_t& i)
            {
                b.emplace_back((*mDraws)[i], true);
            };
        }
    };

    GameData& gd = mData.game;
    DefaultSideBar& dsb = gd.defaultSideBar;
    SideBarWindows& sbw = gd.sideBarWindows;
    GameWindow& gw = gd.gameWindow;
    ChatWindow& cw = gd.chatWindow;
    SpellBar& sb = gd.spellBar;
    Skills& skills = sbw.skills;
    BattleList& battleList = sbw.battleList;
    Vip& vip = sbw.vip;
    UnjustifiedPoints& unjustifiedPoints = sbw.unjustifiedPoints;
    Prey& prey = sbw.prey;
    DefaultSideBar::MiniMap& miniMap = dsb.miniMap;
    DefaultSideBar::Inventory& inventory = dsb.inventory;
    DefaultSideBar::CombatControls& combatControls = dsb.combatControls;
    DefaultSideBar::ControlButtons& controlButtons = dsb.controlButtons;
    DefaultSideBar::PremiumFeatures& premiumFeatures = dsb.premiumFeatures;
    DefaultSideBar::CombatControls::ExpertControls& expertControls = combatControls.expertControls;
    MiscData& misc = mData.misc;
    std::map<std::string, std::function<void(size_t&)>> handlers;
    createButtonHandlers
    ({
        {"button-sidebar-add", gd.addSideBar},
        {"button-sidebar-remove", gd.removeSideBar},

        {"button-menu", dsb.controlButtonsMinMax},

        {"button-store", dsb.store},
        {"button-store-inbox", inventory.storeInbox},
        {"button-store-storexp", skills.xpBoost},

        {"button-logout", controlButtons.logout},
        {"button-options", controlButtons.options},
        {"button-unjustpoints", controlButtons.unjustPoints},
        {"button-questlog", controlButtons.questLog},
        {"button-viplist", controlButtons.vipList},
        {"button-battlelist", controlButtons.battleList},
        {"button-skills", controlButtons.skills},
        {"button-preywidget", controlButtons.prey},

        {"button-combat-pvp", combatControls.pvp},
        {"button-combat-pvp-small", combatControls.pvp},
        {"button-expert", combatControls.expert},
        {"button-expert-small", combatControls.expert},
        {"button-follow", combatControls.follow},
        {"button-stand", combatControls.stand},
        {"button-combat-defensive", combatControls.defensive},
        {"button-combat-balanced", combatControls.balanced},
        {"button-combat-offensive", combatControls.offensive},

        {"button-combat-dovemode", expertControls.dove},
        {"button-combat-redfistmode", expertControls.red},
        {"button-combat-whitehandmode", expertControls.white},
        {"button-combat-yellowhandmode", expertControls.yellow},


        {"automap-button-movedown", miniMap.down},
        {"automap-button-moveup", miniMap.up},
        {"automap-button-zoomout", miniMap.zoomOut},
        {"automap-button-zoomin", miniMap.zoomIn},

        {"console-buttonignore", cw.ignore},
        {"console-buttonnew", cw.newTab},
        {"console-buttonmessages", cw.messages},
        {"console-buttonclose", cw.closeTab},
        {"console-buttonleft", cw.tabLeft},
        {"console-buttonright", cw.tabRight},
        {"console-buttonleft-flash", cw.tabLeft},
        {"console-buttonright-flash", cw.tabRight},
        {"console-buttonleft-update", cw.tabLeft},
        {"console-buttonright-update", cw.tabRight},
        {"console-buttonsay", cw.volume},
        {"console-buttonwhisper", cw.volume},
        {"console-buttonyell", cw.volume},



        {"battlelist-monster", battleList.filter.monster},
        {"battlelist-npc", battleList.filter.npc},
        {"battlelist-party", battleList.filter.party},
        {"battlelist-players", battleList.filter.player},
        {"battlelist-skull", battleList.filter.skull},
        {"button-expand-12x12", battleList.filterMinMax},


        {"button-transfercoins", misc.transferCoins},
        {"button-copytoclipboard", misc.clipBoard},
    }, handlers, "-down", "-up");


    auto createMinMaxButtonHandlers = [this]
    (
        const std::vector<std::pair<std::string, Button&>>& buttons,
        std::map<std::string, std::function<void(size_t&)>>& handlers,
        const std::string& down,
        const std::string& up
    )
    {
        for(auto& pair : buttons)
        {
            Button& b = pair.second;
            handlers[pair.first + up] = [&b, this](size_t& i)
            {
                b = Button((*mDraws)[i], false);
            };
            handlers[pair.first + down] = [&b, this](size_t& i)
            {
                b = Button((*mDraws)[i], true);
            };
        }
    };
    handlers["max-button-small-down"] =
    handlers["max-button-small-highlight-down"] = [this](size_t& i)
    {
        mData.game.minMaxButtons.emplace_back((*mDraws)[i], false, true);
    };
    handlers["max-button-small-up"] =
    handlers["max-button-small-highlight-up"] = [this](size_t& i)
    {
        mData.game.minMaxButtons.emplace_back((*mDraws)[i], false, false);
    };

    handlers["min-button-small-down"] = [this](size_t& i)
    {
        mData.game.minMaxButtons.emplace_back((*mDraws)[i], true, true);
    };
    handlers["min-button-small-up"] = [this](size_t& i)
    {
        mData.game.minMaxButtons.emplace_back((*mDraws)[i], true, false);
    };



    createButtonVecHandlers
    ({
        {"exit-button-small", gd.exitButtons},
    }, handlers, "-down", "-up");

    const std::vector<std::pair<std::string, Button&>> disabledButtons =
    {
        {"button-expert-disabled", combatControls.expert},
        {"button-expert-small-disabled", combatControls.expert},
        {"button-sidebar-add-disabled", gd.addSideBar},
        {"button-sidebar-remove-disabled", gd.removeSideBar},
    };
    for(auto& pair : disabledButtons)
    {
        Button& b = pair.second;
        handlers[pair.first] = [&b, this](size_t& i)
        {
            b = Button((*mDraws)[i], false, false);
        };
    }

    const std::vector<std::pair<std::string, std::vector<Button>&>> disabledVecButtons =
    {
    };
    for(auto& pair : disabledVecButtons)
    {
        std::vector<Button>& b = pair.second;
        handlers[pair.first] = [&b, this](size_t& i)
        {
            b.emplace_back((*mDraws)[i], false, false);
        };
    }


    createButtonVecHandlers
    ({
    }, handlers, "-checked", "-unchecked");

    createButtonHandlers
    ({
        {"button-contextmenu-12x12", battleList.sort},
    }, handlers, "-pressed", "-idle");

    createButtonVecHandlers
    ({
    }, handlers, "-pressed", "-idle");

    createButtonVecHandlers
    ({
        {"console-tab", gd.tabs},
    }, handlers, "-active", "-passive");

    auto createTexButtonHandler = [this](bool isPressed)
    {
        return [this, isPressed](size_t& i)
        {
            size_t iStart = i;
            static const auto isCorner = [](const GuiDraw& d)
            {
                return short(d.botRight.x - d.topLeft.x + 0.5f) == 1 && short(d.botRight.y - d.topLeft.y + 0.5f) == 1;
            };
            SB_EXPECT_TRUE(isCorner((*mDraws)[i]));


            for(size_t numCorners = 0;;)
            {
                SB_EXPECT(i, <, mDraws->size());
                if(isCorner((*mDraws)[i]))
                {
                    numCorners++;
                    if(numCorners == 4)
                    {
                        break;
                    }
                }
                i++;
            }

            mData.game.textButtons.emplace_back((*mDraws)[iStart + (i - iStart + 1) / 2], isPressed);
        };
    };
    handlers["button-9grid-idle"] = createTexButtonHandler(false);
    handlers["button-9grid-pressed"] = createTexButtonHandler(true);

    auto createButtonTexturedHandler = [this](bool isPressed)
    {
        return [this, isPressed](size_t& i)
        {
            const GuiDraw& d = (*mDraws)[i];
            if(d.botRight.x - d.topLeft.x > 10.f && d.botRight.y - d.topLeft.y > 10.f)
            {
                mData.game.texturedButtons.emplace_back(d, isPressed);
            }
        };
    };
    handlers["button-textured-up"] = createTexButtonHandler(false);
    handlers["button-textured-down"] = createTexButtonHandler(true);

    auto createGuiElementHandlers = [this]
    (
        const std::vector<std::pair<std::string, GuiElement&>> rects,
        std::map<std::string, std::function<void(size_t&)>>& handlers
    )
    {
        for(auto& pair : rects)
        {
            GuiElement& r = pair.second;
            handlers[pair.first] = [&r, this](size_t& i)
            {
                r = GuiElement((*mDraws)[i]);
            };
        }
    };
    auto createGuiElementVecHandlers = [this]
    (
        const std::vector<std::pair<std::string, std::vector<GuiElement>&>> rects,
        std::map<std::string, std::function<void(size_t&)>>& handlers
    )
    {
        for(auto& pair : rects)
        {
            std::vector<GuiElement>& r = pair.second;
            handlers[pair.first] = [&r, this](size_t& i)
            {
                r.emplace_back((*mDraws)[i]);
            };
        }
    };

    createGuiElementHandlers
    ({
        {"automap-rose", miniMap.rose},
        {"automap-crosshair-dark", miniMap.crosshair},
        {"automap-crosshair-light", miniMap.crosshair},
        {"hitpoints-bar-filled", pass1.hpFill},
        {"mana-bar-filled", pass1.manaFill},
     }, handlers);

     handlers["icon-adventurers-blessing"] = [this](size_t& i)
     {
         mData.game.defaultSideBar.hasAdventurersBlessing = true;
     };

    createGuiElementVecHandlers
    ({
        {"hitpoints-manapoints-bar-border", pass1.hpManaBorders},
     }, handlers);

    auto createCreatureFlagHandlers = [this]
    (
        const std::vector<std::pair<std::string, CreatureFlag::Type>>& types,
        std::vector<CreatureFlag>& flags,
        std::map<std::string, std::function<void(size_t&)>>& handlers
    )
    {
        for(auto& pair : types)
        {
            CreatureFlag::Type type = pair.second;
            handlers[pair.first] = [&flags, this, type](size_t& i)
            {
                CreatureFlag f;
                f.draw = &(*mDraws)[i];
                f.type = type;
                flags.push_back(f);
            };
        }
    };


    using CF = CreatureFlag::Type;
    createCreatureFlagHandlers
    ({
        {"creature-speech-flags-directions", CF::DIRECTIONS},
        {"creature-speech-flags-quest", CF::QUEST},
        {"creature-speech-flags-talk", CF::TALK},
        {"creature-speech-flags-trade", CF::TRADE},
        {"creature-state-flags-guild-war-blue", CF::GUILD_WAR_BLUE},
        {"creature-state-flags-guild-war-green", CF::GUILD_WAR_GREEN},
        {"creature-state-flags-guild-war-red", CF::GUILD_WAR_RED},
        {"creature-state-flags-guild-white", CF::GUILD_WHITE},
        {"creature-state-flags-guild-yellow", CF::GUILD_YELLOW},
        {"creature-state-flags-lightning-red", CF::LIGHTNING_RED},
        {"creature-state-flags-party-gray-unknown", CF::PARTY_GRAY},
        {"creature-state-flags-party-invitee", CF::PARTY_INVITEE},
        {"creature-state-flags-party-inviter", CF::PARTY_INVITER},
        {"creature-state-flags-party-leader", CF::PARTY_LEADER},
        {"creature-state-flags-party-leader-shared-xp", CF::PARTY_LEADER_SHARED_XP},
        {"creature-state-flags-party-leader-shared-xp-fail", CF::PARTY_LEADER_SHARED_XP_FAIL},
        {"creature-state-flags-party-member", CF::PARTY_MEMBER},
        {"creature-state-flags-party-member-shared-xp", CF::PARTY_MEMBER_SHARED_XP},
        {"creature-state-flags-party-member-shared-xp-fail", CF::PARTY_MEMBER_SHARED_XP_FAIL},
        {"creature-state-flags-skull-black", CF::SKULL_BLACK},
        {"creature-state-flags-skull-green", CF::SKULL_GREEN},
        {"creature-state-flags-skull-orange", CF::SKULL_ORANGE},
        {"creature-state-flags-skull-red", CF::SKULL_RED},
        {"creature-state-flags-skull-white", CF::SKULL_WHITE},
        {"creature-state-flags-skull-yellow", CF::SKULL_YELLOW},
        {"creature-state-flags-summon-green", CF::SUMMON_GREEN},
        {"creature-state-flags-summon-red", CF::SUMMON_RED},
    }, gw.creatureFlags, handlers);


    auto createPlayerStateFlagHandlers = []
    (
        const std::vector<std::pair<std::string, uint32_t>>& states,
        uint32_t& flags,
        std::map<std::string, std::function<void(size_t&)>>& handlers
    )
    {
        for(auto& pair : states)
        {
            uint32_t state = pair.second;
            handlers[pair.first] = [&flags, state](size_t& i)
            {
                flags |= state;
            };
        }
    };

    namespace PS = PlayerState;
    createPlayerStateFlagHandlers
    ({
        {"player-state-flags-poisoned", PS::POISONED},
        {"player-state-flags-burning", PS::BURNING},
        {"player-state-flags-electrified", PS::ELECTRIFIED},
        {"player-state-flags-drunk", PS::DRUNK},
        {"player-state-flags-magic-shield", PS::MAGIC_SHIELD},
        {"player-state-flags-slowed", PS::SLOWED},
        {"player-state-flags-haste", PS::HASTE},
        {"player-state-flags-logout-block", PS::LOGOUT_BLOCK},
        {"player-state-flags-drowning", PS::DROWNING},
        {"player-state-flags-freezing", PS::FREEZING},
        {"player-state-flags-dazzled", PS::DAZZLED},
        {"player-state-flags-cursed", PS::CURSED},
        {"player-state-flags-strengthened", PS::STRENGTHENED},
        {"player-state-flags-protection-zone-block", PS::PROTECTION_ZONE_BLOCK},
        {"player-state-flags-protection-zone", PS::PROTECTION_ZONE},
        {"player-state-flags-bleeding", PS::BLEEDING},
        {"player-state-flags-hungry", PS::HUNGRY},
        {"player-state-guildwar-flag", PS::GUILD_WAR},
        {"player-state-playerkiller-flags-black", PS::SKULL_BLACK},
        {"player-state-playerkiller-flags-green", PS::SKULL_GREEN},
        {"player-state-playerkiller-flags-orange", PS::SKULL_ORANGE},
        {"player-state-playerkiller-flags-red", PS::SKULL_RED},
        {"player-state-playerkiller-flags-white", PS::SKULL_WHITE},
        {"player-state-playerkiller-flags-yellow", PS::SKULL_YELLOW},
    }, dsb.conditions, handlers);

    auto createMiniMapMarkerHandlers = [this]
    (
        const std::vector<std::pair<std::string, MiniMapMarker::Type>>& types,
        std::vector<MiniMapMarker>& markers,
        std::map<std::string, std::function<void(size_t&)>>& handlers
    )
    {
        for(auto& pair : types)
        {
            MiniMapMarker::Type type = pair.second;
            handlers[pair.first] = [&markers, this, type](size_t& i)
            {
                MiniMapMarker f;
                f.draw = &(*mDraws)[i];
                f.type = type;
                markers.push_back(f);
            };
        }
    };

    using MT = MiniMapMarker::Type;
    createMiniMapMarkerHandlers
    ({
        {"markers-checkmark", MT::CHECKMARK},
        {"markers-questionmark", MT::QUESTIONMARK},
        {"markers-exclamationmark", MT::EXCLAMATIONMARK},
        {"markers-star", MT::STAR},
        {"markers-crossmark", MT::CROSSMARK},
        {"markers-cross", MT::CROSS},
        {"markers-mouth", MT::MOUTH},
        {"markers-brush", MT::BRUSH},
        {"markers-sword", MT::SWORD},
        {"markers-flag", MT::FLAG},
        {"markers-lock", MT::LOCK},
        {"markers-bag", MT::BAG},
        {"markers-skull", MT::SKULL},
        {"markers-dollar", MT::DOLLAR},
        {"markers-red-up", MT::RED_UP},
        {"markers-red-down", MT::RED_DOWN},
        {"markers-red-right", MT::RED_RIGHT},
        {"markers-red-left", MT::RED_LEFT},
        {"markers-green-up", MT::GREEN_UP},
        {"markers-green-down", MT::GREEN_DOWN},
    }, miniMap.markers, handlers);

    auto createEmptyEquipmentSlotHandler = [this]
    (
        const std::vector<std::pair<std::string, EmptyEquipmentSlot::Type>>& types,
        std::vector<EmptyEquipmentSlot>& slots,
        std::map<std::string, std::function<void(size_t&)>>& handlers
    )
    {
        for(auto& pair : types)
        {
            EmptyEquipmentSlot::Type type = pair.second;
            handlers[pair.first] = [&slots, this, type](size_t& i)
            {
                EmptyEquipmentSlot f;
                f.draw = &(*mDraws)[i];
                f.type = type;
                slots.push_back(f);
            };
        }
    };

    using ET = EmptyEquipmentSlot::Type;
    createEmptyEquipmentSlotHandler
    ({
        {"inventory-back", ET::BACK},
        {"inventory-feet", ET::FEET},
        {"inventory-finger", ET::FINGER},
        {"inventory-head", ET::HEAD},
        {"inventory-hip", ET::HIP},
        {"inventory-left-hand", ET::LEFT_HAND},
        {"inventory-right-hand", ET::RIGHT_HAND},
        {"inventory-legs", ET::LEGS},
        {"inventory-neck", ET::NECK},
        {"inventory-torso", ET::TORSO},
    }, inventory.emptySlots, handlers);


    handlers["containerslot"] = [this](size_t& i)
    {
        mData.game.sideBarWindows.containers.emplace_back();
        Container& c = mData.game.sideBarWindows.containers.back();
        unsigned short drawCallId = (*mDraws)[i].drawCallId;
        while(i < mDraws->size())
        {
            i += 4;
            SB_EXPECT(i, <, mDraws->size());
            const GuiDraw& d = (*mDraws)[i];
            SB_EXPECT(mBaseNames[i], ==, "containerslot");

            short width = d.botRight.x - d.topLeft.x + 0.5f;
            short height = d.botRight.y - d.topLeft.y + 0.5f;
            if(width == 32 && height == 32)
            {
                c.slots.emplace_back(d);
            }
            else if(width == 32 && height == 18)
            {
                // soul/cap slot
            }
            else
            {
                SB_THROW("Unexpected containerslot size: " + std::to_string(width) + "x" + std::to_string(height));
            }


            i += 5;
            if(i >= mDraws->size() || (*mDraws)[i].drawCallId != drawCallId || mBaseNames[i] != "containerslot")
            {

                i--;
                if(c.slots.empty())
                {
                    mData.game.sideBarWindows.containers.pop_back();
                }
                return;
            }
        }
    };

    handlers["widget-borderimage"] = [this](size_t& i)
    {
        mData.game.sideBarWindows.windows.emplace_back();
        SideBarWindow& w = mData.game.sideBarWindows.windows.back();
        const GuiDraw* d = &(*mDraws)[i];
        unsigned short drawCallId = d->drawCallId;

        w.titleBar.local.x = d->topLeft.x + 0.5f;
        w.titleBar.local.y = d->topLeft.y + 0.5f;

        Vertex v;
        d->getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, v.x, v.y, d->topLeft.x, d->topLeft.y);
        w.titleBar.screen.x = v.x + 0.5f;
        w.titleBar.screen.y = v.y + 0.5f;

        i += 3;
        SB_EXPECT(i, <, mDraws->size());
        d = &(*mDraws)[i];
        SB_EXPECT(mBaseNames[i], ==, "widget-borderimage");
        SB_EXPECT(d->drawCallId, ==, drawCallId);

        w.titleBar.local.width = (unsigned short)(d->botRight.x + 0.5f) - w.titleBar.local.x;
        w.titleBar.local.height = (unsigned short)(d->botRight.y + 0.5f) - w.titleBar.local.y;
        w.titleBar.screen.width = w.titleBar.local.width;
        w.titleBar.screen.height = w.titleBar.local.height;
        SB_EXPECT(w.titleBar.local.width, ==, 176);
        SB_EXPECT(w.titleBar.local.height, ==, 15);



        w.clientArea = w.titleBar;
        w.clientArea.local.y += w.titleBar.local.height;
        w.clientArea.screen.y += w.titleBar.local.height;
        w.clientArea.local.width = 0;
        w.clientArea.local.height = 0;
        w.clientArea.screen.width = 0;
        w.clientArea.screen.height = 0;


        i++;
        if(i >= mDraws->size())
        {
            return;
        }

        d = &(*mDraws)[i];
        if(d->drawCallId != drawCallId)
        {
            i--;
            return;
        }

        SB_EXPECT(mBaseNames[i], ==, "widget-borderimage");
        SB_EXPECT(w.clientArea.local.x, ==, (unsigned short)(d->topLeft.x + 0.5f));
        SB_EXPECT(w.clientArea.local.y, ==, (unsigned short)(d->topLeft.y + 0.5f));

        while(i < mDraws->size())
        {
            d = &(*mDraws)[i];
            if(d->drawCallId != drawCallId)
            {
                i--;
                break;
            }
            i++;
        }

        d = &(*mDraws)[i];
        SB_EXPECT(mBaseNames[i], ==, "widget-borderimage");
        unsigned short right = (unsigned short)(d->botRight.x + 0.5f);
        unsigned short bot = (unsigned short)(d->botRight.y + 0.5f);
        SB_EXPECT(right, ==, w.titleBar.local.x + w.titleBar.local.width);
        w.clientArea.local.width = right - w.clientArea.local.x;
        w.clientArea.local.height = bot - w.clientArea.local.y;
        w.clientArea.screen.width = w.clientArea.local.width;
        w.clientArea.screen.height = w.clientArea.local.height;
    };


    return handlers;
}
