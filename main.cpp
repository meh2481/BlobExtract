//Created by Daxar 5/24/15
//For Spriters-resource ripping project http://www.vg-resource.com/thread-26897.html
//Do whatever you want with this code

#include "lz11.h"

uint16_t byteSwap16(uint16_t byte)
{
	uint16_t hibyte = (byte & 0xff00) >> 8;
	uint16_t lobyte = (byte & 0x00ff);
	return lobyte << 8 | hibyte;
}

uint32_t byteSwap32(uint32_t val)
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | (val >> 16);
}

float toFloat(uint32_t val)
{
	float f;
	memcpy(&f, &val, sizeof(float));
	return f;
}

float byteSwapFloat(uint32_t val)
{
	val = byteSwap32(val);
	return toFloat(val);
}

Vec2 byteSwapVec2(fakeVec2 v)
{
	Vec2 ret;
	ret.x = byteSwapFloat(v.x);
	ret.y = byteSwapFloat(v.y);
	return ret;
}

piece byteSwapPiece(fakePiece p)
{
	piece ret;
	ret.topLeft = byteSwapVec2(p.topLeft);
	ret.topLeftUV = byteSwapVec2(p.topLeftUV);
	ret.topRight = byteSwapVec2(p.topRight);
	ret.topRightUV = byteSwapVec2(p.topRightUV);
	ret.bottomRight = byteSwapVec2(p.bottomRight);
	ret.bottomRightUV = byteSwapVec2(p.bottomRightUV);
	ret.bottomLeft = byteSwapVec2(p.bottomLeft);
	ret.bottomLeftUV = byteSwapVec2(p.bottomLeftUV);
	return ret;
}

RGBQUAD makeColor(int a, int r, int b, int g) 
{
	RGBQUAD quad = { a, r, b, g };
	return quad;
}

FIBITMAP* makeTexture(int* bytes, int byteslen, int width, int height)
{
	int* buffer1 = new int[byteslen / 2];
	int* buffer2 = new int[byteslen / 2];

	int i1 = 0, i2 = 0;
	int pointer = 0;

	while(i1 + i2 < byteslen) 
	{
		for(int j = 0; j < 32; j++)
		{
			buffer1[i1++] = bytes[pointer++];
		}
		for(int k = 0; k < 32; k++)
		{
			buffer2[i2++] = bytes[pointer++];
		}
	}
	
	FIBITMAP* img = FreeImage_Allocate(width, height, 32);

	int x = 0, y = 0;
	i1 = 0;
	i2 = 0;

	while(i1 + i2 < byteslen) 
	{
		for(int h = 0; h < 4; h++) 
		{
			for(int w = 0; w < 4; w++) 
			{
				RGBQUAD colorQuad = makeColor(buffer2[i2++], buffer2[i2++], buffer1[i1 + 1], buffer1[i1]);
				i1 += 2;
				FreeImage_SetPixelColor(img, x + w, y + h, &colorQuad);
			}
		}
		x += 4;
		if(x >= width)
		{
			x = 0;
			y += 4;
		}
	}
	delete[] buffer1;
	delete[] buffer2;
	FreeImage_FlipVertical(img);	//Silly FreeImage, storing things upside-down
	return img;
}

FIBITMAP* PieceImage(FIBITMAP* curImg, list<piece> pieces, Vec2 maxul, Vec2 maxbr)
{
	uint32_t width, height;
	Vec2 OutputSize;
	Vec2 CenterPos;
	OutputSize.x = -maxul.x + maxbr.x;
	OutputSize.y = maxul.y - maxbr.y;
	CenterPos.x = -maxul.x;
	CenterPos.y = maxul.y;
	OutputSize.x = uint32_t(OutputSize.x);
	OutputSize.y = uint32_t(OutputSize.y);
	
	width = FreeImage_GetWidth(curImg);
	height = FreeImage_GetHeight(curImg);

	FIBITMAP* result = FreeImage_Allocate(OutputSize.x, OutputSize.y, 32);

	//Patch image together from pieces
	for(list<piece>::iterator lpi = pieces.begin(); lpi != pieces.end(); lpi++)
	{
		//Adding 0.5 and casting to int rounds to nearest pixel
		FIBITMAP* imgPiece = FreeImage_Copy(curImg, 
											(int)((lpi->topLeftUV.x) * width + 0.5), (int)((lpi->topLeftUV.y) * height + 0.5), 
											(int)((lpi->bottomRightUV.x) * width + 0.5), (int)((lpi->bottomRightUV.y) * height + 0.5));
		
		//Since pasting doesn't allow you to post an image onto a particular position of another, do that by hand
		int curPos = 0;
		int srcW = FreeImage_GetWidth(imgPiece);
		int srcH = FreeImage_GetHeight(imgPiece);
		unsigned pitch = FreeImage_GetPitch(imgPiece);
		unsigned destpitch = FreeImage_GetPitch(result);
		BYTE* bits = (BYTE*)FreeImage_GetBits(imgPiece);
		BYTE* destBits = (BYTE*)FreeImage_GetBits(result);
		Vec2 DestPos = CenterPos;
		DestPos.x += lpi->topLeft.x;
		DestPos.y = OutputSize.y - srcH;
		DestPos.y -= CenterPos.y;
		DestPos.y += lpi->topLeft.y;
		DestPos.x = (unsigned int)(DestPos.x);
		DestPos.y = ceil(DestPos.y);
		for(int y = 0; y < srcH; y++)
		{
			BYTE* pixel = bits;
			BYTE* destpixel = destBits;
			destpixel += (unsigned)(DestPos.y + y) * destpitch;
			destpixel += (unsigned)(DestPos.x * 4);
			for(int x = 0; x < srcW; x++)
			{
				destpixel[FI_RGBA_RED] = pixel[FI_RGBA_RED];
				destpixel[FI_RGBA_GREEN] = pixel[FI_RGBA_GREEN];
				destpixel[FI_RGBA_BLUE] = pixel[FI_RGBA_BLUE];
				destpixel[FI_RGBA_ALPHA] = pixel[FI_RGBA_ALPHA];
				pixel += 4;
				destpixel += 4;
			}
			bits += pitch;
		}
		
		FreeImage_Unload(imgPiece);
	}
	
	return result;
}

void splitImages(const char* cFilename)
{
	vector<uint8_t> vData;
	FILE* fh = fopen(cFilename, "rb");
	if(fh == NULL)
	{
		cerr << "Unable to open input file " << cFilename << endl;
		return;
	}
	fseek(fh, 0, SEEK_END);
	size_t fileSize = ftell(fh);
	fseek(fh, 0, SEEK_SET);
	vData.reserve(fileSize);
	size_t amt = fread(vData.data(), fileSize, 1, fh);
	fclose(fh);
	cout << "Splitting images from file " << cFilename << endl;
	
	//Figure out what we'll be naming the images
	string sName = cFilename;
	//First off, strip off filename extension
	size_t namepos = sName.find(".anb");
	if(namepos != string::npos)
		sName.erase(namepos);
	//Next, strip off any file path before it
	namepos = sName.rfind('/');
	if(namepos == string::npos)
		namepos = sName.rfind('\\');
	if(namepos != string::npos)
		sName.erase(0, namepos+1);
		
	//Create the folder we'll be saving into
	#ifdef _WIN32
		CreateDirectory(TEXT(sName.c_str()), NULL);
	#else
		#error Do something here to create folder
	#endif
	
	//Read in animation header
	anbHeader ANBh;
	memcpy(&ANBh, &(vData.data()[0]), sizeof(anbHeader));
	
	ANBh.numImages = byteSwap32(ANBh.numImages);
	ANBh.fileEnd = byteSwap32(ANBh.fileEnd);
	
	//Copy compressed image data into int buffer, since that's what Ploaj's functions expect
	int* compressed;
	compressed = new int[fileSize];
	for(int i = 0; i < fileSize; i++)
		compressed[i] = vData[i];
	
	//Read headers in first pass
	vector<list<piece> > pieces;
	vector<texHeader> tHeaders;
	Vec2 maxul;
	Vec2 maxbr;
	maxul.x = maxul.y = maxbr.x = maxbr.y = 0;
	for(int iCurFile = 0; iCurFile < ANBh.numImages; iCurFile++)
	{
		//Read in our next texture header
		texHeader th;
		memcpy(&th, &(vData.data()[sizeof(anbHeader)+sizeof(texHeader)*iCurFile]), sizeof(texHeader));
		
		//Byte swap everything so it's correct
		th.pieceOffset = byteSwap32(th.pieceOffset);
		th.imageOffset = byteSwap32(th.imageOffset);
		th.imageH = byteSwap16(th.imageH);
		th.imageW = byteSwap16(th.imageW);
		th.numPieces = byteSwap16(th.numPieces);
	
		//Read in list of image pieces
		list<piece> pieces2;
		for(uint32_t j = 0; j < th.numPieces; j++)
		{
			piece p;
			fakePiece f;
			memcpy(&f, &(vData.data()[th.pieceOffset+j*sizeof(fakePiece)]), sizeof(fakePiece));
			
			p = byteSwapPiece(f);
			//Store our maximum values, so we know how large the image is
			if(p.topLeft.x < maxul.x)
				maxul.x = p.topLeft.x;
			if(p.topLeft.y > maxul.y)
				maxul.y = p.topLeft.y;
			if(p.bottomRight.x > maxbr.x)
				maxbr.x = p.bottomRight.x;
			if(p.bottomRight.y < maxbr.y)
				maxbr.y = p.bottomRight.y;
				
			//cout << "Piece: " << p.topLeft.x << ", " << p.topLeft.y << ", " << p.topLeftUV.x  << ", " << p.topLeftUV.y << ", " << p.bottomRight.x << ", " << p.bottomRight.y << ", " << p.bottomRightUV.x << ", " << p.bottomRightUV.y << endl;
			
			pieces2.push_back(p);
		}
		pieces.push_back(pieces2);
		tHeaders.push_back(th);
	}
	
	//Decompress second pass
	for(int iCurFile = 0; iCurFile < ANBh.numImages; iCurFile++)
	{		
		//Decompress LZX data
		int decompSz;
		int* buf = lzx_decompress(&(compressed[tHeaders[iCurFile].imageOffset]), &decompSz);
		
		//Do pixel calculations to make a usable image from this
		FIBITMAP* img = makeTexture(buf, decompSz, tHeaders[iCurFile].imageW, tHeaders[iCurFile].imageH);
		
		//Spin through pieces and create the final image
		FIBITMAP* pieced = PieceImage(img, pieces[iCurFile], maxul, maxbr);
		
		//Save image
		ostringstream oss;
		oss << sName << "/" << setfill('0') << setw(3) << iCurFile+1 << ".png";
		cout << "Saving " << oss.str() << endl;
		FreeImage_Save(FIF_PNG, pieced, oss.str().c_str());
		
		FreeImage_Unload(img);
		FreeImage_Unload(pieced);
		
		delete[] buf;
	}
	delete[] compressed;
}

int main(int argc, char** argv)
{
	FreeImage_Initialise();
	list<string> sFilenames;
	//Parse commandline
	for(int i = 1; i < argc; i++)
	{
		string s = argv[i];
		sFilenames.push_back(s);
	}
	
	//Decompress ANB files
	for(list<string>::iterator i = sFilenames.begin(); i != sFilenames.end(); i++)
		splitImages((*i).c_str());
	FreeImage_DeInitialise();
	return 0;
}