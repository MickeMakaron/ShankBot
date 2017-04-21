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

SideBarWindowDiff::SideBarWindowDiff()
{
    mPreviousFrame.data.reset(new SideBarWindowAssembler::Data());
    mPreviousEqualFrame.data.reset(new SideBarWindowAssembler::Data());
    mCurrentEqualFrame.data.reset(new SideBarWindowAssembler::Data());
}

void SideBarWindowDiff::parse(const Frame& frame, const SideBarWindowAssembler::Data& data)
{
    mData = Data();

    mCurrentFrame.frame = frame;
    mCurrentFrame.data.reset(new SideBarWindowAssembler::Data(data));

    using T = SideBarWindow::Type;
    parse(mPreviousFrame.data->battle.get(), mCurrentFrame.data->battle.get(), T::BATTLE);
    parse(mPreviousFrame.data->skills.get(), mCurrentFrame.data->skills.get(), T::SKILLS);
    parse(mPreviousFrame.data->prey.get(), mCurrentFrame.data->prey.get(), T::PREY);
    parse(mPreviousFrame.data->vip.get(), mCurrentFrame.data->vip.get(), T::VIP);
    parse(mPreviousFrame.data->unjustifiedPoints.get(), mCurrentFrame.data->unjustifiedPoints.get(), T::UNJUSTIFIED_POINTS);

    parseContainers(mPreviousFrame.data->containers, mCurrentFrame.data->containers);

    mPreviousFrame = mCurrentFrame;
}

const SideBarWindowDiff::Data& SideBarWindowDiff::getData() const
{
    return mData;
}

bool SideBarWindowDiff::isContainerEqual(const ContainerWindow& w1, const ContainerWindow& w2)
{
    return w1.capacity == w2.capacity;
}

bool SideBarWindowDiff::isSpriteDrawEqual(const SpriteDraw* oldW, const SpriteDraw* newW)
{
    if((oldW == nullptr) != (newW == nullptr))
    {
        return false;
    }

    if(oldW == nullptr)
    {
        return true;
    }

    return oldW->pairings.front().objects.front() == newW->pairings.front().objects.front();
}

bool SideBarWindowDiff::isItemEqual(const ContainerWindow::Item& i1, const ContainerWindow::Item& i2)
{
    if(i1.count != i2.count)
    {
        return false;
    }
    return isSpriteDrawEqual(i1.sprite, i2.sprite);
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
    assert(!(oldItems.empty() && newItems.empty()));

    int dSize = newItems.size() - oldItems.size();
    if(std::abs(dSize) > 1)
    {
        return false;
    }

    if(oldItems.empty() || newItems.empty()) // A single item was added/removed
    {
        ContainerItemEvent e;
        e.oldWindow = &oldW;
        e.newWindow = &newW;

        if(oldItems.empty()) // This means newItems.size() == 1 and dSize == 1
        {
            e.type = ContainerItemEvent::Type::ADD;
            e.iNew = 0;
        }
        else // This means oldItems.size() == 1 and dSize == -1
        {
            e.type = ContainerItemEvent::Type::REMOVE;
            e.iOld = 0;
        }

        mData.containerItemEvents.push_back(e);
        return true;
    }

    std::vector<size_t> oldStackIndices;
    std::vector<size_t> newStackIndices;
    for(size_t i = 0; i < oldItems.size(); i++)
    {
        if(oldItems[i].count > 1)
        {
            oldStackIndices.push_back(i);
        }
    }
    for(size_t i = 0; i < newItems.size(); i++)
    {
        if(newItems[i].count > 1)
        {
            newStackIndices.push_back(i);
        }
    }
    int dNumStacks = newStackIndices.size() - oldStackIndices.size();
    if(std::abs(dNumStacks) > 1)
    {
        return false;
    }

    size_t iOldBegin = 0;
    size_t iNewBegin = 0;

    std::vector<ContainerWindow::Item> simulatedNewItems;
    size_t iRemovedItem = -1;
    if(dSize == 0)
    {
        simulatedNewItems = oldItems;
    }
    else if(dSize == 1)
    {
        simulatedNewItems.push_back(newItems[0]);
        simulatedNewItems.insert(simulatedNewItems.end(), oldItems.begin(), oldItems.end());
    }
    else
    {
        iRemovedItem = 0;
        for(; iRemovedItem < newItems.size(); iRemovedItem++)
        {
            if(oldItems[iRemovedItem].sprite != nullptr &&
               !isItemEqual(oldItems[iRemovedItem], newItems[iRemovedItem]))
            {
                break;
            }
        }
        assert(iRemovedItem != oldItems.size());
        simulatedNewItems.insert(simulatedNewItems.end(), oldItems.begin(), oldItems.begin() + iRemovedItem);
        simulatedNewItems.insert(simulatedNewItems.end(), oldItems.begin() + iRemovedItem + 1, oldItems.end());
    }
    if(simulatedNewItems.size() != newItems.size())
    {
        return false;
    }

    size_t iSpriteMismatch = 0;
    for(; iSpriteMismatch < newItems.size(); iSpriteMismatch++)
    {
        if(!isSpriteDrawEqual(simulatedNewItems[iSpriteMismatch].sprite, newItems[iSpriteMismatch].sprite))
        {
            break;
        }
    }

    std::vector<ContainerItemEvent> events;
    size_t iMove = -1;
    if(dSize != 0)
    {
        if(iSpriteMismatch < newItems.size())
        {
            return false;
        }

        ContainerItemEvent e;
        e.oldWindow = &oldW;
        e.newWindow = &newW;
        if(dSize == 1)
        {
            e.type = ContainerItemEvent::Type::ADD;
            e.iNew = 0;
        }
        else
        {
            e.type = ContainerItemEvent::Type::REMOVE;
            assert(iRemovedItem < oldItems.size());
            e.iOld = iRemovedItem;
        }
        events.push_back(e);
    }
    else
    {
        if(iSpriteMismatch < newItems.size())
        {
            size_t iSpriteMismatchBegin = iSpriteMismatch;
            for(iSpriteMismatch = newItems.size() - 1; iSpriteMismatch > 0; iSpriteMismatch--)
            {
                if(!isSpriteDrawEqual(simulatedNewItems[iSpriteMismatch].sprite, newItems[iSpriteMismatch].sprite))
                {
                    break;
                }
            }
            size_t iSpriteMismatchEnd = iSpriteMismatch;
            for(iSpriteMismatch = iSpriteMismatchBegin; iSpriteMismatch < iSpriteMismatchEnd; iSpriteMismatch++)
            {
                if(!isSpriteDrawEqual(simulatedNewItems[iSpriteMismatch].sprite, newItems[iSpriteMismatch + 1].sprite))
                {
                    break;
                }
            }
            if(iSpriteMismatch != iSpriteMismatchEnd)
            {
                return false;
            }

            const ContainerWindow::Item& item1 = simulatedNewItems[iSpriteMismatchEnd];
            const ContainerWindow::Item& item2 = newItems[0];
            if(!isSpriteDrawEqual(item1.sprite, item2.sprite))
            {
                return false;
            }
            int i = iSpriteMismatchEnd;
            while(iSpriteMismatchEnd < simulatedNewItems.size() &&
                  !isItemEqual(simulatedNewItems[iSpriteMismatchEnd], item2) &&
                  isSpriteDrawEqual(simulatedNewItems[iSpriteMismatchEnd].sprite, item1.sprite))
            {
                iSpriteMismatchEnd--;
            }
            if(iSpriteMismatchEnd == 0 || !isItemEqual(simulatedNewItems[iSpriteMismatchEnd], item2))
            {
                return false;
            }
            ContainerItemEvent e;
            e.type = ContainerItemEvent::Type::MOVE;
            e.oldWindow = &oldW;
            e.newWindow = &newW;
            e.iOld = iSpriteMismatchEnd;
            e.iNew = 0;
            events.push_back(e);
            std::rotate(simulatedNewItems.begin(),
                        simulatedNewItems.begin() + iSpriteMismatchEnd,
                        simulatedNewItems.begin() + iSpriteMismatchEnd + 1);
            iMove = iSpriteMismatchEnd;
        }
    }

    std::vector<size_t> countMismatchIndices;
    for(size_t i = 0; i < newItems.size(); i++)
    {
        if(simulatedNewItems[i].count != newItems[i].count)
        {
            countMismatchIndices.push_back(i);
        }
    }
    if(countMismatchIndices.size() > 2)
    {
        return false;
    }

    if(countMismatchIndices.size() == 2)
    {
        size_t i0 = countMismatchIndices[0];
        size_t i1 = countMismatchIndices[1];

        const ContainerWindow::Item& oldItem0 = simulatedNewItems[i0];
        const ContainerWindow::Item& oldItem1 = simulatedNewItems[i1];
        const ContainerWindow::Item& newItem0 = newItems[i0];
        const ContainerWindow::Item& newItem1 = newItems[i1];
        int d0 = newItem0.count - oldItem0.count;
        int d1 = newItem1.count - oldItem1.count;
        if(d0 != -d1)
        {
            return false;
        }
        if(!isSpriteDrawEqual(oldItem0.sprite, newItem0.sprite) ||
           !isSpriteDrawEqual(oldItem1.sprite, newItem1.sprite) ||
           !isSpriteDrawEqual(oldItem0.sprite, oldItem1.sprite))
        {
            return false;
        }
    }

    if(dSize != 0)
    {
        int iOffset = (dSize == 1 ? -1 : 0);
        for(const size_t i : countMismatchIndices)
        {
            ContainerItemEvent e;
            e.oldWindow = &oldW;
            e.newWindow = &newW;
            e.iOld = i + iOffset + (i < iRemovedItem ? 0 : 1);
            e.iNew = i;
            int d = newW.items[e.iNew].count - oldW.items[e.iOld].count;
            assert(d != 0);
            if(d > 0)
            {
                e.type = ContainerItemEvent::Type::STACK_INCREASE;
            }
            else
            {
                e.type = ContainerItemEvent::Type::STACK_DECREASE;
            }
            events.push_back(e);
        }
    }
    else
    {
        for(const size_t i : countMismatchIndices)
        {
            ContainerItemEvent e;
            e.oldWindow = &oldW;
            e.newWindow = &newW;
            if(i == 0)
            {
                e.iOld = (iMove < oldW.items.size() ? iMove : i);
            }
            else if(i <= iMove)
            {
                e.iOld = (iMove < oldW.items.size() ? i - 1 : i);
            }
            else
            {
                e.iOld = i;
            }
            e.iNew = i;
            int d = newW.items[e.iNew].count - oldW.items[e.iOld].count;
            assert(d != 0);
            if(d > 0)
            {
                e.type = ContainerItemEvent::Type::STACK_INCREASE;
            }
            else
            {
                e.type = ContainerItemEvent::Type::STACK_DECREASE;
            }
            events.push_back(e);
        }
    }


    mData.containerItemEvents.insert(mData.containerItemEvents.end(), events.begin(), events.end());
    return true;
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


    bool isEqual =  oldData.size() == newData.size() &&
                    std::equal(oldData.begin(),
                               oldData.end(),
                               newData.begin(),
                               [](const ContainerWindow& oldW, const ContainerWindow& newW)
                               {
                                   return isContainerEqual(oldW, newW) && isContainerContentsEqual(oldW, newW);
                               });

    if(isEqual)
    {
        mPreviousEqualFrame = mCurrentEqualFrame;
        mCurrentEqualFrame = mCurrentFrame;
    }
    else
    {
        return;
    }

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

    const std::vector<ContainerWindow>& previousEqualData = mPreviousEqualFrame.data->containers;
    const std::vector<ContainerWindow>& currentEqualData = mCurrentEqualFrame.data->containers;
    std::vector<const ContainerWindow*> oldContainers(previousEqualData.size());
    std::vector<const ContainerWindow*> newContainers(currentEqualData.size());
    std::transform(previousEqualData.begin(),
                   previousEqualData.end(),
                   oldContainers.begin(),
                   [](const ContainerWindow& w){return &w;});

    std::transform(currentEqualData.begin(),
                   currentEqualData.end(),
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
