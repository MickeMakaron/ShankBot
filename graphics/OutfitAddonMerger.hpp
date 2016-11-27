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
#ifndef GRAPHICS_LAYER_OUTFIT_ADDON_MERGER_HPP
#define GRAPHICS_LAYER_OUTFIT_ADDON_MERGER_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "Sprite.hpp"
namespace sb
{
namespace tibiaassets
{
    class Object;
}
}
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <memory>
#include <set>
#include <map>
#include <list>
///////////////////////////////////

namespace GraphicsLayer
{
    class OutfitAddonMerger
    {
        private:
            class SnapshotMerger
            {
                public:
                    struct OutfitSnapshot
                    {
                        std::vector<size_t> spriteIds;
                    };

                public:
                    SnapshotMerger(const OutfitSnapshot& snap);
                    void process(const std::shared_ptr<Sprite>& spr);

                    bool isReady() const;
                    const std::set<size_t>& getNeededSprites() const;

                    std::vector<std::shared_ptr<Sprite>> merge() const;

                private:
                    std::set<size_t> mNeededSprites;
                    size_t mBaseSpriteId;
                    std::shared_ptr<Sprite> mBaseSprite;
                    std::vector<std::shared_ptr<Sprite>> mAddonSprites;

                    typedef std::vector<size_t> Combination;
                    typedef std::vector<Combination> Combinations;
                    static const std::vector<Combinations> M_COMBINATIONS_MAP;
            };

        public:
            void addOutfit(const sb::tibiaassets::Object& outfit);
            std::vector<std::shared_ptr<Sprite>> processSprite(unsigned int id, size_t width, size_t height, const unsigned char* pixels);
            bool isEmpty() const;

        private:
            std::vector<SnapshotMerger::OutfitSnapshot> getOutfitSnapshots(const sb::tibiaassets::Object& outfit) const;

        private:
            std::map<size_t, std::list<std::shared_ptr<SnapshotMerger>>> mMerges;
    };
}


#endif // GRAPHICS_LAYER_OUTFIT_ADDON_MERGER_HPP
