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
#ifndef GRAPHICS_LAYER_MAP_2D_HPP
#define GRAPHICS_LAYER_MAP_2D_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "Frame.hpp"
///////////////////////////////////

namespace GraphicsLayer
{
    template<typename TX, typename TY, typename TValue>
    class Map2d
    {
        private:
            typedef std::map<TY, std::map<TX, TValue>> RowMap;
            typedef std::map<TX, TValue> ValueMap;

        public:
            struct Element
            {
                explicit Element(TX x, TY y, TValue& v)
                : x(x)
                , y(y)
                , v(v)
                {

                }

                TX x;
                TY y;
                TValue& v;
            };

            class Iterator
            {
                private:
                    typedef typename RowMap::iterator RowIterator;
                    typedef typename ValueMap::iterator  ValueIterator;

                public:
                    explicit Iterator(const RowIterator& rowIt, const ValueIterator& valIt, const RowMap& rows);
                    explicit Iterator(const RowIterator& rowIt, const RowMap& rows);

                    bool operator!=(const Iterator& other) const;
                    bool operator==(const Iterator& other) const;
                    Iterator& operator=(const Iterator& other);
                    Iterator& operator++();
                    Element operator*() const;

                private:
                    RowIterator mRowIt;
                    ValueIterator mValIt;
                    const RowMap& mRows;
            };

        public:
            void set(TX x, TX y, const TValue& val);
            TValue& get(TX x, TX y);
            Iterator find(TX x, TY y);
            Iterator begin();
            Iterator end();
            bool empty() const;
            size_t size() const;

        private:
            RowMap mMap;
            size_t mSize;
    };

}

#include "Map2d.inl"


#endif // GRAPHICS_LAYER_MAP_2D_HPP
