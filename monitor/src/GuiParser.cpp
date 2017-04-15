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
using namespace sb::utility;
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

    if(mDraws == nullptr)
    {
        return;
    }

    mHalfFrameWidth = frame.width / 2.f;
    mHalfFrameHeight = frame.height / 2.f;

    mBaseNames.resize(mDraws->size());
    std::transform(mDraws->begin(), mDraws->end(), mBaseNames.begin(), [](const GuiDraw& d)
    {
        return file::basenameNoExt(d.name);
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
//            std::cout << "Unhandled: " << mBaseNames[i] << std::endl;
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

    const GuiDraw* border = pass1.hpManaBorders[0].draw;
    float borderWidth = border->botRight.x - border->topLeft.x;

    const GuiDraw* mana = pass1.manaFill.draw;
    const GuiDraw* hp = pass1.hpFill.draw;

    DefaultSideBar::StatusBar& sb = mData.game.defaultSideBar.statusBar;
    sb.manaPercent = (mana ? mana->botRight.x - mana->topLeft.x : 0.f) / borderWidth;
    sb.hpPercent = (hp ? hp->botRight.x - hp->topLeft.x : 0.f) / borderWidth;

    std::reverse(mData.game.sideBarWindows.containers.begin(), mData.game.sideBarWindows.containers.end());
    std::reverse(mData.game.sideBarWindows.windows.begin(), mData.game.sideBarWindows.windows.end());
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
    ir.x = round(topLeft.x);
    ir.y = round(topLeft.y);
    ir.width = round(botRight.x - topLeft.x);
    ir.height = round(botRight.y - topLeft.y);

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
    SpellBar& sb = gd.spellBar;
    Vip& vip = sbw.vip;
    Prey& prey = sbw.prey;
    DefaultSideBar::MiniMap& miniMap = dsb.miniMap;
    DefaultSideBar::Inventory& inventory = dsb.inventory;
    DefaultSideBar::PremiumFeatures& premiumFeatures = dsb.premiumFeatures;
    MiscData& misc = mData.misc;
    std::map<std::string, std::function<void(size_t&)>> handlers;
    auto createUniqueButtonHandlers = [this](const std::vector<std::pair<std::string, UniqueButton::Type>>& buttons,
                                             std::map<std::string, std::function<void(size_t&)>>& handlers,
                                             const std::string& down,
                                             const std::string& up)
    {
        for(auto& pair : buttons)
        {
            UniqueButton::Type type = pair.second;
            handlers[pair.first + up] = [type, this](size_t& i)
            {
                mData.game.uniqueButtons[(size_t)type] = UniqueButton((*mDraws)[i], type, false);
            };
            handlers[pair.first + down] = [type, this](size_t& i)
            {
                mData.game.uniqueButtons[(size_t)type] = UniqueButton((*mDraws)[i], type, true);
            };
        }
    };

    using UBT = UniqueButton::Type;
    createUniqueButtonHandlers
    ({
        {"automap-button-movedown", UBT::MINI_MAP_DOWN},
        {"automap-button-moveup", UBT::MINI_MAP_UP},
        {"automap-button-zoomout", UBT::MINI_MAP_ZOOM_OUT},
        {"automap-button-zoomin", UBT::MINI_MAP_ZOOM_IN},

        {"button-store-inbox", UBT::INVENTORY_STORE_INBOX},


        {"button-combat-dovemode", UBT::COMBAT_EXPERT_DOVE},
        {"button-combat-redfistmode", UBT::COMBAT_EXPERT_RED},
        {"button-combat-whitehandmode", UBT::COMBAT_EXPERT_WHITE},
        {"button-combat-yellowhandmode", UBT::COMBAT_EXPERT_YELLOW},
        {"button-combat-pvp", UBT::COMBAT_PVP},
        {"button-combat-pvp-small", UBT::COMBAT_PVP},
        {"button-expert", UBT::COMBAT_EXPERT},
        {"button-expert-small", UBT::COMBAT_EXPERT},
        {"button-follow", UBT::COMBAT_FOLLOW},
        {"button-stand", UBT::COMBAT_STAND},
        {"button-combat-defensive", UBT::COMBAT_DEFENSIVE},
        {"button-combat-balanced", UBT::COMBAT_BALANCED},
        {"button-combat-offensive", UBT::COMBAT_OFFENSIVE},


        {"button-menu", UBT::CONTROL_MIN_MAX},
        {"button-logout", UBT::CONTROL_LOGOUT},
        {"button-options", UBT::CONTROL_OPTIONS},
        {"button-unjustpoints", UBT::CONTROL_UNJUSTIFIED_POINTS},
        {"button-questlog", UBT::CONTROL_QUEST_LOG},
        {"button-viplist", UBT::CONTROL_VIP},
        {"button-battlelist", UBT::CONTROL_BATTLE},
        {"button-skills", UBT::CONTROL_SKILLS},
        {"button-preywidget", UBT::CONTROL_PREY},

        {"button-store", UBT::DEFAULT_SIDE_BAR_STORE},

        {"button-store-storexp", UBT::SKILLS_XP_BOOST},

        {"battlelist-monster", UBT::BATTLE_MONSTER},
        {"battlelist-npc", UBT::BATTLE_NPC},
        {"battlelist-party", UBT::BATTLE_PARTY},
        {"battlelist-players", UBT::BATTLE_PLAYER},
        {"battlelist-skull", UBT::BATTLE_SKULL},
        {"button-expand-12x12", UBT::BATTLE_FILTER_MIN_MAX},

        {"console-buttonignore", UBT::CHAT_IGNORE},
        {"console-buttonnew", UBT::CHAT_NEW_TAB},
        {"console-buttonmessages", UBT::CHAT_SERVER_MESSAGES},
        {"console-buttonclose", UBT::CHAT_CLOSE_TAB},
        {"console-buttonleft", UBT::CHAT_TAB_LEFT},
        {"console-buttonright", UBT::CHAT_TAB_RIGHT},
        {"console-buttonleft-flash", UBT::CHAT_TAB_LEFT},
        {"console-buttonright-flash", UBT::CHAT_TAB_RIGHT},
        {"console-buttonleft-update", UBT::CHAT_TAB_LEFT},
        {"console-buttonright-update", UBT::CHAT_TAB_RIGHT},
        {"console-buttonsay", UBT::CHAT_SAY},
        {"console-buttonwhisper", UBT::CHAT_WHISPER},
        {"console-buttonyell", UBT::CHAT_YELL},

//            SPELL_BAR_ATTACK,
//            SPELL_BAR_HEALING,
//            SPELL_BAR_SUPPORT,
//            SPELL_BAR_SPECIAL,

        {"button-sidebar-add", UBT::SIDE_BAR_ADD},
        {"button-sidebar-remove", UBT::SIDE_BAR_REMOVE},

        {"button-transfercoins", UBT::TRANSFER_COINS},
        {"button-copytoclipboard", UBT::CLIP_BOARD},
    }, handlers, "-down", "-up");

    createUniqueButtonHandlers
    ({
        {"button-contextmenu-12x12", UBT::BATTLE_SORT},
    }, handlers, "-pressed", "-idle");

    const std::vector<std::pair<std::string, UniqueButton::Type>> disabledUniqueButtons =
    {
        {"button-expert-disabled", UBT::COMBAT_EXPERT},
        {"button-expert-small-disabled", UBT::COMBAT_EXPERT},
        {"button-sidebar-add-disabled", UBT::SIDE_BAR_ADD},
        {"button-sidebar-remove-disabled", UBT::SIDE_BAR_REMOVE},
    };
    for(auto& pair : disabledUniqueButtons)
    {
        UniqueButton::Type type = pair.second;
        handlers[pair.first] = [type, this](size_t& i)
        {
            mData.game.uniqueButtons[(size_t)type] = UniqueButton((*mDraws)[i], type, false, false);
        };
    }

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
                return round(d.botRight.x - d.topLeft.x) == 1 && round(d.botRight.y - d.topLeft.y) == 1;
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

    auto createPreyBonusHandlers = [this]
    (
        const std::vector<std::pair<std::string, Prey::Bonus::Type>>& types,
        std::vector<Prey::Bonus>& bonuses,
        std::map<std::string, std::function<void(size_t&)>>& handlers
    )
    {
        for(auto& pair : types)
        {
            Prey::Bonus::Type type = pair.second;
            handlers[pair.first] = [&bonuses, this, type](size_t& i)
            {
                Prey::Bonus f;
                f.draw = &(*mDraws)[i];
                f.type = type;
                bonuses.push_back(f);
            };
        }
    };
    using BT = Prey::Bonus::Type;
    createPreyBonusHandlers
    ({
        {"prey-bonus-damage-boost-small", BT::DAMAGE_BOOST},
        {"prey-bonus-damage-reduction-small", BT::DAMAGE_REDUCTION},
        {"prey-bonus-improved-loot-small", BT::IMPROVED_LOOT},
        {"prey-bonus-improved-xp-small", BT::IMPROVED_XP},
        {"prey-bonus-none-small", BT::NONE},
    }, prey.bonuses, handlers);


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

            short width = round(d.botRight.x - d.topLeft.x);
            short height = round(d.botRight.y - d.topLeft.y);
            if(width == 32 && height == 32)
            {
                c.slots.emplace_back(d);
            }
            else if((width == 32 && height == 18) || (width == 30 && height == 40))
            {
                // soul/cap slot
            }
            else
            {
                SB_THROW("Unexpected containerslot size: ", width, "x", height);
            }


            i += 5;
            if(i >= mDraws->size() || (*mDraws)[i].drawCallId != drawCallId || mBaseNames[i] != "containerslot")
            {
                i--;
                if(c.slots.empty())
                {
                    mData.game.sideBarWindows.containers.pop_back();
                }

                if(c.slots.size() > 1)
                {
                    const Vertex& v1 = c.slots.front().draw->topLeft;
                    const Vertex& v2 = c.slots.back().draw->topLeft;
                    if(v1.x > v2.x || v1.y > v2.y)
                    {
                        std::reverse(c.slots.begin(), c.slots.end());
                    }
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

        w.titleBar.local.x = round(d->topLeft.x);
        w.titleBar.local.y = round(d->topLeft.y);

        Vertex v;
        d->getScreenCoords(mHalfFrameWidth, mHalfFrameHeight, v.x, v.y, d->topLeft.x, d->topLeft.y);
        w.titleBar.screen.x = round(v.x);
        w.titleBar.screen.y = round(v.y);

        i += 3;
        SB_EXPECT(i, <, mDraws->size());
        d = &(*mDraws)[i];
        SB_EXPECT(mBaseNames[i], ==, "widget-borderimage");
        SB_EXPECT(d->drawCallId, ==, drawCallId);

        w.titleBar.local.width = round(d->botRight.x) - w.titleBar.local.x;
        w.titleBar.local.height = round(d->botRight.y) - w.titleBar.local.y;
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
        SB_EXPECT(w.clientArea.local.x, ==, round(d->topLeft.x));
        SB_EXPECT(w.clientArea.local.y, ==, round(d->topLeft.y));

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
        unsigned short right = round(d->botRight.x);
        unsigned short bot = round(d->botRight.y);
        SB_EXPECT(right, ==, w.titleBar.local.x + w.titleBar.local.width);
        w.clientArea.local.width = right - w.clientArea.local.x;
        w.clientArea.local.height = bot - w.clientArea.local.y;
        w.clientArea.screen.width = w.clientArea.local.width;
        w.clientArea.screen.height = w.clientArea.local.height;
    };


    return handlers;
}
