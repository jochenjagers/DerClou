/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "gfx\filter.h"
#include "gfx\gfx.h"
#include <string.h>

unsigned char *gfxFilter_x4_Buffer = NULL;

void gfxFilter_x1_Copy(	void *srcmem,
						long srcw,
						long srch,
						void *dstmem,
						long dstpitch)
{
	unsigned char *src, *dst;
	long y;

	src = (unsigned char *)srcmem;
	dst = (unsigned char *)dstmem;
	if (srcw == dstpitch) {
		memcpy(dst, src, srcw * srch);
	} else {
		for (y = 0; y < srch; y++) {
			memcpy(dst, src, srcw);
			src += srcw;
			dst += dstpitch;
		}
	}
}

void gfxFilter_x2_Copy(	void *srcmem,
						long srcw,
						long srch,
						void *dstmem,
						long dstpitch)
{
	unsigned char *src, *dst;
	long x, x2, x2d, y;

	src = (unsigned char *)srcmem;
	dst = (unsigned char *)dstmem;
	for (y = 0; y < srch; y++) {
		for (x = 0; x < srcw; x++) {
			x2 = x<<1;
			x2d = x2+dstpitch;
			dst[x2] = src[x];
			dst[x2+1] = src[x];
			dst[x2d] = src[x];
			dst[x2d+1] = src[x];
		}
		src += srcw;
		dst += dstpitch << 1;
	}
}

void gfxFilter_x3_Copy(	void *srcmem,
						long srcw,
						long srch,
						void *dstmem,
						long dstpitch)
{
	unsigned char *src, *dst;
	long x, y, x3, x3d, x3d2, d2, d3;

	d2 = dstpitch << 1;
	d3 = dstpitch * 3;
	src = (unsigned char *)srcmem;
	dst = (unsigned char *)dstmem;
	for (y = 0; y < srch; y++) {
		for (x = 0; x < srcw; x++) {
			x3 = x * 3;
			x3d = x3+dstpitch;
			x3d2 = x3+d2;

			dst[x3] = src[x];
			dst[x3+1] = src[x];
			dst[x3+2] = src[x];

			dst[x3d] = src[x];
			dst[x3d+1] = src[x];
			dst[x3d+2] = src[x];

			dst[x3d2] = src[x];
			dst[x3d2+1] = src[x];
			dst[x3d2+2] = src[x];
		}
		src += srcw;
		dst += d3;
	}
}

void gfxFilter_x4_Copy(	void *srcmem,
						long srcw,
						long srch,
						void *dstmem,
						long dstpitch)
{
	unsigned char *src, *dst;
	long x, y, x4, d2, d3, d4, x4d, x4d2, x4d3;

	d2 = dstpitch << 1;
	d3 = dstpitch * 3;
	d4 = dstpitch << 2;
	src = (unsigned char *)srcmem;
	dst = (unsigned char *)dstmem;
	for (y = 0; y < srch; y++) {
		for (x = 0; x < srcw; x++) {
			x4 = x << 2;
			x4d = x4+dstpitch;
			x4d2 = x4+d2;
			x4d3 = x4+d3;

			dst[x4] = src[x];
			dst[x4+1] = src[x];
			dst[x4+2] = src[x];
			dst[x4+3] = src[x];

			dst[x4d] = src[x];
			dst[x4d+1] = src[x];
			dst[x4d+2] = src[x];
			dst[x4d+3] = src[x];

			dst[x4d2] = src[x];
			dst[x4d2+1] = src[x];
			dst[x4d2+2] = src[x];
			dst[x4d2+3] = src[x];

			dst[x4d3] = src[x];
			dst[x4d3+1] = src[x];
			dst[x4d3+2] = src[x];
			dst[x4d3+3] = src[x];
		}
		src += srcw;
		dst += d4;
	}
}

void gfxFilter_x2_Scale2x(	void *srcmem,
							long srcw,
							long srch,
							void *dstmem,
							long dstpitch)
{
	unsigned long loopw, looph, loopw2, looph2, ts = 0, td = 0;
	unsigned char B, D, E, F, H;
	unsigned char *src, *dst;

	src = (unsigned char *)srcmem;
	dst = (unsigned char *)dstmem;
	for (looph = 0, looph2 = 0; looph < srch; ++looph, looph2 += 2) {
		for (loopw = 0, loopw2 = 0; loopw < srcw; ++loopw, loopw2 += 2) {
			/*
			+-+-+-+
			|A|B|C|
			|-+-+-|
			|D|E|F|
			|-+-+-|
			|G|H|I|
			+-+-+-+
			*/
			B = src[(looph ? (ts - srcw) : 0) + loopw];
			D = src[ts + (loopw ? (loopw - 1) : 0)];
			E = src[ts + loopw];
			F = src[ts + (loopw < (srcw-2) ? (loopw + 1) : (srcw-1))];
			H = src[(looph < (srch-2) ? (ts + srcw) : ((srch-1)*srcw)) + loopw];
			/*
			+--+--+
			|E0|E1|
			+--+--+
			|E2|E3|
			+--+--+
			*/
			if ((B != H) && (D != F)) {
				dst[td + loopw2] = (D == B) ? D : E;
				dst[td + loopw2 + 1] = (B == F) ? F : E;
				dst[td + dstpitch + loopw2] = (D == H) ? D : E;
				dst[td + dstpitch + loopw2 + 1] = (H == F) ? F : E;
			} else {
				dst[td + loopw2] = E;
				dst[td + loopw2 + 1] = E;
				dst[td + dstpitch + loopw2] = E;
				dst[td + dstpitch + loopw2 + 1] = E;
			}
		}
		ts += srcw;
		td += dstpitch<<1;
	}
}

void gfxFilter_x3_Scale3x(	void *srcmem,
							long srcw,
							long srch,
							void *dstmem,
							long dstpitch)
{
	unsigned long loopw, looph, loopw3, looph3, ts = 0, td = 0;
	unsigned char A, B, C, D, E, F, G, H, I;
	unsigned char *src, *dst;
	long dstpitch2 = dstpitch << 1;

	src = (unsigned char *)srcmem;
	dst = (unsigned char *)dstmem;
	for (looph = 0, looph3 = 0; looph < srch; ++looph, looph3 += 3) {
		for (loopw = 0, loopw3 = 0; loopw < srcw; ++loopw, loopw3 += 3) {
			/*
			+-+-+-+
			|A|B|C|
			|-+-+-|
			|D|E|F|
			|-+-+-|
			|G|H|I|
			+-+-+-+
			*/
			A = src[(looph ? (ts - srcw) : 0) + (loopw ? (loopw - 1) : 0)];		// ??
			B = src[(looph ? (ts - srcw) : 0) + loopw];
			C = src[(looph ? (ts - srcw) : 0) + (loopw < (srcw-2) ? (loopw + 1) : (srcw-1))];		// ??

			D = src[ts + (loopw ? (loopw - 1) : 0)];
			E = src[ts + loopw];
			F = src[ts + (loopw < (srcw-2) ? (loopw + 1) : (srcw-1))];

			G = src[(looph < (srch-2) ? (ts + srcw) : ((srch-1)*srcw)) + (loopw ? (loopw - 1) : 0)];	// ??
			H = src[(looph < (srch-2) ? (ts + srcw) : ((srch-1)*srcw)) + loopw];
			I = src[(looph < (srch-2) ? (ts + srcw) : ((srch-1)*srcw)) + (loopw < (srcw-2) ? (loopw + 1) : (srcw-1))];	// ??

			/*
			+--+--+--+
			|E0|E1|E2|
			+--+--+--+
			|E3|E4|E5|
			+--+--+--+
			|E6|E7|E8|
			+--+--+--+
			*/
			if ((B != H) && (D != F)) {
				dst[td + loopw3] = D == B ? D : E;
				dst[td + loopw3 + 1] = (D == B && E != C) || (B == F && E != A) ? B : E;
				dst[td + loopw3 + 2] = B == F ? F : E;
				dst[td + dstpitch + loopw3] = (D == B && E != G) || (D == H && E != A) ? D : E;
				dst[td + dstpitch + loopw3 + 1] = E;
				dst[td + dstpitch + loopw3 + 2] = (B == F && E != I) || (H == F && E != C) ? F : E;
				dst[td + dstpitch2 + loopw3] = D == H ? D : E;
				dst[td + dstpitch2 + loopw3 + 1] = (D == H && E != I) || (H == F && E != G) ? H : E;
				dst[td + dstpitch2 + loopw3 + 2] = H == F ? F : E;
			} else {
				dst[td + loopw3] = E;
				dst[td + loopw3 + 1] = E;
				dst[td + loopw3 + 2] = E;
				dst[td + dstpitch + loopw3] = E;
				dst[td + dstpitch + loopw3 + 1] = E;
				dst[td + dstpitch + loopw3 + 2] = E;
				dst[td + dstpitch2 + loopw3] = E;
				dst[td + dstpitch2 + loopw3 + 1] = E;
				dst[td + dstpitch2 + loopw3 + 2] = E;
			}
		}
		ts += srcw;
		td += dstpitch*3;
	}
}

void gfxFilter_x4_Scale4x(	void *srcmem,
							long srcw,
							long srch,
							void *dstmem,
							long dstpitch)
{
	gfxFilter_x2_Scale2x(srcmem, srcw, srch, gfxFilter_x4_Buffer, 640);
	gfxFilter_x2_Scale2x(gfxFilter_x4_Buffer, 640, 400, dstmem, dstpitch);
}
