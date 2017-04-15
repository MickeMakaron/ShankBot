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
#include "monitor/SideBarWindowDiff.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <algorithm>
#include <cassert>
///////////////////////////////////


SideBarWindowDiff::SideBarWindowDiff(const SideBarWindowAssembler::Data& oldData, const SideBarWindowAssembler::Data& newData)
{
    using T = SideBarWindow::Type;
    parse(oldData.battle.get(), newData.battle.get(), T::BATTLE);
    parse(oldData.skills.get(), newData.skills.get(), T::SKILLS);
    parse(oldData.prey.get(), newData.prey.get(), T::PREY);
    parse(oldData.vip.get(), newData.vip.get(), T::VIP);
    parse(oldData.unjustifiedPoints.get(), newData.unjustifiedPoints.get(), T::UNJUSTIFIED_POINTS);

    parseContainers(oldData.containers, newData.containers);
}

const SideBarWindowDiff::Data& SideBarWindowDiff::getData() const
{
    return mData;
}

bool SideBarWindowDiff::isContainerEqual(const ContainerWindow& w1, const ContainerWindow& w2)
{
    return w1.capacity == w2.capacity;
}

bool SideBarWindowDiff::isSpriteDrawEqual(const SpriteDraw& d1, const SpriteDraw& d2)
{
    return d1.pairings.front().objects.front() == d2.pairings.front().objects.front();
}

bool SideBarWindowDiff::isItemEqual(const ContainerWindow::Item& i1, const ContainerWindow::Item& i2)
{
    if(i1.count != i2.count)
    {
        return false;
    }
    return isSpriteDrawEqual(*i1.sprite, *i2.sprite);
}

bool SideBarWindowDiff::isContainerContentsEqual(const ContainerWindow& w1, const ContainerWindow& w2)
{
    if(w1.items.size() != w2.items.size())
    {
        return false;
    }

    for(size_t i = 0; i < w1.items.size(); i++)
    {
        if(!isItemEqual(w1.items[i], w2.items[i]))
        {
            return false;
        }
    }

    return true;
}

bool SideBarWindowDiff::isContainerContentsMoved(const ContainerWindow& oldW, const ContainerWindow& newW)
{
    const std::vector<ContainerWindow::Item>& oldItems = oldW.items;
    const std::vector<ContainerWindow::Item>& newItems = newW.items;
    if(oldItems.size() == newItems.size())
    {
        size_t j = 0;
        for(; j < oldItems.size(); j++)
        {
            if(!isSpriteDrawEqual(*oldItems[j].sprite, *newItems[j].sprite))
            {
                break;
            }
        }
        if(j == oldItems.size()) // All sprites same. Must have moved item between stacks.
        {
            std::vector<std::pair<ContainerWindow::Item, ContainerWindow::Item>> mismatches;
            for(j = 0; j < newItems.size(); j++)
            {
                if(oldItems[j].count != newItems[j].count)
                {
                    mismatches.emplace_back(oldItems[j], newItems[j]);
                }
            }
            if(mismatches.size() == 2)
            {
                int d0 = mismatches[0].first.count - mismatches[0].second.count;
                int d1 = mismatches[1].first.count - mismatches[1].second.count;
                if(d0 == -d1)
                {
                    return true;
                }
            }
            return false;
        }
        else
        {
            const ContainerWindow::Item* movedItem = nullptr;
            for(j = 0; j + 1 < newItems.size(); j++)
            {
                if(!isItemEqual(oldItems[j], newItems[j + 1]))
                {
                    movedItem = &oldItems[j];
                    j++;
                    for(; j < newItems.size(); j++)
                    {
                        if(!isItemEqual(oldItems[j], newItems[j]))
                        {
                            return false;
                        }
                    }
                    break;
                }
            }
            return movedItem && isItemEqual(*movedItem, newItems[0]);
        }
    }
    else
    {
        int dSize = newItems.size() - oldItems.size();
        if(dSize == 1) // Was item moved to this?
        {
            size_t j = 0;
            for(; j + 1 < newItems.size(); j++)
            {
                if(!isItemEqual(oldItems[j], newItems[j + 1]))
                {
                    break;
                }
            }
            return j == oldItems.size();
        }
        else if(dSize == -1) //  Was item moved from this?
        {
            size_t j = 0;
            for(; j + 1 < oldItems.size(); j++)
            {
                if(!isItemEqual(oldItems[j + 1], newItems[j]))
                {
                    break;
                }
            }
            return j == newItems.size();
        }
    }

    return false;
};

void SideBarWindowDiff::parse(const SideBarWindow* oldW, const SideBarWindow* newW, SideBarWindow::Type type)
{
    WindowEvent e;
    e.windowType = type;
    if((oldW == nullptr) != (newW == nullptr))
    {
        if(oldW == nullptr)
        {
            e.type = WindowEvent::Type::OPEN;
            e.newWindow = newW;
        }
        else
        {
            e.type = WindowEvent::Type::CLOSE;
            e.oldWindow = oldW;
        }
        mData.windowEvents.push_back(e);
        return;
    }

    if(oldW == nullptr)
    {
        return;
    }

    e.oldWindow = oldW;
    e.newWindow = newW;
    if(oldW->titleBar.x != newW->titleBar.x || oldW->titleBar.y != newW->titleBar.y)
    {
        e.type = WindowEvent::Type::MOVE;
        mData.windowEvents.push_back(e);
    }

    if(oldW->isMinimized != newW->isMinimized)
    {
        e.type = oldW->isMinimized ? WindowEvent::Type::MAX : WindowEvent::Type::MIN;
        mData.windowEvents.push_back(e);
    }
    else if(oldW->clientArea.height != newW->clientArea.height)
    {
        e.type = WindowEvent::Type::RESIZE;
        mData.windowEvents.push_back(e);
    }
}

void SideBarWindowDiff::parseContainers(const std::vector<ContainerWindow>& oldData, const std::vector<ContainerWindow>& newData)
{
    using T = SideBarWindow::Type;
    if(oldData.empty())
    {
        for(const ContainerWindow& w : newData)
        {
            parse(nullptr, &w, T::CONTAINER);
        }
        return;
    }

    if(newData.empty())
    {
        for(const ContainerWindow& w : oldData)
        {
            parse(&w, nullptr, T::CONTAINER);
        }
        return;
    }

    std::vector<const ContainerWindow*> oldContainers(oldData.size());
    std::vector<const ContainerWindow*> newContainers(newData.size());

    std::transform(oldData.begin(),
                   oldData.end(),
                   oldContainers.begin(),
                   [](const ContainerWindow& w){return &w;});

    std::transform(newData.begin(),
                   newData.end(),
                   newContainers.begin(),
                   [](const ContainerWindow& w){return &w;});

    using XGroup = std::pair<std::vector<const ContainerWindow*>, std::vector<const ContainerWindow*>>;
    using WindowGroup = std::map<int, XGroup>;
    std::vector<WindowGroup> windowGroups;
    for(auto it = oldContainers.begin(); it != oldContainers.end();)
    {
        windowGroups.emplace_back();
        auto& xGroups = windowGroups.back();

        const ContainerWindow& w1 = **it;
        xGroups[w1.titleBar.x].first.push_back(&w1);
        auto nextIt = std::next(it);
        while(nextIt != oldContainers.end())
        {
            const ContainerWindow& w2 = **nextIt;
            if(isContainerEqual(w1, w2) && (isContainerContentsEqual(w1, w2) || isContainerContentsMoved(w1, w2)))
            {
                xGroups[w2.titleBar.x].first.push_back(&w2);
                nextIt = oldContainers.erase(nextIt);
            }
            else
            {
                nextIt++;
            }
        }
        nextIt = newContainers.begin();
        while(nextIt != newContainers.end())
        {
            const ContainerWindow& w2 = **nextIt;
            if(isContainerEqual(w1, w2) && (isContainerContentsEqual(w1, w2) || isContainerContentsMoved(w1, w2)))
            {
                xGroups[w2.titleBar.x].second.push_back(&w2);
                nextIt = newContainers.erase(nextIt);
            }
            else
            {
                nextIt++;
            }
        }

        it = oldContainers.erase(it);
    }
    for(auto it = newContainers.begin(); it != newContainers.end();)
    {
        windowGroups.emplace_back();
        auto& xGroups = windowGroups.back();

        const ContainerWindow& w1 = **it;
        xGroups[w1.titleBar.x].second.push_back(&w1);
        auto nextIt = std::next(it);
        while(nextIt != newContainers.end())
        {
            const ContainerWindow& w2 = **nextIt;
            if(isContainerEqual(w1, w2) && (isContainerContentsEqual(w1, w2) || isContainerContentsMoved(w1, w2)))
            {
                xGroups[w2.titleBar.x].second.push_back(&w2);
                nextIt = newContainers.erase(nextIt);
            }
            else
            {
                nextIt++;
            }
        }

        it = newContainers.erase(it);
    }

    std::vector<const ContainerWindow*> globalLost;
    std::vector<const ContainerWindow*> globalFound;
    for(const WindowGroup& wg : windowGroups)
    {
        std::vector<const ContainerWindow*> lost;
        std::vector<const ContainerWindow*> found;

        for(auto& pair : wg)
        {
            const std::vector<const ContainerWindow*>& oldGroup = pair.second.first;
            const std::vector<const ContainerWindow*>& newGroup = pair.second.second;

            size_t minSize = std::min(oldGroup.size(), newGroup.size());
            for(size_t i = 0; i < minSize; i++)
            {
                parse(oldGroup[i], newGroup[i], T::CONTAINER);
            }

            if(minSize == oldGroup.size())
            {
                found.insert(found.end(), newGroup.begin() + minSize, newGroup.end());
            }
            else
            {
                lost.insert(lost.end(), oldGroup.begin() + minSize, oldGroup.end());
            }
        }

        auto movedFoundIt = found.end();
        auto movedLostIt = std::find_if(lost.begin(), lost.end(), [&found, &movedFoundIt](const ContainerWindow* l)
        {
            movedFoundIt = std::find_if(found.begin(), found.end(), [l](const ContainerWindow* f)
            {
                return  isContainerEqual(*l, *f) &&
                        isContainerContentsEqual(*l, *f) &&
                        std::abs(f->titleBar.x - l->titleBar.x) < l->titleBar.width;
            });

            return movedFoundIt != found.end();
        });
        if(movedLostIt != lost.end())
        {
            assert(movedFoundIt != found.end());
            parse(*movedLostIt, *movedFoundIt, T::CONTAINER);

            lost.erase(movedLostIt);
            found.erase(movedFoundIt);
        }

        globalLost.insert(globalLost.end(), lost.begin(), lost.end());
        globalFound.insert(globalFound.end(), found.begin(), found.end());
    }

    for(auto it = globalLost.begin(); it != globalLost.end();)
    {
        const ContainerWindow* w1 = *it;
        auto sameIt = std::find_if(globalFound.begin(), globalFound.end(), [w1](const ContainerWindow* w2)
        {
            return  w1->titleBar.x == w2->titleBar.x &&
                    w1->titleBar.y == w2->titleBar.y &&
                    w1->isMinimized != w2->isMinimized;
        });
        if(sameIt != globalFound.end())
        {
            parse(*it, *sameIt, T::CONTAINER);
            it = globalLost.erase(it);
            globalFound.erase(sameIt);
        }
        else
        {
            it++;
        }
    }

    for(const ContainerWindow* w : globalLost)
    {
        parse(w, nullptr, T::CONTAINER);
    }
    for(const ContainerWindow* w : globalFound)
    {
        parse(nullptr, w, T::CONTAINER);
    }
}
