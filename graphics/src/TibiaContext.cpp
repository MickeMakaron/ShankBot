///////////////////////////////////
// Internal ShankBot headers
#include "TibiaContext.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


TibiaContext::TibiaContext
(
    UPtr<TibiaDat>& dat,
    UPtr<SpriteObjectBindings>& spriteObjectBindings,
    UPtr<ImageTree>& spriteColorTree,
    UPtr<ImageTree>& spriteTransparencyTree
)
{
    mDat.reset(dat.release());
    mSpriteObjectBindings.reset(spriteObjectBindings.release());
    mSpriteColorTree.reset(spriteColorTree.release());
    mSpriteTransparencyTree.reset(spriteTransparencyTree.release());
}

const TibiaDat& TibiaContext::getDat() const
{
    return *mDat;
}

const SpriteObjectBindings& TibiaContext::getSpriteObjectBindings() const
{
    return *mSpriteObjectBindings;
}

const ImageTree& TibiaContext::getSpriteColorTree() const
{
    return *mSpriteColorTree;
}

const ImageTree& TibiaContext::getSpriteTransparencyTree() const
{
    return *mSpriteTransparencyTree;
}
