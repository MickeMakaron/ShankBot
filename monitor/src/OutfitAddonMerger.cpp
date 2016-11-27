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
///////////////////////////////////
// Internal ShankBot headers
#include "monitor/OutfitAddonMerger.hpp"
#include "tibiaassets/Object.hpp"
#include "utility/utility.hpp"
using namespace sb::tibiaassets;
using namespace GraphicsLayer;
///////////////////////////////////

///////////////////////////////////
// Qt
#include "QtGui/QImage"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cassert>
///////////////////////////////////


const std::vector<OutfitAddonMerger::SnapshotMerger::Combinations> OutfitAddonMerger::SnapshotMerger::M_COMBINATIONS_MAP =
{
    sb::utility::generateCommutativeCombinations(0),
    sb::utility::generateCommutativeCombinations(1),
    sb::utility::generateCommutativeCombinations(2),
    sb::utility::generateCommutativeCombinations(3),
    sb::utility::generateCommutativeCombinations(4)
};

OutfitAddonMerger::SnapshotMerger::SnapshotMerger(const OutfitSnapshot& snap)
{
    assert(!snap.spriteIds.empty());
    mBaseSpriteId = snap.spriteIds.front();

    for(const size_t& id : snap.spriteIds)
        mNeededSprites.insert(id);
}

void OutfitAddonMerger::SnapshotMerger::process(const std::shared_ptr<Sprite>& spr)
{
    if(mNeededSprites.erase(spr->id) == 0)
        return;

    if(spr->id == mBaseSpriteId)
        mBaseSprite = spr;
    else
        mAddonSprites.push_back(spr);
}

bool OutfitAddonMerger::SnapshotMerger::isReady() const
{
    return mNeededSprites.empty();
}

const std::set<size_t>& OutfitAddonMerger::SnapshotMerger::getNeededSprites() const
{
    return mNeededSprites;
}

std::vector<std::shared_ptr<Sprite>> OutfitAddonMerger::SnapshotMerger::merge() const
{
    assert(isReady());
    assert(mBaseSprite != nullptr);

    std::vector<std::shared_ptr<Sprite>> merges;
    merges.push_back(mBaseSprite);

    size_t width = mBaseSprite->width;
    size_t height = mBaseSprite->height;
    for(const std::shared_ptr<Sprite>& spr : mAddonSprites)
        assert(spr->height == height && spr->width == width);

    static size_t saveCount = 0;
//            QImage img(mBaseSprite->pixels.data(), width, height, QImage::Format_RGBA8888);
//            img.save(QString::fromStdString(stringify("alphaUnion/", saveCount++, ".png")));

    for(const Combination& combo : M_COMBINATIONS_MAP[mAddonSprites.size()])
    {
        std::vector<unsigned char> alphaUnion = mBaseSprite->pixels;
        for(size_t index : combo)
            alphaUnion = sb::utility::rgbaAlphaUnion(alphaUnion.data(), mAddonSprites[index]->pixels.data(), width, height);

        merges.push_back(std::make_shared<Sprite>(mBaseSpriteId, width, height, alphaUnion));

//                QImage img(alphaUnion.data(), width, height, QImage::Format_RGBA8888);
//                img.save(QString::fromStdString(stringify("alphaUnion/", saveCount++, ".png")));
    }


    return merges;
}

void OutfitAddonMerger::addOutfit(const Object& outfit)
{
    if(outfit.type != Object::Type::OUTFIT)
        return;

    const Object::SpriteInfo& info = outfit.someInfos.front().spriteInfo;
    if(info.numAddons <= 1 || info.numMounts <= 1)
        return;

    for(const SnapshotMerger::OutfitSnapshot& snap : getOutfitSnapshots(outfit))
    {
        std::shared_ptr<SnapshotMerger> merger = std::make_shared<SnapshotMerger>(snap);
        for(const size_t& id : merger->getNeededSprites())
            mMerges[id].push_back(merger);
    }
}

std::vector<std::shared_ptr<Sprite>> OutfitAddonMerger::processSprite(unsigned int id, size_t width, size_t height, const unsigned char* pixels)
{
    auto mergersIt = mMerges.find(id);
    if(mergersIt == mMerges.end())
        return {};

    std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>(id, width, height, pixels);
    std::vector<std::shared_ptr<Sprite>> merges;
    for(const std::shared_ptr<SnapshotMerger>& m : mergersIt->second)
    {
        m->process(sprite);
        if(m->isReady())
        {
            std::vector<std::shared_ptr<Sprite>> newMerges = m->merge();
            merges.insert(merges.end(), newMerges.begin(), newMerges.end());
        }
    }

    mMerges.erase(mergersIt);

    return merges;
}

bool OutfitAddonMerger::isEmpty() const
{
    return mMerges.empty();
}


std::vector<OutfitAddonMerger::SnapshotMerger::OutfitSnapshot> OutfitAddonMerger::getOutfitSnapshots(const Object& outfit) const
{
    assert(outfit.type == Object::Type::OUTFIT);

    std::vector<SnapshotMerger::OutfitSnapshot> snapshots;
    for(const Object::SomeInfo& someInfo : outfit.someInfos)
    {
        const Object::SpriteInfo& info = someInfo.spriteInfo;
        size_t colSize = info.numAddons;
        size_t colStride = info.numBlendFrames;
        size_t rowSize = info.numBlendFrames * info.numDirections;
        size_t rowStride = rowSize * (colSize - 1);

        snapshots.reserve(snapshots.size() + info.spriteIds.size() / (colSize * colStride));
        for(size_t i = 0; i < info.spriteIds.size(); i += rowStride)
        {
            for(size_t x = 0; x < rowSize; x += colStride, i += colStride)
            {
                snapshots.emplace_back();
                std::vector<size_t>& ids = snapshots.back().spriteIds;
                ids.reserve(colSize);
                for(size_t y = 0; y < colSize; y++)
                    ids.push_back(info.spriteIds[i + y * rowSize]);
            }
        }

    }
    return snapshots;
}
