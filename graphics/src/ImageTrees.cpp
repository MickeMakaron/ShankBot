///////////////////////////////////
// Internal ShankBot headers
#include "ImageTrees.hpp"
#include "ImageTree.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


ImageTrees::TreePtr ImageTrees::mSpriteColor;
ImageTrees::TreePtr ImageTrees::mSpriteTransparency;

void ImageTrees::initializeSpriteColorTree(std::string binPath)
{
    mSpriteColor = TreePtr(new ImageTree(binPath));
}

void ImageTrees::initializeSpriteTransparencyTree(std::string binPath)
{
    mSpriteTransparency = TreePtr(new ImageTree(binPath));
}

const ImageTree& ImageTrees::getSpriteColorTree()
{
    if(mSpriteColor.get() == nullptr)
        throw std::runtime_error("Tried to fetch SpriteColorTree without initializing it first.");

    return *mSpriteColor;
}

const ImageTree& ImageTrees::getSpriteTransparencyTree()
{
    if(mSpriteTransparency.get() == nullptr)
        throw std::runtime_error("Tried to fetch SpriteTransparencyTree without initializing it first.");

    return *mSpriteTransparency;
}
