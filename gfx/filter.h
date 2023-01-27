/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef __filter_h__
#define __filter_h__

extern unsigned char *gfxFilter_x4_Buffer;

typedef void (*gfxFilterFunc)(	void *srcmem,
								long srcw,
								long srch,
								void *dstmem,
								long dstpitch);

extern void gfxFilter_x1_Copy(	void *srcmem,
								long srcw,
								long srch,
								void *dstmem,
								long dstpitch);

extern void gfxFilter_x2_Copy(	void *srcmem,
								long srcw,
								long srch,
								void *dstmem,
								long dstpitch);

extern void gfxFilter_x3_Copy(	void *srcmem,
								long srcw,
								long srch,
								void *dstmem,
								long dstpitch);

extern void gfxFilter_x4_Copy(	void *srcmem,
								long srcw,
								long srch,
								void *dstmem,
								long dstpitch);
								
extern void gfxFilter_x2_Scale2x(	void *srcmem,
									long srcw,
									long srch,
									void *dstmem,
									long dstpitch);

extern void gfxFilter_x3_Scale3x(	void *srcmem,
									long srcw,
									long srch,
									void *dstmem,
									long dstpitch);

extern void gfxFilter_x4_Scale4x(	void *srcmem,
									long srcw,
									long srch,
									void *dstmem,
									long dstpitch);

#endif /* __filter_h__ */
