#ifndef GRAPHICS_LAYER_IMAGE_TREES
#define GRAPHICS_LAYER_IMAGE_TREES


///////////////////////////////////
// Internal ShankBot headers
namespace GraphicsLayer
{
    class ImageTree;
}
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <string>
#include <memory>
///////////////////////////////////

namespace GraphicsLayer
{
    class ImageTrees
    {
        public:
            static void initializeSpriteColorTree(std::string binPath);
            static void initializeSpriteTransparencyTree(std::string binPath);

            static const ImageTree& getSpriteColorTree();
            static const ImageTree& getSpriteTransparencyTree();


        private:
            typedef std::unique_ptr<ImageTree> TreePtr;
            static TreePtr mSpriteColor;
            static TreePtr mSpriteTransparency;
    };
}


#endif // GRAPHICS_LAYER_IMAGE_TREES
