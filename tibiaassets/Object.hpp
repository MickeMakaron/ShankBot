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
#ifndef SB_TIBIAASSETS_OBJECT_HPP
#define SB_TIBIAASSETS_OBJECT_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "tibiaassets/config.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <vector>
#include <string>
///////////////////////////////////

namespace sb
{
namespace tibiaassets
{
    struct Object
    {
        struct FrameInfo
        {
            unsigned short animationDelayMin = 0;
            unsigned short animationDelayMax = 0;
        };

        ///////////////////////////////////

        struct AnimationInfo
        {
            std::vector<FrameInfo> frameInfos;
        };

        ///////////////////////////////////

        struct SpriteInfo
        {
            unsigned char numDirections;
            unsigned char numAddons;
            unsigned char numMounts;
            unsigned char numBlendFrames;
            unsigned char unknown1;

            std::vector<unsigned int> spriteIds;

            AnimationInfo animationInfo;
        };

        ///////////////////////////////////

        enum class AnimationType : unsigned char
        {
            IDLE = 0x00,
            MOVING = 0x01,
        };

        struct SomeInfo
        {
            unsigned char unknown1;
            AnimationType animationType;
            SpriteInfo spriteInfo;
        };

        ///////////////////////////////////

        enum class VocationRestriction : unsigned char
        {
            NONE,
            KNIGHT,
            PALADIN,
            SORCERER,
            DRUID,
            ENUM_END,
        };

        enum class ClassRestriction : unsigned char
        {
            ENUM_BEGIN,
            ARMOR,
            AMULET,
            BOOTS,
            CONTAINER,
            DECORATION,
            FOOD,
            HELMET,
            LEGS,
            OTHER,
            POTION,
            RING,
            RUNE,
            SHIELD,
            TOOL,
            VALUABLE,
            AMMUNITION,
            AXE,
            CLUB,
            DISTANCE,
            SWORD,
            WAND,
            PREMIUM_SCROLL,
            TIBIA_COIN,
            ENUM_END,
            NONE = 0xff,
        };

        struct MarketInfo
        {
            unsigned int id1 = 0;
            unsigned int id2 = 0;
            VocationRestriction vocationRestriction = VocationRestriction::NONE;
            ClassRestriction classRestriction = ClassRestriction::NONE;
            unsigned short levelRestriction = 0;
            std::string name;
        };

        ///////////////////////////////////

        enum class BodyRestriction : unsigned char
        {
            TWO_HANDED,
            HELMET,
            AMULET,
            BACKPACK,
            ARMOR,
            SHIELD,
            WEAPON,
            LEGS,
            BOOTS,
            RING,
            BELT,
            PURSE,
            ENUM_END,
            NONE = 0xff,
        };

        enum class MiniMapColor : unsigned char
        {
            BLACK = 0,
            DARK_GREEN = 12,
            GREEN = 24,
            SLIME_GREEN = 30,
            BLUE = 40,
            LIGHT_BLUE = 51,
            DARK_GRAY = 86,
            BROWN = 114,
            LIGHT_BROWN = 121,
            GRAY = 129,
            LIGHT_GREEN = 140,
            LIGHT_TURQUOISE = 179,
            RED = 186,
            ORANGE = 192,
            BEIGE = 207,
            YELLOW = 210,
            WHITE = 215,
            INVALID = 0xff,
        };

        struct ItemInfo
        {
            bool isGround = false;
            bool isRightClickable = false;
            unsigned short walkSpeed = 0;
            unsigned char topOrder = 0;
            bool isContainer = false;
            bool isStackable = false;
            bool isUsable = false;
            bool isReadable = false;
            bool isWritable = false;
            unsigned short maxCharacters = 0;
            bool isFluid = false;
            bool isSplash = false;
            bool isBlocking = false;
            bool isMovable = true;
            bool isBlockingRangedAttack = false;
            bool isPathBlocking = false;
            bool isPickupable = false;
            bool isHangable = false;
            bool isWall = false;
            bool isRotatable = false;
            bool isFloorChange = false;
            bool hasIdleAnimation = false;
            unsigned char lightBrightness = 0;
            unsigned short lightColor = 0;
            unsigned short height = 0;
            bool hasMinimapColor = false;
            MiniMapColor minimapColor = MiniMapColor::INVALID;
            bool isTransparent = false;
//            unsigned short category = 0;
//            unsigned short marketId1 = 0;
//            unsigned short marketId2 = 0;
//            std::string name;
//            unsigned short profession = 0;
//            unsigned short level = 0;
            unsigned short defaultAction = -1;
            unsigned char offsetX = 0;
            unsigned char offsetY = 0;


            BodyRestriction bodyRestriction = BodyRestriction::NONE;
            bool isItemDestroyer = 0; // full tile?
            bool hasMarketInfo = false;
            MarketInfo marketInfo;
        };


        enum class Type : unsigned char
        {
            ITEM = 0x0a,
            OUTFIT = 0x12,
            PROJECTILE = 0x22,
            EFFECT = 0x1a,
            NONE = 0xff,
        };
        Type type = Type::NONE;
        unsigned int id = 0;
        std::vector<SomeInfo> someInfos;
        ItemInfo itemInfo;
    };
}
}


#endif // SB_TIBIAASSETS_OBJECT_HPP
