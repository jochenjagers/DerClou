/*
  IFF ANIM decoder by Markus Wolf http://murkymind.de (v1.01 2007-03-30)
  Ported and adapted by Oliver Gantert (2014-06-28)
  Last change by Oliver Gantert (2017-11-09)
*/
#include "iffanim.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "error\error.h"
#include "memory\memory.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (~0)
#endif

#define IFFANIM_FORMATINFO_BUFSIZE  1000  //size of string buffer for returning information about ANIM file
#define IFFANIM_PITCH  4           //pitch of scanline for frame output ("GetFrame()") can be set: multiple of 1, 2, 3 or 4 bytes

//struct for a single frame, used in a memory frame list
// contains needed anim header information for decompression
// some fields are not used currently
typedef struct {
	int				delta_compression;  //determines compression type
	int				mask;       //for XOR mode only

	int				w;          //XOR mode only
	int				h;          //XOR mode only
	int				x;          //XOR mode only
	int				y;          //XOR mode only

	int				reltime;    //relative display time in 1/60 sec
	int				interleave; //indicates how many frames back the data is to modify
	unsigned long	bits;       //options for some compressions as flags

	char *			cmap;          //original cmap (if exists), else NULL, number of color entries depends on bits per pixel resolution
	char *			data;          //original pixel data from file (maybe compressed)
	int				datasize;       //size of data in bytes
} iffanim_frame;

//struct for embedded audio, created with Amiga software "Wave Tracer DS" by ""
typedef struct {
	int				freq;      //playback sample frequency
	int				nch;       //number of channels: 0 no audio, 1 mono, 2 stereo (left, right interleaved), other values aren't supported
	int				bps;       //bits per sample point
	float			volume;    //volume: 0..1

	int				n;          //equal to nframes (the last frame data may contain 2 SBDY chunks, or somewhere else for joined animations)
	char *			data;       //audio buffer (Big Endian byte order, signed)
	int  *			dataoffset; //list of audio sample start, which starts playing at current frame (for every frame), in bytes, begins on full frames
	int				datasize;   //total audio data size in bytes
} iffanim_audio;

struct IffAnim {
	//animation attributes
	int				w;         //width of anim (original format)
	int				h;         //height of anim (original format)
	int				bpp;         //bits per pixel of anim (original format)
	int				mask;            //indicates mask type defined in BMHD chunk (0,1,2,3)
	int				ham;             //"true" if ham mode
	int				ehb;             //"true" if extra half bright palette
	int				compressed;      //indicates compression type of first frame (0 or 1, only byterun is supported) (other frames have delta compression)
	unsigned char	dcompressions[256]; //bit array indicates which delta compression methods are used in the anim (there may be mixed compression modes)

	int				nframes;       //number of frames
	int				lentime;       //overall length of animation in 1/60 seconds (original format)
	iffanim_frame *	frame;  //list containing all frames (original format, still delta compressed)

	//decoded image in common format ready for output
	int				disp_bpp;       //display format (doesn't change), in common supported format: (1..8 bit converted to 8, ham to 24 bpp)
	int				disp_pitch;     //number of byte per scanline (size of dispframe in bytes: h * disp_pitch), can be considered as always 0

	char *			disp_frame;     //decoded frame, display ready converted (a line padded to full 32 bit)
	char *			disp_cmap;      //cmap, if 8 bit display mode (for all frames the same), else NULL

	char *			prev_disp_frame; //previous display frame (a player may need this for double buffering with resize handling without creating an extra frame copy)
	char *			prev_disp_cmap;  //previous display frame map

	//current state (frame attributes)
	char *			prevframe;   //frame before current (internal/original frame format)
	char *			prevcmap;    //pointer to cmap of previous frame
	char *			curframe;    //buffer for decoded frame, in original format (multiplanar)
	char *			curcmap;     //ptr to cmap in frame list, can be redefined for a frame
	int				framesize;   //size of a decoded frame in bytes (each scanline has a multiple of 16 bit -> number of bytes is even)
	int				frameno;     //current frame (as number)

	iffanim_audio	audio;  //contains audio data, if supported

	int				num_disp_frames;   //number of frames converted to display format (set to 0 after loading)
	int				loopanim;          //if set, when looping, the next frame of the last frame is frame 2, else frame 0
	int				loop;              //if unset, there is an error when trying to load the next frame of the last frame, else the animation restarts
	char			formatinfo[IFFANIM_FORMATINFO_BUFSIZE];   //buffer for returning format information
};

static int IffAnim_AllocDeltaFrames(IffAnim *anim);
static int IffAnim_AllocDisplayFrames(IffAnim *anim);
static int IffAnim_AllocDisplayCmap(IffAnim *anim);
static int IffAnim_ConvertHamTo24bpp(IffAnim *anim, void *dst_, void *src_, void *cmap_, int w, int h, int hambits, int dst_pitch);
static int IffAnim_BitplanarToChunky(IffAnim *anim, void *dst_, void *src_, int w, int h, int bitssrc, int bitsdst, int dst_pitch);
static int IffAnim_DecodeByteRun(IffAnim *anim, void *dst_, void *data_, int datasize, int w, int h, int bpp, int mask);
static int IffAnim_DecodeByteVerticalDelta(IffAnim *anim, void *dst_, void *data_, int w, int bpp);
static int IffAnim_DecodeLSVerticalDelta7(IffAnim *anim, void *dst_, void *data_, int w, int bpp, int long_data);
static int IffAnim_DecodeLSVerticalDelta8(IffAnim *anim, void *dst_, void *data_, int w, int bpp, int long_data);
static int IffAnim_DecodeDeltaJ(IffAnim *anim, void *dst_, void *delta_, int w, int h, int bpp);
static int IffAnim_FindChunk(IffAnim *anim, FILE *file, char *idreq, int len);
static int IffAnim_GetNumFrames(IffAnim *anim, FILE *file);
static int IffAnim_read_ANHD(IffAnim *anim, FILE *file, iffanim_frame *frame);
static int IffAnim_read_CMAP(IffAnim *anim, FILE *file, iffanim_frame *frame);
static int IffAnim_InterleaveStereo(IffAnim *anim, char *data, int datasize, int bps);
static int IffAnim_read_SBDY(IffAnim *anim, FILE *file, int searchlen, char **audiobuf, int *audiobufsize);
static int IffAnim_ReadFrames(IffAnim *anim, FILE *file);
static int IffAnim_PrintInfo(IffAnim *anim);
static int IffAnim_DecodeFrame(IffAnim *anim, char *dstframe, int index);
static long fget32(FILE *file);
static int fget16(FILE *file);

static int IffAnim_AllocDeltaFrames(IffAnim *anim)
{
	//allocate buffer for delta decoded frames
	int framesize;
	framesize = ((anim->w + 1) & (~1)) * anim->bpp * anim->h;
	anim->prevframe = (char *)MemAlloc(framesize);
	if (anim->prevframe) {
		anim->curframe = (char *)MemAlloc(framesize);
		if (anim->curframe) {
			return(TRUE);
		}
		MemFree(anim->prevframe, framesize);
		anim->prevframe = NULL;
	}
	return(FALSE);
}

static int IffAnim_AllocDisplayFrames(IffAnim *anim)
{
	//allocate buffer for display frames
	int framesize;
	anim->disp_pitch = (((anim->w * anim->disp_bpp / 8) + (IFFANIM_PITCH - 1)) / IFFANIM_PITCH) * IFFANIM_PITCH;  // pitch of scanline: rounding up to next multiple of IFFANIM_PITCH bytes
	framesize = anim->disp_pitch * anim->h;
	anim->prev_disp_frame = (char *)MemAlloc(framesize);
	if (anim->prev_disp_frame) {
		anim->disp_frame = (char *)MemAlloc(framesize);
		if (anim->disp_frame) {
			return(TRUE);
		}
		MemFree(anim->prev_disp_frame, framesize);
		anim->prev_disp_frame = NULL;
	}
	return(FALSE);
}

static int IffAnim_AllocDisplayCmap(IffAnim *anim)
{
	//allocate buffer for display color map if necessary
	int framesize;
	if (anim->disp_bpp <= 8) {
		framesize = (1 << anim->disp_bpp) * 4;
		anim->prev_disp_cmap = (char *)MemAlloc(framesize);
		if (anim->prev_disp_cmap) {
			anim->disp_cmap = (char *)MemAlloc(framesize);
			if (anim->disp_cmap) {
				return(TRUE);
			}
			MemFree(anim->prev_disp_cmap, framesize);
			anim->prev_disp_cmap = NULL;
		}
	} else {
		anim->prev_disp_cmap = NULL;
		anim->disp_cmap = NULL;
		return(TRUE);
	}
	return(FALSE);
}

IffAnim *IffAnim_Open(char *fname)
{
	IffAnim *anim;
	FILE *file;

	anim = (IffAnim *)MemAlloc(sizeof(IffAnim));
	if (anim) {
		file = fopen(fname, "rb");
		if (file) {
			anim->nframes = IffAnim_GetNumFrames(anim, file);
			if (anim->nframes > 0) {
				if (IffAnim_ReadFrames(anim, file)) {
					if (IffAnim_AllocDeltaFrames(anim)) {
						//determine display format
						if ((anim->bpp <= 8) && !anim->ham) {
							anim->disp_bpp = 8;
						} else {
							anim->disp_bpp = 24;
						}
						if (IffAnim_AllocDisplayFrames(anim)) {
							if (IffAnim_AllocDisplayCmap(anim)) {
								if (IffAnim_PrintInfo(anim)) {
									//set default attributes
									IffAnim_SetLoopAnim(anim, FALSE);
									IffAnim_SetLoop(anim, TRUE);
									if (IffAnim_Reset(anim)) {
										return(anim);
									}
								}
							}
						}
					}
				}
			}
			fclose(file);
		}
		IffAnim_Close(anim);
	}
	return(NULL);
}

void IffAnim_Close(IffAnim *anim)
{
	int i;
	int framesize;
	
	if (anim) {
		framesize = (1 << anim->disp_bpp) * 4;
		if (anim->prev_disp_cmap) {
			MemFree(anim->prev_disp_cmap, framesize);
		}
		if (anim->disp_cmap) {
			MemFree(anim->disp_cmap, framesize);
		}

		framesize = anim->disp_pitch * anim->h;
		if (anim->prev_disp_frame) {
			MemFree(anim->prev_disp_frame, framesize);
		}
		if (anim->disp_frame) {
			MemFree(anim->disp_frame, framesize);
		}

		framesize = ((anim->w + 1) & (~1)) * anim->bpp * anim->h;
		if (anim->prevframe) {
			if (anim->curframe) {	// LucyG 2017-09-11 memory leak
				MemFree(anim->curframe, framesize);
			}
			MemFree(anim->prevframe, framesize);
		}
		if (anim->frame) {
			for (i = 0; i < anim->nframes; i++) {
				if (anim->frame[i].data) {
					MemFree(anim->frame[i].data, anim->frame[i].datasize);
				}
				if (anim->frame[i].cmap) {
					MemFree(anim->frame[i].cmap, (1 << anim->bpp) * 3);
				}
			}
			MemFree(anim->frame, sizeof(iffanim_frame) * anim->nframes);	// LucyG 2017-11-09 size was wrong
		}
		if (anim->audio.dataoffset) {
			MemFree(anim->audio.dataoffset, sizeof(int) * anim->audio.n);
		}
		if (anim->audio.data) {
			MemFree(anim->audio.data, anim->audio.datasize);
		}
		MemFree(anim, sizeof(IffAnim));
	}
}

//src must be in original bitplanar format, destination is chunky
//cmap pointer must point to existing palette (R,G,B format)
//"hambits" must be 6 or 8
static int IffAnim_ConvertHamTo24bpp(IffAnim *anim, void *dst_, void *src_, void *cmap_, int w, int h, int hambits, int dst_pitch)
{
	int i, j, k;        //loop counter
	int data, mode;   //parts of a HAM value
 	char colbuf[3];    //color component buffer: R,G,B
 	unsigned char *dstL = (unsigned char*)dst_; //start of current line in destination
	unsigned char *srcL = (unsigned char*)src_; //start of current line in source
	unsigned char *src,*dst;                    //working pointers
 	int bitpos;               //bit position in a plane of the soure data
	char *cmap = (char *)cmap_;
 	int bitplane_pitch = (w + 15) / 16 * 2;     //pitch of a bitplane multiple of 16 bit  in bytes
	int src_pitch = bitplane_pitch * hambits;   //pitch of line of bitplanes
 	int modeshift = hambits - 2;           //to shift mode bits into correct useful position
	int datamask =  (1 << modeshift) - 1;  //to mask mode bits
	int datashift = 10 - hambits;          //to shift the data to upper bits for 8bit color component value

	if ((hambits != 8) && (hambits != 6)) {
		return(FALSE);
	}

	for (j = 0; j < h; j++) { // for every line
		dst = dstL;
		memcpy(colbuf, cmap, 3);  //set to border color (index 0)
		for (i = 0; i < w; i++) {   //for every pixel of line
			data = 0;
			bitpos = i;
			src = srcL + (bitpos / 8);     //position of the first byte to read
			bitpos = 7 - (bitpos & 0x7);
			for (k = 0; k < hambits; k++) {
				data |= ((*src >> bitpos) & 0x1) << k;  //shift to bit position 0, then to it's right position
				src += bitplane_pitch;
			}
			mode = data >> modeshift;   //upper 2 bits
			data = data & datamask;     //lower 6 bits
			//change color component
			switch (mode) {
				case 0x0: memcpy(colbuf, cmap + (data * 3), 3); break; //RGB values from color map
				case 0x1: colbuf[2] = data << datashift; break;    //change blue upper bits
				case 0x2: colbuf[0] = data << datashift; break;    //change red upper bits
				case 0x3: colbuf[1] = data << datashift; break;    //change green upper bits
			}
			memcpy(dst, colbuf, 3); //write RGB color to dst
			dst += 3;
		}
		dstL += dst_pitch;
		srcL += src_pitch;
	} 
	return(TRUE);
}

//convert a bitplanar frame to chunky
//converts also bits per pixel (low to high only, make sure: bitssrc <= bitsdst)
static int IffAnim_BitplanarToChunky(IffAnim *anim, void *dst_, void *src_, int w, int h, int bitssrc, int bitsdst, int dst_pitch)
{
	unsigned char *dst = (unsigned char *)dst_;
	unsigned char *src = (unsigned char *)src_;
	int i, j, k;
	int bitval;     //for storing a bit
	int srcBitOfs;  //offset from beginning of line of source in bits
	int dstBitOfs;  //offset from beginning of line of dest. in bits
	int downshift;
	int BitPlaneRowLen = ((w + 15) >> 4) << 4; //for single bitplane, in bits
	int LineLenSrc = (BitPlaneRowLen >> 3) * bitssrc; //in bytes
	int LineLenDst = dst_pitch;    //in bytes
	//number of padding bits per pixel (for bpp conversion)
	int padbits = bitsdst - bitssrc;

	for (k = 0; k < h; k++) {
		memset(dst, 0, LineLenDst);
		dstBitOfs = 0;
		for (j = 0; j < w; j++) {
			//for all bits of a pixel
			srcBitOfs = j;
			downshift = 7 - (srcBitOfs & 0x7);   //bit order in a bitplane byte: 0x1 masks bit 7, 0x80 masks bit 0 of color index
			for (i = 0; i < bitssrc; i++) {
				bitval = (src[srcBitOfs >> 3] >> downshift)  &  0x1;   //get bit from bitplane
				dst[dstBitOfs >> 3] |= bitval << i;                    //write bit to chunky buffer
				dstBitOfs++; 
				srcBitOfs += BitPlaneRowLen;
			}
			dstBitOfs += padbits;
		}
		src += LineLenSrc;
		dst += LineLenDst;
	}
	return(TRUE);
}

//decode RLE ("byterun" aka "packer", aka "PackBits" on Macintosh) compressed line
//normally only the first frame is packed with it (except delta method 0 or 1)
//mask plane is ignored (if available)
static int IffAnim_DecodeByteRun(IffAnim *anim, void *dst_, void *data_, int datasize, int w, int h, int bpp, int mask)
{
	int i, j;
	char *dst = (char*)dst_;   //destination (uncompressed)
	char *src = (char*)data_;  //byte code (compressed)
	int planepitch;     //pitch of a bitplane
	int linepitch;      //pitch of a scanline with mask plane
	int n, val;
	int posdst;         //write position in dst
	int possrc;         //read position in src
 
	planepitch = (w + 15) / 16 * 2;   //pitch of a plane
	linepitch = planepitch * bpp;     //pitch of a line without mask
	if (mask == 1) {
		linepitch += planepitch;        //add mask plane pitch to line pitch
	}
	if (!dst || !src) {
		return(FALSE);
	}
	possrc = 0;   //position in src buffer

	//for each line decode to dst buffer
	for (i = 0; i < h; i++) {
		posdst = 0;
		j = planepitch * bpp;  //number of bytes for the scanline to decode (without mask)
		//while scanline data is not decoded
		while (j > 0) {
			n = src[possrc++];   //get type
			if (n >= 0) {         //copy number of bytes
				n = n + 1;
				if (n > j) {         //overflow protection
					memcpy(dst + posdst, src + possrc, j);
				} else {
					memcpy(dst + posdst, src + possrc, n);
				}
				possrc += n;
			} else if (n != -128) { //multiple times the same byte value
				n = -n + 1;
				val = src[possrc++];
				if (n > j) {        //overflow protection
					memset(dst + posdst, val, j);
				} else {
					memset(dst + posdst, val, n);
				}
			}
			posdst += n;
			j -= n;
		}
		dst += planepitch * bpp;                 //set pointer to beginning of next line
	}
	return(TRUE);
}

// Decode Byte Vertical Delta compression (compression 5)
static int IffAnim_DecodeByteVerticalDelta(IffAnim *anim, void *dst_, void *data_, int w, int bpp)
{
	unsigned char *data = (unsigned char *)data_;
	char *dst = (char*)dst_;
	int i, j, k;       //loop counter
	int ofsdst;      //offset in destination buffer
	int ofssrc;      //offset in compressed data (delta chunk)
	int op, val;     //holds opcode , data value
	//width of a plane within a line in bytes (number of columns in a plane)
	int ncolumns = ((w + 15) / 16) * 2;
	//total len of a line in destination buffer, in bytes
	int dstpitch = ncolumns * bpp;

	//for every plane
	for(k = 0; k < bpp; k++) {
		//get offset (pointer) to compressed opcodes and data of current plane
		ofssrc = k * 4;
		ofssrc = (data[ofssrc] << 24) | (data[ofssrc + 1] << 16) | (data[ofssrc + 2] << 8) | data[ofssrc + 3];
		if (ofssrc) {   //no change in plane if pointer index is 0
			//for each column of a plane (column: a byte from every line -> vertically)
			for (j = 0; j < ncolumns; j++) {
				ofsdst = j + k * ncolumns;   //set dst offset for current column, a column starts in the first scanline
				//get number of ops for the column and interpret
				for (i = data[ofssrc++]; i > 0; i--) {
					op = data[ofssrc++];      //get opcode
					//if SAME_OP, opcode 0
					if (op == 0) {
						op =  data[ofssrc++];  //number of same bytes
						val = data[ofssrc++];
						while (op) {
							dst[ofsdst] = val;
							ofsdst += dstpitch;
							op--;
						}
					} else if (op < 0x80) { //if SKIP_OP, high bit is 0
						ofsdst += op * dstpitch;
					} else { //if UNIQ_OP, high bit is set
						op &= 0x7f;  //set high bit to 0
						while (op) {
							dst[ofsdst] = data[ofssrc++];
							ofsdst += dstpitch;
							op--;
						}
					}
				} //end for all ops
			}  //end for all columns
		}
	} // end for all planes
	return(TRUE);
}

//decode delta 7 long or short
static int IffAnim_DecodeLSVerticalDelta7(IffAnim *anim, void *dst_, void *data_, int w, int bpp, int long_data)
{
	unsigned char *data = (unsigned char *)data_;  //source buffer
	char *dst = (char*)dst_;
	int i, j;
	unsigned long p_da;     //offset to data in source
	unsigned long p_op;     //offset to opcode in source buffer
	int ofsdst;        //offset in destination buffer in bytes
	int op;            //holds opcode
	int opcnt;         //op counter
	short val16;     //holds 16 bit data
	long val32;     //holds 32 bit data
	int t;             //help variable
	int ncolumns;      //number of columns per bitplane (total number of columns for "long" data), each with size "wordsize"
	int wordsize;      //bytes for one data word: 32 (long) or 16 bit (short)
	int dstpitch;      //length of a scanline in destination buffer in bytes
	int half = FALSE; //last column maybe only with 16 instead of 32 bits

	if (long_data) {
		wordsize = 4;              
		ncolumns = (w + 31) / 32;
		if (((w + 15) / 16 * 2) != ((w + 31) / 32 * 4)) {  //relating to the width of a video frame, possibly for the last column of each plane one must copy only 16 bit words, 
			half = TRUE;
		}
	} else {
		wordsize = 2;
		ncolumns = (w + 15) / 16;
	}
	dstpitch = ((w + 15) / 16 * 2) * bpp;

	//for every plane
	for (i = 0; i < bpp; i++) {
		//get 32 bit offsets (pointers) to data and opcodes for the current plane, stored as Big Endian
		t = i * 4;
		p_op = (data[t] << 24) | (data[t + 1] << 16) | (data[t + 2] << 8) | data[t + 3];
		p_da = (data[t + 32] << 24) | (data[t + 33] << 16) | (data[t + 34] << 8) | data[t + 35];
		if (p_op) {  //if opcode pointer index not 0 => plane is modified
			//for each column
			for (j = 0; j < ncolumns; j++) {
				//set dst byte start offset for current column, a column starts in the first scanline
				ofsdst = (j + i * ncolumns) * wordsize;
				//correct if last column has only 16 bit
				if (half) {
					ofsdst -= (2 * i);
				}
				//get number of ops for the column
				opcnt = data[p_op++];
				//interpret all ops of the column
				while (opcnt) {
					op = data[p_op++];   //fetch opcode
					if ((wordsize == 2) || (half && ((j + 1) == ncolumns))) {   //2 bytes per data word, or 16 bit of last column with 32 bit byte wordsize
						//SAME_OP, opcode is 0
						if (op == 0) {
							op = data[p_op++];   //number of same words
							val16 = *((short *)(data + p_da));  //get data word
							p_da += wordsize;
							while (op) {
								*((short *)(dst + ofsdst)) = val16;
								ofsdst += dstpitch;
								op--;
							}
						} else if (op < 128) {
							//SKIP_OP, high bit is not set
							ofsdst += dstpitch * op;
						} else {
							//UNIQ_OP, high bit is set
							op &= 0x7f;  //mask out high bit and use as counter
							while (op) {
								*((short *)(dst + ofsdst)) = *((short *)(data + p_da));
								p_da += wordsize;
								ofsdst += dstpitch;
								op--;
							}
						}
					} else {   //4 bytes per data word
						//SAME_OP, opcode is 0
						if (op == 0) {
							op = data[p_op++];   //number of same words
							val32 = *((long *)(data + p_da));  //get data word
							p_da += 4;
							while (op) {
								*((long *)(dst + ofsdst)) = val32;
								ofsdst += dstpitch;
								op--;
							}
						} else if (op < 128) {
							//SKIP_OP, high bit is not set
							ofsdst += dstpitch * op;
						} else {
							//UNIQ_OP, high bit is set 
							op &= 0x7f;
							while (op) {
								*((long *)(dst + ofsdst)) = *((long *)(data + p_da));
								p_da += 4;
								ofsdst += dstpitch;
								op--;
							}
						}
					}
					opcnt--;
				} //end for number of ops
			}
		}
	}
	return(TRUE);
}

// decompress delta mode 8 long or short
static int IffAnim_DecodeLSVerticalDelta8(IffAnim *anim, void *dst_, void *data_, int w, int bpp, int long_data)
{
	unsigned char *data = (unsigned char *)data_;  //source buffer
	char *dst = (char*)dst_;
	int i, j;
	unsigned long p_op;       //offset to opcode in source buffer
	int ofsdst;          //offset in destination buffer in bytes
	unsigned long op;         //holds opcode
	unsigned int opcnt;  //op counter
	short val16;       //holds 16 bit data
	long val32;       //holds 32 bit data
	int t;               //help variable
	int ncolumns;        //number of columns per bitplane (total number of columns for "long" data), each with size "wordsize"
	int wordsize;        //bytes for one data word: 32 (long) or 16 bit (short)
	int dstpitch;        //length of a scanline in destination buffer in bytes
	int half = FALSE;   //last column maybe only with 16 instead of 32 bits

	if (long_data) {
		wordsize = 4;              
		ncolumns = (w + 31) / 32;
		if (((w + 15) / 16 * 2) != ((w + 31) / 32 * 4)) {  //relating to the width of a video frame, possibly for the last column of each plane one must copy only 16 bit words, 
			half = TRUE;
		}
	} else {
		wordsize = 2;
		ncolumns = (w + 15) / 16;
	}
	dstpitch = ((w + 15) / 16 * 2) * bpp;

	//for every plane
	for (i = 0; i < bpp; i++) {
		//get 32 bit offset (pointer) to opcodes for the current plane, stored as Big Endian
		t = i * 4;
		p_op = (data[t] << 24) | (data[t + 1] << 16) | (data[t + 2] << 8) | data[t + 3];
		if (p_op) {  //if opcode pointer index not 0 => plane is modified
			//for each column
			for (j = 0; j < ncolumns; j++) {
				//set dst byte start offset for current column, a column starts in the first scanline
				ofsdst = (j + i * ncolumns) * wordsize;
				if (wordsize == 2) {
					opcnt = (data[p_op] << 8) | data[p_op + 1]; //get number of ops for the column
				} else {
					if (half) {
						ofsdst -= (2 * i);                 //correct if last column has only 16 bit
					}
					opcnt = (data[p_op] << 24) | (data[p_op + 1] << 16) | (data[p_op + 2] << 8) | data[p_op + 3];
				}
				p_op += wordsize;
				//interpret all ops of the column
				while (opcnt) {
					//fetch opcode
					if (wordsize == 2) {
						op = (data[p_op] << 8) | data[p_op + 1];   
					} else {
						op = (data[p_op] << 24) | (data[p_op + 1] << 16) | (data[p_op + 2] << 8) | data[p_op + 3];
					}
					p_op += wordsize;
					if ((wordsize == 2) || (half && ((j + 1) == ncolumns))) {   //2 bytes per data word, or 16 bit of last column with 32 bit byte wordsize
						//SAME_OP, opcode is 0
						if (op == 0) {
							op = (data[p_op] << 8) | data[p_op + 1]; //number of same words
							p_op += 2;
							val16 = *((short *)(data + p_op));      //get data word
							p_op += 2;
							while (op) {
								*((short *)(dst + ofsdst)) = val16;
								ofsdst += dstpitch;
								op--;
							}
						} else if (op < 0x8000) {
							//SKIP_OP, high bit is not set
							ofsdst += dstpitch * op;
						} else {
							//UNIQ_OP, high bit is set 
							op &= 0x7fff;   //mask out high bit and use as counter
							while (op) {
								*((short *)(dst + ofsdst)) = *((short *)(data + p_op));
								p_op += 2;
								ofsdst += dstpitch;
								op--;
							}
						}
					} else {   //4 bytes per data word
						//SAME_OP, opcode is 0
						if (op == 0) {
							op = (data[p_op] << 24) | (data[p_op + 1] << 16) | (data[p_op + 2] << 8) | data[p_op + 3];   //number of same words
							p_op += 4;
							val32 = *((long *)(data + p_op));  //get data word
							p_op += 4;
							while (op) {
								*((long *)(dst + ofsdst)) = val32;
								ofsdst += dstpitch;
								op--;
							}
						} else if (op < 0x80000000) {
							//SKIP_OP, high bit is not set
							ofsdst += dstpitch * op;
						} else {
							//UNIQ_OP, high bit is set 
							op &= 0x7fffffff;
							while (op) {
								*((long *)(dst + ofsdst)) = *((long *)(data + p_op));
								p_op += 4;
								ofsdst += dstpitch;
								op--;
							}
						}
					}
					opcnt--;
				} //end for number of ops
			}
		}
	}
	return(TRUE);
}

/*
The following function is an interpretation of the
"Delta_J" code from " "xanim2801.tar.gz" (XAnim Revision 2.80.1).
It is modified to draw to a bitplanar frame buffer (BODY data format) instead a
chunky one, thus it fits better into a decoding pipeline.

 char *dst,     //Image Buffer pointer (old frame data, BODY format)
 void *delta_,  //delta data
 int  w,        //width in pixels
 int  h,        //height in pixels
 int  bpp       //bits per pixel (depth, number of bitplanes)
*/
static int IffAnim_DecodeDeltaJ(IffAnim *anim, void *dst_, void *delta_, int w, int h, int bpp)
{
	unsigned char *image = (unsigned char*)dst_;
	unsigned char *delta = (unsigned char*)delta_;
	long   pitch;     //scanline width in bytes
	unsigned char   *i_ptr;    //used as destination pointer into the frame buffer
	unsigned long  type, r_flag, b_cnt, g_cnt, r_cnt; 
	int b, g, r, d;    //loop counters
	unsigned long  offset;    //byte offset
	int planepitch_byte = (w + 7) / 8;      //plane pitch as multiple of 8 bits, needed to calc the right offset
	int planepitch = ((w + 15) / 16) * 2;   //width of a line of a single bitplane in bytes (multiple of 16 bit)
	int kludge_j;
	int exitflag = 0;
	pitch = planepitch * bpp;               //size of a scanline in bytes (bitplanar BODY buffer)
	//for pixel width < 320 we need the horizontal byte offset in a bitplane on a 320 pixel wide screen
	if (w < 320) {
		kludge_j = (320 - w) / 8 / 2;  //byte offset
	} else {
		kludge_j = 0;
	}
	//loop until block type 0 appears (or any unsupported type with unknown byte structure)
	while (!exitflag) {
		//read compression type and reversible_flag ("reversible" means XOR operation) 
		type = ((delta[0]) << 8) | (delta[1]);
		delta += 2;
		//switch on compression type
		switch (type) {
			case 0:
				exitflag = 1;
			break;  // end of list, delta frame complete -> leave
			case 1:
				//read reversible_flag
				r_flag = (*delta++) << 8; r_flag |= (*delta++);
				// Get byte count and group count 
				b_cnt = (*delta++) << 8; b_cnt |= (*delta++);
				g_cnt = (*delta++) << 8; g_cnt |= (*delta++);
				// Loop through groups
				for (g = 0; g < g_cnt; g++) {
					offset = (*delta++) << 8; offset |= (*delta++);
					//get real byte offset in IFF BODY data
					if (kludge_j) {
						offset = ((offset/(320 / 8)) * pitch) + (offset % (320/ 8)) - kludge_j;
					} else {
						offset = ((offset/planepitch_byte) * pitch) + (offset % planepitch_byte);
					}
					i_ptr = image + offset;  //BODY data pointer
					//read and apply "byte count" * "bpp" bytes (+ optional pad byte for even number of bytes)
					//1 byte for each plane -> modifies up to 8 bits
					// byte count represents number of rows

					// Loop thru byte count
					for (b = 0; b < b_cnt; b++) {  //number of vertical steps
						for (d = 0; d < bpp; d++) {  //loop thru planes, a delta byte for each plane
							if (r_flag) {
								*i_ptr ^= *delta++;
							} else {
								*i_ptr  = *delta++;
							}
							i_ptr += planepitch;    //go to next plane
						} // end of depth loop 
					} // end of byte loop
					if ((b_cnt * bpp) & 0x1) {
						delta++;  //read pad byte (group contains even number of bytes)
					}
				} //end of group loop 
			break;
			case 2:
				//read reversible_flag
				r_flag = (*delta++) << 8; r_flag |= (*delta++);

				// Read row count, byte count and group count
				r_cnt = (*delta++) << 8; r_cnt |= (*delta++);
				b_cnt = (*delta++) << 8; b_cnt |= (*delta++);
				g_cnt = (*delta++) << 8; g_cnt |= (*delta++);
 
				// Loop through groups
				for (g = 0; g < g_cnt; g++) {
					offset = (*delta++) << 8; offset |= (*delta++);
					//get real byte offset in IFF BODY data
					if (kludge_j) {
						offset = ((offset/(320 / 8)) * pitch) + (offset % (320/ 8)) - kludge_j;
					} else {
						offset = ((offset/planepitch_byte) * pitch) + (offset % planepitch_byte);
					}
					// Loop through rows
					for (r = 0; r < r_cnt; r++) {
						for (d = 0; d < bpp; d++) { // loop thru planes
							i_ptr = image + offset + (r * pitch) + d * planepitch;
             
							for (b = 0; b < b_cnt; b++) { // loop through byte count
								if (r_flag) {
									*i_ptr ^= *delta++;
								} else {
									*i_ptr  = *delta++;
								}
								i_ptr++;      // data is horizontal
							} // end of byte loop
						} // end of depth loop
					} // end of row loop
					if ((r_cnt * b_cnt * bpp) & 0x01) {
						delta++; // pad to even number of bytes
					}
				} // end of group loop
			break;
			default: //unknown type
				//exitflag = 1;
				return(FALSE);
			break;
		}  // end of type switch
	}  // end of while loop
	return(TRUE);
}

int IffAnim_CurrentFrameIndex(IffAnim *anim)
{
	if (anim->loopanim && (anim->frameno >= (anim->nframes - 2))) {
		return(anim->frameno - (anim->nframes - 2));
	}
	return(anim->frameno);
}

static long fget32(FILE *file)
{
	return((fgetc(file) << 24) | (fgetc(file) << 16) | (fgetc(file) << 8) | fgetc(file));
}

static int fget16(FILE *file)
{
	return((fgetc(file) << 8) | fgetc(file));
}

//find the chunk with the 4 byte id of "idreq" within a range from the current file position
//searches only in one level, file pointer must point to a chunk id inside this level
//returns start position in file of requested chunk and positions the file pointer to it's id
//not requested chunks are skipped
//returns -1 if chunk not found within range
static int IffAnim_FindChunk(IffAnim *anim, FILE *file, char *idreq, int len)
{
	char id[4];
	int chunksize;
	int pos;
	
	pos = ftell(file);
	len += pos;
	while (pos < len) {
		fread(id, 1, 4, file);
		if (!memcmp(id, idreq, 4)) {
			break;
		}
		chunksize = fget32(file);
		chunksize = (chunksize + 1) & (~1);
		pos += chunksize + 8;
		fseek(file, chunksize, SEEK_CUR);
	}
	if (pos >= len) {
		return(-1);
	}
	fseek(file, pos, SEEK_SET);
	return(pos);
}

//verify chunk structure
//count and return number of frames in the file
static int IffAnim_GetNumFrames(IffAnim *anim, FILE *file)
{
	char idbuf[4];
	int chunksize;
	int numframes;
	
	numframes = 0;
	fseek(file, 0, SEEK_SET);
	//check for FORM id
	fread(idbuf, 1, 4, file);
	if (memcmp(idbuf, "FORM", 4)) {
		return(-1);
	}
	fseek(file, 4, SEEK_CUR);
	//check for ANIM id
	fread(idbuf, 1, 4, file);
	if (memcmp(idbuf, "ANIM", 4)) {
		return(-1);
	}
	//count number of FORM...ILBM chunks (frames) within file
	do {
		memset(idbuf, 0, 4);
		//check for FORM id
		fread(idbuf, 1, 4, file);
		if (memcmp(idbuf, "FORM", 4)) {
			break;
		}
		chunksize = fget32(file);
		chunksize = (chunksize + 1) & (~1);
		//check for ILBM id
		fread(idbuf, 1, 4, file);
		if (memcmp(idbuf, "ILBM", 4)) {
			break;
		}
		//skip ILBM
		fseek(file, chunksize - 4, SEEK_CUR);
		//frame found
		numframes++;
	} while (TRUE);
	return(numframes);
}

static int IffAnim_read_ANHD(IffAnim *anim, FILE *file, iffanim_frame *frame)
{
	fseek(file, 8, SEEK_CUR);
	frame->delta_compression = fgetc(file);
	frame->mask = fgetc(file);
	frame->w = fget16(file);
	frame->h = fget16(file);
	frame->x = fget16(file);
	frame->y = fget16(file);
	fseek(file, 4, SEEK_CUR);
	frame->reltime = fget32(file);
	frame->interleave = fgetc(file);
	fseek(file, 1, SEEK_CUR);
	frame->bits = fget32(file);
	return(TRUE);
}

static int IffAnim_read_CMAP(IffAnim *anim, FILE *file, iffanim_frame *frame)
{
	int j;
	int ncolors;
	int palsize;

	fseek(file, 8, SEEK_CUR);
	//allocate mem for cmap
	ncolors = 1 << anim->bpp;
	palsize = ncolors * 3;   //RGB entries
	frame->cmap = (char *)MemAlloc(palsize);
	if (!frame->cmap) {
		return(FALSE);
	}
	//read cmap, handle EHB mode (second half are darker versions of the previous colors)
	if (anim->ehb) {
		palsize = palsize / 2;
		fread(frame->cmap, 1, palsize, file);
		for (j = 0; j < palsize; j++) {
			frame->cmap[palsize + j] = frame->cmap[j] / 2;  // every color value divided by 2 (half brightness)
		}
	} else {
		fread(frame->cmap, 1, palsize, file);
	}
	return(TRUE);
}

/* - make audio interleaved -> reordering
  current sample point order:    0L,1L,2L,3L,... | 0R,1R,2R,3R,...
  requested sample point order:  0L,0R,1L,1R,2L,2R,3L,3R,...

 - structure of bit depth other than 8 ist unknown, although 1..32 should be supported as the format spec. says
 -> support only for 8 and 16 bit
*/
static int IffAnim_InterleaveStereo(IffAnim *anim, char *data, int datasize, int bps)
{
	int i;
	int nframes;
	char *newdata;
 
	if (!data) {
		return(FALSE);
	}
	nframes = datasize / 2 / ((bps + 7) / 8);  //number of sample frames in "data"
	newdata = (char *)MemAlloc(datasize);
	if (!newdata) {
		return(FALSE);
	}
	//reorder
	if (bps <= 8) { //8 bit per point
		char *sl8 = (char *)data;
		char *sr8 = (char *)(data + (datasize / 2));
		char *dst8 = (char *)newdata;
		for (i = 0; i < nframes; i++) {
			*dst8 = sl8[i];
			dst8[1] = sr8[i];
			dst8 += 2;
		}
	} else {        //16 bit per point
		short *sl16  = (short *)data;
		short *sr16  = (short *)(data + (datasize / 2));
		short *dst16 = (short *)newdata;
		for (i = 0; i < nframes; i++) {
			*dst16 = sl16[i];
			dst16[1] = sr16[i];
			dst16 += 2;
		}
	}
	//copy reordered points to old buffer
	memcpy(data, newdata, datasize);
	MemFree(newdata, datasize);
	return(TRUE);
}

static int IffAnim_read_SBDY(IffAnim *anim, FILE *file, int searchlen, char **audiobuf, int *audiobufsize)
{
	char *tptr;    //help pointer
	int chunksize;
	int startpos;

	if (!audiobuf || !audiobufsize) {
		return(FALSE);
	}
	startpos = ftell(file);
	//file pointer should point to first SBDY chunk
	fseek(file, 4, SEEK_CUR);
	chunksize = fget32(file);
	*audiobuf = (char *)MemAlloc(chunksize);
	if (!(*audiobuf)) {
		return(FALSE);
	}
	*audiobufsize = chunksize;
	fread(*audiobuf, 1, chunksize, file);
	//interleave stereo channels
	if ((anim->audio.nch == 2) && anim->audio.bps) {
		IffAnim_InterleaveStereo(anim, *audiobuf, chunksize, anim->audio.bps);
	}
	//in case there is a second SBDY chunk, join the data to first one
	if (IffAnim_FindChunk(anim, file, "SBDY", searchlen - ((int)ftell(file) - startpos)) != -1) {
		fseek(file, 4, SEEK_CUR);
		chunksize = fget32(file);
		tptr = (char *)MemAlloc(*audiobufsize + chunksize);
		if (!tptr) {
			return(FALSE);
		}
		memcpy(tptr, *audiobuf,  *audiobufsize);       //copy data of first SBDY
		fread(tptr + *audiobufsize, 1, chunksize, file);  //read first SBDY data to mem
		MemFree(*audiobuf, *audiobufsize);  //delete old, too small buffer
		*audiobuf = tptr;    //set pointer to new buffer
		//interleave stereo channels
		if ((anim->audio.nch == 2) && anim->audio.bps) {
			IffAnim_InterleaveStereo(anim, *audiobuf + *audiobufsize, chunksize, anim->audio.bps);
		}
		*audiobufsize += chunksize; 
	}
	anim->audio.datasize += *audiobufsize;
	return(TRUE);
}

//check file for valid frames, read to mem, get lentime
static int IffAnim_ReadFrames(IffAnim *anim, FILE *file)
{
	char **tabuf;    //temporary audio data buffer list, an allocated block for each frame
	int *tabufsize;  //list of size of a block in bytes (for each frame)
	int i, l;
	int chunksize;
	int ILBMsize;   //size of ILBM chunk
	int filepos;    //marks position in file 
	int pos;        //for temporary use
	int body;
	char *framemem;
	char *ptr;
	int sync;

	anim->lentime = 0;
	memset(anim->dcompressions, 0, sizeof(anim->dcompressions));

	//allocate / init frame list
#ifdef THECLOU_DEBUG_ALLOC
	anim->frame = (iffanim_frame *)MemAllocDbg(sizeof(iffanim_frame) * anim->nframes, __FILE__, "IffAnim_ReadFrames 1");
#else
	anim->frame = (iffanim_frame *)MemAlloc(sizeof(iffanim_frame) * anim->nframes);
#endif
	if (!anim->frame) {
		return(FALSE);
	}
	/*
	for (i = 0; i < nframes; i++) {
		frame[i].cmap = NULL;
		frame[i].data = NULL;
	}
	*/
	//set get pointer of file to first frame
	fseek(file, 8, SEEK_SET);
	if (IffAnim_FindChunk(anim, file, "ANIM", 100) < 0) {
		return(FALSE);
	}
	fseek(file, 4, SEEK_CUR);
	//for all frames
	for (i = 0; i < anim->nframes; i++) {
		fseek(file, 4, SEEK_CUR);
		//get size of ILBM chunk
		ILBMsize = fget32(file);
		ILBMsize -= 4;
		//save ILBM start position
		fseek(file, 4, SEEK_CUR);
		filepos = ftell(file);
		//the following is only for frame 0
		if (!i) {
			//search for SXHD (audio header)
			//init audio struct
			if (IffAnim_FindChunk(anim, file, "SXHD", ILBMsize) >= 0) {
				anim->audio.n = anim->nframes;
#ifdef THECLOU_DEBUG_ALLOC
				tabuf = (char **)MemAllocDbg(sizeof(char *) * anim->audio.n, __FILE__, "IffAnim_ReadFrames 2");        //temporary audio data buffer list, an allocated block for each frame
#else
				tabuf = (char **)MemAlloc(sizeof(char *) * anim->audio.n);        //temporary audio data buffer list, an allocated block for each frame
#endif
				if (!tabuf) {
					return(FALSE);
				}
#ifdef THECLOU_DEBUG_ALLOC
				tabufsize = (int *)MemAllocDbg(sizeof(int) * anim->audio.n, __FILE__, "IffAnim_ReadFrames 3");     //list of size of a block in bytes (for each frame)
#else
				tabufsize = (int *)MemAlloc(sizeof(int) * anim->audio.n);     //list of size of a block in bytes (for each frame)
#endif
				if (!tabufsize) {
					return(FALSE);
				}
				/*
				for (j = 0; j < audio.n; j++) {  //init dynamically allocated lists
					tabuf[j] = NULL;
					tabufsize[j] = 0;
				}
				*/
				fseek(file, 8, SEEK_CUR);
				anim->audio.bps = (unsigned char)fgetc(file);
				anim->audio.volume = (float)fgetc(file) / 64.0f;
				fseek(file, 13, SEEK_CUR);
				anim->audio.nch = fgetc(file);     //only "1" or "2" supported
				anim->audio.freq = fget32(file);
			}
			fseek(file, filepos, SEEK_SET);  //back to ilbm chunk start
			//search for BMHD
			pos = IffAnim_FindChunk(anim, file, "BMHD", ILBMsize);
			if (pos < 0) {
				Log("\tBMHD chunk not found in first frame");
				return(FALSE);
			}
			fseek(file, 8, SEEK_CUR);
			//read relevant format info
			anim->w = fget16(file);
			anim->h = fget16(file);
			fseek(file, 4, SEEK_CUR);
			anim->bpp = fgetc(file); // bitplanes, same as bits per pixel
			anim->mask = fgetc(file);
			anim->compressed = fgetc(file);
			anim->framesize = (((anim->w + 15) / 16) * 2) * anim->bpp * anim->h; //multiple of 16 bit per plane
			//check compression
			if (anim->compressed > 1) {
				Log("\tUnknown compression of first frame");
				return(FALSE);
			}
			fseek(file, filepos, SEEK_SET);  //back to ilbm chunk start
			//search for CAMG chunk (for identifying HAM mode)
			anim->ham = FALSE;
			anim->ehb = FALSE;
			pos = IffAnim_FindChunk(anim, file, "CAMG", ILBMsize);
			if (pos >= 0) {
				fseek(file, pos + 10, SEEK_SET);
				//check if HAM or EHB mode is set
				if (fgetc(file) & 0x8) {
					anim->ham = TRUE;
				}
				if (fgetc(file) & 0x80) {
					anim->ehb = TRUE;
				}
			}
			fseek(file, filepos, SEEK_SET);  //back to ilbm chunk start
			//search & read CMAP
			if (anim->bpp <= 8) {
				pos = IffAnim_FindChunk(anim, file, "CMAP", ILBMsize);
				if (pos < 0) {
					Log("\tNo CMAP chunk found in first frame (bit resolution requires a CMAP)");
					return(FALSE);
				}
				if (!IffAnim_read_CMAP(anim, file, &(anim->frame[i]))) {
					return(FALSE);
				}
				fseek(file, filepos, SEEK_SET);  //back to ilbm chunk start
			}
		} else {
			//search for new CMAP
			pos = IffAnim_FindChunk(anim, file, "CMAP", ILBMsize);
			if (pos >= 0) {
				IffAnim_read_CMAP(anim, file, &(anim->frame[i]));
			}
			fseek(file, filepos, SEEK_SET);
		}
		//search and read SBDY
		if ((anim->audio.nch > 0) && (anim->audio.n > i) && tabuf && tabufsize) {
			//read first SBDY
			if (IffAnim_FindChunk(anim, file, "SBDY", ILBMsize) >= 0) {
				if (IffAnim_read_SBDY(anim, file, ILBMsize, &(tabuf[i]), &(tabufsize[i])) < 0) {  //if error occurs
					break;
				}
			}
			fseek(file, filepos, SEEK_SET);  //back to ilbm chunk start
		}
		//search & read ANHD
		pos = IffAnim_FindChunk(anim, file, "ANHD", ILBMsize);
		if (pos >= 0) {
			IffAnim_read_ANHD(anim, file, &(anim->frame[i]));
		} else {
			anim->frame[i].reltime = 0;
			anim->frame[i].delta_compression = 0;
		}
		anim->lentime += anim->frame[i].reltime;
		fseek(file, filepos, SEEK_SET);  //back to ilbm chunk start
		//check DLTA compression
		if ((anim->frame[i].delta_compression != 0) &&
			(anim->frame[i].delta_compression != 5) &&
			(anim->frame[i].delta_compression != 7) &&
			(anim->frame[i].delta_compression != 8) &&
			(anim->frame[i].delta_compression != 74)) {
			Log("\tDLTA compression method %d is not supported but used in frame %d\n", anim->frame[i].delta_compression, i);
			break;   //-> at least show the already loaded frames with supported compression
		}
		//search & read BODY or DLTA chunk
		body = FALSE;
		pos = IffAnim_FindChunk(anim, file, "BODY", ILBMsize);
		if (pos >= 0) {
			body = TRUE;
		} else {
			fseek(file, filepos, SEEK_SET);
			pos = IffAnim_FindChunk(anim, file, "DLTA", ILBMsize);
			if (pos < 0) {
				Log("\tno BODY or DLTA chunk found for frame %d\n", i);
				break;    //we stop reading frames here, maybe some frames are already read
			}
		}
		fseek(file, 4, SEEK_CUR);
		//get chunksize, allocate data buffer, read data
		chunksize = fget32(file);
#ifdef THECLOU_DEBUG_ALLOC
		anim->frame[i].data = (char *)MemAllocDbg(chunksize, __FILE__, "IffAnim_ReadFrames 4");
#else
		anim->frame[i].data = (char *)MemAlloc(chunksize);
#endif
		if (!anim->frame[i].data) {
			Log("\tMemAlloc %d", chunksize);
			break;
		}
		anim->frame[i].datasize = chunksize;
		fread(anim->frame[i].data, 1, chunksize, file);
		//decompress data from body chunks if RLE compressed, only "body" chunk data can be RLE compressed
		if (body && anim->compressed) {
			anim->framesize = ((anim->w + 15) / 16) * 2 * anim->bpp * anim->h;
#ifdef THECLOU_DEBUG_ALLOC
			framemem = (char *)MemAllocDbg(anim->framesize, __FILE__, "IffAnim_ReadFrames 5");  //memory for the RLE decompression is needed
#else
			framemem = (char *)MemAlloc(anim->framesize);  //memory for the RLE decompression is needed
#endif
			if (!framemem) {
				Log("\tMemAlloc %d", anim->framesize);
				break;
			}
			IffAnim_DecodeByteRun(anim, framemem, anim->frame[i].data, anim->frame[i].datasize, anim->w, anim->h, anim->bpp, anim->mask); 
			MemFree(anim->frame[i].data, anim->frame[i].datasize);        //delete compressed data
			anim->frame[i].data = framemem;      //insert decompressed data into framelist
			anim->frame[i].datasize = anim->framesize; //update size of decompressed data
			if ((anim->frame[i].delta_compression != 0) && (anim->frame[i].delta_compression != 1)) { //in some files the delta compression is not set correctly for the first BODY frame, only 0 (uncompressed) or 1 (XOR map) is allowed (which can be RLE compression although)
				anim->frame[i].delta_compression = 0;       //assume 0 is the correct value
			}
		}
		//set bit in compression mode list (body chunks aren't delta compressed)
		anim->dcompressions[anim->frame[i].delta_compression] = 1;
		//set file pointer to next frame
		fseek(file, filepos + ILBMsize, SEEK_SET);
	}
	//correct number of frames (if there are any corrupt frames)
	anim->nframes = i;
	//copy audio to single buffer
	if (anim->audio.datasize > 0) {
#ifdef THECLOU_DEBUG_ALLOC
		anim->audio.data = (char *)MemAllocDbg(anim->audio.datasize, __FILE__, "IffAnim_ReadFrames 6");
#else
		anim->audio.data = (char *)MemAlloc(anim->audio.datasize);
#endif
		ptr = anim->audio.data;
#ifdef THECLOU_DEBUG_ALLOC
		anim->audio.dataoffset = (int *)MemAllocDbg(sizeof(int) * anim->audio.n, __FILE__, "IffAnim_ReadFrames 7");
#else
		anim->audio.dataoffset = (int *)MemAlloc(sizeof(int) * anim->audio.n);
#endif
		sync = 0;
		for (l = 0; l < anim->audio.n; l++) {
			anim->audio.dataoffset[l] = sync;          //set audio start byte for current video frame
			memcpy(ptr, tabuf[l], tabufsize[l]); //copy to single data buffer
			MemFree(tabuf[l], tabufsize[l]);
			ptr += tabufsize[l];
			sync += tabufsize[l];
		}
		MemFree(tabuf, sizeof(char *) * anim->audio.n);
		MemFree(tabufsize, sizeof(int) * anim->audio.n);
	}
	if (!i) {
		return(FALSE);
	}
	return(TRUE);
}

static int IffAnim_PrintInfo(IffAnim *anim)
{
	int i, n, t;
	char buffer[IFFANIM_FORMATINFO_BUFSIZE];
	
	sprintf(anim->formatinfo,
		"number of frames: %d\n"
		"width: %d\n"
		"height: %d\n"
		"bits per pixel (planar): %d\n"
		"bits per pixel (chunky): %d\n"
		"mask: %d\n"
		"HAM: %s\n"
		"EHB: %s\n"
		"total time in 1/60 sec: %d\n",
		anim->nframes, anim->w, anim->h, anim->bpp, anim->disp_bpp, anim->mask,
		anim->ham ? "yes" : "no",
		anim->ehb ? "yes" : "no",
		anim->lentime);

	//info about compressions
	strcat(anim->formatinfo, "compressions: ");
	if (anim->compressed) {
		strcat(anim->formatinfo, "RLE,");
	}
	n = 0;
	for (i = 0; i < 256; i++) {  //list all delta compression modes
		if (anim->dcompressions[i]) {
			n++;
			if (n == 1) {
				sprintf(buffer," %d", i);
			} else {
				sprintf(buffer,", %d", i);
			}
			strcat(anim->formatinfo, buffer);
		}
	}
	strcat(anim->formatinfo, "\n");

	if (IffAnim_GetAudioFormat(anim, NULL, NULL, NULL)) {
		strcat(anim->formatinfo, "audio format:\n");
		sprintf(buffer,
			" channels: %d\n"
			" bits per sample: %d\n"
			" sample rate: %d\n",
			anim->audio.nch, anim->audio.bps, anim->audio.freq);
		strcat(anim->formatinfo, buffer);
		if ((anim->audio.nch != 0) && (anim->audio.bps != 0)) {  //prevent division with 0
			t = anim->audio.datasize / anim->audio.nch * 8 / anim->audio.bps;
		} else {
			t = 0;
		}
		sprintf(buffer, " number of sample frames: %d\n", t);
		strcat(anim->formatinfo, buffer);
	}
	return(TRUE);
}

int IffAnim_SetLoopAnim(IffAnim *anim, int state)
{
	if (state && (anim->nframes >= 4)) {
		anim->loopanim = TRUE;
	} else {
		anim->loopanim = FALSE;
	}
	return(anim->loopanim);
}

char *IffAnim_GetInfoText(IffAnim *anim)
{
	return(anim->formatinfo);
}

// decode frame from frame List, decide which decoding function to call
//  "dstframe" : bitplanar frame buffer, each plane per line padded to multiple of 16 bit
//  "index"    : frame number
static int IffAnim_DecodeFrame(IffAnim *anim, char *dstframe, int index)
{
	if (index > anim->nframes) {
		return(FALSE);
	}
	//decode frame
	switch (anim->frame[index].delta_compression) {
		//uncompressed
		case 0:
			memcpy(dstframe, anim->frame[index].data, anim->frame[index].datasize);
			return(TRUE);
		break;
		//Byte vertical delta compression
		case 5:
			return(IffAnim_DecodeByteVerticalDelta(anim, dstframe, anim->frame[index].data, anim->w, anim->bpp));
		break;
		//Short/Long vertical delta method 7
		case 7:
			return(IffAnim_DecodeLSVerticalDelta7(anim, dstframe, anim->frame[index].data, anim->w, anim->bpp, (anim->frame[index].bits & 0x1) ? TRUE : FALSE));
		break;
		//Short/Long vertical delta method 8
		case 8:
			return(IffAnim_DecodeLSVerticalDelta8(anim, dstframe, anim->frame[index].data, anim->w, anim->bpp, (anim->frame[index].bits & 0x1) ? TRUE : FALSE));
		break;
		case 74:
			return(IffAnim_DecodeDeltaJ(anim, dstframe, anim->frame[index].data, anim->w, anim->h, anim->bpp));
		break;
	}
	return(FALSE);
}

char *IffAnim_GetFramePlanar(IffAnim *anim, int *framesize_)
{
	if (framesize_) {
		*framesize_ = anim->framesize;
	}
	return(anim->curframe);
}

void *IffAnim_GetFrame(IffAnim *anim)
{
	return(anim->disp_frame);
}

void *IffAnim_GetCmap(IffAnim *anim)
{
	return(anim->disp_cmap);
}

void *IffAnim_GetPrevFrame(IffAnim *anim)
{
	return(anim->prev_disp_frame);
}

void *IffAnim_GetPrevCmap(IffAnim *anim)
{
	return(anim->prev_disp_cmap);
}

int IffAnim_Reset(IffAnim *anim)
{
	anim->frameno = -1;   //so next frame is 0
	if (anim->frame[0].delta_compression) {  //set to black, if frame 0 has delta compression
		memset(anim->prevframe, 0, anim->framesize);  //the next frame will be decoded to "prevframe"
	}
	IffAnim_NextFrame(anim);    //decompress, increment internal counter (swaps curframe <-> prevframe buffers)
	//make prevframe and curframe the same
	memcpy(anim->prevframe, anim->curframe, anim->framesize);
	anim->prevcmap = anim->curcmap;
	return(TRUE);
}

int IffAnim_NextFrame(IffAnim *anim)
{
	char *temp;
	if ((anim->frameno + 1) >= anim->nframes) { //if last frame
		if (!anim->loop) {   //abort, do nothing (display frame remains)
			return(FALSE);
		} else {                      //handle looping
			if (anim->loopanim && (anim->nframes >= 4)) {          //continue at frame 2 (skip the first 2)
				anim->frameno = 1;
			} else {
				return(IffAnim_Reset(anim));    // loads the first frame
			}
		}
	}
	anim->frameno++;
	//decompress to prevframe
	IffAnim_DecodeFrame(anim, anim->prevframe, anim->frameno);
	//get cmap pointer
	if ((anim->bpp <= 8) && anim->frame[anim->frameno].cmap) {
		anim->prevcmap = anim->frame[anim->frameno].cmap;
	}
	//swap frame buffer pointers
	temp = anim->curframe;
	anim->curframe = anim->prevframe;
	anim->prevframe = temp;
	//swap cmap
	temp = anim->curcmap;
	anim->curcmap = anim->prevcmap;
	anim->prevcmap = temp;
	return(TRUE);
}

int IffAnim_ConvertFrame(IffAnim *anim)
{
	char *t;
	int ncolors;
	int i;
	char *src;
	char *dst;

	//swap pointers
	t = anim->disp_frame;
	anim->disp_frame = anim->prev_disp_frame;
	anim->prev_disp_frame = t;
	t = anim->disp_cmap;
	anim->disp_cmap = anim->prev_disp_cmap;
	anim->prev_disp_cmap = t;
	//convert multi to single planar display format (including bpp conversion)
	if (anim->ham) {
		IffAnim_ConvertHamTo24bpp(anim, anim->disp_frame, anim->curframe, anim->curcmap, anim->w, anim->h, anim->bpp, anim->disp_pitch);
	} else {
		IffAnim_BitplanarToChunky(anim, anim->disp_frame, anim->curframe, anim->w, anim->h, anim->bpp, anim->disp_bpp, anim->disp_pitch);
	}
	//convert cmap: R,G,B  to  R,G,B,0
	ncolors = 1 << anim->bpp;
	src = anim->curcmap;
	dst = anim->disp_cmap;
	if (anim->disp_bpp <= 8) {
		for (i = 0; i < ncolors ;i++) {
			memcpy(dst, src,3);
			src += 3;
			dst += 4;
		}
	}
	anim->num_disp_frames++;
	return(TRUE);
}

int IffAnim_GetInfo(IffAnim *anim, int *w_, int *h_, int *bpp_, int *pitch_, int *nframes_, int *mslentime_)
{
	if (w_) {
		*w_ = anim->w;
	}
	if (h_) {
		*h_ = anim->h;
	}
	if (bpp_) {
		*bpp_ = anim->disp_bpp;
	}
	if (nframes_) {
		if (anim->loopanim && (anim->nframes >= 4)) {
			*nframes_ = anim->nframes - 2;
		} else {
			*nframes_ = anim->nframes;
		}
	}
	if (pitch_) {
		*pitch_ = anim->disp_pitch; 
	}
	if (mslentime_) {
		if (anim->loopanim && (anim->nframes >= 4)) {
			*mslentime_ = (anim->lentime - anim->frame[0].reltime - anim->frame[1].reltime) * 1000 / 60;
		} else {
			*mslentime_ = anim->lentime * 1000 / 60;
		}
	}
	return(TRUE);
}

int IffAnim_GetDelayTime(IffAnim *anim)
{
	return(anim->frame[anim->frameno].reltime * 1000 / 60);
}

int IffAnim_GetDelayTimeOriginal(IffAnim *anim)
{
	return(anim->frame[anim->frameno].reltime);
}

int IffAnim_SetLoop(IffAnim *anim, int state)
{
	anim->loop = state ? TRUE : FALSE;
	return(anim->loop);
}

int IffAnim_GetAudioFormat(IffAnim *anim, int *nch, int *bps, int *freq)
{
	if (anim->audio.nch <= 0) {
		return(FALSE);
	}
	if (nch) {
		*nch = anim->audio.nch;
	}
	if (bps) {
		*bps = anim->audio.bps;
	}
	if (freq) {
		*freq = anim->audio.freq;
	}
	return(TRUE);
}

char *IffAnim_GetAudioData(IffAnim *anim, int *size)
{
	if (size) {
		*size = anim->audio.datasize;
	}
	return(anim->audio.data);
}

int IffAnim_GetAudioOffset(IffAnim *anim, int index)
{
	if ((!anim->audio.dataoffset) || (index >= anim->audio.n) || (index < 0)) {
		return(0);
	}
	return(anim->audio.dataoffset[index]);
}

int IffAnim_GetAudioOffsetMS(IffAnim *anim, int index, int msoffs)
{
	int bpsf = anim->audio.bps * anim->audio.nch;
	return(IffAnim_GetAudioOffset(anim, index) + (msoffs * anim->audio.freq * bpsf / 1000));
}

int IffAnim_GetAudioFrameSize(IffAnim *anim, int index)
{
	if ((!anim->audio.dataoffset) || (index >= anim->audio.n)) {
		return(0);
	}
	if ((index + 1) >= anim->audio.n) {
		return(anim->audio.datasize - anim->audio.dataoffset[index]);
	}
	return(anim->audio.dataoffset[index + 1] - anim->audio.dataoffset[index]);
}
