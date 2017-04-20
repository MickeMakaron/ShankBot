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
}

void SideBarWindowDiff::parse(const Frame& frame, const SideBarWindowAssembler::Data& data)
{
    mData = Data();

    mCurrentFrame.frame = frame;
    mCurrentFrame.data.reset(new SideBarWindowAssembler::Data(data));

    mPendingContainerItemEventsFrontBuffer.clear();
    mPendingContainerItemEventsFrontBuffer.swap(mPendingContainerItemEvents);

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
    auto pendingContainerItemEventIt = std::find_if(mPendingContainerItemEventsFrontBuffer.begin(),
                                                    mPendingContainerItemEventsFrontBuffer.end(),
                                                    [&oldW](const PendingContainerItemEvent& e)
    {
        return e.newWindow == &oldW;
    });
    if(pendingContainerItemEventIt != mPendingContainerItemEventsFrontBuffer.end())
    {
        PendingContainerItemEvent e = *pendingContainerItemEventIt;
        mPendingContainerItemEventsFrontBuffer.erase(pendingContainerItemEventIt);
        size_t size = mPendingContainerItemEvents.size();
        bool result = isContainerContentsMoved(*e.oldWindow, newW);
        assert(size == mPendingContainerItemEvents.size());
        return result;
    }

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

//    size_t iOldStackBegin = 0;
//    size_t iNewStackBegin = 0;
//    if(dNumStacks == 1)
//    {
//        iNewStackBegin = 1;
//    }
//    else if(dNumStacks == -1)
//    {
//        iOldStackBegin = 1;
//    }
//    assert(oldStackIndices.size() - iOldStackBegin == newStackIndices.size() - iNewStackBegin);
//    assert(iOldStackBegin + dNumStacks == iNewStackBegin);
//    std::vector<size_t> oldCountMismatchIndices;
//    for(size_t iOld = iOldBegin, iNew = iNewBegin; iOld < oldStackIndices.size(); iOld++, iNew++)
//    {
//        if(oldItems[iOld].count != newItems[iNew].count)
//        {
//            oldCountMismatchIndices.push_back(iOld);
//        }
//    }
//    if(oldCountMismatchIndices.size() > 2)
//    {
//        return false;
//    }
//
//    if(oldCountMismatchIndices.size() == 2)
//    {
//        int d0 = newItems[oldCountMismatchIndices[0] + dNumItems].count - [oldCountMismatchIndices[0]].count;
//        int d1 = newItems[oldCountMismatchIndices[1] + dNumItems].count - [oldCountMismatchIndices[1]].count;
//        if(d0 != -d1)
//        {
//            return false;
//        }
//    }

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

    std::vector<size_t> nullIndices;
    size_t iSpriteMismatch = 0;
    for(; iSpriteMismatch < newItems.size(); iSpriteMismatch++)
    {
        if(simulatedNewItems[iSpriteMismatch].sprite == nullptr)
        {
            nullIndices.push_back(iSpriteMismatch);
        }
        else if(!isSpriteDrawEqual(simulatedNewItems[iSpriteMismatch].sprite, newItems[iSpriteMismatch].sprite))
        {
            break;
        }
    }


    if(dSize != 0)
    {
        if(iSpriteMismatch < newItems.size())
        {
            if(dSize == 1)
            {
                for(iSpriteMismatch = 1; iSpriteMismatch < oldItems.size(); iSpriteMismatch++)
                {
                    if(oldItems[iSpriteMismatch].sprite == nullptr)
                    {
                    }
                    else if(!isSpriteDrawEqual(oldItems[iSpriteMismatch].sprite, newItems[iSpriteMismatch].sprite))
                    {
                        break;
                    }
                }
                if(iSpriteMismatch < oldItems.size())
                {
                    return false;
                }

                PendingContainerItemEvent e;
                e.oldWindow = &oldW;
                e.newWindow = &newW;
                e.oldFrame = mPreviousFrame;
                e.newFrame = mCurrentFrame;
                mPendingContainerItemEvents.push_back(e);
                return true;
            }
            else
            {
                if(iSpriteMismatch == iRemovedItem)
                {
                    iSpriteMismatch++;
                    for(; iSpriteMismatch < newItems.size(); iSpriteMismatch++)
                    {
                        if(simulatedNewItems[iSpriteMismatch].sprite == nullptr)
                        {
                        }
                        else if(!isItemEqual(simulatedNewItems[iSpriteMismatch], newItems[iSpriteMismatch]))
                        {
                            break;
                        }
                    }

                    if(iSpriteMismatch < newItems.size())
                    {
                        return false;
                    }
                    if(!isSpriteDrawEqual(oldItems[iRemovedItem].sprite, newItems[iRemovedItem].sprite))
                    {
                        return false;
                    }

                    PendingContainerItemEvent e;
                    e.oldWindow = &oldW;
                    e.newWindow = &newW;
                    e.oldFrame = mPreviousFrame;
                    e.newFrame = mCurrentFrame;
                    mPendingContainerItemEvents.push_back(e);
                    return true;
                }
            }
            return false;
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
            int d0 = newItems[countMismatchIndices[0]].count - simulatedNewItems[countMismatchIndices[0]].count;
            int d1 = newItems[countMismatchIndices[1]].count - simulatedNewItems[countMismatchIndices[1]].count;
            if(d0 != -d1)
            {
                return false;
            }
        }

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
            mData.containerItemEvents.push_back(e);
        }

        for(const size_t i : nullIndices)
        {
            ContainerItemEvent e;
            e.type = ContainerItemEvent::Type::ADD;
            e.oldWindow = &oldW;
            e.newWindow = &newW;
            e.iOld = i + iOffset + (i < iRemovedItem ? 0 : 1);
            e.iNew = i;
            mData.containerItemEvents.push_back(e);
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
        mData.containerItemEvents.push_back(e);

        return true;
    }
    else
    {
        if(iSpriteMismatch == 0)
        {
            for(iSpriteMismatch = newItems.size() - 1; iSpriteMismatch > 0; iSpriteMismatch--)
            {
                if(simulatedNewItems[iSpriteMismatch].sprite == nullptr)
                {
                    nullIndices.push_back(iSpriteMismatch);
                }
                else if(!isSpriteDrawEqual(simulatedNewItems[iSpriteMismatch].sprite, newItems[iSpriteMismatch].sprite))
                {
                    break;
                }
            }
            if(iSpriteMismatch == 0)
            {
                // Sprite replacement bug, possibly.
                PendingContainerItemEvent e;
                e.type = ContainerItemEvent::Type::MOVE;
                e.oldWindow = &oldW;
                e.newWindow = &newW;
                e.oldFrame = mPreviousFrame;
                e.newFrame = mCurrentFrame;
                mPendingContainerItemEvents.push_back(e);
                return true;
            }

            size_t iSpriteMismatchEnd = iSpriteMismatch;
            std::vector<size_t> mismatchIndices;
            mismatchIndices.push_back(0);
            for(iSpriteMismatch = 0; iSpriteMismatch < iSpriteMismatchEnd; iSpriteMismatch++)
            {
                if(simulatedNewItems[iSpriteMismatch].sprite == nullptr)
                {
                    nullIndices.push_back(iSpriteMismatch);
                }
                else if(!isSpriteDrawEqual(simulatedNewItems[iSpriteMismatch].sprite, newItems[iSpriteMismatch + 1].sprite))
                {
//                    mismatchIndices.push_back(iSpriteMismatch + 1);
//                    iSpriteMismatch++;
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
            ContainerItemEvent e;
            e.type = ContainerItemEvent::Type::MOVE;
            e.oldWindow = &oldW;
            e.newWindow = &newW;
            e.iOld = iSpriteMismatchEnd;
            e.iNew = 0;
            mData.containerItemEvents.push_back(e);

            for(const size_t i : nullIndices)
            {
                ContainerItemEvent e;
                e.type = ContainerItemEvent::Type::ADD;
                e.oldWindow = &oldW;
                e.newWindow = &newW;
                e.iOld = i;
                e.iNew = (i < iSpriteMismatchEnd ? i + 1 : i);
                mData.containerItemEvents.push_back(e);
            }
            return true;
        }
        else if(iSpriteMismatch < newItems.size())
        {
            return false;
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
            PendingContainerItemEvent e;
            e.oldWindow = &oldW;
            e.newWindow = &newW;
            e.oldFrame = mPreviousFrame;
            e.newFrame = mCurrentFrame;
            mPendingContainerItemEvents.push_back(e);
            return true;
        }

        if(countMismatchIndices.size() == 2)
        {
            size_t i0 = countMismatchIndices[0];
            size_t i1 = countMismatchIndices[1];
            if(std::abs(int(i1 - i0)) == 1)
            {
                PendingContainerItemEvent e;
                e.oldWindow = &oldW;
                e.newWindow = &newW;
                e.oldFrame = mPreviousFrame;
                e.newFrame = mCurrentFrame;
                mPendingContainerItemEvents.push_back(e);
                return true;
            }
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

        for(const size_t i : countMismatchIndices)
        {
            ContainerItemEvent e;
            e.oldWindow = &oldW;
            e.newWindow = &newW;
            e.iOld = i;
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
            mData.containerItemEvents.push_back(e);
        }

        for(const size_t i : nullIndices)
        {
            ContainerItemEvent e;
            e.type = ContainerItemEvent::Type::ADD;
            e.oldWindow = &oldW;
            e.newWindow = &newW;
            e.iOld = i;
            e.iNew = i;
            mData.containerItemEvents.push_back(e);
        }
        return true;
    }

    return false;
//
//    if(dSize == 1)
//    {
//        iNewBegin = 1;
//    }
//    else if(dSize == -1)
//    {
//        iOldBegin = 1; // Nope...
//    }
//    else
//    {
//        iOldBegin = 1;
//        iNewBegin = 1;
//    }
//    assert(oldItems.size() - iOldBegin == newItems.size() - iNewBegin);
//    assert(iOldBegin + dSize == iNewBegin);
//    std::vector<size_t> oldNullIndices;
//    size_t iOld = iOldBegin;
//    for(size_t iNew = iNewBegin; iOld < oldItems.size(); iOld++, iNew++)
//    {
//        if(oldItems[iOld].sprite == nullptr)
//        {
//            oldNullIndices.push_back(iOld);
//        }
//        else if(!isSpriteDrawEqual(oldItems[iOld].sprite, newItems[iNew].sprite))
//        {
//            break;
//        }
//    }
//
//    if(dSize != 0)
//    {
//        if(iOld < oldItems.size())
//        {
//            return false;
//        }
//
//        std::vector<size_t> oldCountMismatchIndices;
//        for(size_t i = iOldBegin, iNew = iNewBegin; i < oldItems.size(); i++, iNew++)
//        {
//            if(oldItems[i].count != newItems[iNew].count)
//            {
//                oldCountMismatchIndices.push_back(i);
//            }
//        }
//        if(oldCountMismatchIndices.size() > 2)
//        {
//            return false;
//        }
//
//        if(oldCountMismatchIndices.size() == 2)
//        {
//            int d0 = newItems[oldCountMismatchIndices[0] + dSize].count - [oldCountMismatchIndices[0]].count;
//            int d1 = newItems[oldCountMismatchIndices[1] + dSize].count - [oldCountMismatchIndices[1]].count;
//            if(d0 != -d1)
//            {
//                return false;
//            }
//        }
//
//        for(const size_t i : oldCountMismatchIndices)
//        {
//            ContainerItemEvent e;
//            e.oldWindow = &oldW;
//            e.newWindow = &newW;
//            e.iOld = i;
//            e.iNew = i + dSize;
//            int d = newW.items[e.iNew].count - oldW.items[e.iOld].count;
//            assert(d != 0);
//            if(d > 0)
//            {
//                e.type == ContainerItemEvent::Type::STACK_INCREASE;
//            }
//            else
//            {
//                e.type == ContainerItemEvent::Type::STACK_DECREASE;
//            }
//            mData.containerItemEvents.push_back(e);
//        }
//
//        ContainerItemEvent e;
//        e.type = dSize == 1 ? ContainerItemEvent::Type::ADD : ContainerItemEvent::Type::REMOVE;
//        e.oldWindow = &oldW;
//        e.newWindow = &newW;
//        e.iOld = 0;
//        e.iNew = 0;
//        mData.containerItemEvents.push_back(e);
//        return true;
//    }
//
//
//
//    if(dSize == 0)
//    {
//        bool areStacksEqual = std::equal(oldItems.begin(),
//                                         oldItems.end()
//                                         newItems.begin(),
//                                         [](const ContainerWindow::Item& i1, const ContainerWindow::Item& i2)
//                                         {
//                                            return i1.count == i2.count;
//                                         });
////
////        if(oldCountMismatchIndices.empty())
////        {
////
////        }
////        if(oldItems[0].sprite == nullptr)
////        {
////            oldNullIndices.insert(oldNullIndices.begin(), 0);
////        }
////        else if(!isSpriteDrawEqual(oldItems[0].sprite, newItems[0].sprite))
////        {
////
////        }
////        int stackOffset = 0;
////        iOld = iOldStackBegin;
////        for(size_t iNew = iNewStackBegin; iOld < oldStackIndices.size(); iOld++, iNew++)
////        {
////            int offset = newStackIndices[i] - oldStackIndices[i];
////            if(stackOffset == 0)
////            {
////                stackOffset = offset;
////            }
////            else if(offset != stackOffset)
////            {
////                return false;
////            }
////        }
//    }
//
//    for(const size_t i : oldNullIndices)
//    {
//        ContainerItemEvent e;
//        e.type == ContainerItemEvent::Type::ADD;
//        e.oldWindow = &oldW;
//        e.newWindow = &newW;
//        e.iOld = i;
//        e.iNew = i + dSize;
//        mData.containerItemEvents.push_back(e);
//    }
//
//
//
//
//    if(dSize == 0)
//    {
//        bool isFirstSpriteEqual = isSpriteDrawEqual(oldItems[0].sprite, newItems[0].sprite);
//        size_t j = 1;
//        for(; j < oldItems.size(); j++)
//        {
//            if(!isSpriteDrawEqual(oldItems[j].sprite, newItems[j].sprite))
//            {
//                break;
//            }
//        }
//        if(j == oldItems.size()) // All sprites same. Must have moved item between stacks.
//        {
//            if(!isFirstSpriteEqual)
//            {
//                const SpriteDraw* overWritingSprite = newItems[0].sprite;
//                auto sourceIt = std::find_if(oldItems.begin() + 1,
//                                             oldItems.end(),
//                                             [overWritingSprite](const ContainerWindow::Item& item)
//                {
//                    return isSpriteDrawEqual(overWritingSprite, item.sprite);
//                });
//                if(sourceIt == oldItems.end())
//                {
//
//                }
//            }
//
//            std::vector<size_t> mismatches;
//            for(j = 0; j < newItems.size(); j++)
//            {
//                if(oldItems[j].count != newItems[j].count)
//                {
//                    mismatches.push_back(j);
//                }
//            }
//
//            assert(mismatches.size() != 0);
//
//            if(mismatches.size() > 2)
//            {
//                return false;
//            }
//
//            for(const size_t itemIndex : mismatches)
//            {
//                int d = newW.items[itemIndex].count - oldW.items[itemIndex].count;
//                assert(d != 0);
//                ContainerItemEvent e;
//                e.oldWindow = &oldW;
//                e.newWindow = &newW;
//                e.iOld = itemIndex;
//                e.iNew = itemIndex;
//                if(d > 0)
//                {
//                    e.type == ContainerItemEvent::Type::STACK_INCREASE;
//                }
//                else
//                {
//                    e.type == ContainerItemEvent::Type::STACK_DECREASE;
//                }
//                mData.containerItemEvents.push_back(e);
//            }
//            return true;
//        }
//        else
//        {
//            const ContainerWindow::Item* movedItem = nullptr;
//            size_t iMovedItemOld = -1;
//            for(j = 0; j + 1 < newItems.size(); j++)
//            {
//                if(!isItemEqual(oldItems[j], newItems[j + 1]))
//                {
//                    iMovedItemOld = j;
//                    j++;
//                    for(; j < newItems.size(); j++)
//                    {
//                        if(!isItemEqual(oldItems[j], newItems[j]))
//                        {
//                            return false;
//                        }
//                    }
//                    break;
//                }
//            }
//
//            if(iMovedItemOld >= oldItems.size() || !isItemEqual(oldItems[iMovedItemOld], newItems[0]))
//            {
//                return false;
//            }
//
//            ContainerItemEvent e;
//            e.type = ContainerItemEvent::Type::MOVE;
//            e.oldWindow = &oldW;
//            e.newWindow = &newW;
//            e.iOld = iMovedItemOld;
//            e.iNew = 0;
//            mData.containerItemEvents.push_back(e);
//            return true;
//        }
//    }
//    else
//    {
//        int dSize = newItems.size() - oldItems.size();
//        if(dSize > 1 || dSize < -1)
//        {
//            return false;
//        }
//
//        size_t iOld = dSize > 0 ? 0 : -dSize;
//        size_t iNew = dSize > 0 ? dSize : 0;
//        assert(iNew - iOld == dSize);
//        std::vector<size_t> countMismatchesOld;
//        for(; iOld < oldItems.size(); iOld++, iNew++)
//        {
//            assert(iNew < newItems.size());
//            if(!isSpriteDrawEqual(oldItems[iOld].sprite, newItems[iNew].sprite))
//            {
//                break;
//            }
//
//            if(oldItems[iOld].count != newItems[iNew].count)
//            {
//                countMismatchesOld.push_back(iOld);
//            }
//        }
//        if(iOld < oldItems.size())
//        {
//            return false;
//        }
//
//        if(countMismatchesOld.size() > 2)
//        {
//            return false;
//        }
//
//        for(const size_t oldItemIndex : countMismatchesOld)
//        {
//            ContainerItemEvent e = ContainerItemEvent();
//            e.oldWindow = &oldW;
//            e.newWindow = &newW;
//            e.iOld = oldItemIndex;
//            e.iNew = oldItemIndex + dSize;
//            int d = newW.items[e.iNew].count - oldW.items[e.iOld].count;
//            assert(d != 0);
//            if(d > 0)
//            {
//                e.type == ContainerItemEvent::Type::STACK_INCREASE;
//            }
//            else
//            {
//                e.type == ContainerItemEvent::Type::STACK_DECREASE;
//            }
//            mData.containerItemEvents.push_back(e);
//        }
//
//        ContainerItemEvent e;
//        e.type = dSize == 1 ? ContainerItemEvent::Type::ADD : ContainerItemEvent::Type::REMOVE;
//        e.newWindow = &newW;
//        e.iNew = 0;
//        mData.containerItemEvents.push_back(e);
//
//        return true;
//    }
//
//    return false;
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

    bool isEqual = oldData.size() == newData.size();
    if(isEqual)
    {
        isEqual = std::equal(oldData.begin(),
                             oldData.end(),
                             newData.begin(),
                             [](const ContainerWindow& oldW, const ContainerWindow& newW)
                             {
                                 return isContainerEqual(oldW, newW) && isContainerContentsEqual(oldW, newW);
                             });
    }

    if(isEqual)
    {
        if(!mHasPendingInequality)
        {
            mPreviousEqualFrame = mCurrentFrame;
            return;
        }
    }
    else
    {
        mHasPendingInequality = true;
        return;
    }

    mHasPendingInequality = false;

    const std::vector<ContainerWindow>& previousEqualData = mPreviousEqualFrame.data->containers;
    std::vector<const ContainerWindow*> oldContainers(previousEqualData.size());
    std::vector<const ContainerWindow*> newContainers(newData.size());
    std::transform(previousEqualData.begin(),
                   previousEqualData.end(),
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
