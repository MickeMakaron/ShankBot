#ifndef GRAPHICS_LAYER_TIBIA_CONTEXT
#define GRAPHICS_LAYER_TIBIA_CONTEXT

///////////////////////////////////
// Internal ShankBot headers
#include "TibiaDat.hpp"
#include "ImageTree.hpp"
#include "SpriteObjectBindings.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <memory>
///////////////////////////////////

namespace GraphicsLayer
{
   class TibiaContext
    {
        private:
            template<typename T>
            using UPtr = std::unique_ptr<T>;

        public:
            explicit TibiaContext
            (
                UPtr<TibiaDat>& dat,
                UPtr<SpriteObjectBindings>& spriteObjectBindings,
                UPtr<ImageTree>& spriteColorTree,
                UPtr<ImageTree>& spriteTransparencyTree
            );

            const TibiaDat& getDat() const;
            const SpriteObjectBindings& getSpriteObjectBindings() const;
            const ImageTree& getSpriteColorTree() const;
            const ImageTree& getSpriteTransparencyTree() const;

        private:
            UPtr<TibiaDat> mDat;
            UPtr<SpriteObjectBindings> mSpriteObjectBindings;
            UPtr<ImageTree> mSpriteColorTree;
            UPtr<ImageTree> mSpriteTransparencyTree;
    };
}


#endif // GRAPHICS_LAYER_TIBIA_CONTEXT
