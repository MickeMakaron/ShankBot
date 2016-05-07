#!/usr/bin/env python3

# ShankBot project
# Mikael Hernvall 2016
# Connect sprites with Tibia objects.

import sys, getopt
import os.path
from shutil import copyfile

def bit16ToInt(byte1, byte2):
	return int.from_bytes([byte1, byte2], byteorder='little', signed=False)


def bit32ToInt(byte1, byte2, byte3, byte4):
	return int.from_bytes([byte1, byte2, byte3, byte4], byteorder='little', signed=False)


class Object:
	objectId = 0
	spriteIds = []
	
	def __init__(self, objId):
		self.objectId = objId
		self.spriteIds = []
		
class Sprite:
	spriteId = 0
	objectIds = []
	
	def __init__(self, spriteId):
		self.spriteId = spriteId
		self.objectIds = set()


def readData(tibiaDatPath, tibiaSprites, outPath):		
	b = open(tibiaDatPath, "rb").read()

	index = 0
	
	version = bit32ToInt(b[index], b[index + 1], b[index + 2], b[index + 3])
	index += 4
	
	numItems = bit16ToInt(b[index], b[index + 1])
	index += 2
	
	numOutfits = bit16ToInt(b[index], b[index + 1])
	index += 2
	
	numEffects = bit16ToInt(b[index], b[index + 1])
	index += 2
	
	numDistances = bit16ToInt(b[index], b[index + 1])
	index += 2
	
	numObjects = numItems + numOutfits + numEffects + numDistances

	objects = []
	itemId = 100
	maxSpriteId = 0
	names = {}
	print("Reading Tibia.dat...")
	for i in range(0, numObjects):
		if(itemId + i > numObjects):
			break;

		isOutfit = (itemId + i > numItems and itemId + i <= numItems + numOutfits)

		if(isOutfit):
			while(b[index] != 0xff):
				index += 1
				
			index += 1
			
		else:
			while(b[index] != 0xff):
				bCurrent = b[index]
				index += 1
				if(bCurrent == 0x00): # Ground tile
					walkSpeed = bit16ToInt(b[index], b[index + 1])
					index += 2
				elif(bCurrent == 0x01): # Rendered third last
					topOrder = 1
				elif(bCurrent == 0x02): # Rendered second last
					topOrder = 2
				elif(bCurrent == 0x03): # Rendered last (always on top)
					topOrder = 3
				elif(bCurrent == 0x04): # Is a container
					isContainer = True
				elif(bCurrent == 0x05): # Stackable
					isStackable = True
				#elif(bCurrent == 0x06): # N/A
				elif(bCurrent == 0x07):
					isUsable = True					
				elif(bCurrent == 0x08): # Readable and writable
					isReadable = True
					isWriteable = True
					maxCharacters = bit16ToInt(b[index], b[index + 1])
					index += 2
				elif(bCurrent == 0x09): # Readable
					isReadable = True
					maxCharacters = bit16ToInt(b[index], b[index + 1])
					index += 2
				elif(bCurrent == 0x0a): # Fluid containers (including filled ones, like "vial of water")
					isFluid = True
				elif(bCurrent == 0x0b): # A splash on the ground (retrievable with a fluid container)
					isSplash = True
				elif(bCurrent == 0x0c): # Blocking. Player cannot move here.
					isBlocking = True
				elif(bCurrent == 0x0d): # Immovable
					isMovable = False
				elif(bCurrent == 0x0f): # The pathfinding will not walk on this (fire fields, for example)
					isPathBlocking = True
				#elif(bCurrent == 0x10): # No move animation ???
				#elif(bCurrent == 0x11): # Blocks path of creatures
				elif(bCurrent == 0x12): # Hangable on a wall
					isHangable = True
				elif(bCurrent == 0x13): # Horizontal wall
					isWall = True
				elif(bCurrent == 0x14): # Vertical wall
					isWall = True
				elif(bCurrent == 0x15): # Rotatable
					isRotatable = True
				elif(bCurrent == 0x16): # Light data
					lightBrightness = bit16ToInt(b[index], b[index + 1])
					index += 2
					lightColor = bit16ToInt(b[index], b[index + 1])
					index += 2
				#elif(bCurrent == 0x17): # N/A
				elif(bCurrent == 0x18): # Floor change tile
					isFloorChange = True
				elif(bCurrent == 0x19): # N/A
					index += 4
				elif(bCurrent == 0x1a): # Height. For example, if you put something on a parcel, that thing will be rendered {height} higher than normal.
					height = bit16ToInt(b[index], b[index + 1])
					index += 2
				#elif(bCurrent == 0x1b): # N/A
				#elif(bCurrent == 0x1c): # N/A
				elif(bCurrent == 0x1d): # Color on the minimap
					minimapColor = bit16ToInt(b[index], b[index + 1])
					index += 2
				elif(bCurrent == 0x1e): # Help???
					if(bit16ToInt(b[index], b[index + 1] == 1112)):
						isReadable = True
					index += 2
				#elif(bCurrent == 0x1f): # Full tile ???
				elif(bCurrent == 0x20): # Transparent
					isTransparent = True
					
					
				elif(bCurrent == 0x21): # N/A
					index += 2
				elif(bCurrent == 0x22): # Market data
					category = bit16ToInt(b[index], b[index + 1])
					index += 2
					marketId1 = bit16ToInt(b[index], b[index + 1])
					index += 2
					marketId2 = bit16ToInt(b[index], b[index + 1])
					index += 2
					
					nameLength = bit16ToInt(b[index], b[index + 1])
					index += 2
					
					name = b[index:index + nameLength].decode("latin-1")
					names[itemId + i] = name
					index += nameLength
					
					profession = bit16ToInt(b[index], b[index + 1])
					index += 2
					
					level = bit16ToInt(b[index], b[index + 1])
					index += 2
				elif(bCurrent == 0x23):
					defaultAction = bit16ToInt(b[index], b[index + 1])
					index += 2
				#elif(bCurrent == 0xfe): # Usable ???

			index += 1
		
		hasOutfitThingy = False
		if(isOutfit == True):
			hasOutfitThingy = (bit16ToInt(b[index], b[index + 1]) == 2)
			index += 2
		
		
		width = b[index]
		index += 1
		

		height = b[index]
		index += 1

		
		if(width != 1 or height != 1):
			index += 1
			
		blendFrames = b[index]
		index += 1

		
		
		divX = b[index]
		index += 1
		

		divY = b[index]
		index += 1

		
		divZ = b[index]
		index += 1
		
		animationLength = b[index]
		index += 1



		if(animationLength > 1):
			index += 6			
			index += animationLength * 2 * 4
		
		numSprites = width * height * blendFrames * divX * divY * divZ * animationLength
		sprites = set()
		for sprite in range(0, numSprites):
			sprites.add(bit32ToInt(b[index], b[index + 1], b[index + 2], b[index + 3]))
			index += 4
			
		
		if(isOutfit and hasOutfitThingy):
			index += 1
			
			width = b[index]
			index += 1

			height = b[index]
			index += 1
			
			if(width != 1 or height != 1):
				index += 1
				
			blendFrames = b[index]
			index += 1

			divX = b[index]
			index += 1

			divY = b[index]
			index += 1
			
			divZ = b[index]
			index += 1
			
			animationLength = b[index]
			index += 1
			
			if(animationLength > 1):
				index += 6			
				index += animationLength * 2 * 4
				
			numSprites = width * height * blendFrames * divX * divY * divZ * animationLength
			for sprite in range(0, numSprites):
				sprites.add(bit32ToInt(b[index], b[index + 1], b[index + 2], b[index + 3]))
				index += 4
				

		o = Object(itemId + i)
		for sprite in sprites:
			if(sprite != 0):
				#folder = outDir + "/" + str(itemId + i)
				#spriteFile = tibiaSprites + "/" + str(sprite) + ".png"
				
				#if(os.path.isdir(folder) == False):		
					#os.makedirs(folder) 
				
				#spriteDest = folder + "/" + str(sprite) + ".png"
				#if(os.path.isfile(spriteDest) == False):
					#print(spriteDest)
					#copyfile(spriteFile, spriteDest)
					
				o.spriteIds.append(sprite);
				if(sprite > maxSpriteId):
					maxSpriteId = sprite
				
				
				
		objects.append(o)
				
	print(names)
	spriteObjectBindings = []
	spritesPerList = 1000
	numLists = int(maxSpriteId / spritesPerList) + 1
	for i in range(0, numLists):
		spriteObjectBindings.append([])
	
	
	print("Creating bindings...")
	for o in objects:
		for s in o.spriteIds:
			duplicateAlreadyRegistered = False
			dupes = spriteObjectBindings[int(s / spritesPerList)]
			for d in dupes:
				if(d.spriteId == s):
					duplicateAlreadyRegistered = True
					d.objectIds.add(o.objectId)
					break
				
			if(duplicateAlreadyRegistered == False):
				duplicateSprite = Sprite(s)
				duplicateSprite.objectIds.add(o.objectId)
				dupes.append(duplicateSprite)
	

	outStr = ""
	for d in spriteObjectBindings:
		
		for s in d:
			#if(len(s.objectIds) > 1):
			assert(len(s.objectIds) > 0)
			outStr += str(s.spriteId)
			for o in s.objectIds:
				outStr += " " + str(o)
				
			outStr += "\n"

	print("Writing bindings to '" + outPath + "'.")
	f = open(outPath, 'w')
	f.write(outStr)
	f.close()
	
	
			
		


def printHelp():
	print("This script is used in conjunction with the extractSprites script.")
	print("The sprites location is the output directory of that script.")
	print('Usage: extractData.py -i <Tibia.dat path> -s <sprites directory> -o <output directory path>')

def main(argv):
	#tibiaDat = '';
	#tibiaSprites = '';
	#outPath = '';
	#try:
		#opts, args = getopt.getopt(argv,"hi:s:o:")
	#except getopt.GetoptError:
		#printHelp();
		#sys.exit(2)
	
	#argCount = 0
	#for opt, arg in opts:
		#if opt == '-h':
			#printHelp();
			#sys.exit()
		#elif opt in ("-i"):
			#tibiaDat = arg
			#argCount += 1
		#elif opt in ("-s"):
			#tibiaSprites = arg
			#argCount += 1
		#elif opt in ("-o"):
			#outPath = arg
			#argCount += 1

	#if(argCount < 3):
		#printHelp()
		#sys.exit()

	#if(os.path.isfile(tibiaDat) == False):
		#print("Could not find file '", tibiaDat, "'.")
		#sys.exit(2)
		
	#if(os.path.isdir(tibiaSprites) == False):
		#print("Could not find directory '", tibiaSprites, "'.")
		#sys.exit(2)
		
	tibiaDat = "/home/vendrii/programs/Tibia/Tibia.dat"
	tibiaSprites = ""
	outPath = "out"

	readData(tibiaDat, tibiaSprites, outPath)



if(__name__ == "__main__"):
	main(sys.argv[1:])
