
#define GL_GLEXT_PROTOTYPES
#include "GL/gl.h"
#include "GL/glx.h"

#include <iostream>
#include <map>
#include <cstring>


#include <vector>

#include <unistd.h>

#include <dlfcn.h>
#include <sys/mman.h>

#include "SharedMemoryProtocol.hpp"
#include <list>

namespace GraphicsMonitor
{
    struct Texture
    {
        GLsizei width;
        GLsizei height;
    };

    std::map<GLuint, Texture> textures;

    struct Vector2f
    {
        float x;
        float y;
    };

    std::vector<Vector2f> vertices;
    std::vector<Vector2f> texCoords;

    bool isPerformingPrimitiveDrawSession = false;

    GLuint current2dTexture = 0;
    GLuint currentFramebuffer = 0;
    GLuint currentDrawTexture = 0;

    size_t swapCount = 0;


    Atom tibiaRunningAtom;

    int shmFd = strtol(getenv("SHANKBOT_SHARED_MEMORY_FD"), NULL, 10);

    GraphicsLayer::SharedMemoryProtocol::SharedMemorySegment* shm = nullptr;

    std::vector<GraphicsLayer::SharedMemoryProtocol::PixelData> pixelPackets;
    std::vector<GraphicsLayer::SharedMemoryProtocol::DrawCall> drawCallPackets;
    std::vector<unsigned int> numDrawCallsPerFrame;

    bool updateDrawCalls = false;

    size_t drawsPerFrame = 0;
}

void writeBoundTextureToFile();

///////////////////////////////////

void glXSwapBuffers(Display* dpy, GLXDrawable drawable)
{
    using namespace GraphicsMonitor;

    void (*originalFunc)(Display*, GLXDrawable) = (void (*)(Display*, GLXDrawable))dlsym(RTLD_NEXT, "glXSwapBuffers");

//    std::cout << "Swapping draw buffers..." << swapCount++ << std::endl;
//    std::cout << "DrawsPerFrame: " << drawsPerFrame << std::endl;
    numDrawCallsPerFrame.push_back(drawsPerFrame);
    drawsPerFrame = 0;

    originalFunc(dpy, drawable);
    if(glGetError() != GL_NO_ERROR)
        std::cout << "ERROR: glXSwapBuffers 1"<< std::endl;

    GLuint previousId = current2dTexture;

//    glBindTexture(GL_TEXTURE_2D, 3);
//    writeBoundTextureToFile();
//    glBindTexture(GL_TEXTURE_2D, previousId);

//    for(size_t i = 1; i < 5; i++)
//    {
//        glBindTexture(GL_TEXTURE_2D, i);
//        writeBoundTextureToFile();
//    }
//    glBindTexture(GL_TEXTURE_2D, previousId);


    updateDrawCalls = false;
//    std::cout << "NUM DRAW CALLS WAITING: " << drawCallPackets.size() << std::endl;
    if(!shm->hasPendingChanges)
    {
        memcpy(shm->pixelData, pixelPackets.data(), pixelPackets.size() * sizeof(GraphicsLayer::SharedMemoryProtocol::PixelData));
        shm->numPixelData = pixelPackets.size();

        memcpy(shm->drawCall, drawCallPackets.data(), drawCallPackets.size() * sizeof(GraphicsLayer::SharedMemoryProtocol::DrawCall));
        shm->numDrawCall = drawCallPackets.size();

        memcpy(shm->numDrawCallsPerFrame, numDrawCallsPerFrame.data(), numDrawCallsPerFrame.size() * sizeof(unsigned int));
        shm->numFrames = numDrawCallsPerFrame.size();

        shm->hasPendingChanges = true;
        updateDrawCalls = true;

        pixelPackets.clear();
        drawCallPackets.clear();
        numDrawCallsPerFrame.clear();
    }

    if(glGetError() != GL_NO_ERROR)
        std::cout << "ERROR: glXSwapBuffers 2" << std::endl;
}

///////////////////////////////////

void glBindTexture(GLenum target, GLuint texture)
{
    void (*originalFunc)(GLenum, GLuint) = (void (*)(GLenum, GLuint))dlsym(RTLD_NEXT, "glBindTexture");
    originalFunc(target, texture);

    if(target != GL_TEXTURE_2D)
        std::cout << "TEXTURE NOT BOUND AS GL_TEXTURE_2D" << std::endl;

    GraphicsMonitor::current2dTexture = texture;
    if(glGetError() != GL_NO_ERROR)
        std::cout << "ERROR: glBindTexture" << std::endl;
}

///////////////////////////////////

void glBegin(GLenum mode)
{
    if(GraphicsMonitor::isPerformingPrimitiveDrawSession)
        return;

    void (*originalFunc)(GLenum) = (void (*)(GLenum))dlsym(RTLD_NEXT, "glBegin");

    GraphicsMonitor::vertices.clear();
    GraphicsMonitor::texCoords.clear();

//    if(GraphicsMonitor::currentFramebuffer != 0)
//    {
//    //    if(GraphicsMonitor::currentDrawTexture == 1 && GraphicsMonitor::current2dTexture >= 5 && GraphicsMonitor::current2dTexture <= 19)
//    //    if(GraphicsMonitor::currentDrawTexture == 3 && GraphicsMonitor::current2dTexture >= 5 && GraphicsMonitor::current2dTexture <= 19)
//        if(GraphicsMonitor::currentDrawTexture == 3)
//            return;
//
//        if(GraphicsMonitor::currentDrawTexture == 1 && GraphicsMonitor::current2dTexture < 5)
//            return;
//    }
//    else
//    {
//        if(GraphicsMonitor::current2dTexture >= 5 && GraphicsMonitor::current2dTexture <= 19)
//            return;
//    }



    if(mode == GL_QUADS)
    {

        GraphicsMonitor::isPerformingPrimitiveDrawSession = true;
        originalFunc(mode);
    }
}

///////////////////////////////////

#include <fstream>
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

// COORDINATES IN OPENGL COORDINATES
void writeBoundTextureAreaToFile(std::string fileName, size_t x, size_t y, size_t width, size_t height)
{
    GraphicsMonitor::Texture texture = GraphicsMonitor::textures[GraphicsMonitor::current2dTexture];

    size_t destSize = width * height * 3;
    unsigned char* pixels = new unsigned char[destSize];

    size_t srcSize = texture.width * texture.height * 3;
    unsigned char* textureData = new unsigned char[srcSize];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);

    for(size_t iSrc = (x + y * texture.width) * 3, iDest = 0; iSrc < srcSize && iDest < destSize; iSrc += texture.width * 3, iDest += width * 3)
        memcpy(&pixels[iDest], &textureData[iSrc], width * 3);

    writePixelsToFile(fileName, pixels, width, height);

    delete[] pixels;
    delete[] textureData;
}

#include <sstream>

void writeBoundTextureToFile()
{
    GLint id = GraphicsMonitor::current2dTexture;
    GraphicsMonitor::Texture texture = GraphicsMonitor::textures[id];
    unsigned char* pixels = new unsigned char[texture.width * texture.height * 3];

    std::stringstream sstream;
    sstream << "tileBuffers/" << id;

    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    writePixelsToFile(sstream.str(), pixels, texture.width, texture.height);

    delete[] pixels;
}

void glEnd()
{
    using namespace GraphicsMonitor;
    using namespace GraphicsLayer::SharedMemoryProtocol;

    if(!isPerformingPrimitiveDrawSession)
        return;

    isPerformingPrimitiveDrawSession = false;

    void (*originalFunc)() = (void (*)())dlsym(RTLD_NEXT, "glEnd");
    originalFunc();


    if(glGetError() != GL_NO_ERROR)
        std::cout << "ERROR: glEnd" << std::endl;


    /*
     ********************************************************
     ** INFO ABOUT TIBIA'S USAGE OF TEXTURES
     ********************************************************
     * At the time of writing, Tibia uses 19 textures as buffers before drawing.
     * The roles of these are as follows:
     *
     * 1: This texture represents the final drawing of the game world, including lighting.
     * No HUDs or other UI elements are drawn to this texture.
     *
     * 2: Seems to be only used for storing the main menu texture.
     *
     * 3: This texture contains the drawing of the game world, without lighting and animated
     * objects. It is the step before texture 1.
     *
     * 4: The minimap.
     *
     * 5-19: Sprites. These textures contain the sprites that may currently be drawn.
     *
     ********************************************************
     * Below is some info about how the textures are used.
     *
     * The textures 5-19 are always used as source textures. The target is always
     * another texture.
     *
     * 3 is used as target texture from 5-19. It is then used as source texture to 1.
     *
     * 1 is used as target texture from 3 and any animated objects in textures 5-19.
     *
     * The only textures that are used as source texture to be drawn to the screen are
     * 1 and 4. EXCEPT for UI elements in 5-19.
     *
     ********************************************************
     * What does this mean?
     *
     * If Tibia is rendering to screen (framebuffer == 0), it means the following:
     * 1. Is source texture 1? Then the game world is being drawn (without UI elements like text).
     * 2. Is source texture 4? Then the minimap is being drawn.
     * 3. Is source texture 5-19? Then UI elements are being drawn.
     *
     * If Tibia is rendering to another texture, it means the following:
     * 1. Is target 3? Then non-animated objects are being drawn.
     * 2. Is target 1? Then animated objects are being drawn.
     *
     *
    */


    if(vertices.size() != 4)
    {
//        std::cout << "Drawing non-rect primitive" << std::endl;

    }
    else if(texCoords.size() == 4)
    {
        Vector2f texCoordMax;
        Vector2f texCoordMin;

        if(texCoords[2].x > texCoords[0].x)
        {
            texCoordMax.x = texCoords[2].x;
            texCoordMin.x = texCoords[0].x;
        }
        else
        {
            texCoordMax.x = texCoords[0].x;
            texCoordMin.x = texCoords[2].x;
        }

        if(texCoords[2].y > texCoords[0].y)
        {
            texCoordMax.y = texCoords[2].y;
            texCoordMin.y = texCoords[0].y;
        }
        else
        {
            texCoordMax.y = texCoords[0].y;
            texCoordMin.y = texCoords[2].y;
        }


        Vector2f vertexMin;
        Vector2f vertexMax;

        if(vertices[2].x > vertices[0].x)
        {
            vertexMax.x = vertices[2].x;
            vertexMin.x = vertices[0].x;
        }
        else
        {
            vertexMax.x = vertices[0].x;
            vertexMin.x = vertices[2].x;
        }

        if(vertices[2].y > vertices[0].y)
        {
            vertexMax.y = vertices[2].y;
            vertexMin.y = vertices[0].y;
        }
        else
        {
            vertexMax.y = vertices[0].y;
            vertexMin.y = vertices[2].y;
        }

        Texture texture = textures[current2dTexture];

        if(texCoordMin.x > 0.99f || texCoordMin.y > 0.99f)
        {
//            std::cout << "Drawing mysterious things... " << std::endl;
        }
        else //if(updateDrawCalls)
        {
//            if(currentFramebuffer != 0 && current2dTexture > 3 && (currentDrawTexture == 1/* || currentDrawTexture == 3*/))
            {


            size_t texWidth = (texCoordMax.x - texCoordMin.x) * texture.width;
            size_t texHeight = (texCoordMax.y - texCoordMin.y) * texture.height;
            float widthFloat = texCoordMax.x - texCoordMin.x;
            float heightFloat = texCoordMax.y - texCoordMin.y;

            DrawCall packet;
            packet.sourceTextureId = current2dTexture;
            packet.targetTextureId = (currentFramebuffer == 0) ? 0 : currentDrawTexture;
            packet.texX = texCoordMin.x * texture.width;
            packet.texY = texCoordMin.y * texture.height;
            packet.screenX = vertexMin.x;
            packet.screenY = vertexMin.y;
            packet.width = vertexMax.x - vertexMin.x;
            packet.height = vertexMax.y - vertexMin.y;
            packet.texWidth = texWidth;
            packet.texHeight = texHeight;

            if(drawCallPackets.size() >= MAX_NUM_DRAW_CALL)
            {
                 std::cout << "ShankBot is too slow to handle incoming draw calls. Tibia client will crash" << std::endl;
            }

            drawsPerFrame++;
            drawCallPackets.push_back(packet);



//                std::stringstream sstream;
//                sstream << "monDrawCalls/" << currentDrawTexture << "-" << current2dTexture << "-" << packet.texX << "x" << packet.texY;
//                drawsPerFrame++;
//                writeBoundTextureAreaToFile(sstream.str(), packet.texX, packet.texY, packet.width, packet.height);
            }
//            else if(currentFramebuffer == 0 && current2dTexture > 3)
//            {
//
//                size_t width = (texCoordMax.x - texCoordMin.x) * texture.width;
//                size_t height = (texCoordMax.y - texCoordMin.y) * texture.height;
//                float widthFloat = texCoordMax.x - texCoordMin.x;
//                float heightFloat = texCoordMax.y - texCoordMin.y;
//
//                DrawCall packet;
//                packet.sourceTextureId = current2dTexture;
//                packet.targetTextureId = (currentFramebuffer == 0) ? 0 : currentDrawTexture;
//                packet.texX = texCoordMin.x * texture.width;
//                packet.texY = texCoordMin.y * texture.height;
//                packet.screenX = vertexMin.x;
//                packet.screenY = vertexMin.y;
//                packet.width = width;
//                packet.height = height;
//
////                std::stringstream sstream;
////                sstream << "monDrawCalls/" << currentDrawTexture << "-" << current2dTexture << "-" << packet.texX << "x" << packet.texY;
////                drawsPerFrame++;
////                writeBoundTextureAreaToFile(sstream.str(), packet.texX, packet.texY, packet.width, packet.height);
//
//            }


        }
    }
}

///////////////////////////////////

void glVertex2i(GLint x,  GLint y)
{
    void (*originalFunc)(GLint, GLint) = (void (*)(GLint, GLint))dlsym(RTLD_NEXT, "glVertex2i");
    originalFunc(x, y);

    GraphicsMonitor::vertices.push_back({(float)x, (float)y});
}

///////////////////////////////////

void glTexCoord2d(GLdouble  s,  GLdouble  t)
{
    void (*originalFunc)(GLdouble, GLdouble) = (void (*)(GLdouble, GLdouble))dlsym(RTLD_NEXT, "glTexCoord2d");
    originalFunc(s, t);

    GraphicsMonitor::texCoords.push_back({(float)s, (float)t});
}

///////////////////////////////////

void glTexImage2D
(
    GLenum target,
  	GLint level,
  	GLint internalFormat,
  	GLsizei width,
  	GLsizei height,
  	GLint border,
  	GLenum format,
  	GLenum type,
  	const GLvoid * data
)
{
    GLuint id = GraphicsMonitor::current2dTexture;
    std::cout << "texxing " << id << std::endl;
    GraphicsMonitor::textures[(GLuint)id] = {width, height};

    void (*originalFunc)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*) = (void (*)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*))dlsym(RTLD_NEXT, "glTexImage2D");

    originalFunc(target, level, internalFormat, width, height, border, format, type, data);
}

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

///////////////////////////////////
size_t herpderp = 0;
void glTexSubImage2D( 	GLenum target,
  	GLint level,
  	GLint xoffset,
  	GLint yoffset,
  	GLsizei width,
  	GLsizei height,
  	GLenum format,
  	GLenum type,
  	const GLvoid * pixels)
{
    using namespace GraphicsMonitor;
    using namespace GraphicsLayer::SharedMemoryProtocol;

    if(width == 32 && height == 32)
    {
        const Texture& texture = textures[current2dTexture];
        if(texture.height == 512 && texture.width == 512)
        {
            PixelData packet;
            memcpy(packet.pixels, pixels, 32 * 32 * 4);
            packet.targetTextureId = current2dTexture;
            packet.texX = xoffset;
            packet.texY = yoffset;

            pixelPackets.push_back(packet);
//            herpderp++;
//
//            std::stringstream sstream;
//            sstream << "subTex/" << herpderp;
//            unsigned char* rgb = rgbaToRgb((unsigned char*)pixels, width, height);
//            writePixelsToFile(sstream.str(), rgb, width, height);
//            delete[] rgb;
        }
    }

    void (*originalFunc)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*)
    = (void (*)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*))dlsym(RTLD_NEXT, "glTexSubImage2D");
    originalFunc(target, level, xoffset, yoffset, width, height, format, type, pixels);
}

///////////////////////////////////

void glBindFramebufferEXT(GLenum target, GLuint framebuffer)
{
    void (*originalFunc)(GLenum, GLuint) = (void(*)(GLenum, GLuint))dlsym(RTLD_NEXT, "glBindFramebufferEXT");
    originalFunc(target, framebuffer);

    GraphicsMonitor::currentFramebuffer = framebuffer;
//    std::cout << "TARGET: ------------- " << target << std::endl;

    if(glGetError() != GL_NO_ERROR)
        std::cout << "ERROR: glBindFramebufferEXT" << std::endl;
}


void glFramebufferTexture2DEXT(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
    void (*originalFunc)(GLenum, GLenum, GLenum, GLuint, GLint) = (void(*)(GLenum, GLenum, GLenum, GLuint, GLint))dlsym(RTLD_NEXT, "glFramebufferTexture2DEXT");
//    std::cout << GL_COLOR_ATTACHMENT0_EXT << std::endl;
//    std::cout   << "Attaching tex to framebuffer" << std::endl
//                << "\t" << "target: " << target << std::endl
//                << "\t" << "attachment: " << attachment << std::endl
//                << "\t" << "texture: " << texture << std::endl;

    GraphicsMonitor::currentDrawTexture = texture;

    originalFunc(target, attachment, textarget, texture, level);
}

///////////////////////////////////

Window XGetSelectionOwner(Display* d, Atom selection)
{
    if(selection == GraphicsMonitor::tibiaRunningAtom)
        return None;

    Window (*originalFunc)(Display*, Atom) = (Window (*)(Display*, Atom))dlsym(RTLD_NEXT, "XGetSelectionOwner");
    return originalFunc(d, selection);
}

///////////////////////////////////

Atom XInternAtom(Display* d, const char* name, int ifExists)
{
    Atom (*originalFunc)(Display*, const char*, int) = (Atom (*)(Display*, const char*, int))dlsym(RTLD_NEXT, "XInternAtom");

    Atom atom = originalFunc(d, name, ifExists);

    if(strcmp(name, "TIBIARUNNING") == 0)
        GraphicsMonitor::tibiaRunningAtom = atom;

    return atom;

}




Window XCreateWindow
(
    Display* display,
    Window parent,
    int x,
    int y,
    unsigned int width,
    unsigned int height,
    unsigned int borderWidth,
    int depth,
    unsigned int xClass,
    Visual* visual,
    unsigned long valueMask,
    XSetWindowAttributes* attributes
)
{

    Window (*originalFunc)(Display*, Window, int, int, unsigned int, unsigned int, unsigned int, int, unsigned int, Visual*, unsigned long, XSetWindowAttributes*) =
    (Window (*)(Display*, Window, int, int, unsigned int, unsigned int, unsigned int, int, unsigned int, Visual*, unsigned long, XSetWindowAttributes*))dlsym(RTLD_NEXT, "XCreateWindow");

    using namespace GraphicsMonitor;
    shm = (GraphicsLayer::SharedMemoryProtocol::SharedMemorySegment*)mmap(nullptr, GraphicsLayer::SharedMemoryProtocol::NUM_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if(shm == MAP_FAILED)
    {
        std::cout << "Could not map shared memory." << std::endl;
        std::cout << errno << std::endl;
    }

    shm->hasPendingChanges = false;

    Window xWinId = originalFunc(display, parent, x, y, width, height, borderWidth, depth, xClass, visual, valueMask, attributes);
    shm->xWindowId = xWinId;

    return xWinId;
}


//Display* XOpenDisplay(const char* name)
//{
//    Display* (*originalFunc)(const char*) = (Display* (*)(const char*))dlsym(RTLD_NEXT, "XOpenDisplay");
//
//    std::cout << name[0] << std::endl;
//    std::cout << "Name:" << name << std::endl;
//
//    return originalFunc(name);
//}
