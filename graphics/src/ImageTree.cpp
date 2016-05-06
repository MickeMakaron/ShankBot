///////////////////////////////////
// Internal ShankBot headers
#include "ImageTree.hpp"
using namespace GraphicsLayer;
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <fstream>
#include <sstream>
///////////////////////////////////



ImageTree::ImageTree(const std::vector<std::vector<unsigned char>>& sprites, const std::vector<size_t>& ids)
: mRoot(new Node())
{
    if(sprites.size() != ids.size())
        throw std::runtime_error("Sprite count does not match ID count.");

    for(size_t i = 0; i < sprites.size(); i++)
    {
        if(!sprites[i].empty())
            insert(mRoot, Sprite(&sprites[i], ids[i]));
    }

}

ImageTree::ImageTree(std::string filePath)
: mRoot(new Node())
{
    loadFromBinaryFile(filePath);
}

void ImageTree::loadFromTextFile(std::string filePath)
{
    std::ifstream file(filePath);

    loadFromTextFile(mRoot, file);

    file.close();
}

void ImageTree::loadFromTextFile(NodePtr& node, std::istream& file)
{
    std::string sprites;
    std::getline(file, sprites);

    std::string children;
    std::getline(file, children);

    std::istringstream istream(sprites);
    unsigned int spriteId;
    istream >> spriteId;
    while(istream.good())
    {
        node->sprites.push_back(Sprite(nullptr, spriteId));
        istream >> spriteId;
    }

    istream.str(std::string());
    istream.clear();

    istream.str(children);
    unsigned int pixelValue;
    istream >> pixelValue;
    while(istream.good())
    {
        auto it = node->children.insert(std::make_pair(pixelValue, NodePtr(new Node())));
        loadFromTextFile(it.first->second, file);

        istream >> pixelValue;
    }
}


void ImageTree::loadFromBinaryFile(std::string filePath)
{
    std::ifstream file(filePath, std::ios::binary);

    loadFromBinaryFile(mRoot, file);

    file.close();
}

void ImageTree::loadFromBinaryFile(NodePtr& node, std::istream& file)
{
    unsigned char numSprites;
    file.read((char*)&numSprites, sizeof(numSprites));
    if(numSprites > 0)
    {
        unsigned int* sprites = new unsigned int[numSprites];
        file.read((char*)sprites, sizeof(unsigned int) * numSprites);
        for(size_t i = 0; i < numSprites; i++)
            node->sprites.push_back(Sprite(nullptr, sprites[i]));

        delete[] sprites;
    }

    unsigned short numChildren;
    file.read((char*)&numChildren, sizeof(numChildren));
    if(numChildren > 0)
    {
        unsigned char* children = new unsigned char[numChildren];
        file.read((char*)children, sizeof(unsigned char) * numChildren);
        for(size_t i = 0; i < numChildren; i++)
        {
            auto it = node->children.insert(std::make_pair(children[i], NodePtr(new Node())));
            loadFromBinaryFile(it.first->second, file);
        }

        delete[] children;
    }
}


void ImageTree::insert(NodePtr& node, Sprite sprite, size_t level)
{
    if(level >= sprite.pixels->size())
    {
        node->sprites.push_back(sprite);
        return;
    }


    if(!node->sprites.empty())
    {
        node->sprites.push_back(sprite);

        for(auto it = node->sprites.begin(); it != node->sprites.end();)
        {
            if(level < it->pixels->size())
            {
                auto child = node->children.insert(std::make_pair(it->pixels->operator[](level), NodePtr(new Node())));
                insert(child.first->second, *it, level + 1);

                node->sprites.erase(it++);
            }
            else
                it++;
        }
    }
    else
    {
        if(node->children.empty())
        {
            node->sprites.push_back(sprite);
            return;
        }
        else
        {
            auto it = node->children.insert(std::make_pair(sprite.pixels->operator[](level), NodePtr(new Node())));
            insert(it.first->second, sprite, level + 1);
        }
    }
}

size_t ImageTree::getSize() const
{
    return getSize(mRoot);
}

size_t ImageTree::getSize(const NodePtr& node) const
{
    size_t size = 0;
    for(const auto& pair : node->children)
        size += getSize(pair.second);

    size += node->sprites.size();

    return size;
}

bool ImageTree::find(const std::vector<unsigned char>& pixels, std::list<size_t>& ids) const
{
    Node* node = mRoot.get();
    for(size_t i = 0; i < pixels.size(); i++)
    {

        auto it = node->children.find(pixels[i]);

        if(it == node->children.end())
        {
            break;
        }

        node = it->second.get();
    }

    if(node->sprites.empty())
        return false;

    for(const Sprite& sprite : node->sprites)
        ids.push_back(sprite.id);
    return true;
}

void ImageTree::writeToTextFile(std::string filePath) const
{
    std::ofstream file(filePath);
    writeToTextFile(mRoot, file);
    file.close();
}

void ImageTree::writeToTextFile(const NodePtr& node, std::ostream& file) const
{
    for(const Sprite& sprite : node->sprites)
        file << sprite.id << " ";

    file << std::endl;

    for(const auto& pair : node->children)
        file << (int)pair.first << " ";

    file << std::endl;

    for(const auto& pair : node->children)
        writeToTextFile(pair.second, file);
}


void ImageTree::writeToBinaryFile(std::string filePath) const
{
    std::ofstream file(filePath, std::ios::binary);
    writeToBinaryFile(mRoot, file);
    file.close();
}

void ImageTree::writeToBinaryFile(const NodePtr& node, std::ostream& file) const
{
    unsigned int* sprites = new unsigned int[node->sprites.size()];
    size_t i = 0;
    for(const Sprite& sprite : node->sprites)
        sprites[i++] = sprite.id;

    unsigned char numSprites = node->sprites.size();
    file.write((char*)&numSprites, sizeof(numSprites));
    file.write((char*)sprites, sizeof(unsigned int) * (node->sprites.size()));
    delete[] sprites;


    unsigned short numChildren = node->children.size();
    file.write((char*)&numChildren, sizeof(numChildren));


    unsigned char* children = new unsigned char[node->children.size()];
    i = 0;
    for(const auto& pair : node->children)
        children[i++] = pair.first;

    file.write((char*)children, sizeof(unsigned char) * node->children.size());
    delete[] children;

    for(const auto& pair : node->children)
        writeToBinaryFile(pair.second, file);
}

std::list<unsigned char> ImageTree::trace(size_t id) const
{
    std::list<unsigned char> breadcrumbs;
    bool isFound = false;
    trace(mRoot, id, isFound, breadcrumbs);

    return breadcrumbs;
}

void ImageTree::trace(const NodePtr& node, size_t id, bool& isFound, std::list<unsigned char>& breadcrumbs) const
{
    for(const Sprite& sprite : node->sprites)
        if(sprite.id == id)
        {
            isFound = true;
            return;
        }

    for(const auto& pair : node->children)
    {
        trace(pair.second, id, isFound, breadcrumbs);
        if(isFound)
        {
            breadcrumbs.push_front(pair.first);
            return;
        }
    }
}


