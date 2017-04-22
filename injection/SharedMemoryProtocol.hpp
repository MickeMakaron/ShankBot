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
#ifndef GRAPHICS_LAYER_SHARED_MEMORY_PROTOCOL_HPP
#define GRAPHICS_LAYER_SHARED_MEMORY_PROTOCOL_HPP

#if defined(_WIN32)
#include <windows.h>
#else
#endif


///////////////////////////////////
// Internal ShankBot headers
#include "utility/Matrix.hpp"
#include "utility/Color.hpp"
#include "utility/PixelFormat.hpp"
///////////////////////////////////

///////////////////////////////////
// STD C++
#include <cstdint>
///////////////////////////////////

namespace GraphicsLayer
{
    namespace SharedMemoryProtocol
    {
        struct Message
        {

            enum class MessageType : unsigned char
            {
                PIXEL_DATA,
                TEXTURE_DATA,
                VERTEX_BUFFER_WRITE,
                VERTEX_ATTRIB_POINTER,
                DRAW_CALL,
                TRANSFORMATION_MATRIX,
                UNIFORM_4_F,
                COPY_TEXTURE,
                FILE_IO,

                SCREEN_PIXELS,
                INVALID
            };

            Message() : messageType(MessageType::INVALID){}
            MessageType messageType;
        };

        struct PixelData : public Message
        {
            unsigned short texX;
            unsigned short texY;
            unsigned short width;
            unsigned short height;
            unsigned int targetTextureId;
            sb::utility::PixelFormat format;
        };

        struct CopyTexture : public Message
        {
            unsigned short srcX;
            unsigned short srcY;
            unsigned short targetX;
            unsigned short targetY;
            unsigned short width;
            unsigned short height;
            unsigned int sourceTextureId;
            unsigned int targetTextureId;
        };

        struct TextureData : public Message
        {
            unsigned short width;
            unsigned short height;
            unsigned int id;
        };

        struct VertexBufferWrite : public Message
        {
            unsigned int bufferId = 0;
            unsigned int numBytes = 0;
        };

        struct VertexAttribPointer : public Message
        {
            typedef float Order;
            typedef unsigned short Index;

            unsigned char index = 0;
            unsigned int bufferId = 0;
            unsigned int stride = 0;
            unsigned int offset = 0;
        };

        struct DrawCall : public Message
        {
            enum class PrimitiveType : unsigned char
            {
                TRIANGLE,
                TRIANGLE_STRIP,
                TRIANGLE_FAN
            };

            DrawCall() : blendColor(), type(PrimitiveType::TRIANGLE){}

            unsigned short bufferId = 0;
            unsigned short programId = 0;
            bool isDepthTestEnabled = false;
            bool isDepthWriteEnabled = true;
            unsigned int sourceTextureId = 0;
            unsigned int targetTextureId = 0;
            unsigned int indicesOffset = 0;
            unsigned int numIndices = 0;
            Color blendColor;
            PrimitiveType type;
        };

        struct TransformationMatrix : public Message
        {
            TransformationMatrix() : matrix() {};

            unsigned short programId = 0;
            sb::utility::Matrix<float, 4, 4> matrix;
        };

        struct Uniform4f : public Message
        {
            unsigned short programId = 0;
            unsigned short location = 0;
            float values[4];
        };

        struct FileIo : public Message
        {
            enum class Type : unsigned char
            {
                READ,
                WRITE,
                INVALID
            };

            Type type = Type::INVALID;
            unsigned short pathSize = 0;
        };

        struct Frame
        {
            unsigned int size = sizeof(Frame);
            unsigned short width = 0;
            unsigned short height = 0;
        };


        const unsigned int DATA_BUFFER_SIZE = 1 << 24;
        const char* const SHARED_MEMORY_ENV_VAR_NAME = "SHANKBOT_SHARED_MEMORY_NAME";
        const unsigned int SHARED_MEMORY_NAME_LENGTH = 128;

        namespace DataFilter
        {
            const uint32_t Standard       = 0;
            const uint32_t ScreenPixels   = 1 << 0;
        };

        struct SharedMemorySegment
        {
            char data[DATA_BUFFER_SIZE];
            unsigned int size;

            #if defined(_WIN32)
            HWND window = NULL;
            HANDLE parentProcessHandle = NULL;
            HANDLE semWrite = NULL;
            HANDLE semRead = NULL;
            #else

            #endif


            uint32_t dataFilters = DataFilter::Standard;

            bool isClientAttached = false;
        };

        const unsigned int NUM_BYTES = sizeof(SharedMemorySegment);
    }
}



#endif // GRAPHICS_LAYER_SHARED_MEMORY_PROTOCOL_HPP
