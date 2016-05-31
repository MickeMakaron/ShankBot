#ifndef GRAPHICS_LAYER_IMAGE_TREE
#define GRAPHICS_LAYER_IMAGE_TREE

///////////////////////////////////
// STD C++
#include <vector>
#include <memory>
#include <unordered_map>
#include <list>
///////////////////////////////////

namespace GraphicsLayer
{
    class ImageTree
    {
        public:
            struct Sprite
            {
                explicit Sprite(const std::vector<unsigned char>* pixels, size_t id) : pixels(pixels), id(id){};

                const std::vector<unsigned char>* pixels;
                const size_t id;
                int numValuesIgnored;
            };

            struct Node;
            typedef std::unique_ptr<Node> NodePtr;
            struct Node
            {
                explicit Node(size_t level) : level(level){};

                std::unordered_map<unsigned char, NodePtr> children;
                std::list<Sprite> sprites;
                size_t level;
            };

        public:
            explicit ImageTree(const std::vector<std::vector<unsigned char>>& sprites, const std::vector<size_t>& ids);
            explicit ImageTree(std::string filePath);


            bool find(const std::vector<unsigned char>& pixels, std::list<size_t>& ids) const;
            size_t getSize() const;
            void writeToBinaryFile(std::string filePath) const;
            std::list<unsigned char> trace(size_t id) const;



        private:
            size_t getSize(const NodePtr& node) const;
            void insert(NodePtr& node, Sprite sprite);
            void writeToBinaryFile(const NodePtr& node, std::ostream& file) const;
            void trace(const NodePtr& node, size_t id, bool& isFound, std::list<unsigned char>& breadcrumbs) const;

            void forEachNode(NodePtr& node, std::function<void(NodePtr& node)> function);

            void removeEmptyNodes();


            void loadFromBinaryFile(std::string filePath);
            void loadFromBinaryFile(NodePtr& node, std::istream& file);

        private:
            NodePtr mRoot;

    };
}


#endif // GRAPHICS_LAYER_IMAGE_TREE
