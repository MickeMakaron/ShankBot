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
#include "test/utility.hpp"
#include "utility/Vertex.hpp"
#include "monitor/ParsedFrame.hpp"
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// Qt
#include "QtGui/QImage"
///////////////////////////////////


namespace sb
{
namespace test
{

void expectEq(const Vertex& v1, const Vertex& v2)
{
    EXPECT_FLOAT_EQ(v1.x, v2.x);
    EXPECT_FLOAT_EQ(v1.y, v2.y);
}

void expectEq(const Gui::Container& c1, const Gui::Container& c2)
{
    EXPECT_EQ(c1.name, c2.name);
    EXPECT_EQ(c1.capacity, c2.capacity);
    EXPECT_EQ(c1.items, c2.items);
    EXPECT_FLOAT_EQ(c1.scroll, c2.scroll);
    expectEq(c1.window, c2.window);
}

void expectEq(const Gui::NpcTradeWindow::Offer& o1, const Gui::NpcTradeWindow::Offer& o2)
{
    EXPECT_EQ(o1.name, o2.name);
    EXPECT_EQ(o1.objects, o2.objects);
    EXPECT_EQ(o1.cost, o2.cost);
    EXPECT_FLOAT_EQ(o1.weight, o2.weight);
    EXPECT_EQ(o1.isAffordable, o2.isAffordable);
}

void expectEq(const Gui::NpcTradeWindow& w1, const Gui::NpcTradeWindow& w2)
{
    expectEq(w1.visibleOffers, w2.visibleOffers);

    EXPECT_EQ(w1.selectedOfferIndex, w2.selectedOfferIndex);
    EXPECT_EQ(w1.amount, w2.amount);
    EXPECT_EQ(w1.totalPrice, w2.totalPrice);
    EXPECT_EQ(w1.availableMoney, w2.availableMoney);
    expectEq(w1.window, w2.window);
    expectEq(w1.buyButton, w2.buyButton);
    expectEq(w1.sellButton, w2.sellButton);
    expectEq(w1.okButton, w2.okButton);
    EXPECT_EQ(w1.currentTab, w2.currentTab);
}

void expectEq(const Gui::BattleWindow::Outfit& o1, const Gui::BattleWindow::Outfit& o2)
{
    EXPECT_FLOAT_EQ(o1.hpPercent, o2.hpPercent);
    EXPECT_EQ(o1.name, o2.name);
    EXPECT_EQ(o1.objects, o2.objects);
}

void expectEq(const Gui::BattleWindow& w1, const Gui::BattleWindow& w2)
{
    expectEq(w1.outfits, w2.outfits);
    expectEq(w1.selectedOutfit, w2.selectedOutfit);
}

void expectEq(const Gui::Rect& r1, const Gui::Rect& r2)
{
    expectEq(r1.pos, r2.pos);
    expectEq(r1.size, r2.size);
}

void expectEq(const Gui::SideBottomWindow& w1, const Gui::SideBottomWindow& w2)
{
    EXPECT_EQ(w1.type, w2.type);
    EXPECT_EQ(w1.hasResizer, w2.hasResizer);
    EXPECT_EQ(w1.isMinimized, w2.isMinimized);
    expectEq(w1.clientArea, w2.clientArea);
    expectEq(w1.exitButton, w2.exitButton);
    expectEq(w1.minMaxButton, w2.minMaxButton);

    if(w1.hasResizer)
    {
        expectEq(w1.resizer, w2.resizer);
    }

    using T = Gui::SideBottomWindow::Type;
    switch(w1.type)
    {
        case T::CONTAINER:
            expectEq(w1.containerUpButton, w2.containerUpButton);
            break;
        case T::BATTLE:
            expectEq(w1.expandButton, w2.expandButton);
            break;

        default:
            break;
    }
}

void expectEq(const Gui::Button& b1, const Gui::Button& b2)
{
    EXPECT_EQ(b1.x, b2.x);
    EXPECT_EQ(b1.y, b2.y);
    EXPECT_EQ(b1.width, b2.width);
    EXPECT_EQ(b1.height, b2.height);
    EXPECT_EQ(b1.isDown, b2.isDown);
    EXPECT_EQ(b1.text, b2.text);
}


void expectEq(const Gui::Data& f1, const Gui::Data& f2)
{
    EXPECT_EQ(f1.state, f2.state);
    EXPECT_EQ(f1.cap, f2.cap);
    EXPECT_EQ(f1.soul, f2.soul);
    EXPECT_EQ(f1.mana, f2.mana);
    EXPECT_EQ(f1.hp, f2.hp);
    EXPECT_FLOAT_EQ(f1.hpLevel, f2.hpLevel);
    EXPECT_FLOAT_EQ(f1.manaLevel, f2.manaLevel);
    EXPECT_EQ(f1.level, f2.level);
    EXPECT_EQ(f1.experience, f2.experience);
    EXPECT_EQ(f1.xpGainRate, f2.xpGainRate);
    EXPECT_EQ(f1.speed, f2.speed);
    EXPECT_EQ(f1.foodMinutes, f2.foodMinutes);
    EXPECT_EQ(f1.staminaMinutes, f2.staminaMinutes);
    EXPECT_EQ(f1.offlineTrainingMinutes, f2.offlineTrainingMinutes);
    EXPECT_EQ(f1.magicLevel, f2.magicLevel);
    EXPECT_EQ(f1.fistLevel, f2.fistLevel);
    EXPECT_EQ(f1.clubLevel, f2.clubLevel);
    EXPECT_EQ(f1.swordLevel, f2.swordLevel);
    EXPECT_EQ(f1.axeLevel, f2.axeLevel);
    EXPECT_EQ(f1.distanceLevel, f2.distanceLevel);
    EXPECT_EQ(f1.shieldingLevel, f2.shieldingLevel);
    EXPECT_EQ(f1.fishingLevel, f2.fishingLevel);
    EXPECT_EQ(f1.critChance, f2.critChance);
    EXPECT_EQ(f1.critDamage, f2.critDamage);
    EXPECT_EQ(f1.hpLeechChance, f2.hpLeechChance);
    EXPECT_EQ(f1.hpLeechAmount, f2.hpLeechAmount);
    EXPECT_EQ(f1.manaLeechChance, f2.manaLeechChance);
    EXPECT_EQ(f1.manaLeechAmount, f2.manaLeechAmount);

    EXPECT_EQ(f1.equipment, f2.equipment);
    EXPECT_EQ(f1.onlineVips, f2.onlineVips);
    EXPECT_EQ(f1.offlineVips, f2.offlineVips);


    expectEq(f1.containers, f2.containers);
//        expectEq(f1.sideBottomWindows, f2.sideBottomWindows);
    expectEq(f1.npcTradeWindow, f2.npcTradeWindow);
    expectEq(f1.battleWindow, f2.battleWindow);
}

void expectEq(const Scene::Object& f1, const Scene::Object& f2)
{
    EXPECT_EQ(f1.isOnStack, f2.isOnStack);
    EXPECT_EQ(f1.layer, f2.layer);
    EXPECT_EQ(f1.object, f2.object);
    EXPECT_EQ(f1.tileX, f2.tileX);
    EXPECT_EQ(f1.tileY, f2.tileY);
    EXPECT_FLOAT_EQ(f1.screenX, f2.screenX);
    EXPECT_FLOAT_EQ(f1.screenY, f2.screenY);
}

void expectEq(const Scene::Tile& f1, const Scene::Tile& f2)
{
    expectEq(f1.knownLayerObjects, f2.knownLayerObjects);
    EXPECT_EQ(f1.height, f2.height);
    EXPECT_EQ(f1.numLayers, f2.numLayers);
    EXPECT_EQ(f1.stackStartLayer, f2.stackStartLayer);
    EXPECT_EQ(f1.tileX, f2.tileX);
    EXPECT_EQ(f1.tileY, f2.tileY);
}

void expectEq(const Scene::Data& f1, const Scene::Data& f2)
{
    expectEq(f1.tiles, f2.tiles);
}

void expectEq(const ParsedFrame& f1, const ParsedFrame& f2)
{
    expectEq(f1.gui, f2.gui);
    expectEq(f1.scene, f2.scene);
}


void expectEq(const Draw& d1, const Draw& d2)
{
    expectEq(d1.topLeft, d2.topLeft);
    expectEq(d1.botRight, d2.botRight);

    EXPECT_EQ(d1.transform == nullptr, d2.transform == nullptr);
    if(d1.transform != nullptr)
    {
        expectEq(*d1.transform, *d2.transform);
    }
}

void expectEq(const SpriteDraw::SpriteObjectPairing& p1, const SpriteDraw::SpriteObjectPairing& p2)
{
    EXPECT_EQ(p1.spriteId, p2.spriteId);
    EXPECT_EQ(p1.objects, p2.objects);
}

void expectEq(const SpriteDraw& d1, const SpriteDraw& d2)
{
    expectEq((const Draw&)d1, (const Draw&)d2);
    expectEq(d1.pairings, d2.pairings);
}

void expectEq(const GuiDraw& d1, const GuiDraw& d2)
{
    expectEq((const Draw&)d1, (const Draw&)d2);
    EXPECT_EQ(d1.name, d2.name);
}

void expectEq(const GlyphDraw& d1, const GlyphDraw& d2)
{
    expectEq((const Draw&)d1, (const Draw&)d2);
    EXPECT_EQ(d1.character, d2.character);
}

void expectEq(const TextDraw& d1, const TextDraw& d2)
{
    expectEq((const Draw&)d1, (const Draw&)d2);
    EXPECT_EQ(d1.color.packed, d2.color.packed);
    EXPECT_EQ(d1.isOutlined, d2.isOutlined);
    expectEq(d1.glyphDraws, d2.glyphDraws);
}

void expectEq(const RectDraw& d1, const RectDraw& d2)
{
    expectEq((const Draw&)d1, (const Draw&)d2);
    EXPECT_EQ(d1.color.packed, d2.color.packed);
}

void expectEq(const MiniMapDraw& d1, const MiniMapDraw& d2)
{
    expectEq((const Draw&)d1, (const Draw&)d2);
    EXPECT_EQ(d1.pixels == nullptr, d2.pixels == nullptr);
    if(d1.pixels != nullptr)
    {
        EXPECT_EQ(*d1.pixels, *d2.pixels);
    }
}


void expectEq(const RawImage& d1, const RawImage& d2)
{
    EXPECT_EQ(d1.format, d2.format);
    EXPECT_EQ(d1.width, d2.width);
    EXPECT_EQ(d1.height, d2.height);
    EXPECT_EQ(d1.pixels, d2.pixels);
}

void expectEq(const Frame& f1, const Frame& f2)
{
    EXPECT_EQ(f1.hasMiniMapMoved, f2.hasMiniMapMoved);
    EXPECT_EQ(f1.miniMapX, f2.miniMapX);
    EXPECT_EQ(f1.miniMapY, f2.miniMapY);
    EXPECT_EQ(f1.miniMapScreenX, f2.miniMapScreenX);
    EXPECT_EQ(f1.miniMapScreenY, f2.miniMapScreenY);
    EXPECT_EQ(f1.miniMapScreenWidth, f2.miniMapScreenWidth);
    EXPECT_EQ(f1.miniMapScreenHeight, f2.miniMapScreenHeight);
    EXPECT_EQ(f1.hasViewUpdated, f2.hasViewUpdated);
    EXPECT_EQ(f1.viewX, f2.viewX);
    EXPECT_EQ(f1.viewY, f2.viewY);
    EXPECT_EQ(f1.viewWidth, f2.viewWidth);
    EXPECT_EQ(f1.viewHeight, f2.viewHeight);
    EXPECT_EQ(f1.width, f2.width);
    EXPECT_EQ(f1.height, f2.height);

    expectEq(f1.spriteDraws, f2.spriteDraws);
    expectEq(f1.guiDraws, f2.guiDraws);
    expectEq(f1.guiSpriteDraws, f2.guiSpriteDraws);
    expectEq(f1.textDraws, f2.textDraws);
    expectEq(f1.rectDraws, f2.rectDraws);
//        expectEq(f1.fileIo, f2.fileIo);
    expectEq(f1.miniMapDraws, f2.miniMapDraws);

    std::shared_ptr<RawImage> flippedScreenPixels;
    if(f1.screenPixels != nullptr)
    {
        QImage flippedScreen(f1.screenPixels->pixels.data(), f1.screenPixels->width, f1.screenPixels->height, QImage::Format_RGBA8888);
        flippedScreen = flippedScreen.mirrored(false, true);
        flippedScreenPixels.reset(new RawImage(sb::utility::PixelFormat::RGBA, flippedScreen.width(), flippedScreen.height(), flippedScreen.bits()));
        EXPECT_EQ(flippedScreen.byteCount(), f1.screenPixels->pixels.size());
    }
    expectEq(flippedScreenPixels, f2.screenPixels);
}

}
}
