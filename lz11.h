//Created by Daxar 5/24/15
//For Spriters-resource ripping project http://www.vg-resource.com/thread-26897.html
//Do whatever you want with this code

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <cstdio>
#ifdef _WIN32
	#include <windows.h>
#endif
#include "FreeImage.h"
#include <list>
#include <cmath>
#include <cstring>
#include <iomanip>
using namespace std;

typedef uint8_t byte;
typedef uint32_t uint;

int* lzx_decompress(int* instream, int* outsize);

typedef struct 
{
	uint32_t numImages;
	uint32_t fileEnd;
} anbHeader;

typedef struct
{
	uint32_t pieceOffset;
	uint32_t imageOffset;
	uint16_t imageW;
	uint16_t imageH;
	uint8_t  unknown0[8];
	uint16_t numPieces;
	uint8_t  unknown1[12];
} texHeader;

typedef struct
{
	float x;
	float y;
} Vec2;

typedef struct 
{
	Vec2 topLeft;
	Vec2 topLeftUV;
	Vec2 topRight;
	Vec2 topRightUV;
	Vec2 bottomRight;
	Vec2 bottomRightUV;
	Vec2 bottomLeft;
	Vec2 bottomLeftUV;
} piece;

//Helper classes for going big endian -> little endian
typedef struct
{
	uint32_t x;
	uint32_t y;
} fakeVec2;

typedef struct 
{
	fakeVec2 topLeft;
	fakeVec2 topLeftUV;
	fakeVec2 topRight;
	fakeVec2 topRightUV;
	fakeVec2 bottomRight;
	fakeVec2 bottomRightUV;
	fakeVec2 bottomLeft;
	fakeVec2 bottomLeftUV;
} fakePiece;