// {SHANK_BOT_LICENSE_BEGIN}
/****************************************************************
****************************************************************
*
* ShankBot - Automation software for the MMORPG Tibia.
* Copyright (C) 2016 Mikael Hernvall
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
#ifndef SB_TIBIAASSETS_APPEARANCES_READER_HPP
#define SB_TIBIAASSETS_APPEARANCES_READER_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "tibiaassets/config.hpp"
#include "tibiaassets/Object.hpp"
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
class SHANK_BOT_TIBIAASSETS_DECLSPEC AppearancesReader
{
    private:
        enum class FrameInfoOpCode : unsigned char
        {
            ANIMATION_DELAY_MIN = 0x08,
            ANIMATION_DELAY_MAX = 0x10,
        };

        enum class AnimationInfoOpCode : unsigned char
        {
            UNKNOWN1 = 0x08,
            UNKNOWN2 = 0x10,
            UNKNOWN3 = 0x20,
            UNKNOWN4 = 0x18,
            FRAME_INFO = 0x32,
            UNKNOWN5 = 0x28,
        };

        enum class SpriteInfoOpCode : unsigned char
        {
            NUM_DIRECTIONS = 0x08,
            NUM_ADDONS = 0x10,
            NUM_MOUNTS = 0x18,
            NUM_BLEND_FRAMES = 0x20,
            UNKNOWN1 = 0x40,
            SPRITE_ID = 0x28,
            ANIMATION_INFO = 0x32,
            UNKNOWN2 = 0x38,
            UNKNOWN3 = 0x4a,
        };

        enum class SomeInfoOpCode : unsigned char
        {
            UNKNOWN1 = 0x8,
            ANIMATION_TYPE = 0x10,
            SPRITE_INFO = 0x1a,
        };

        enum class MarketInfoOpCode : unsigned char
        {
            MARKET_ID_1 = 0x10,
            MARKET_ID_2 = 0x18,
            VOCATION_RESTRICTION = 0x28,
            CLASS_RESTRICTION = 0x08,
            LEVEL_RESTRICTION = 0x30,
            NAME = 0x22,
        };

        enum class ItemInfoOpCode : unsigned short
        {
            UNKNOWN1 = 0x38, // Is 0xfe in old client ("Usable?").
            USABLE = 0x48,
            IMMOVABLE = 0x70,
            STACKABLE = 0x30,
            BLOCKING = 0x68,
            GROUND_INFO = 0x0a,
            MINIMAP_INFO = 0xf2,
            OFFSET_INFO = 0xd2,
            LIGHT_INFO = 0xba,
            MARKET_INFO = 0x122,
            HEIGHT_INFO = 0xda,
            BLOCKING_PATH = 0x80,
            BLOCKING_RANGED_ATTACK = 0x78,
            PICKUPABLE = 0x90,
            CONTAINER = 0x28,
            UNKNOWN6 = 0x40, // Is 0x06 in old client
            TOP_ORDER_2 = 0x18,
            BODY_RESTRICTION = 0x112,
            UNKNOWN9 = 0xe0, // UNKNOWN in old client as well (0x1b)
            TOP_ORDER_3 = 0x20,
            TOP_ORDER_1 = 0x10,
            FLOOR_CHANGE = 0xc8,
            DEFAULT_ACTION = 0x11a,
            TRANSPARENT_TILE = 0x108,
            UNKNOWN15 = 0xfa,
            FLUID = 0x98,
            HANGABLE = 0xa0,
            WALL_DIRECTION = 0xaa,
            ROTATABLE = 0xb0,
            UNKNOWN20 = 0x128,
            FULL_TILE = 0x100,
            UNKNOWN22 = 0x88,
            WRITABLE = 0x52,
            READABLE = 0x5a,
            SPLASH = 0x60,
            UNKNOWN26 = 0xc0,
            UNKNOWN27 = 0x130,
            UNKNOWN28 = 0xe8,
        };

        enum class ObjectOpCode : unsigned char
        {
            ID = 0x08,
            SOME_INFO = 0x12,
            ITEM_INFO = 0x1a,
        };

        enum class GroundInfoOpCode : unsigned char
        {
            WALK_SPEED = 0x08
        };

        enum class MinimapInfoOpCode : unsigned char
        {
            COLOR = 0x08
        };

        enum class OffsetInfoOpCode : unsigned char
        {
            OFFSET_X = 0x08,
            OFFSET_Y = 0x10,
        };

        enum class HeightInfoOpCode : unsigned char
        {
            HEIGHT = 0x08
        };

        enum class LightInfoOpCode : unsigned char
        {
            DISTANCE = 0x08,
            COLOR = 0x10,
        };

    public:
        explicit AppearancesReader(std::string path);
        const std::vector<Object>& getObjects() const;
    private:
        void readAppearances(std::string path);

        bool readBoolean(std::istream& stream) const;
        std::string readString(std::istream& stream) const;

        Object readObject(std::istream& stream) const;

        Object::SomeInfo readSomeInfo(std::istream& stream) const;
        Object::SpriteInfo readSpriteInfo(std::istream& stream) const;
        Object::AnimationInfo readAnimationInfo(std::istream& stream) const;
        Object::FrameInfo readFrameInfo(std::istream& stream) const;

        Object::ItemInfo readItemInfo(std::istream& stream) const;
        Object::BodyRestriction readBodyRestriction(std::istream& stream) const;
        Object::VocationRestriction readVocationRestriction(std::istream& stream) const;
        Object::ClassRestriction readClassRestriction(std::istream& stream) const;
        Object::MarketInfo readMarketInfo(std::istream& stream) const;
        void readLightInfo(unsigned char& distance, unsigned short& color, std::istream& stream) const;
        void readOffset(unsigned char& x, unsigned char& y, std::istream& stream) const;
        unsigned short readWalkSpeed(std::istream& stream) const;
        Object::MiniMapColor readMinimapColor(std::istream& stream) const;
        unsigned char readHeight(std::istream& stream) const;
        unsigned short readMaxCharacters(std::istream& stream) const;
        unsigned char readDefaultAction(std::istream& stream) const;

        void readItemUnknown15(std::istream& stream) const;

    private:
        unsigned short mNumItems;
        unsigned short mNumOutfits;
        unsigned short mNumEffects;
        unsigned short mNumProjectiles;

        std::vector<Object> mObjects;
};
}
}


#endif // SB_TIBIAASSETS_APPEARANCES_READER_HPP
