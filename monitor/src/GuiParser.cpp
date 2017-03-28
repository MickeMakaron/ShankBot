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

GuiParser::GuiParser(const TibiaContext& context)
: mContext(context)
, mGuiDrawHandlers(initGuiDrawHandlers())
{
}

void GuiParser::update(const Frame& frame)
{
    mCurrentFrame = frame;
    mData = Data();
    pass1 = Pass1();
    mIsCurrentFrameParsed = false;
}

const GuiParser::Data& GuiParser::getData()
{
    parseCurrentFrame();
    return mData;
}

void GuiParser::parseCurrentFrame()
{
    if(mIsCurrentFrameParsed)
        return;

    mHalfFrameWidth = mCurrentFrame.width / 2.f;
    mHalfFrameHeight = mCurrentFrame.height / 2.f;

    for(const GuiDraw& g : *mCurrentFrame.guiDraws)
    {
        auto foundIt = mGuiDrawHandlers.find(sb::utility::file::basename(g.name));
        if(foundIt != mGuiDrawHandlers.end())
        {
            foundIt->second(g);
        }
        else
        {
            std::cout << "Unhandled: " << sb::utility::file::basename(g.name) << std::endl;
        }
    }

    runPass1();

    mIsCurrentFrameParsed = true;
}

IRect GuiParser::merge(const IRect& r1, const IRect& r2) const
{

}

DrawRect GuiParser::merge(const DrawRect& r1, const DrawRect& r2) const
{

}

void GuiParser::assembleScrollBars()
{
//    auto assembleHandles = [](const std::vector<DrawRect>& handles)
//    {
//        assert(handles.size() % 3 == 0);
//        std::vector<DrawRect> out;
//        for(size_t i = 0; i + 2 < handles.size(); i += 3)
//        {
//            const DrawRect& min = handles[i];
//            const DrawRect& max = handles[i + 2];
//
//            DrawRect handle = min;
//            handle.local.width = max.local.x - min.local.x;
//            handle.local.height = max.local.y - min.local.y;
//            handle.screen.width = max.screen.x - min.screen.x;
//            handle.screen.height = max.screen.y - min.screen.y;
//            out.push_back(handle);
//        }
//        return out;
//    };
//
//    std::vector<DrawRect> vHandles = assembleHandles(pass1.scrollVerticalHandles);
//    std::vector<DrawRect> hHandles = assembleHandles(pass1.scrollHorizontalHandles);
//
//    auto assembleButtons = [](const std::vector<TmpButton>& min, const std::vector<TmpButton>& max, std::vector<DrawRect>& handles, bool isHorizontal)
//    {
//        assert(min.size() == max.size());
//        std::vector<ScrollBar> out;
//        for(size_t i = 0; i < min.size(); i++)
//        {
//            ScrollBar bar;
//            bar.isHorizontal = isHorizontal;
//            bar.min = min[i];
//            bar.max = max[i];
//            out.push_back(bar);
//        }
//        return out;
//    };
//
//    std::vector<ScrollBar>
//
//
//    ScrollBar bar;

}

void GuiParser::runPass1()
{
//    assembleScrollBars();


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

TmpButton GuiParser::createButton(const Draw& d, bool isDown, bool isEnabled)
{
    TmpButton b;
    b.screenRect = getScreenRect(d);
    b.localRect = getRect(d.topLeft, d.botRight);
    b.isDown = isDown;
    b.isEnabled = isEnabled;
    return b;
}



std::map<std::string, std::function<void(const GuiDraw&)>> GuiParser::initGuiDrawHandlers()
{


    auto createButtonHandlers = [this]
    (
        const std::vector<std::pair<std::string, TmpButton&>>& buttons,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers,
        const std::string& down,
        const std::string& up
    )
    {
        for(auto& pair : buttons)
        {
            TmpButton& b = pair.second;
            handlers[pair.first + up] = [&b, this](const GuiDraw& d)
            {
                b = createButton(d, false);
            };
            handlers[pair.first + down] = [&b, this](const GuiDraw& d)
            {
                b = createButton(d, true);
            };
        }
    };

    auto createButtonVecHandlers = [this]
    (
        const std::vector<std::pair<std::string, std::vector<TmpButton>&>>& buttons,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers,
        const std::string& down,
        const std::string& up
    )
    {
        for(auto& pair : buttons)
        {
            std::vector<TmpButton>& b = pair.second;
            handlers[pair.first + up] = [&b, this](const GuiDraw& d)
            {
                b.push_back(createButton(d, false));
            };
            handlers[pair.first + down] = [&b, this](const GuiDraw& d)
            {
                b.push_back(createButton(d, true));
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
    std::map<std::string, std::function<void(const GuiDraw&)>> handlers;
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
    }, handlers, "-down.png", "-up.png");

    createButtonVecHandlers
    ({
        {"max-button-small", pass1.smallMaxButtons},
        {"max-button-small-highlight", pass1.smallMaxButtons},
        {"min-button-small", pass1.smallMinButtons},
        {"exit-button-small", pass1.smallExitButtons},
        {"scrollbar-buttondown", pass1.scrollDownButtons},
        {"scrollbar-buttonup", pass1.scrollUpButtons},
        {"scrollbar-buttonright", pass1.scrollRightButtons},
        {"scrollbar-buttonleft", pass1.scrollLeftButtons},
        {"slider-buttonleft", pass1.scrollLeftButtons},
        {"slider-buttonright", pass1.scrollRightButtons},

        {"container-level-up", pass1.containerUpButtons},
        {"container-browse-left", pass1.containerLeftButtons},
        {"container-browse-right", pass1.containerRightButtons},

        {"button-blue", pass1.blueButtons},
        {"button-gold", pass1.goldButtons},
        {"button-green", pass1.greenButtons},
        {"button-grey", pass1.greyButtons},

        {"button-arrow-left", pass1.outfitLeftButtons},
        {"button-arrow-right", pass1.outfitRightButtons},
        {"button-storeicon", pass1.outfitStoreButtons},
    }, handlers, "-down.png", "-up.png");

    const std::vector<std::pair<std::string, TmpButton&>> disabledButtons =
    {
        {"button-expert-disabled.png", combatControls.expert},
        {"button-expert-small-disabled.png", combatControls.expert},
        {"button-sidebar-add-disabled.png", gd.addSideBar},
        {"button-sidebar-remove-disabled.png", gd.removeSideBar},
    };
    for(auto& pair : disabledButtons)
    {
        TmpButton& b = pair.second;
        handlers[pair.first] = [&b, this](const GuiDraw& d)
        {
            b = createButton(d, false, false);
        };
    }

    const std::vector<std::pair<std::string, std::vector<TmpButton>&>> disabledVecButtons =
    {
        {"radiobutton-checked-disabled.png", pass1.radioButtons},
    };
    for(auto& pair : disabledVecButtons)
    {
        std::vector<TmpButton>& b = pair.second;
        handlers[pair.first] = [&b, this](const GuiDraw& d)
        {
            b.push_back(createButton(d, false, false));
        };
    }


    createButtonVecHandlers
    ({
        {"checkbox", pass1.checkBoxes},
        {"radiobutton", pass1.radioButtons},
    }, handlers, "-checked.png", "-unchecked.png");

    createButtonHandlers
    ({
        {"button-contextmenu-12x12", battleList.sort},
    }, handlers, "-pressed.png", "-idle.png");

    createButtonVecHandlers
    ({
        {"button-dropdown-20x20", pass1.dropDownButtons},
    }, handlers, "-pressed.png", "-idle.png");

    createButtonVecHandlers
    ({
        {"console-tab", cw.tabs},
    }, handlers, "-active.png", "-passive.png");

    handlers["button-9grid-idle.png"] = [this](const GuiDraw& d)
    {
        TmpButton b = createButton(d, false);
        if(b.screenRect.width > 3 && b.screenRect.height > 3)
        {
            pass1.grid9Buttons.push_back(b);
        }
    };
    handlers["button-9grid-pressed.png"] = [this](const GuiDraw& d)
    {
        TmpButton b = createButton(d, true);
        if(b.screenRect.width > 3 && b.screenRect.height > 3)
        {
            pass1.grid9Buttons.push_back(b);
        }
    };
    handlers["button-textured-up.png"] = [this](const GuiDraw& d)
    {
        TmpButton b = createButton(d, false);
        if(b.screenRect.width > 10 && b.screenRect.height > 10)
        {
            pass1.texturedButtons.push_back(b);
        }
    };
    handlers["button-textured-down.png"] = [this](const GuiDraw& d)
    {
        TmpButton b = createButton(d, true);
        if(b.screenRect.width > 10 && b.screenRect.height > 10)
        {
            pass1.texturedButtons.push_back(b);
        }
    };

    auto createDrawRectHandlers = [this]
    (
        const std::vector<std::pair<std::string, DrawRect&>> rects,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers
    )
    {
        for(auto& pair : rects)
        {
            DrawRect& r = pair.second;
            handlers[pair.first] = [&r, this](const GuiDraw& d)
            {
                r = getDrawRect(d);
            };
        }
    };
    auto createDrawRectVecHandlers = [this]
    (
        const std::vector<std::pair<std::string, std::vector<DrawRect>&>> rects,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers
    )
    {
        for(auto& pair : rects)
        {
            std::vector<DrawRect>& r = pair.second;
            handlers[pair.first] = [&r, this](const GuiDraw& d)
            {
                r.push_back(getDrawRect(d));
            };
        }
    };

    createDrawRectHandlers
    ({
        {"automap-rose.png", miniMap.rose},
        {"automap-crosshair-dark.png", miniMap.crosshair},
        {"automap-crosshair-light.png", miniMap.crosshair},
        {"hitpoints-bar-filled.png", pass1.hpFill},
        {"mana-bar-filled.png", pass1.manaFill},
     }, handlers);

     handlers["icon-adventurers-blessing.png"] = [this](const GuiDraw& d)
     {
         mData.game.defaultSideBar.hasAdventurersBlessing = true;
     };

    createDrawRectVecHandlers
    ({
        {"hitpoints-manapoints-bar-border.png", pass1.hpManaBorders},
        {"spellgroup-icons-attack.png", pass1.spellGroups},
        {"spellgroup-icons-healing.png", pass1.spellGroups},
        {"spellgroup-icons-support.png", pass1.spellGroups},
        {"spellgroup-icons-special.png", pass1.spellGroups},

        {"scrollbar-handle-vertical.png", pass1.scrollVerticalHandles},
        {"scrollbar-handle-horizontal.png", pass1.scrollHorizontalHandles},
     }, handlers);

    auto createCreatureFlagHandlers = [this]
    (
        const std::vector<std::pair<std::string, CreatureFlag::Type>>& types,
        std::vector<CreatureFlag>& flags,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers
    )
    {
        for(auto& pair : types)
        {
            CreatureFlag::Type type = pair.second;
            handlers[pair.first] = [&flags, this, type](const GuiDraw& d)
            {
                CreatureFlag f;
                f.rect = getDrawRect(d);
                f.type = type;
                flags.push_back(f);
            };
        }
    };


    using CF = CreatureFlag::Type;
    createCreatureFlagHandlers
    ({
        {"creature-speech-flags-directions.png", CF::DIRECTIONS},
        {"creature-speech-flags-quest.png", CF::QUEST},
        {"creature-speech-flags-talk.png", CF::TALK},
        {"creature-speech-flags-trade.png", CF::TRADE},
        {"creature-state-flags-guild-war-blue.png", CF::GUILD_WAR_BLUE},
        {"creature-state-flags-guild-war-green.png", CF::GUILD_WAR_GREEN},
        {"creature-state-flags-guild-war-red.png", CF::GUILD_WAR_RED},
        {"creature-state-flags-guild-white.png", CF::GUILD_WHITE},
        {"creature-state-flags-guild-yellow.png", CF::GUILD_YELLOW},
        {"creature-state-flags-lightning-red.png", CF::LIGHTNING_RED},
        {"creature-state-flags-party-gray-unknown.png", CF::PARTY_GRAY},
        {"creature-state-flags-party-invitee.png", CF::PARTY_INVITEE},
        {"creature-state-flags-party-inviter.png", CF::PARTY_INVITER},
        {"creature-state-flags-party-leader.png", CF::PARTY_LEADER},
        {"creature-state-flags-party-leader-shared-xp.png", CF::PARTY_LEADER_SHARED_XP},
        {"creature-state-flags-party-leader-shared-xp-fail.png", CF::PARTY_LEADER_SHARED_XP_FAIL},
        {"creature-state-flags-party-member.png", CF::PARTY_MEMBER},
        {"creature-state-flags-party-member-shared-xp.png", CF::PARTY_MEMBER_SHARED_XP},
        {"creature-state-flags-party-member-shared-xp-fail.png", CF::PARTY_MEMBER_SHARED_XP_FAIL},
        {"creature-state-flags-skull-black.png", CF::SKULL_BLACK},
        {"creature-state-flags-skull-green.png", CF::SKULL_GREEN},
        {"creature-state-flags-skull-orange.png", CF::SKULL_ORANGE},
        {"creature-state-flags-skull-red.png", CF::SKULL_RED},
        {"creature-state-flags-skull-white.png", CF::SKULL_WHITE},
        {"creature-state-flags-skull-yellow.png", CF::SKULL_YELLOW},
        {"creature-state-flags-summon-green.png", CF::SUMMON_GREEN},
        {"creature-state-flags-summon-red.png", CF::SUMMON_RED},
    }, gw.creatureFlags, handlers);


    auto createPlayerStateFlagHandlers = []
    (
        const std::vector<std::pair<std::string, uint32_t>>& states,
        uint32_t& flags,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers
    )
    {
        for(auto& pair : states)
        {
            uint32_t state = pair.second;
            handlers[pair.first] = [&flags, state](const GuiDraw& d)
            {
                flags |= state;
            };
        }
    };

    namespace PS = PlayerState;
    createPlayerStateFlagHandlers
    ({
        {"player-state-flags-poisoned.png", PS::POISONED},
        {"player-state-flags-burning.png", PS::BURNING},
        {"player-state-flags-electrified.png", PS::ELECTRIFIED},
        {"player-state-flags-drunk.png", PS::DRUNK},
        {"player-state-flags-magic-shield.png", PS::MAGIC_SHIELD},
        {"player-state-flags-slowed.png", PS::SLOWED},
        {"player-state-flags-haste.png", PS::HASTE},
        {"player-state-flags-logout-block.png", PS::LOGOUT_BLOCK},
        {"player-state-flags-drowning.png", PS::DROWNING},
        {"player-state-flags-freezing.png", PS::FREEZING},
        {"player-state-flags-dazzled.png", PS::DAZZLED},
        {"player-state-flags-cursed.png", PS::CURSED},
        {"player-state-flags-strengthened.png", PS::STRENGTHENED},
        {"player-state-flags-protection-zone-block.png", PS::PROTECTION_ZONE_BLOCK},
        {"player-state-flags-protection-zone.png", PS::PROTECTION_ZONE},
        {"player-state-flags-bleeding.png", PS::BLEEDING},
        {"player-state-flags-hungry.png", PS::HUNGRY},
        {"player-state-guildwar-flag.png", PS::GUILD_WAR},
        {"player-state-playerkiller-flags-black.png", PS::SKULL_BLACK},
        {"player-state-playerkiller-flags-green.png", PS::SKULL_GREEN},
        {"player-state-playerkiller-flags-orange.png", PS::SKULL_ORANGE},
        {"player-state-playerkiller-flags-red.png", PS::SKULL_RED},
        {"player-state-playerkiller-flags-white.png", PS::SKULL_WHITE},
        {"player-state-playerkiller-flags-yellow.png", PS::SKULL_YELLOW},
    }, dsb.conditions, handlers);

    auto createMiniMapMarkerHandlers = [this]
    (
        const std::vector<std::pair<std::string, MiniMapMarker::Type>>& types,
        std::vector<MiniMapMarker>& markers,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers
    )
    {
        for(auto& pair : types)
        {
            MiniMapMarker::Type type = pair.second;
            handlers[pair.first] = [&markers, this, type](const GuiDraw& d)
            {
                MiniMapMarker f;
                f.rect = getDrawRect(d);
                f.type = type;
                markers.push_back(f);
            };
        }
    };

    using MT = MiniMapMarker::Type;
    createMiniMapMarkerHandlers
    ({
        {"markers-checkmark.png", MT::CHECKMARK},
        {"markers-questionmark.png", MT::QUESTIONMARK},
        {"markers-exclamationmark.png", MT::EXCLAMATIONMARK},
        {"markers-star.png", MT::STAR},
        {"markers-crossmark.png", MT::CROSSMARK},
        {"markers-cross.png", MT::CROSS},
        {"markers-mouth.png", MT::MOUTH},
        {"markers-brush.png", MT::BRUSH},
        {"markers-sword.png", MT::SWORD},
        {"markers-flag.png", MT::FLAG},
        {"markers-lock.png", MT::LOCK},
        {"markers-bag.png", MT::BAG},
        {"markers-skull.png", MT::SKULL},
        {"markers-dollar.png", MT::DOLLAR},
        {"markers-red-up.png", MT::RED_UP},
        {"markers-red-down.png", MT::RED_DOWN},
        {"markers-red-right.png", MT::RED_RIGHT},
        {"markers-red-left.png", MT::RED_LEFT},
        {"markers-green-up.png", MT::GREEN_UP},
        {"markers-green-down.png", MT::GREEN_DOWN},
    }, miniMap.markers, handlers);

    auto createEmptyEquipmentSlotHandler = [this]
    (
        const std::vector<std::pair<std::string, EmptyEquipmentSlot::Type>>& types,
        std::vector<EmptyEquipmentSlot>& slots,
        std::map<std::string, std::function<void(const GuiDraw&)>>& handlers
    )
    {
        for(auto& pair : types)
        {
            EmptyEquipmentSlot::Type type = pair.second;
            handlers[pair.first] = [&slots, this, type](const GuiDraw& d)
            {
                EmptyEquipmentSlot f;
                f.rect = getDrawRect(d);
                f.type = type;
                slots.push_back(f);
            };
        }
    };

    using ET = EmptyEquipmentSlot::Type;
    createEmptyEquipmentSlotHandler
    ({
        {"inventory-back.png", ET::BACK},
        {"inventory-feet.png", ET::FEET},
        {"inventory-finger.png", ET::FINGER},
        {"inventory-head.png", ET::HEAD},
        {"inventory-hip.png", ET::HIP},
        {"inventory-left-hand.png", ET::LEFT_HAND},
        {"inventory-right-hand.png", ET::RIGHT_HAND},
        {"inventory-legs.png", ET::LEGS},
        {"inventory-neck.png", ET::NECK},
        {"inventory-torso.png", ET::TORSO},
    }, inventory.emptySlots, handlers);


    handlers["containerslot.png"] = [this](const GuiDraw& d)
    {
        DrawRect r = getDrawRect(d);
        if(r.screen.width > 2 && r.screen.height > 2)
        {
            pass1.containerSlots.push_back(r);
        }
    };


    return handlers;
}
