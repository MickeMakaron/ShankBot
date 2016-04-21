#!/usr/bin/env python3

# ShankBot project
# Mikael Hernvall 2016
# Extract sprites from Tibia.spr
# Run the script with the "-h" flag for usage instructions.


import sys, getopt
import binascii
import struct
from PIL import Image

import os.path

def readBytesFromFile(fileName):
	with open(fileName, "rb") as f:
		
		b = bytes()
		#while True:
		chunk = f.read()
		b += chunk
			#if chunk:
				#for b in chunk:
					#yield b
			#else:
				#break
	return b


def bit16ToInt(byte1, byte2):
	return int.from_bytes([byte1, byte2], byteorder='little', signed=False)


def bit32ToInt(byte1, byte2, byte3, byte4):
	return int.from_bytes([byte1, byte2, byte3, byte4], byteorder='little', signed=False)


def incrementPixelCoordinates(x, y, width=32):
	x += 1
	if(x >= width):
		x = 0
		y += 1
		
	return [x, y]

def writeSpriteAt(b, index, fileName):
	transparentPixelColor = (b[index], b[index + 1], b[index + 2], 0)
	index += 3
	
	statedSize = bit16ToInt(b[index], b[index + 1])
	size = 32
	index += 2
	
	image = Image.new('RGBA', (size, size))
	pixels = image.load()

	x = 0
	y = 0
	imageIndex = 0
	numPixels = size * size
	numBytesRead = 0
	while(imageIndex < numPixels):
		numTransparentPixels = bit16ToInt(b[index], b[index + 1])

		index += 2
		numBytesRead += 2
		
		for i in range(imageIndex, imageIndex + numTransparentPixels):
			pixels[x, y] = transparentPixelColor
			imageIndex += 1
			[x, y] = incrementPixelCoordinates(x, y)

		numOpaquePixels = bit16ToInt(b[index], b[index + 1])
		
		index += 2
		numBytesRead += 2
		
		for i in range(imageIndex, imageIndex + numOpaquePixels):
			if(numBytesRead >= statedSize):
				break;
				
			red = b[index]
			index += 1
			green = b[index]
			index += 1
			blue = b[index]
			index += 1

			pixels[x, y] = (red, green, blue, 255)
			numBytesRead += 3
			imageIndex += 1;
			
			[x, y] = incrementPixelCoordinates(x, y)
			
		if(numBytesRead >= statedSize):
			for i in range(imageIndex, numPixels):
				pixels[x, y] = transparentPixelColor
				[x, y] = incrementPixelCoordinates(x, y)
					
			break
	
	outFile = fileName + ".png"
	
	print("Extracting to: ", outFile)
	image.save(outFile)
		
	return

def extractSprites(tibiaSprPath, outDir):
	index = 0
	
	b = readBytesFromFile(tibiaSprPath)
	
	version = bit32ToInt(b[index], b[index + 1], b[index + 2], b[index + 3])
	index += 4
	
	numSprites = bit32ToInt(b[index], b[index + 1], b[index + 2], b[index + 3])
	index += 4

	for i in range(0, numSprites):
		spriteIndex = bit32ToInt(b[index], b[index + 1], b[index + 2], b[index + 3])
		index += 4
		
		if(spriteIndex != 0):
			writeSpriteAt(b, spriteIndex, outDir + "/" + str(i))
			
		


def printHelp():
	print('Usage: read.py -i <Tibia.spr path> -o <output directory path>')

def main(argv):
	tibiaSpr = ''
	outDir = ''
	try:
		opts, args = getopt.getopt(argv,"hi:o:")
	except getopt.GetoptError:
		printHelp();
		sys.exit(2)
	
	argCount = 0
	for opt, arg in opts:
		if opt == '-h':
			printHelp();
			sys.exit()
		elif opt in ("-i"):
			tibiaSpr = arg
			argCount += 1
		elif opt in ("-o"):
			outDir = arg
			argCount += 1

	if(argCount < 2):
		printHelp()
		sys.exit()

	if(os.path.isfile(tibiaSpr) == False):
		print("Could not find file '", tibiaSpr, "'.")
		sys.exit(2)
		
	if(os.path.isdir(outDir) == False):
		os.makedirs(outDir)

	extractSprites(tibiaSpr, outDir)



if(__name__ == "__main__"):
	main(sys.argv[1:])
