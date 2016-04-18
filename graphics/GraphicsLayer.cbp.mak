#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -Iincl
CFLAGS = -Wall -std=c++14
RESINC = 
LIBDIR = 
LIB = -lpng
LDFLAGS = 

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/GraphicsLayer

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = bin/Release/GraphicsLayer

OBJ_DEBUG = $(OBJDIR_DEBUG)/src/PngImage.o $(OBJDIR_DEBUG)/src/SpriteCache.o $(OBJDIR_DEBUG)/src/SpriteDatabase.o $(OBJDIR_DEBUG)/src/TileBuffer.o $(OBJDIR_DEBUG)/src/main.o $(OBJDIR_DEBUG)/src/utility.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/src/PngImage.o $(OBJDIR_RELEASE)/src/SpriteCache.o $(OBJDIR_RELEASE)/src/SpriteDatabase.o $(OBJDIR_RELEASE)/src/TileBuffer.o $(OBJDIR_RELEASE)/src/main.o $(OBJDIR_RELEASE)/src/utility.o

all: debug release

clean: clean_debug clean_release

before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/src/PngImage.o: src/PngImage.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/PngImage.cpp -o $(OBJDIR_DEBUG)/src/PngImage.o

$(OBJDIR_DEBUG)/src/SpriteCache.o: src/SpriteCache.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/SpriteCache.cpp -o $(OBJDIR_DEBUG)/src/SpriteCache.o

$(OBJDIR_DEBUG)/src/SpriteDatabase.o: src/SpriteDatabase.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/SpriteDatabase.cpp -o $(OBJDIR_DEBUG)/src/SpriteDatabase.o

$(OBJDIR_DEBUG)/src/TileBuffer.o: src/TileBuffer.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/TileBuffer.cpp -o $(OBJDIR_DEBUG)/src/TileBuffer.o

$(OBJDIR_DEBUG)/src/main.o: src/main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/main.cpp -o $(OBJDIR_DEBUG)/src/main.o

$(OBJDIR_DEBUG)/src/utility.o: src/utility.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/utility.cpp -o $(OBJDIR_DEBUG)/src/utility.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)/src

before_release: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/src/PngImage.o: src/PngImage.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/PngImage.cpp -o $(OBJDIR_RELEASE)/src/PngImage.o

$(OBJDIR_RELEASE)/src/SpriteCache.o: src/SpriteCache.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/SpriteCache.cpp -o $(OBJDIR_RELEASE)/src/SpriteCache.o

$(OBJDIR_RELEASE)/src/SpriteDatabase.o: src/SpriteDatabase.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/SpriteDatabase.cpp -o $(OBJDIR_RELEASE)/src/SpriteDatabase.o

$(OBJDIR_RELEASE)/src/TileBuffer.o: src/TileBuffer.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/TileBuffer.cpp -o $(OBJDIR_RELEASE)/src/TileBuffer.o

$(OBJDIR_RELEASE)/src/main.o: src/main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/main.cpp -o $(OBJDIR_RELEASE)/src/main.o

$(OBJDIR_RELEASE)/src/utility.o: src/utility.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/utility.cpp -o $(OBJDIR_RELEASE)/src/utility.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE)/src

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

