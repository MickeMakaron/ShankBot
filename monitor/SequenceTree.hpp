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
#ifndef GRAPHICS_LAYER_SEQUENCE_TREE_HPP
#define GRAPHICS_LAYER_SEQUENCE_TREE_HPP

///////////////////////////////////
// STD C++
#include <vector>
#include <memory>
#include <unordered_map>
#include <list>
///////////////////////////////////

namespace GraphicsLayer
{
    class SequenceTree
    {
        public:
            struct Element
            {
                explicit Element(const std::vector<size_t>* sequence, unsigned int id) : sequence(sequence), id(id){};

                const std::vector<size_t>* sequence;
                const unsigned int id;
                int numValuesIgnored;
            };

            struct Node;
            typedef std::unique_ptr<Node> NodePtr;
            struct Node
            {
                explicit Node(unsigned int level) : level(level){};

                std::unordered_map<size_t, NodePtr> children;
                std::list<Element> elements;
                unsigned int level;
            };

        public:
            explicit SequenceTree(const std::vector<std::vector<size_t>>& sequences, const std::vector<unsigned int>& ids);
            explicit SequenceTree(std::string filePath);


            bool find(const std::vector<size_t>& sequence, std::list<unsigned int>& ids) const;
            size_t getSize() const;
            void writeToBinaryFile(std::string filePath) const;
            std::list<size_t> trace(unsigned int id) const;



        private:
            size_t getSize(const NodePtr& node) const;
            void insert(NodePtr& node, Element element);
            void writeToBinaryFile(const NodePtr& node, std::ostream& file) const;
            void trace(const NodePtr& node, unsigned int id, bool& isFound, std::list<size_t>& breadcrumbs) const;

            void forEachNode(NodePtr& node, std::function<void(NodePtr& node)> function);

            void removeEmptyNodes();


            void loadFromBinaryFile(std::string filePath);
            void loadFromBinaryFile(NodePtr& node, std::istream& file);

        private:
            NodePtr mRoot;

    };
}


#endif // GRAPHICS_LAYER_SEQUENCE_TREE_HPP
