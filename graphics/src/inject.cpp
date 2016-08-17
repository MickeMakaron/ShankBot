#include <windows.h>
#include <iostream>
#include <cstdio>
#include "FunctionDetour.hpp"
#include "SharedMemoryProtocol.hpp"
#include "Vertex.hpp"
//
////#include <GL/gl.h>
////#include <GL/glext.h>
#include "GL/glew.h"

#include <cassert>
#include <fstream>
#include <algorithm>
#include <list>

#include <sstream>

using namespace GraphicsLayer;

void writePixelsToFile(std::string fileName, const unsigned char* pixels, size_t width, size_t height)
{

    std::ofstream file;
    file.open(fileName.c_str(), std::ios::trunc);
    file    << "P3" << std::endl
            << width << " " << height << std::endl
            << "255" << std::endl;

    for(size_t i = 0; i < width * height * 3; i++)
        file << (int)pixels[i] << " ";

}

///////////////////////////////////

unsigned char* rgbaToRgb(const unsigned char* rgba, size_t width, size_t height)
{
    unsigned char* rgb = new unsigned char[width * height * 3];
    for(size_t iDest = 0, iSrc = 0; iSrc < width * height * 4; iSrc++)
    {
        if((iSrc + 1) % 4 != 0)
        {
            rgb[iDest] = ((unsigned char*)rgba)[iSrc];
            iDest++;
        }
    }

    return rgb;
}


FunctionDetour* loadLibDetour = nullptr;
FunctionDetour* genTexDetour = nullptr;
FunctionDetour* swapBufDetour = nullptr;
FunctionDetour* delTexDetour = nullptr;
FunctionDetour* texImgDetour = nullptr;
FunctionDetour* subTexImgDetour = nullptr;
FunctionDetour* compTexImgDetour = nullptr;
FunctionDetour* subCompTexImgDetour = nullptr;
FunctionDetour* bufDatDetour = nullptr;
FunctionDetour* bufSubDatDetour = nullptr;
FunctionDetour* bindTexDetour = nullptr;
FunctionDetour* drawArraysDetour = nullptr;
FunctionDetour* drawElementsDetour = nullptr;
FunctionDetour* bindFramebufferDetour = nullptr;
FunctionDetour* framebufferTexDetour = nullptr;
FunctionDetour* drawBuffersDetour = nullptr;
FunctionDetour* activeTextureDetour = nullptr;
FunctionDetour* vertexAttribPointerDetour = nullptr;
FunctionDetour* bindBufferDetour = nullptr;
FunctionDetour* createContextDetour = nullptr;
FunctionDetour* makeCurrentDetour = nullptr;

FunctionDetour* createFileDetour = nullptr;
FunctionDetour* readFileDetour = nullptr;
FunctionDetour* readFileExDetour = nullptr;


FunctionDetour* createWindowDetour = nullptr;

WINGDIAPI PROC (WINAPI *getProc)(LPCSTR) = nullptr;

GLenum APIENTRY (*getError)() = nullptr;
void APIENTRY (*getIntegerv)(GLenum, GLint*) = nullptr;

#include <map>
#include <vector>

namespace GraphicsMonitor
{
    struct Texture
    {
        int width;
        int height;
    };

    size_t swapCount = 0;
    std::map<GLuint, Texture> textures;
//    std::map<GLenum, GLuint> targets;

    GLuint boundTexture = 0;
    GLuint boundFramebuffer = 0;
    GLuint boundBuffer = 0;

    struct TextureUnit
    {
        std::map<GLenum, GLuint> targets;
        GLenum id;
    };

    class TextureUnitHolder
    {
        public:
            explicit TextureUnitHolder();

            void setTexture(GLenum target, GLuint texture);
            GLuint getTexture(GLenum target) const;
            void setActiveTextureUnit(GLenum unit);

            // DEBUG
            const std::vector<TextureUnit>& getTextureUnits() const;

        private:
            std::vector<TextureUnit> mTextureUnits;
            TextureUnit* mActiveTextureUnit;
    };

    TextureUnitHolder::TextureUnitHolder()
    {
        TextureUnit unit0;
        unit0.id = GL_TEXTURE0;
        mTextureUnits.push_back(unit0);
        mActiveTextureUnit = &mTextureUnits[0];
    }

    void TextureUnitHolder::setTexture(GLenum target, GLuint texture)
    {
        mActiveTextureUnit->targets[target] = texture;
    }

    void TextureUnitHolder::setActiveTextureUnit(GLenum unit)
    {
        if(unit < GL_TEXTURE0)
            throw std::runtime_error("Invalid argument in TextureUnitHolder::setActiveTextureUnit");

        size_t index = unit - GL_TEXTURE0;
        if(index >= mTextureUnits.size())
        {
            size_t oldSize = mTextureUnits.size();
            mTextureUnits.resize(index + 1);
            for(size_t i = oldSize; i < mTextureUnits.size(); i++)
                mTextureUnits[i].id = i + GL_TEXTURE0;
        }
        mActiveTextureUnit = &mTextureUnits[index];
    }

    GLuint TextureUnitHolder::getTexture(GLenum target) const
    {
        return mActiveTextureUnit->targets[target];
    }

    const std::vector<TextureUnit>& TextureUnitHolder::getTextureUnits() const
    {
        return mTextureUnits;
    }



    std::map<GLuint, GLuint> framebufferTextureAttachment;
    TextureUnitHolder textureUnits;
    GLuint sceneBuffer = 0;

//    std::map<GLuint, SharedMemoryProtocol::VertexBuffer> buffers;
//    std::list<GLuint> updatedBuffers;

    std::vector<SharedMemoryProtocol::PixelData> pixelData;
    std::vector<unsigned char> pixelBuffer;

    std::vector<SharedMemoryProtocol::VertexBufferWrite> bufferWrites;
    std::vector<char> vertexBuffer;
    std::vector<SharedMemoryProtocol::VertexBufferInfo> bufferInfos;
    std::vector<SharedMemoryProtocol::DrawCall> drawCalls;
    std::vector<SharedMemoryProtocol::TextureData> textureData;

    std::vector<SharedMemoryProtocol::Frame> frames;
    SharedMemoryProtocol::Frame currentFrame;


    std::map<GLuint, SharedMemoryProtocol::VertexBufferWrite> vertexBuffers;

    GLuint tileSheetTextureId = 0;

    enum TextureBuffers : unsigned char
    {
        SHADED,
        UNSHADED,
        SHADING_MAP,
        NUM_BUFFERS
    };
    std::vector<GLuint> textureBuffers;

    bool areExtendedOpenGlFunctionsHooked = false;

    SharedMemoryProtocol::SharedMemorySegment* shm = nullptr;

    float peakDrawCalls = 0.f;
    float peakPixelData = 0.f;
    float peakBufferInfo = 0.f;
    float peakBufferWrite = 0.f;
    float peakBuffer = 0.f;
    float peakPixel = 0.f;
}

void APIENTRY drawArrays(GLenum mode, GLint first, GLsizei count)
{
    using namespace GraphicsMonitor;

    if(boundFramebuffer != 0 && framebufferTextureAttachment.find(boundFramebuffer) != framebufferTextureAttachment.end())
    {
        if(textureBuffers.size() == 3 && framebufferTextureAttachment[boundFramebuffer] == textureBuffers[TextureBuffers::UNSHADED])
        {
//            std::cout << "drawArrays " << boundTexture << "->" << framebufferTextureAttachment[boundFramebuffer] << std::endl;
//            std::cout << "framebuf: " << boundFramebuffer << std::endl;
        }
    }

    drawArraysDetour->disable();
    ((void APIENTRY (*)(GLenum, GLint, GLsizei))drawArraysDetour->getFunction())(mode, first, count);
    drawArraysDetour->enable();
}

void WINAPI bindBuffer(GLenum target, GLuint buffer)
{
//    std::cout << "bindbuffer: " << buffer << "->" << target << std::endl;

    GraphicsMonitor::boundBuffer = buffer;

    bindBufferDetour->disable();
    ((void WINAPI (*)(GLenum, GLuint))bindBufferDetour->getFunction())(target, buffer);
    bindBufferDetour->enable();
}

bool isFloat = true;

void WINAPI vertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer)
{


    if(index < 2)
    {
        if(type != GL_FLOAT)
            isFloat = false;
    }
    else
    {
        using namespace GraphicsMonitor;
        SharedMemoryProtocol::VertexBufferInfo info;
        info.bufferId = boundBuffer;

        unsigned int vertexSize;
        if(isFloat)
        {
            info.vertexType = SharedMemoryProtocol::VertexBufferInfo::VertexType::TEXTURED;
            vertexSize = sizeof(GraphicsLayer::TexturedVertex);
        }
        else
        {
            info.vertexType = SharedMemoryProtocol::VertexBufferInfo::VertexType::COLORED;
            vertexSize = sizeof(GraphicsLayer::ColoredVertex);
        }

        info.unknownOffset = (int)pointer;
        info.numVertices = info.unknownOffset / vertexSize;
        info.numUnknown = info.numVertices;

        unsigned int numBytes = vertexBuffers[boundBuffer].numBytes;
        info.indicesOffset = info.unknownOffset + info.numUnknown * sizeof(SharedMemoryProtocol::VertexBufferInfo::Unknown);
        unsigned int remainingBytes = numBytes - (int)info.indicesOffset;

        info.numIndices = remainingBytes / sizeof(SharedMemoryProtocol::VertexBufferInfo::Index);




        bufferInfos.push_back(info);
        currentFrame.numVertexBufferInfos++;

//        SharedMemoryProtocol::VertexBuffer& bufferData = buffers[boundBuffer];
//
//        bufferData.vertices = (Vertex*)bufferData.data;
//        int offset = (int)pointer;
//        bufferData.numVertices = offset / sizeof(Vertex);
//
//        bufferData.unknown = (float*)((char*)bufferData.data + offset);
//        bufferData.numUnknown = bufferData.numVertices;
//
//        bufferData.indices = (unsigned short*)((char*)bufferData.unknown + sizeof(float) * bufferData.numUnknown);
//        bufferData.numIndices = (bufferData.numBytes - ((int)bufferData.indices - (int)bufferData.data)) / sizeof(unsigned short);
        isFloat = true;
    }

//    std::cout << "vertexAttribPointer: " << index << " " << size << " " << type << " " << (normalized ? "true" : "false") << " " << stride << " " << (int)pointer <<  std::endl;



    vertexAttribPointerDetour->disable();
    ((void WINAPI (*)(GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*))
     vertexAttribPointerDetour->getFunction())(index, size, type, normalized, stride, pointer);
    vertexAttribPointerDetour->enable();
}

void APIENTRY drawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
    using namespace GraphicsMonitor;

//    std::cout << "drawElements " << mode << " " << count << " " << type << " " << (int)indices << " (buf: " << boundBuffer << std::endl;


    SharedMemoryProtocol::DrawCall drawCall;
    drawCall.bufferId = boundBuffer;
    drawCall.sourceTextureId = textureUnits.getTexture(GL_TEXTURE_2D);
    if(boundFramebuffer != 0)
    {
        auto it = framebufferTextureAttachment.find(boundFramebuffer);
        assert(it != framebufferTextureAttachment.end());

        drawCall.targetTextureId = it->second;
    }
    switch(mode)
    {
        case GL_TRIANGLES:
            drawCall.type = SharedMemoryProtocol::DrawCall::PrimitiveType::TRIANGLE;
            break;

        case GL_TRIANGLE_STRIP:
            drawCall.type = SharedMemoryProtocol::DrawCall::PrimitiveType::TRIANGLE_STRIP;
            break;

        default:
            std::stringstream sstream;
            sstream << "Unsupported primitive type: " << mode << ".";
            std::cout << sstream.str();
            throw std::runtime_error(sstream.str());
            break;
    }
    drawCalls.push_back(drawCall);
    currentFrame.numDrawCalls++;

    if(boundFramebuffer != 0 && framebufferTextureAttachment.find(boundFramebuffer) != framebufferTextureAttachment.end())
    {
        if(textureBuffers.size() == 3 && framebufferTextureAttachment[boundFramebuffer] == textureBuffers[TextureBuffers::UNSHADED])
        {


//
//            std::cout << "drawElements " << boundTexture << "->" << framebufferTextureAttachment[boundFramebuffer] << std::endl;
//            std::cout << "buffer: " << boundBuffer << std::endl;
//            std::cout << "framebuf: " << boundFramebuffer << std::endl;
//            std::cout   << "\tmode: " << mode << std::endl
//                        << "\tcount: " << count << std::endl
//                        << "\ttype: " << type << std::endl;
//
//            std::cout << "Texture units: " << std::endl;
//            sceneBuffer = boundBuffer;
//            for(const TextureUnit& unit : textureUnits.getTextureUnits())
//            {
//                std::cout << unit.id << std::endl;
//                for(auto pair2 : unit.targets)
//                {
//                    GLenum target = pair2.first;
//                    GLuint texture = pair2.second;
//
//                    std::cout   << "\ttarget: " << target << std::endl
//                                << "\ttexture: " << texture << std::endl;
//                }
//                std::cout << "------------" << std::endl;
//            }
//            Sleep(1000);
        }
    }

    drawElementsDetour->disable();
    ((void APIENTRY (*)(GLenum, GLsizei, GLenum, const GLvoid*))drawElementsDetour->getFunction())(mode, count, type, indices);
    drawElementsDetour->enable();
}

void WINAPI compTexImg
(
    GLenum target,
    GLint level,
    GLenum internalformat,
    GLsizei width,
    GLsizei height,
    GLint border,
    GLsizei imageSize,
    const GLvoid* data
)
{
//    std::cout << "comptexImg " << width << "x" << height << std::endl;

    compTexImgDetour->disable();
    ((void WINAPI (*)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid*))
        compTexImgDetour->getFunction())(target, level, internalformat, width, height, border, imageSize, data);
    compTexImgDetour->enable();
}

void WINAPI subCompTexImg
(
    GLenum target,
  	GLint level,
  	GLint xoffset,
  	GLint yoffset,
  	GLsizei width,
  	GLsizei height,
  	GLenum format,
  	GLsizei imageSize,
  	const GLvoid* data
)
{
//    std::cout << "subcomptexImg " << width << "x" << height << " - pos: " << xoffset << "x" << yoffset << std::endl;

    subCompTexImgDetour->disable();
    ((void WINAPI (*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid*))
        subCompTexImgDetour->getFunction())(target, level, xoffset, yoffset, width, height, format, imageSize, data);
    subCompTexImgDetour->enable();
}

void WINAPI bufSubDat
(
    GLenum target,
  	GLint offset,
  	GLsizei size,
  	const GLvoid* data
)
{
//    std::cout << "glBufferSubData: " << size << " bytes" << std::endl;

    bufSubDatDetour->disable();
    ((void WINAPI (*)(GLenum, GLint, GLsizei, const GLvoid*))
        bufSubDatDetour->getFunction())(target, offset, size, data);
    bufSubDatDetour->enable();
}

void WINAPI bindTex(GLenum target, GLuint texture)
{
    using namespace GraphicsMonitor;

    textureUnits.setTexture(target, texture);

    bindTexDetour->disable();
    ((void WINAPI (*)(GLenum, GLuint))
        bindTexDetour->getFunction())(target, texture);
    bindTexDetour->enable();
}

void WINAPI bufDat
(
    GLenum target,
    GLsizei size,
    const GLvoid* data,
    GLenum usage
)
{
    using namespace GraphicsMonitor;

    GLuint buf = boundBuffer;
    std::cout << "glBufferData: " << target << " " << size << " " << usage << " -> " << buf << std::endl;

    SharedMemoryProtocol::VertexBufferWrite& buffer = vertexBuffers[buf];
    buffer.bufferId = buf;
    buffer.numBytes = size;

    bufferWrites.push_back(buffer);
    currentFrame.numVertexBufferWrites++;

    vertexBuffer.insert(vertexBuffer.end(), (char*)data, (char*)data + size);
    currentFrame.numVertexBytes += size;

//    bufferData.data = malloc(size);
//    bufferData.numBytes = size;
//    memcpy(bufferData.data, data, size);
//
//    updatedBuffers.push_back(buf);



    bufDatDetour->disable();
    ((void WINAPI (*)(GLenum, GLsizei, const GLvoid*, GLenum))
        bufDatDetour->getFunction())(target, size, data, usage);
    bufDatDetour->enable();
}

void insertTextureData(unsigned short width, unsigned short height, unsigned char id)
{
    if(id == 0)
        return;

    using namespace GraphicsLayer::SharedMemoryProtocol;
    using namespace GraphicsMonitor;
    TextureData data;
    data.width = width;
    data.height = height;
    data.id = id;
    textureData.push_back(data);
    currentFrame.numTextureData++;
}

void insertPixelData
(
    unsigned char* data,
    unsigned char targetTextureId,
    unsigned short x,
    unsigned short y,
    unsigned short width,
    unsigned short height,
    GLenum format,
    GLenum type
)
{
    if(data == nullptr)
        return;

    using namespace GraphicsMonitor;
    using namespace SharedMemoryProtocol;

    assert(format == GL_BGRA || format == GL_RGBA || format == GL_ALPHA);
    assert(type == GL_UNSIGNED_BYTE);

    PixelData p;
    p.height = height;
    p.width = width;
    p.texX = x;
    p.texY = y;
    p.targetTextureId = targetTextureId;

    unsigned int bytesPerPixel = 4;
    typedef PixelData::PixelFormat PixelFormat;
    switch(format)
    {
        case GL_RGBA:
            p.format = PixelFormat::RGBA;
            bytesPerPixel = 4;
            break;

        case GL_BGRA:
            p.format = PixelFormat::BGRA;
            bytesPerPixel = 4;
            break;

        case GL_ALPHA:
            p.format = PixelFormat::ALPHA;
            bytesPerPixel = 1;
            break;

        default:
            std::stringstream sstream;
            sstream << "Unsupported pixel format: " << format << ".";
            std::cout << sstream.str();
            throw std::runtime_error(sstream.str());
    }

    pixelData.push_back(p);
    currentFrame.numPixelData++;

    size_t size = width * height * bytesPerPixel;
    pixelBuffer.insert
    (
        pixelBuffer.end(),
        data,
        data + size
    );
    currentFrame.numPixelBytes += size;
}

void WINAPI texImg
(
    GLenum target,
  	GLint level,
  	GLint internalFormat,
  	GLsizei width,
  	GLsizei height,
  	GLint border,
  	GLenum format,
  	GLenum type,
  	const GLvoid* data
)
{
    GLuint textureId = GraphicsMonitor::textureUnits.getTexture(target);
    std::cout << "texImg " << width << "x" << height << std::endl;
    std::cout << "\ttarget: " << target << std::endl;
    if(target == GL_PROXY_TEXTURE_2D)
        std::cout << "\t(GL_PROXY_TEXTURE_2D)" << std::endl;
    if(data == nullptr)
        std::cout << "\tNULL DATA" << std::endl;
    std::cout << "\tlevel: " << level << std::endl;
    std::cout << "\tid:" << textureId << std::endl;
    std::cout << "\tintform: " << internalFormat << std::endl;
    std::cout << "\tformat: " << format << std::endl;
    std::cout << "\ttype: " << type << std::endl;
    std::cout << "\tborder: " << border << std::endl;

    if(target == GL_TEXTURE_2D)
    {
        GraphicsMonitor::textures[textureId] = {width, height};


        if(width == 480 && height == 352)
        {
            GraphicsMonitor::textureBuffers.push_back(textureId);
            assert(GraphicsMonitor::textureBuffers.size() <= GraphicsMonitor::TextureBuffers::NUM_BUFFERS);
        }

        if(width == 2048 && height == 2048)
        {
            GraphicsMonitor::tileSheetTextureId = textureId;
        }
    }
    insertTextureData(width, height, textureId);
    insertPixelData((unsigned char*)data, textureId, 0, 0, width, height, format, type);


    texImgDetour->disable();
    ((void WINAPI (*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*))
        texImgDetour->getFunction())(target, level, internalFormat, width, height, border, format, type, data);
    texImgDetour->enable();
}


size_t subTexCount = 0;
void WINAPI subTexImg
(
    GLenum target,
  	GLint level,
  	GLint xoffset,
  	GLint yoffset,
  	GLsizei width,
  	GLsizei height,
  	GLenum format,
  	GLenum type,
  	const GLvoid* pixels
)
{
    using namespace GraphicsMonitor;
        std::cout << "subteximg " << width << "x" << height << " - pos: " << xoffset << "x" << yoffset << " -> " << textureUnits.getTexture(target) << std::endl;

    if(width == 32 && height == 32)
    {
//        std::cout << "subteximg " << width << "x" << height << " - pos: " << xoffset << "x" << yoffset << " -> " << textureUnits.getTexture(target) << std::endl;
    }

//    if(textureUnits.getTexture(target) == tileSheetTextureId && (width != 1 && height != 1))
    {
        std::cout << "subtex: " << format << " " << type << std::endl;
        unsigned char* rgb = rgbaToRgb((const unsigned char*)pixels, width, height);
        if(format == GL_BGRA)
        {
            for(size_t i = 2; i < width * height * 3; i += 3)
                std::swap(rgb[i - 2], rgb[i]);
        }
        std::stringstream sstream;
        sstream << "tiles/" << subTexCount++ << "-" << xoffset << "x" << yoffset << "_" << width << "x" << height << "-" << textureUnits.getTexture(target) << ".ppm";
//        writePixelsToFile(sstream.str(), rgb, width, height);
        delete[] rgb;
    }

//    SharedMemoryProtocol::PixelData p;
//    p.height = height;
//    p.width = width;
//    p.texX = 0;
//    p.texY = 0;
//    p.targetTextureId = textureId;
//
//    pixelData.push_back(p);
//    GraphicsMonitor::currentFrame.numPixelData++;

    insertPixelData
    (
        (unsigned char*)pixels,
        textureUnits.getTexture(target),
        xoffset,
        yoffset,
        width,
        height,
        format,
        type
    );


    subTexImgDetour->disable();
    ((void WINAPI (*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))
        subTexImgDetour->getFunction())(target, level, xoffset, yoffset, width, height, format, type, pixels);
    subTexImgDetour->enable();
}


void writeBoundTextureToFile(std::string dir)
{
    GLint id = GraphicsMonitor::textureUnits.getTexture(GL_TEXTURE_2D);

    GraphicsMonitor::Texture texture = GraphicsMonitor::textures[id];

    unsigned char* pixels = new unsigned char[texture.width * texture.height * 3];

    std::stringstream sstream;
    sstream << dir << "/" << id << ".ppm";

    writePixelsToFile(sstream.str(), pixels, texture.width, texture.height);

    delete[] pixels;

}

void WINAPI drawBuffers(GLsizei n, const GLenum* bufs)
{
    drawBuffersDetour->disable();
    ((void WINAPI (*)(GLsizei, const GLenum*))drawBuffersDetour->getFunction())(n, bufs);
    drawBuffersDetour->enable();
}



void WINAPI bindFramebuffer(GLenum target, GLuint framebuffer)
{
//    std::cout << "bindFramebuffer: " << framebuffer << "->" << target << std::endl;

    GraphicsMonitor::boundFramebuffer = framebuffer;

    bindFramebufferDetour->disable();
    ((void WINAPI (*)(GLenum, GLuint))bindFramebufferDetour->getFunction())(target, framebuffer);
    bindFramebufferDetour->enable();
}

void WINAPI activeTexture(GLenum texture)
{
    GraphicsMonitor::textureUnits.setActiveTextureUnit(texture);

    activeTextureDetour->disable();
    ((void WINAPI (*)(GLenum))activeTextureDetour->getFunction())(texture);
    activeTextureDetour->enable();
}


void WINAPI framebufferTex(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    GraphicsMonitor::framebufferTextureAttachment[GraphicsMonitor::boundFramebuffer] = texture;

    framebufferTexDetour->disable();
    ((void WINAPI (*)(GLenum, GLenum, GLenum, GLuint, GLint))framebufferTexDetour->getFunction())(target, attachment, textarget, texture, level);
    framebufferTexDetour->enable();
}

void updateSharedMemory()
{
    using namespace GraphicsMonitor;
    using namespace SharedMemoryProtocol;

    if(!shm->hasPendingChanges)
    {
        size_t vertexBufferSize = vertexBuffer.size();
        size_t bufferWritesSize = bufferWrites.size();
        size_t bufferInfosSize = bufferInfos.size();
        size_t drawCallsSize = drawCalls.size();
        size_t pixelBufferSize = pixelBuffer.size();
        size_t pixelDataSize = pixelData.size();
        size_t textureDataSize = textureData.size();

        size_t numFramesFittingInSharedMemory = frames.size();
        while
        (
            vertexBufferSize > VERTEX_BUFFER_SIZE ||
            bufferWritesSize > MAX_NUM_VERTEX_BUFFERS ||
            bufferInfosSize > MAX_NUM_VERTEX_BUFFER_INFOS ||
            drawCallsSize > MAX_NUM_DRAW_CALL ||
            pixelBufferSize > PIXEL_BUFFER_SIZE ||
            pixelDataSize > MAX_NUM_PIXEL_DATA ||
            textureDataSize > MAX_NUM_TEXTURE_DATA
        )
        {
            assert(numFramesFittingInSharedMemory > 0);

            const Frame& f = frames[numFramesFittingInSharedMemory - 1];
            bufferWritesSize -= f.numVertexBufferWrites;
            bufferInfosSize -= f.numVertexBufferInfos;
            pixelDataSize -= f.numPixelData;
            drawCallsSize -= f.numDrawCalls;
            vertexBufferSize -= f.numVertexBytes;
            pixelBufferSize -= f.numPixelBytes;
            textureDataSize -= f.numTextureData;

            numFramesFittingInSharedMemory--;
        }

        memcpy(shm->frames, frames.data(), numFramesFittingInSharedMemory * sizeof(Frame));
        shm->numFrames = numFramesFittingInSharedMemory;

        memcpy(shm->vertexBuffer, vertexBuffer.data(), vertexBufferSize);
        memcpy(shm->vertexBufferWrites, bufferWrites.data(), bufferWritesSize * sizeof(VertexBufferWrite));
        memcpy(shm->vertexBufferInfos, bufferInfos.data(), bufferInfosSize * sizeof(VertexBufferInfo));
        memcpy(shm->drawCalls, drawCalls.data(), drawCallsSize * sizeof(DrawCall));
        memcpy(shm->pixelBuffer, pixelBuffer.data(), pixelBufferSize);
        memcpy(shm->pixelData, pixelData.data(), pixelDataSize * sizeof(PixelData));
        memcpy(shm->textureData, textureData.data(), textureDataSize * sizeof(TextureData));

        shm->hasPendingChanges = true;

        if(numFramesFittingInSharedMemory == frames.size())
        {
            frames.clear();
            vertexBuffer.clear();
            bufferWrites.clear();
            bufferInfos.clear();
            drawCalls.clear();
            pixelBuffer.clear();
            pixelData.clear();
            textureData.clear();
        }
        else
        {
            frames.erase(frames.begin(), frames.begin() + numFramesFittingInSharedMemory);
            vertexBuffer.erase(vertexBuffer.begin(), vertexBuffer.begin() + vertexBufferSize);
            bufferWrites.erase(bufferWrites.begin(), bufferWrites.begin() + bufferWritesSize);
            bufferInfos.erase(bufferInfos.begin(), bufferInfos.begin() + bufferInfosSize);
            drawCalls.erase(drawCalls.begin(), drawCalls.begin() + drawCallsSize);
            pixelBuffer.erase(pixelBuffer.begin(), pixelBuffer.begin() + pixelBufferSize);
            pixelData.erase(pixelData.begin(), pixelData.begin() + pixelDataSize);
            textureData.erase(textureData.begin(), textureData.begin() + textureDataSize);
        }
    }
}

void printSharedMemoryBufferDiagnostics()
{
    using namespace GraphicsMonitor;
    using namespace SharedMemoryProtocol;

    Frame f = currentFrame;

    // Percentages
    float drawCallPerc = ((float)f.numDrawCalls) / ((float)MAX_NUM_DRAW_CALL) * 100.f;
    float pixelDataPerc = ((float)f.numPixelData) / ((float)MAX_NUM_PIXEL_DATA) * 100.f;
    float bufferInfoPerc = ((float)f.numVertexBufferInfos) / ((float)MAX_NUM_VERTEX_BUFFER_INFOS) * 100.f;
    float bufferWritePerc = ((float)f.numVertexBufferWrites) / ((float)MAX_NUM_VERTEX_BUFFERS) * 100.f;
    float bufferPerc = ((float)vertexBuffer.size()) / ((float)VERTEX_BUFFER_SIZE) * 100.f;
    float pixelPerc = ((float)pixelBuffer.size()) / ((float)PIXEL_BUFFER_SIZE) * 100.f;

    peakDrawCalls = std::max(peakDrawCalls, drawCallPerc);
    peakPixelData = std::max(peakPixelData, pixelDataPerc);
    peakBufferInfo = std::max(peakBufferInfo, bufferInfoPerc);
    peakBufferWrite = std::max(peakBufferWrite, bufferWritePerc);
    peakBuffer = std::max(peakBuffer, bufferPerc);
    peakPixel = std::max(peakPixel, pixelPerc);

    std::cout   << "numDrawCalls: " << f.numDrawCalls << " / " << MAX_NUM_DRAW_CALL
                << "  (" << drawCallPerc << "%)"
                << " peak: " << peakDrawCalls << "%" <<  std::endl

                << "numPixelData: " << f.numPixelData << " / " << MAX_NUM_PIXEL_DATA
                << "  (" << pixelDataPerc << "%)"
                << " peak: " << peakPixelData << "%" << std::endl

                << "numVertexBufferInfos: " << f.numVertexBufferInfos << " / " << MAX_NUM_VERTEX_BUFFER_INFOS
                << "  (" << bufferInfoPerc << "%)"
                << " peak: " << peakBufferInfo << "%" << std::endl

                << "numVertexBufferWrites: " << f.numVertexBufferWrites << " / " << MAX_NUM_VERTEX_BUFFERS
                << "  (" << bufferWritePerc << "%)"
                << " peak: " << peakBufferWrite << "%" << std::endl

                << "vertexBuffer: " << vertexBuffer.size() << " / " << VERTEX_BUFFER_SIZE
                << "  (" << bufferPerc << "%)"
                << " peak: " << peakBuffer << "%" << std::endl

                << "pixelBuffer: " << pixelBuffer.size() << " / " << PIXEL_BUFFER_SIZE
                << "  (" << pixelPerc << "%)"
                << " peak: " << peakPixel << "%" << std::endl;
}


WINGDIAPI BOOL WINAPI swapBuf(HDC hdc)
{
    using namespace GraphicsMonitor;
    using namespace SharedMemoryProtocol;

    swapCount++;
    std::cout << "swapbuf: " << swapCount << std::endl;

    printSharedMemoryBufferDiagnostics();



    assert(currentFrame.numDrawCalls <= MAX_NUM_DRAW_CALL);
    assert(currentFrame.numPixelData <= MAX_NUM_PIXEL_DATA);
    assert(currentFrame.numVertexBufferInfos <= MAX_NUM_VERTEX_BUFFER_INFOS);
    assert(currentFrame.numVertexBufferWrites <= MAX_NUM_VERTEX_BUFFERS);
    assert(currentFrame.numPixelBytes <= PIXEL_BUFFER_SIZE);
    assert(currentFrame.numVertexBytes <= VERTEX_BUFFER_SIZE);

    frames.push_back(currentFrame);

    currentFrame.numDrawCalls = 0;
    currentFrame.numPixelData = 0;
    currentFrame.numVertexBufferInfos = 0;
    currentFrame.numVertexBufferWrites = 0;
    currentFrame.numPixelBytes = 0;
    currentFrame.numVertexBytes = 0;

    updateSharedMemory();


    GLuint textureBackup = textureUnits.getTexture(GL_TEXTURE_2D);

    for(auto pair : textures)
    {
        if(pair.first != 2)
        {
            bindTex(GL_TEXTURE_2D, pair.first);
            writeBoundTextureToFile("tileBuffers");
        }
    }

    bindTex(GL_TEXTURE_2D, textureBackup);



    swapBufDetour->disable();
    BOOL retVal = ((WINGDIAPI BOOL WINAPI (*)(HDC))swapBufDetour->getFunction())(hdc);
    swapBufDetour->enable();

    return retVal;
}

void WINAPI delTex(GLsizei n, const GLuint* textures)
{
//    std::cout << "deltex" << std::endl;

    for(size_t i = 0; i < n; i++)
    {
        GraphicsMonitor::textures.erase(textures[i]);
    }

    delTexDetour->disable();
    ((void WINAPI (*)(GLsizei, const GLuint*))delTexDetour->getFunction())(n, textures);
    delTexDetour->enable();
}

void WINAPI genTexFunc(GLsizei n, GLuint* textures)
{
    genTexDetour->disable();
    ((void WINAPI (*)(GLsizei, GLuint*))genTexDetour->getFunction())(n, textures);
    genTexDetour->enable();
}


WINUSERAPI HWND WINAPI createWindow
(
    DWORD dwExStyle,
    LPCWSTR className,
    LPCWSTR windowName,
    DWORD dwStyle,
    int x,
    int y,
    int width,
    int height,
    HWND parent,
    HMENU menu,
    HINSTANCE instance,
    LPVOID param
)
{

    createWindowDetour->disable();
    HWND retVal = ((WINUSERAPI HWND (WINAPI *)(DWORD,LPCWSTR,LPCWSTR,DWORD,int,int,int,int,HWND,HMENU,HINSTANCE,LPVOID))
                   createWindowDetour->getFunction())
                   (dwExStyle, className, windowName, dwStyle, x, y, width, height, parent, menu, instance, param);
    createWindowDetour->enable();


    char name[256];
    wcstombs(name, windowName, 256);
    if(strcmp(name, "Tibia Beta") == 0)
    {
    }


    return retVal;
}

WINGDIAPI BOOL WINAPI makeCurrent(HDC hdc, HGLRC hglrc)
{
    makeCurrentDetour->disable();
    BOOL retVal = ((WINGDIAPI BOOL (WINAPI *)(HDC, HGLRC))makeCurrentDetour->getFunction())(hdc, hglrc);
    makeCurrentDetour->enable();

    using namespace GraphicsMonitor;
    if(!areExtendedOpenGlFunctionsHooked && hdc && hglrc)
    {
        compTexImgDetour = new FunctionDetour((void*)getProc("glCompressedTexImage2D"), (void*)compTexImg);
        bindFramebufferDetour = new FunctionDetour((void*)getProc("glBindFramebuffer"), (void*)bindFramebuffer);
        subCompTexImgDetour = new FunctionDetour((void*)getProc("glCompressedTexSubImage2D"), (void*)subCompTexImg);
        bufDatDetour = new FunctionDetour((void*)getProc("glBufferData"), (void*)bufDat);
        bufSubDatDetour = new FunctionDetour((void*)getProc("glBufferSubData"), (void*)bufSubDat);
        framebufferTexDetour = new FunctionDetour((void*)getProc("glFramebufferTexture2D"), (void*)framebufferTex);
        drawBuffersDetour = new FunctionDetour((void*)getProc("glDrawBuffers"), (void*)drawBuffers);
        activeTextureDetour = new FunctionDetour((void*)getProc("glActiveTexture"), (void*)activeTexture);
        vertexAttribPointerDetour = new FunctionDetour((void*)getProc("glVertexAttribPointer"), (void*)vertexAttribPointer);
        bindBufferDetour = new FunctionDetour((void*)getProc("glBindBuffer"), (void*)bindBuffer);

        areExtendedOpenGlFunctionsHooked = true;
    }

    return retVal;
}
WINGDIAPI HGLRC WINAPI createContext(HDC hdc)
{
    createContextDetour->disable();
    HGLRC retVal = ((WINGDIAPI HGLRC (WINAPI *)(HDC))createContextDetour->getFunction())(hdc);
    createContextDetour->enable();

    return retVal;
}

WINBASEAPI HANDLE WINAPI createFile
(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
)
{

    std::cout << "create file: ";
    char name[512];
    wcstombs(name, (LPCWSTR)lpFileName, 512);
    std::cout << name << std::endl;
//    for(LPCSTR c = lpFileName; *c != '\0'; c += 2)
//    {
//        std::cout << *c;
//    }
//    std::cout << std::endl;
//    Sleep(100);

    createFileDetour->disable();
    HANDLE retVal = ((WINBASEAPI HANDLE (WINAPI *)(LPCSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE))createFileDetour->getFunction())
    (lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
    createFileDetour->enable();

    return retVal;
}

WINBASEAPI BOOL WINAPI readFile(HANDLE hFile, PVOID lpBuffer, DWORD nNumberOfBytesToRead, PDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
    std::cout << "read file: "  << std::endl;
    Sleep(100);
    readFileDetour->disable();
    BOOL retVal = ((WINBASEAPI BOOL (WINAPI *)(HANDLE,PVOID,DWORD,PDWORD,LPOVERLAPPED))readFileDetour->getFunction())
        (hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
    readFileDetour->enable();

    return retVal;
}

WINBASEAPI BOOL WINAPI readFileEx(HANDLE hFile, PVOID lpBuffer, DWORD nNumberOfBytesToRead, LPOVERLAPPED lpOverlapped, LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    std::cout << "read file ex" << std::endl;
    Sleep(100);

    readFileExDetour->disable();
    BOOL retVal = ((WINBASEAPI BOOL (WINAPI *)(HANDLE,PVOID,DWORD,LPOVERLAPPED,LPOVERLAPPED_COMPLETION_ROUTINE))readFileExDetour->getFunction())
        (hFile, lpBuffer, nNumberOfBytesToRead, lpOverlapped, lpCompletionRoutine);
    readFileExDetour->enable();

    return retVal;
}


WINBASEAPI HINSTANCE WINAPI detourFunc(LPCSTR moduleName)
{
    loadLibDetour->disable();
    HINSTANCE retVal = ((WINBASEAPI HINSTANCE WINAPI (*)(LPCSTR))loadLibDetour->getFunction())(moduleName);
    loadLibDetour->enable();

    std::cout << "module: " << moduleName << std::endl;

    if(strcmp(moduleName, "OPENGL32.DLL") == 0 || strcmp(moduleName, "opengl32.dll") == 0)
    {
        if(genTexDetour)
        {
            delete genTexDetour;
            delete swapBufDetour;
            delete delTexDetour;
            delete texImgDetour;
            delete subTexImgDetour;
            delete bindTexDetour;
            delete drawArraysDetour;
            delete drawElementsDetour;
            delete makeCurrentDetour;
            delete createContextDetour;
        }

        getError = (GLenum APIENTRY (*)())GetProcAddress(retVal, "glGetError");
        getIntegerv = (void APIENTRY (*)(GLenum, GLint*))GetProcAddress(retVal, "glGetIntegerv");

        genTexDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glGenTextures"), (void*)genTexFunc);
        swapBufDetour = new FunctionDetour((void*)GetProcAddress(GetModuleHandle("gdi32.dll"), "SwapBuffers"), (void*)swapBuf);
        delTexDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glDeleteTextures"), (void*)delTex);
        texImgDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glTexImage2D"), (void*)texImg);
        subTexImgDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glTexSubImage2D"), (void*)subTexImg);
        bindTexDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glBindTexture"), (void*)bindTex);
        drawArraysDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glDrawArrays"), (void*)drawArrays);
        drawElementsDetour = new FunctionDetour((void*)GetProcAddress(retVal, "glDrawElements"), (void*)drawElements);

        getProc = (WINGDIAPI PROC (WINAPI *)(LPCSTR))GetProcAddress(retVal, "wglGetProcAddress");
        createContextDetour = new FunctionDetour((void*)GetProcAddress(retVal, "wglCreateContext"), (void*)createContext);
        makeCurrentDetour = new FunctionDetour((void*)GetProcAddress(retVal, "wglMakeCurrent"), (void*)makeCurrent);

    }
    else if(strcmp(moduleName, "USER32.DLL") == 0 || strcmp(moduleName, "user32.dll") == 0)
    {
        if(createWindowDetour)
        {
            delete createWindowDetour;
        }

        createWindowDetour = new FunctionDetour((void*)GetProcAddress(retVal, "CreateWindowExW"), (void*)createWindow);
    }
    else if(strcmp(moduleName, "KERNEL32.DLL") == 0 || strcmp(moduleName, "kernel32.dll") == 0)
    {
        if(readFileDetour)
        {
//            delete createFileDetour;
            delete readFileDetour;
            delete readFileExDetour;
        }

//        createFileDetour = new FunctionDetour((void*)GetProcAddress(retVal, "CreateFileW"), (void*)createFile);
        readFileDetour = new FunctionDetour((void*)GetProcAddress(retVal, "ReadFile"), (void*)readFile);
        readFileExDetour = new FunctionDetour((void*)GetProcAddress(retVal, "ReadFileEx"), (void*)readFileEx);
    }

    return retVal;
}


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
    switch(fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            AllocConsole();
            freopen("CONOUT$", "w", stdout);
            std::cout << "Hallo!" << std::endl;
//            createFileDetour = new FunctionDetour((void*)GetProcAddress(GetModuleHandle("kernel32.dll"), "CreateFileW"), (void*)createFile);
            createFileDetour = new FunctionDetour((void*)GetProcAddress(GetModuleHandle("kernelbase.dll"), "CreateFileW"), (void*)createFile);
//            createFileDetour->disable();


            {
                char szName[] = "Local\\MyFileMappingObject";

                HANDLE hMapFile = OpenFileMapping
                (
                    FILE_MAP_ALL_ACCESS,   // read/write access
                    FALSE,                 // do not inherit the name
                    szName      // name of mapping object
                );

                if (hMapFile == NULL)
                {
                    std::stringstream sstream;
                    sstream << "Could not create shared memory object (" << GetLastError() << ").";
                    std::cout << sstream.str();
                    throw std::runtime_error(sstream.str());
                }

                GraphicsMonitor::shm = (SharedMemoryProtocol::SharedMemorySegment*) MapViewOfFile
                (
                    hMapFile, // handle to map object
                    FILE_MAP_ALL_ACCESS,  // read/write permission
                    0,
                    0,
                    SharedMemoryProtocol::NUM_BYTES
                );

                if (GraphicsMonitor::shm == NULL)
                {
                    std::stringstream sstream;
                    sstream << "Could not map shared memory (" << GetLastError() << ").";
                    std::cout << sstream.str();
                    throw std::runtime_error(sstream.str());
                }

//                std::cout << GraphicsMonitor::shm->numDrawCalls << std::endl;

//                UnmapViewOfFile(pBuf);
//
//                CloseHandle(hMapFile);

                void* loadLib = (void*)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

                if(loadLib == NULL)
                    std::cout << "Cannot get loadLib address" << std::endl;

                loadLibDetour = new FunctionDetour(loadLib, (void*)detourFunc);
            }
            break;

        default:
            break;
    }
    return TRUE;
}
