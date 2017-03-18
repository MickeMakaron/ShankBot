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
#include "monitor/OutfitResolver.hpp"
#include "monitor/Frame.hpp"
#include "tibiaassets/Object.hpp"
#include "monitor/TextBuilder.hpp"
#include "monitor/tibiaIoUtility.hpp"
#include "monitor/TibiaContext.hpp"
using namespace sb::tibiaassets;
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cassert>
#include <iostream>
///////////////////////////////////

OutfitResolver::OutfitResolver(const TibiaContext& context)
: mContext(context)
{

}

void OutfitResolver::resolve(Scene& scene, const Frame& frame, GraphicsMonitorReader& reader)
{
    clear();

    mCurrentFrame = &frame;
    mCurrentScene = &scene;
    mCurrentTileWidth = mCurrentScene->getTileWidth();
    mCurrentTileHeight = mCurrentScene->getTileHeight();
    std::list<Scene::Object> visibleOutfits = getVisibleOutfits(scene);
    std::list<Text> names = getNames();
    std::list<HpBar> hpBars = getHpBars();
    std::list<TextHpPair> pairs = pairHpBarsWithNames(names, hpBars);
    std::list<Collision> collisions = removeCollisions(names, pairs);

    processNpcs(names, visibleOutfits);

    processPlayerCreatures(pairs, visibleOutfits);

    processTypeRequests(reader);
}

void OutfitResolver::clear()
{
    mCurrentFrame = nullptr;
    mCurrentScene = nullptr;
    mNpcs.clear();
    mCreatures.clear();
    mPlayers.clear();
    mUnknowns.clear();
    mTypeRequests.clear();
}

std::list<Scene::Object> OutfitResolver::getVisibleOutfits(Scene& scene) const
{
    std::list<Scene::Object> visibleOutfits;
    scene.forEach([&](const Scene::Tile& tile)
    {
        for(auto it = tile.knownLayerObjects.rbegin(); it != tile.knownLayerObjects.rend(); it++)
        {
            const Object& o = mContext.getObjects()[it->object];
            if(o.type == Object::Type::OUTFIT)
            {
                visibleOutfits.push_back(*it);
                break;
            }
            else if(o.itemInfo.isGround)
                break;
        }
    });
    return visibleOutfits;
}

std::list<Text> OutfitResolver::getNames() const
{
    std::list<Text> names;
    for(const TextDraw& line : *mCurrentFrame->textDraws)
        {
            typedef Text::Type T;
            TextBuilder builder(line, mCurrentFrame->width, mCurrentFrame->height);
            T type = builder.getTextType();
            if(type == T::NAME || type == T::NAME_OBSCURED)
            {
                for(const Text& text : builder.getText())
                {
//                    if(text.x == frame.viewX ||
//                       text.x + text.width == frame.viewX + frame.viewWidth ||
//                       text.y == frame.viewY ||
//                       text.y + text.height == frame.viewY + frame.viewHeight)
//                    {
//                        clampedNames.push_back(text);
//                    }
//                    else
                        names.push_back(text);
                }
            }
        }

    return names;
}

std::list<OutfitResolver::HpBar> OutfitResolver::getHpBars() const
{
    std::list<HpBar> hpBars;
    std::list<HpBar> hpBarBackgrounds;
//    std::cout << "get hp bars: " << std::endl;
    for(const RectDraw& rect : *mCurrentFrame->rectDraws)
    {
        typedef Constants::RectColor Color;
        float screenX;
        float screenY;
        rect.getScreenCoords(float(mCurrentFrame->width) / 2.f, float(mCurrentFrame->height) / 2.f, screenX, screenY);
        float rectWidth = rect.botRight.x - rect.topLeft.x;
        float rectHeight = rect.botRight.y - rect.topLeft.y;

        screenX += 0.5f;
        screenY += 0.5f;
        rectWidth += 0.5f;
        rectHeight += 0.5f;
        Color c = (Color)rect.color.packed;
        switch(c)
        {
            case Color::HP_GRAY:
            case Color::HP_FULL:
            case Color::HP_LIGHT_GREEN:
            case Color::HP_YELLOW:
            case Color::HP_LIGHT_RED:
            case Color::HP_RED:
            case Color::HP_DARK_RED:
                if((int)rectHeight == 2 && screenX < mCurrentFrame->viewX + mCurrentFrame->viewWidth)
                {
                    HpBar b;
                    b.x = screenX;
                    b.y = screenY;
                    b.width = rectWidth;
                    b.height = rectHeight;
                    hpBars.push_back(b);

//                    std::cout << "hp: " << screenX << "x" << screenY << " (" << rectWidth << "x" << rectHeight << ")" << "(" << (int)rect.color.r << ", " << (int)rect.color.g << ", " << (int)rect.color.b << ", " << (int)rect.color.a << ")" << std::endl;
                }
//                else
//                    std::cout << "discarded hp: " << screenX << "x" << screenY << " (" << rectWidth << "x" << rectHeight << ")" << "(" << (int)rect.color.r << ", " << (int)rect.color.g << ", " << (int)rect.color.b << ", " << (int)rect.color.a << ")" << std::endl;

                break;

            case Color::HP_BACKGROUND:
//                if(rectHeight * 4 < rectWidth)
                if((int)rectWidth == 27 && (int)rectHeight == 4)
                {
                    HpBar b;
                    b.x = screenX;
                    b.y = screenY;
                    b.width = rectWidth;
                    b.height = rectHeight;
                    hpBarBackgrounds.push_back(b);

//                    std::cout << "bg: " << screenX << "x" << screenY << " (" << rectWidth << "x" << rectHeight << ")" << "(" << (int)rect.color.r << ", " << (int)rect.color.g << ", " << (int)rect.color.b << ", " << (int)rect.color.a << ")" << std::endl;
                }
//                else
//                    std::cout << "discarded bg: " << screenX << "x" << screenY << " (" << rectWidth << "x" << rectHeight << ")" << "(" << (int)rect.color.r << ", " << (int)rect.color.g << ", " << (int)rect.color.b << ", " << (int)rect.color.a << ")" << std::endl;
                break;


            default:
                break;
        }
    }
    assert(hpBars.size() == hpBarBackgrounds.size());


    for(HpBar& hp : hpBars)
    {
        bool foundBg = false;
        for(auto it = hpBarBackgrounds.begin(); it != hpBarBackgrounds.end(); it++)
        {
            if(hp.x == it->x + 1 && hp.y == it->y + 1)
            {
                foundBg = true;
                hp.x = it->x;
                hp.y = it->y;
                hp.hp = 100.f * float(hp.width) / float(it->width - 2);
                hp.width = it->width;
                hp.height = it->height;
                hpBarBackgrounds.erase(it);
                break;
            }
        }
        assert(foundBg);
    }
    assert(hpBarBackgrounds.empty());
    return hpBars;
}

std::list<OutfitResolver::Collision> OutfitResolver::removeCollisions(std::list<Text>& names, std::list<TextHpPair>& pairs) const
{
    std::list<Collision> collisions;
    const short VIEW_RIGHT = mCurrentFrame->viewX + mCurrentFrame->viewWidth;
    std::function<bool(const Text& t)> addConditionLeft = [](const Text& t){return t.x == 0;};
    std::function<bool(const Text& t)> addConditionRight = [VIEW_RIGHT](const Text& t){return t.x + t.width == VIEW_RIGHT;};
    for(auto it1 = pairs.begin(); it1 != pairs.end();)
    {
        const Text& text = it1->text;
        if(text.x == 0 || text.x + text.width == VIEW_RIGHT)
        {
            Collision collision;


            auto addCondition = text.x == 0 ? addConditionLeft : addConditionRight;
            auto it2 = it1;
            it2++;
            while(it2 != pairs.end())
            {
                if(addCondition(it2->text))
                {
                    collision.pairs.push_back(*it2);
                    pairs.erase(it2++);
                }
                else
                    it2++;
            }
            for(auto nameIt = names.begin(); nameIt != names.end();)
            {
                if(addCondition(*nameIt))
                {
                    collision.names.push_back(*nameIt);
                    names.erase(nameIt++);
                }
                else
                    nameIt++;
            }

            if(!collision.pairs.empty())
            {
                collision.pairs.push_back(*it1);
                pairs.erase(it1++);
                collisions.push_back(collision);
            }
            else
                it1++;
        }
        else
            it1++;
    }

    return collisions;
}

std::list<OutfitResolver::TextHpPair> OutfitResolver::pairHpBarsWithNames(std::list<Text>& names, const std::list<HpBar>& hpBars) const
{
    std::list<TextHpPair> pairs;
    const unsigned short VIEW_RIGHT = mCurrentFrame->viewX + mCurrentFrame->viewWidth;
    for(auto barIt = hpBars.begin(); barIt != hpBars.end(); barIt++)
    {
        const HpBar& b = *barIt;
        unsigned short minDX = -1;
        auto closestName = names.end();
        const unsigned short BAR_MIDDLE_X = b.x + b.width / 2;
        const unsigned short BAR_RIGHT = b.x + b.width;
        for(auto nameIt = names.begin(); nameIt != names.end(); nameIt++)
        {
            short dY = nameIt->y + nameIt->height - b.y;
            if(dY <= MAX_Y_DISTANCE && dY >= -MAX_Y_DISTANCE)
            {
                short dX;
                if(nameIt->x == 0)
                    dX = b.x;
                else if(nameIt->x + nameIt->width == VIEW_RIGHT)
                    dX = VIEW_RIGHT - BAR_RIGHT;
                else
                    dX = nameIt->x + nameIt->width / 2 - BAR_MIDDLE_X;

                if(dX < 0) dX = -dX;
                if(dX < minDX)
                {
                    minDX = dX;
                    closestName = nameIt;
                }

            }
        }

        assert(closestName != names.end());
        TextHpPair pair;
        pair.text = *closestName;
        pair.hp = b;
        pairs.push_back(pair);
        names.erase(closestName);
    }

    assert(pairs.size() == hpBars.size());
    return pairs;
}


std::list<Scene::Object>::iterator OutfitResolver::findMatchingOutfit(short left, short right, short bottom, std::list<Scene::Object>& visibleOutfits) const
{
    auto isMatchY = [bottom](const Scene::Object& o)
    {
        short dY = o.screenY - bottom;
        return dY <= MAX_Y_DISTANCE && dY >= -MAX_Y_DISTANCE;
    };


    const short VIEW_LEFT = mCurrentFrame->viewX;
    const short VIEW_RIGHT = VIEW_LEFT + mCurrentFrame->viewWidth;
    const float TILE_WIDTH = mCurrentTileWidth;
    auto matchingOutfit = visibleOutfits.end();
    if(left == 0)
    {
        short minX = VIEW_RIGHT;
        const short LOWER_BOUND_X = VIEW_LEFT - TILE_WIDTH;
        for(auto outfitIt = visibleOutfits.begin(); outfitIt != visibleOutfits.end(); outfitIt++)
        {
            if(isMatchY(*outfitIt))
            {
                if(outfitIt->screenX < minX && outfitIt->screenX > LOWER_BOUND_X)
                {
                    minX = outfitIt->screenX;
                    matchingOutfit = outfitIt;
                }
            }
        }

    }
    else if(right == VIEW_RIGHT)
    {
        short maxX = VIEW_LEFT;
        const short UPPER_BOUND_X = VIEW_RIGHT;
        for(auto outfitIt = visibleOutfits.begin(); outfitIt != visibleOutfits.end(); outfitIt++)
        {
            if(isMatchY(*outfitIt))
            {
                if(outfitIt->screenX > maxX && outfitIt->screenX < UPPER_BOUND_X)
                {
                    maxX = outfitIt->screenX;
                    matchingOutfit = outfitIt;
                }
            }
        }
    }
    else
    {
        short outfitX = left + ((right - left) - TILE_WIDTH) / 2;
        for(auto outfitIt = visibleOutfits.begin(); outfitIt != visibleOutfits.end(); outfitIt++)
        {
            short dX = outfitIt->screenX - outfitX;
            if(dX < 0) dX = -dX;
            if(isMatchY(*outfitIt) && dX <= MAX_X_DISTANCE)
            {
                matchingOutfit = outfitIt;
                break;
            }
        }
    }

    return matchingOutfit;
}

void OutfitResolver::processNpcs(const std::list<Text>& names, std::list<Scene::Object>& visibleOutfits)
{
    const float TILE_WIDTH = mCurrentTileWidth;
    for(const Text& npcName : names)
    {
        auto matchingOutfit = findMatchingOutfit(npcName.x, npcName.x + npcName.width, npcName.y + npcName.height, visibleOutfits);
        auto result = mOutfits.insert(std::make_pair(npcName.string, Outfit(npcName.string, Outfit::Type::NPC)));
        Outfit& storedOutfit = result.first->second;
        if(!result.second)
            assert(storedOutfit.type == Outfit::Type::NPC);

        Npc npc;
        npc.name = npcName.string;
        if(matchingOutfit != visibleOutfits.end())
        {
            if(storedOutfit.object != -1)
                assert(storedOutfit.object == matchingOutfit->object);
            else
                storedOutfit.object = matchingOutfit->object;

            npc.x = matchingOutfit->screenX;
            npc.y = matchingOutfit->screenY;
            visibleOutfits.erase(matchingOutfit);
        }
        else
        {
            npc.x = npcName.x + (npcName.width - TILE_WIDTH) / 2;
            npc.y = npcName.y + npcName.height;
        }

        npc.object = storedOutfit.object;
        mNpcs.push_back(npc);
    }
}

const std::list<OutfitResolver::Npc>& OutfitResolver::getNpcs() const
{
    return mNpcs;
}

void OutfitResolver::processPlayerCreatures(std::list<TextHpPair>& pairs, std::list<Scene::Object>& visibleOutfits)
{
    for(auto pairIt = pairs.begin(); pairIt != pairs.end(); pairIt++)
    {
        const HpBar& hp = pairIt->hp;
        auto matchingOutfitIt = findMatchingOutfit(hp.x, hp.x + hp.width, hp.y + hp.height, visibleOutfits);

        auto storedIt = mOutfits.find(pairIt->text.string);
        if(matchingOutfitIt != visibleOutfits.end())
        {
            bool hasMounts = mContext.getObjects()[matchingOutfitIt->object].someInfos.front().spriteInfo.numMounts > 1;
            if(storedIt != mOutfits.end())
            {
                Outfit& storedOutfit = storedIt->second;
                assert(storedOutfit.type == Outfit::Type::CREATURE || storedOutfit.type == Outfit::Type::PLAYER);

                if(hasMounts)
                    assert(storedOutfit.type == Outfit::Type::PLAYER);


                if(storedOutfit.type == Outfit::Type::CREATURE)
                {
                    assert(!hasMounts);
                    assert(storedOutfit.object != -1);

//                    if(storedOutfit.object != nullptr)
//                        assert(storedOutfit.object == matchingOutfitIt->object);
//                    else
//                        storedOutfit.object = matchingOutfitIt->object;

                    Creature c;
                    c.name = pairIt->text.string;
                    c.x = matchingOutfitIt->screenX;
                    c.y = matchingOutfitIt->screenY;
                    c.hp = pairIt->hp.hp;
                    c.object = storedOutfit.object;

                    mCreatures.push_back(c);
                }
                else
                {
                    Player p;
                    p.name = pairIt->text.string;
                    p.x = matchingOutfitIt->screenX;
                    p.y = matchingOutfitIt->screenY;
                    p.hp = pairIt->hp.hp;

                    mPlayers.push_back(p);
                }


            }
            else
            {
                if(hasMounts)
                {
                    mOutfits.emplace(pairIt->text.string, Outfit(pairIt->text.string, Outfit::Type::PLAYER));
                    Player p;
                    p.name = pairIt->text.string;
                    p.x = matchingOutfitIt->screenX;
                    p.y = matchingOutfitIt->screenY;
                    p.hp = pairIt->hp.hp;

                    mPlayers.push_back(p);
                }
                else
                {
                    UnknownPlayerCreature pc;
                    pc.name = pairIt->text.string;
                    pc.x = matchingOutfitIt->screenX;
                    pc.y = matchingOutfitIt->screenY;
                    pc.hp = pairIt->hp.hp;
                    pc.object = matchingOutfitIt->object;
                    mTypeRequests.push_back(pc);
                }
            }
            visibleOutfits.erase(matchingOutfitIt);
        }
        else
        {
            const short OUTFIT_LEFT = hp.x + (hp.width - mCurrentTileWidth) / 2;
            const short OUTFIT_TOP = hp.y + hp.height;
            if(storedIt != mOutfits.end())
            {
                const Outfit& storedOutfit = storedIt->second;
                if(storedOutfit.type == Outfit::Type::CREATURE)
                {
                    Creature c;
                    c.name = pairIt->text.string;
                    c.x = OUTFIT_LEFT;
                    c.y = OUTFIT_TOP;
                    c.hp = pairIt->hp.hp;
                    c.object = storedOutfit.object;

                    mCreatures.push_back(c);
                }
                else
                {
                    Player p;
                    p.name = pairIt->text.string;
                    p.x = OUTFIT_LEFT;
                    p.y = OUTFIT_TOP;
                    p.hp = pairIt->hp.hp;

                    mPlayers.push_back(p);
                }
            }
            else
            {
                // Cannot resolve.
                UnknownPlayerCreature pc;
                pc.name = pairIt->text.string;
                pc.x = OUTFIT_LEFT;
                pc.y = OUTFIT_TOP;
                pc.hp = pairIt->hp.hp;
                mUnknowns.push_back(pc);
            }
        }
    }
}

void OutfitResolver::processTypeRequests(GraphicsMonitorReader& reader)
{
    for(const UnknownPlayerCreature& pc : mTypeRequests)
    {
        const short VIEW_LEFT = mCurrentFrame->viewX;
        const short VIEW_RIGHT = VIEW_LEFT + mCurrentFrame->viewWidth;
        const short VIEW_TOP = mCurrentFrame->viewY;
        const short VIEW_BOTTOM = VIEW_TOP + mCurrentFrame->viewHeight;

        unsigned short cmX = pc.x + mCurrentTileWidth / 2;
        if(cmX <= VIEW_LEFT)
            cmX = VIEW_LEFT + 2;
        else if(cmX >= VIEW_RIGHT)
            cmX = VIEW_RIGHT - 2;

        unsigned short cmY = pc.y + mCurrentTileHeight / 2;
        if(cmY <= VIEW_TOP)
            cmY = VIEW_TOP + 2;
        else if(cmY >= VIEW_BOTTOM)
            cmY = VIEW_BOTTOM -2;
        std::vector<std::string> contextMenu = getContextMenu(reader, cmX, cmY);

        bool isOutfit = false;
        bool isPlayer = false;
        for(const std::string& option : contextMenu)
        {
            if(option == "Copy Name")
                isOutfit = true;
            else if(option.find("Message to") != option.npos)
            {
                isPlayer = true;
                break;
            }
        }

        if(isPlayer)
        {
            Player p;
            p.x = pc.x;
            p.y = pc.y;
            p.hp = pc.hp;
            p.name = pc.name;
            auto result = mOutfits.emplace(p.name, Outfit(p.name, Outfit::Type::PLAYER));
            assert(result.second);

            mPlayers.push_back(p);
        }
        else if(isOutfit)
        {
            Creature c;
            c.x = pc.x;
            c.y = pc.y;
            c.hp = pc.hp;
            c.name = pc.name;
            c.object = pc.object;

            mOutfits.emplace(c.name, Outfit(c.name, Outfit::Type::CREATURE, c.object));

            mCreatures.push_back(c);
        }
        else
        {
            // We probably missed the outfit when getting the context menu.
            // It may happen during lag spikes.
            mUnknowns.push_back(pc);
        }
    }
}

const std::list<OutfitResolver::Creature>& OutfitResolver::getCreatures() const
{
    return mCreatures;
}

const std::list<OutfitResolver::Player>& OutfitResolver::getPlayers() const
{
    return mPlayers;
}
