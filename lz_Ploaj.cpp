//Created by Daxar 5/24/15
//For Spriters-resource ripping project http://www.vg-resource.com/thread-26897.html
//C version of Ploaj's LZX decompression method
//Do whatever you want with this code

#include "lz11.h"
int* lzx_decompress(int* instream, int* outsize)
{

	int pointer = 0;

	byte type = (byte) instream[pointer++];
	//cout << "Type: " << (uint32_t)type << endl;
	int decompressedSize = (instream[pointer++])
			| (instream[pointer++] << 8) | (instream[pointer++] << 16);

	int* outstream = new int[decompressedSize];
	int outpointer = 0;

	if (decompressedSize == 0) {
		decompressedSize = (instream[pointer++])
				| (instream[pointer++] << 8) | (instream[pointer++] << 16)
				| (instream[pointer++] << 24);
		;
	}

	int bufferLength = 0x1000;
	int* buffer = new int[bufferLength];
	int bufferOffset = 0;

	int currentOutSize = 0;
	int flags = 0, mask = 1;

	while (currentOutSize < decompressedSize) {
		if (mask == 1) {
			flags = instream[pointer++];
			mask = 0x80;
		} else {
			mask >>= 1;
		}

		if ((flags & mask) > 0) {
			int byte1 = instream[pointer++];

			int length = byte1 >> 4;
			int disp = -1;

			if (length == 0) {
				int byte2 = instream[pointer++];
				int byte3 = instream[pointer++];

				length = (((byte1 & 0x0F) << 4) | (byte2 >> 4)) + 0x11;
				disp = (((byte2 & 0x0F) << 8) | byte3) + 0x1;

			} else if (length == 1) {
				int byte2 = instream[pointer++];
				int byte3 = instream[pointer++];
				int byte4 = instream[pointer++];

				length = (((byte1 & 0x0F) << 12) | (byte2 << 4) | (byte3 >> 4)) + 0x111;
				disp = (((byte3 & 0x0F) << 8) | byte4) + 0x1;

			} else {
				int byte2 = instream[pointer++];

				length = ((byte1 & 0xF0) >> 4) + 0x1;
				disp = (((byte1 & 0x0F) << 8) | byte2) + 0x1;
			}

			int bufIdx = bufferOffset + bufferLength - disp;
			for (int i = 0; i < length; i++) {
				int next = buffer[bufIdx % bufferLength];
				bufIdx++;
				outstream[outpointer++] = next;
				buffer[bufferOffset] = next;
				bufferOffset = (bufferOffset + 1) % bufferLength;
			}
			currentOutSize += length;
		} else {
			int next = instream[pointer++];

			outstream[outpointer++] = next;
			currentOutSize++;
			buffer[bufferOffset] = next;
			bufferOffset = (bufferOffset + 1) % bufferLength;
		}

	}
	delete[] buffer;
	*outsize = decompressedSize;
	return outstream;

}