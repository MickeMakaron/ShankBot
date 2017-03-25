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
#ifndef SB_TEST_UTILITY_HPP
#define SB_TEST_UTILITY_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "monitor/Gui.hpp"
#include "monitor/Scene.hpp"
#include "utility/Matrix.hpp"
#include "monitor/FontSample.hpp"
namespace GraphicsLayer
{
    struct Vertex;
    struct ParsedFrame;
}
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <vector>
#include <list>
#include <memory>
///////////////////////////////////


////////////////////////////////////////
// Google Test
#include "gtest/gtest.h"
////////////////////////////////////////

namespace sb
{
namespace test
{
    void expectEq(const GraphicsLayer::Vertex& v1, const GraphicsLayer::Vertex& v2);
    void expectEq(const GraphicsLayer::Gui::Container& c1, const GraphicsLayer::Gui::Container& c2);
    void expectEq(const GraphicsLayer::Gui::NpcTradeWindow::Offer& o1, const GraphicsLayer::Gui::NpcTradeWindow::Offer& o2);
    void expectEq(const GraphicsLayer::Gui::NpcTradeWindow& w1, const GraphicsLayer::Gui::NpcTradeWindow& w2);
    void expectEq(const GraphicsLayer::Gui::BattleWindow::Outfit& o1, const GraphicsLayer::Gui::BattleWindow::Outfit& o2);
    void expectEq(const GraphicsLayer::Gui::BattleWindow& w1, const GraphicsLayer::Gui::BattleWindow& w2);
    void expectEq(const GraphicsLayer::Gui::Rect& r1, const GraphicsLayer::Gui::Rect& r2);
    void expectEq(const GraphicsLayer::Gui::SideBottomWindow& w1, const GraphicsLayer::Gui::SideBottomWindow& w2);
    void expectEq(const GraphicsLayer::Gui::Button& b1, const GraphicsLayer::Gui::Button& b2);
    void expectEq(const GraphicsLayer::Gui::Data& f1, const GraphicsLayer::Gui::Data& f2);

    void expectEq(const GraphicsLayer::Scene::Tile& f1, const GraphicsLayer::Scene::Tile& f2);
    void expectEq(const GraphicsLayer::Scene::Object& f1, const GraphicsLayer::Scene::Object& f2);
    void expectEq(const GraphicsLayer::Scene::Data& f1, const GraphicsLayer::Scene::Data& f2);

    void expectEq(const GraphicsLayer::ParsedFrame& f1, const GraphicsLayer::ParsedFrame& f2);

    void expectEq(const GraphicsLayer::Draw& d1, const GraphicsLayer::Draw& d2);
    void expectEq(const GraphicsLayer::SpriteDraw::SpriteObjectPairing& p1, const GraphicsLayer::SpriteDraw::SpriteObjectPairing& p2);
    void expectEq(const GraphicsLayer::SpriteDraw& d1, const GraphicsLayer::SpriteDraw& d2);
    void expectEq(const GraphicsLayer::GuiDraw& d1, const GraphicsLayer::GuiDraw& d2);
    void expectEq(const GraphicsLayer::GlyphDraw& d1, const GraphicsLayer::GlyphDraw& d2);
    void expectEq(const GraphicsLayer::TextDraw& d1, const GraphicsLayer::TextDraw& d2);
    void expectEq(const GraphicsLayer::RectDraw& d1, const GraphicsLayer::RectDraw& d2);
    void expectEq(const GraphicsLayer::MiniMapDraw& d1, const GraphicsLayer::MiniMapDraw& d2);
    void expectEq(const GraphicsLayer::RawImage& d1, const GraphicsLayer::RawImage& d2);
    void expectEq(const GraphicsLayer::Frame& f1, const GraphicsLayer::Frame& f2);



    void expectEq(const GraphicsLayer::FontSample::Glyph& f1, const GraphicsLayer::FontSample::Glyph& f2);

    template<typename T>
    void expectEq(const std::shared_ptr<T>& p1, const std::shared_ptr<T>& p2);
    template<typename T>
    void expectEq(const std::list<T>& p1, const std::list<T>& p2);
    template<typename T>
    void expectEq(const std::vector<T>& p1, const std::vector<T>& p2);
    template<typename T, size_t N>
    void expectEq(const std::array<T, N>& p1, const std::array<T, N>& p2);
    template<size_t N, size_t M>
    void expectEq(const sb::utility::Matrix<float, N, M>& m1, const sb::utility::Matrix<float, N, M>& m2);


    template<typename T>
    void expectEq(const std::shared_ptr<T>& p1, const std::shared_ptr<T>& p2)
    {
        EXPECT_EQ(p1 == nullptr, p2 == nullptr);
        if(p1 == nullptr || p2 == nullptr)
        {
            return;
        }

        expectEq(*p1, *p2);
    }



    template<typename T>
    void expectEq(const std::list<T>& p1, const std::list<T>& p2)
    {
        EXPECT_EQ(p1.size(), p2.size());
        if(p1.size() != p2.size())
        {
            return;
        }

        auto it1 = p1.begin();
        auto it2 = p2.begin();
        while(it1 != p1.end())
        {
            expectEq(*it1, *it2);

            it1++;
            it2++;
        }
    }

    template<typename T>
    void expectEq(const std::vector<T>& p1, const std::vector<T>& p2)
    {
        EXPECT_EQ(p1.size(), p2.size());
        if(p1.size() != p2.size())
        {
            return;
        }

        for(size_t i = 0; i < p1.size(); i++)
        {
            expectEq(p1[i], p2[i]);
        }
    }


    template<typename T, size_t N>
    void expectEq(const std::array<T, N>& p1, const std::array<T, N>& p2)
    {
        for(size_t i = 0; i < p1.size(); i++)
        {
            expectEq(p1[i], p2[i]);
        }
    }

    template<size_t N, size_t M>
    void expectEq(const sb::utility::Matrix<float, N, M>& m1, const sb::utility::Matrix<float, N, M>& m2)
    {
        for(size_t i = 0; i < N * M; i++)
        {
            EXPECT_FLOAT_EQ(*(*m1.values + i), *(*m2.values + i));
        }
    }
}
}


#endif // SB_TEST_UTILITY_HPP
