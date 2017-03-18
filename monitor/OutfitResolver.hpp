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
#ifndef GRAPHICS_LAYER_OUTFIT_RESOLVER_HPP
#define GRAPHICS_LAYER_OUTFIT_RESOLVER_HPP


///////////////////////////////////
// Internal ShankBot headers
#include "Scene.hpp"
#include "Text.hpp"
namespace GraphicsLayer
{
    class Frame;
    class GraphicsMonitorReader;
    class TibiaContext;
}
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
#include <list>
#include <map>
///////////////////////////////////

namespace GraphicsLayer
{

class OutfitResolver
{
    public:
        struct Player
        {
            std::string name;
            short x;
            short y;
            float hp;
        };

        struct Npc
        {
            std::string name;
            short x;
            short y;
            size_t object;
        };

        struct Creature
        {
            std::string name;
            short x;
            short y;
            float hp;
            size_t object;
        };

        struct UnknownPlayerCreature
        {
            std::string name;
            short x;
            short y;
            float hp;
            size_t object = -1;
        };

    private:
        struct HpBar
        {
            short x;
            short y;
            unsigned short width;
            unsigned short height;
            float hp;
        };

        struct TextHpPair
        {
            Text text;
            HpBar hp;
        };

        struct Collision
        {
            std::list<TextHpPair> pairs;
            std::list<Text> names;
        };

        struct Outfit
        {
            enum class Type : unsigned char
            {
                NPC,
                CREATURE,
                PLAYER
            };

            explicit Outfit(std::string name, Type type, size_t object = -1)
            : name(name), type(type), object(object){};

            const std::string name;
            const Type type;
            size_t object = -1;
        };

    public:
        explicit OutfitResolver(const TibiaContext& context);

        void resolve(Scene& scene, const Frame& frame, GraphicsMonitorReader& reader);
        const std::list<Npc>& getNpcs() const;
        const std::list<Creature>& getCreatures() const;
        const std::list<Player>& getPlayers() const;
        void clear();


    private:
        std::list<Scene::Object> getVisibleOutfits(Scene& scene) const;
        std::list<Text> getNames() const;
        std::list<HpBar> getHpBars() const;
        std::list<Collision> removeCollisions(std::list<Text>& names, std::list<TextHpPair>& textHpPairs) const;
        std::list<TextHpPair> pairHpBarsWithNames(std::list<Text>& names, const std::list<HpBar>& hpBars) const; // Removes paired names
        void processNpcs(const std::list<Text>& names, std::list<Scene::Object>& visibleOutfits);
        void processPlayerCreatures(std::list<TextHpPair>& pairs, std::list<Scene::Object>& visibleOutfits);
        void processTypeRequests(GraphicsMonitorReader& reader);

        std::list<Scene::Object>::iterator findMatchingOutfit(short left, short right, short bottom, std::list<Scene::Object>& visibleOutfits) const;


    private:
        const TibiaContext& mContext;
        const Frame* mCurrentFrame;
        const Scene* mCurrentScene;
        std::map<std::string, Outfit> mOutfits;
        std::list<Npc> mNpcs;
        std::list<Creature> mCreatures;
        std::list<Player> mPlayers;
        std::list<UnknownPlayerCreature> mUnknowns;
        std::list<UnknownPlayerCreature> mTypeRequests;

        static const short MAX_X_DISTANCE = 4;
        // This might need tweaking. It seems to increase with a larger window size.
        static const short MAX_Y_DISTANCE = 6;

        float mCurrentTileWidth;
        float mCurrentTileHeight;
};

}


#endif // GRAPHICS_LAYER_OUTFIT_RESOLVER_HPP
