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
#ifndef GRAPHICS_LAYER_FRAME_PARSER_HPP
#define GRAPHICS_LAYER_FRAME_PARSER_HPP

///////////////////////////////////
// Internal ShankBot headers
#include "TileBufferCache.hpp"
#include "injection/SharedMemoryProtocol.hpp"
#include "FontSample.hpp"
#include "Frame.hpp"
#include "GenericType.hpp"
#include "CombatSquareSample.hpp"

#include "utility/utility.hpp" // REMOVE LATER
 // REMOVE LATER
#include <cassert> // REMOVE LATER

namespace GraphicsLayer
{
    class TibiaContext;
}
///////////////////////////////////


///////////////////////////////////
// STD C++
#include <set>
#include <memory>
///////////////////////////////////

namespace GraphicsLayer
{
    class FrameParser
    {
        public:
            struct MiniMapData
            {
                static const size_t width = Constants::MINI_MAP_PIXEL_WIDTH;
                static const size_t height = Constants::MINI_MAP_PIXEL_HEIGHT;
                static const size_t bytesPerPixel = sb::utility::BYTES_PER_PIXEL_RGBA;
            };

        private:
            struct VertexBuffer
            {
                void* data = nullptr;
                unsigned int numBytes = 0;

                enum class VertexType : unsigned char
                {
                    TEXTURED,
                    COLORED,
                    TEXT,
                    TEXTURED_NO_ORDER,
                    UNDEFINED
                };

                unsigned int getVerticesOffset() const;

                VertexType vertexType = VertexType::UNDEFINED;
                std::map<unsigned char, SharedMemoryProtocol::VertexAttribPointer> attribPointers;
            };

            struct Texture
            {
                short width = 0;
                short height = 0;
            };

            struct ShaderProgram
            {
                sb::utility::Matrix<float, 4, 4> transform;
                std::map<unsigned int, float[4]> uniform4fs;
            };

            struct TileNumber
            {
                enum class Type : unsigned char
                {
                    INVALID,
                    ICE_DAMAGE,
                    PHYSICAL_DAMAGE,
                    POISON_DAMAGE,
                    XP_GAIN,
                    MANA_GAIN,
                    HP_GAIN,
                    FIRE_DAMAGE,
                    ENERGY_DAMAGE,
                };

                unsigned int number = 0;
                Type type = Type::INVALID;

            };

            class Tile
            {
                public:
                    enum class Type : unsigned char
                    {
                        INVALID,
                        GRAPHICS_RESOURCE_NAMES,
                        SPRITE_OBJECT_PAIRINGS,
                        TILE_NUMBER,
                        BLANK,
                        GLYPH,
                        COMBAT_SQUARE,
                        ENUM_END,
                    };

                public:
                    Tile(size_t width, size_t height, Type type, const std::shared_ptr<GenericType>& data = nullptr);
                    Tile();
                    Tile(Type type, const std::shared_ptr<GenericType>& data = nullptr);

                    Type getType() const;
                    size_t getWidth() const;
                    size_t getHeight() const;
                    GenericType* getData() const;

                    void setWidth(size_t width);
                    void setHeight(size_t height);

                private:
                    size_t mWidth = 0;
                    size_t mHeight = 0;
                    Type mType = Type::INVALID;
                    std::shared_ptr<GenericType> mData;
            };

            template<typename T, Tile::Type type>
            struct TileData : public GenericType
            {
                TileData(const T& d) : data(d){};
                TileData(){};

                static T& fromTile(const Tile& t)
                {
                    assert(t.getType() == type);
                    return ((TileData<T, type>*)t.getData())->data;
                }

                static Tile createTile(size_t width, size_t height, const T& d)
                {
                    return Tile(width, height, type, std::make_shared<TileData<T, type>>(d));
                }

                T data;
            };

            typedef TileData<std::list<std::string>, Tile::Type::GRAPHICS_RESOURCE_NAMES> GraphicsResourceNamesData;
            typedef TileData<std::list<SpriteDraw::SpriteObjectPairing>, Tile::Type::SPRITE_OBJECT_PAIRINGS> SpriteObjectPairingsData;
            typedef TileData<TileNumber, Tile::Type::TILE_NUMBER> TileNumberData;
            typedef TileData<unsigned char, Tile::Type::GLYPH> GlyphData;
            typedef TileData<CombatSquareSample::CombatSquare::Type, Tile::Type::COMBAT_SQUARE> CombatSquareData;

        public:
            explicit FrameParser(const TibiaContext& context);
            std::list<Frame> parse(const SharedMemoryProtocol::SharedMemorySegment* segment);

        private:
            void updateTileBuffer(const SharedMemoryProtocol::PixelData& data, const unsigned char* pixels);
            void updateMiniMapPixels(const SharedMemoryProtocol::PixelData& pixelData, const unsigned char* pixels);

            unsigned char getChar(unsigned textureId, unsigned short x, unsigned short y, unsigned short width, unsigned short height);
            TileNumber getTileNumber(const unsigned char* pixels, unsigned short width, unsigned short height, sb::utility::PixelFormat format) const;
            Tile getTile(unsigned int textureId, unsigned short x, unsigned short y);

            unsigned char getChar(const unsigned char* pixels, unsigned short width, unsigned short height, std::list<unsigned char>* topTen = nullptr) const;
            void copyGlyphs(unsigned int srcTexId, unsigned short srcX, unsigned short srcY, unsigned int tarTexId, unsigned short tarX, unsigned short tarY, unsigned short width, unsigned short height);
            void copyGlyphs(const SharedMemoryProtocol::DrawCall& drawCall);

            void parsePixelData(const SharedMemoryProtocol::PixelData& pixelData, const unsigned char* pixels);
            void parseGlyphPixelData(const SharedMemoryProtocol::PixelData& pixelData, const unsigned char* pixels);
            void parseCopyTexture(const SharedMemoryProtocol::CopyTexture& copy);
            void parseTextureData(const SharedMemoryProtocol::TextureData& textureData);
            void parseVertexBufferWrite(const SharedMemoryProtocol::VertexBufferWrite& bufferWrite, const char* bufferData);
            void parseVertexAttribPointer(const SharedMemoryProtocol::VertexAttribPointer& bufferInfo);
            void parseDrawCall(const SharedMemoryProtocol::DrawCall& drawCall);
            void parseTransformationMatrix(const SharedMemoryProtocol::TransformationMatrix& transform);
            void parseUniform4f(const SharedMemoryProtocol::Uniform4f& uniform);

            void parseRectDraw(const SharedMemoryProtocol::DrawCall& drawCall);
            void parseGlyphDraw(const SharedMemoryProtocol::DrawCall& drawCall);
            void parseSpriteDraw(const SharedMemoryProtocol::DrawCall& drawCall);
            void parseGuiTileDraw(const SharedMemoryProtocol::DrawCall& drawCall);
            void parseTileDraw(const SharedMemoryProtocol::DrawCall& drawCall);
            void parseUnshadedViewDraw(const SharedMemoryProtocol::DrawCall& drawCall);
            void parseMiniMapDraw(const SharedMemoryProtocol::DrawCall& drawCall);
            void parseFileIo(const SharedMemoryProtocol::FileIo& io, const char* data) const;


        private:
            static const unsigned short VIEW_WIDTH = 480;
            static const unsigned short VIEW_HEIGHT = 352;

            const TibiaContext& mContext;
            TileBufferCache<Tile> mTileCache;

            std::map<unsigned short, VertexBuffer> mVertexBuffers;
            std::map<unsigned int, Texture> mTextures;
            std::map<unsigned int, ShaderProgram> mShaderPrograms;
            unsigned int mTileBufferId = 0;
            std::map<unsigned int, std::shared_ptr<std::vector<unsigned char>>> mMiniMapBuffers;

            unsigned int mUnshadedViewBufferId = 0;
            unsigned int mShadedViewBufferId = 0;
            std::set<unsigned int> mGlyphBufferIds;

            Frame mCurrentFrame;

            size_t mDrawCallId;
    };

}


#endif // GRAPHICS_LAYER_FRAME_PARSER_HPP
