
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

    size_t swapCount = 0;


    Atom tibiaRunningAtom;

    int shmFd = strtol(getenv("SHANKBOT_SHARED_MEMORY_FD"), NULL, 10);

    GraphicsLayer::SharedMemoryProtocol::SharedMemorySegment* shm = nullptr;

    std::vector<GraphicsLayer::SharedMemoryProtocol::PixelData> pixelPackets;
    std::vector<GraphicsLayer::SharedMemoryProtocol::DrawCall> drawCallPackets;

    bool updateDrawCalls = false;
}

///////////////////////////////////

void glXSwapBuffers(Display* dpy, GLXDrawable drawable)
{
    using namespace GraphicsMonitor;

    if(shm == nullptr)
    {
        shm = (GraphicsLayer::SharedMemoryProtocol::SharedMemorySegment*)mmap(nullptr, GraphicsLayer::SharedMemoryProtocol::NUM_BYTES, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
        if(shm == MAP_FAILED)
        {
            std::cout << "Could not map shared memory." << std::endl;
            std::cout << errno << std::endl;
        }

        shm->hasPendingChanges = false;
    }
    void (*originalFunc)(Display*, GLXDrawable) = (void (*)(Display*, GLXDrawable))dlsym(RTLD_NEXT, "glXSwapBuffers");

    originalFunc(dpy, drawable);
    if(glGetError() != GL_NO_ERROR)
        std::cout << "ERROR: glXSwapBuffers 1"<< std::endl;

    updateDrawCalls = false;
    if(!shm->hasPendingChanges)
    {
        memcpy(shm->pixelData, pixelPackets.data(), pixelPackets.size() * sizeof(GraphicsLayer::SharedMemoryProtocol::PixelData));
        shm->numPixelData = pixelPackets.size();

        memcpy(shm->drawCall, drawCallPackets.data(), drawCallPackets.size() * sizeof(GraphicsLayer::SharedMemoryProtocol::DrawCall));
        shm->numDrawCall = drawCallPackets.size();

        shm->hasPendingChanges = true;
        updateDrawCalls = true;

        pixelPackets.clear();
        drawCallPackets.clear();
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

    if(mode == GL_QUADS)
    {

        GraphicsMonitor::isPerformingPrimitiveDrawSession = true;
        originalFunc(mode);
    }
}

///////////////////////////////////

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

    if(vertices.size() != 4)
    {
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
        vertexMin.x = std::min(vertices[2].x, vertices[0].x);
        vertexMin.y = std::min(vertices[2].y, vertices[0].y);

        Texture texture = textures[current2dTexture];

        if(texCoordMin.x > 0.99f || texCoordMin.y > 0.99f)
        {

        }
        else if(updateDrawCalls)
        {
            size_t width = (texCoordMax.x - texCoordMin.x) * texture.width;
            size_t height = (texCoordMax.y - texCoordMin.y) * texture.height;

            DrawCall packet;
            packet.textureId = current2dTexture;
            packet.texX = texCoordMin.x * texture.width;
            packet.texY = texCoordMin.y * texture.height;
            packet.screenX = vertexMin.x;
            packet.screenY = vertexMin.y;
            packet.width = width;
            packet.height = height;

            drawCallPackets.push_back(packet);
        }
    }
}

///////////////////////////////////

void glVertex2i(GLint x,  GLint y)
{
    void (*originalFunc)(GLint, GLint) = (void (*)(GLint, GLint))dlsym(RTLD_NEXT, "glVertex2i");
    originalFunc(x, y);

    GraphicsMonitor::vertices.push_back({x, y});
}

///////////////////////////////////

void glTexCoord2d(GLdouble  s,  GLdouble  t)
{
    void (*originalFunc)(GLdouble, GLdouble) = (void (*)(GLdouble, GLdouble))dlsym(RTLD_NEXT, "glTexCoord2d");
    originalFunc(s, t);

    GraphicsMonitor::texCoords.push_back({s, t});
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

///////////////////////////////////

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
            packet.textureId = current2dTexture;
            packet.texX = xoffset;
            packet.texY = yoffset;

            pixelPackets.push_back(packet);
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

    if(glGetError() != GL_NO_ERROR)
        std::cout << "ERROR: glBindFramebufferEXT" << std::endl;
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
