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
namespace GraphicsLayer
{
    template<typename TX, typename TY, typename TValue>
    Map2d<TX, TY, TValue>::Iterator::Iterator(const RowIterator& rowIt, const ValueIterator& valIt, const RowMap& rows)
    : mRowIt(rowIt)
    , mValIt(valIt)
    , mRows(rows)
    {

    }

    template<typename TX, typename TY, typename TValue>
    Map2d<TX, TY, TValue>::Iterator::Iterator(const RowIterator& rowIt, const RowMap& rows)
    : mRowIt(rowIt)
    , mRows(rows)
    {

    }

    template<typename TX, typename TY, typename TValue>
    bool Map2d<TX, TY, TValue>::Iterator::operator!=(const Iterator& other) const
    {
        if(mRowIt == mRows.end())
            return mRowIt != other.mRowIt;
        else
            return mRowIt != other.mRowIt || mValIt != other.mValIt;
    }

    template<typename TX, typename TY, typename TValue>
    bool Map2d<TX, TY, TValue>::Iterator::operator==(const Iterator& other) const
    {
        return !(operator!=(other));
    }


    template<typename TX, typename TY, typename TValue>
    typename Map2d<TX, TY, TValue>::Iterator& Map2d<TX, TY, TValue>::Iterator::operator=(const Iterator& other)
    {
        mRowIt = other.mRowIt;
        mValIt = other.mValIt;
        return *this;
    }

    template<typename TX, typename TY, typename TValue>
    typename Map2d<TX, TY, TValue>::Iterator& Map2d<TX, TY, TValue>::Iterator::operator++()
    {
        mValIt++;
        if(mValIt == mRowIt->second.end())
        {
            mRowIt++;
            mValIt = mRowIt->second.begin();
        }

        return *this;
    }

    template<typename TX, typename TY, typename TValue>
    typename Map2d<TX, TY, TValue>::Element Map2d<TX, TY, TValue>::Iterator::operator*() const
    {
        return Element(mValIt->first, mRowIt->first, mValIt->second);
    }

    template<typename TX, typename TY, typename TValue>
    void Map2d<TX, TY, TValue>::set(TX x, TX y, const TValue& val)
    {
        const size_t PRE_SIZE = mMap[y].size();
        mMap[y][x] = val;
        const size_t POST_SIZE = mMap[y].size();
        mSize += POST_SIZE - PRE_SIZE;
    }

    template<typename TX, typename TY, typename TValue>
    TValue& Map2d<TX, TY, TValue>::get(TX x, TX y)
    {
        const size_t PRE_SIZE = mMap[y].size();
        TValue& v = mMap[y][x];
        const size_t POST_SIZE = mMap[y].size();
        mSize += POST_SIZE - PRE_SIZE;

        return v;
    }

    template<typename TX, typename TY, typename TValue>
    typename Map2d<TX, TY, TValue>::Iterator Map2d<TX, TY, TValue>::begin()
    {
        if(mMap.empty() || mMap.begin()->second.empty())
            return Iterator(mMap.end(), mMap);
        else
            return Iterator(mMap.begin(), mMap.begin()->second.begin(), mMap);
    }

    template<typename TX, typename TY, typename TValue>
    typename Map2d<TX, TY, TValue>::Iterator Map2d<TX, TY, TValue>::end()
    {
        return Iterator(mMap.end(), mMap);
    }

    template<typename TX, typename TY, typename TValue>
    typename Map2d<TX, TY, TValue>::Iterator Map2d<TX, TY, TValue>::find(TX x, TY y)
    {
        auto rowIt = mMap.find(y);
        if(rowIt == mMap.end())
            return Iterator(mMap.end(), mMap);
        else
        {
            auto valIt = rowIt->second.find(x);
            if(valIt == rowIt->second.end())
                return Iterator(mMap.end(), mMap);
            else
                return Iterator(rowIt, valIt, mMap);
        }
    }

    template<typename TX, typename TY, typename TValue>
    bool Map2d<TX, TY, TValue>::empty() const
    {
        return mSize == 0;
    }

    template<typename TX, typename TY, typename TValue>
    size_t Map2d<TX, TY, TValue>::size() const
    {
        return mSize;
    }
}
