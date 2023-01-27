/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef __scale2x_h__
#define __scale2x_h__

/*
 * Scale2x implementation.
 * This is an implementation of the Scale2x algorithm, originally
 * invented by the AdvanceMAME team.
 * A 'template' macro is used to produce an optimized function with
 * hard-coded dimensions.
 */

#define FUNCTION_SCALE2X(name,type,srcw,srch) \
	void name(const type *srcpix, type *dstpix) { \
		unsigned long loopw, looph, loopw2, looph2, ts = 0, td = 0; \
		type B, D, E, F, H; \
		for (looph = 0, looph2 = 0; looph < srch; ++looph, looph2 += 2) { \
			for (loopw = 0, loopw2 = 0; loopw < srcw; ++loopw, loopw2 += 2) { \
				B = srcpix[(looph ? ts - srcw : 0) + loopw]; \
				D = srcpix[ts + (loopw ? loopw - 1 : 0)]; \
				E = srcpix[ts + loopw]; \
				F = srcpix[ts + (loopw < (srcw-2) ? loopw + 1 : (srcw-1))]; \
				H = srcpix[(looph < (srch-2) ? ts + srcw : (srch-1)*srcw) + loopw]; \
				dstpix[td + loopw2] = D == B && B != F && D != H ? D : E; \
				dstpix[td + loopw2 + 1] = B == F && B != D && F != H ? F : E; \
				dstpix[td + (srcw<<1) + loopw2] = D == H && D != B && H != F ? D : E; \
				dstpix[td + (srcw<<1) + loopw2 + 1] = H == F && D != H && B != F ? F : E; \
			} \
			ts += srcw; \
			td += srcw<<2; \
		} \
	}

#define FUNCTION_SCALE2X_OFFS(name,type,srcw,rectw,recth) \
	void name(const type *srcpix, type *dstpix) { \
		unsigned long loopw, looph, loopw2, looph2, ts = 0, td = 0; \
		type B, D, E, F, H; \
		for (looph = 0, looph2 = 0; looph < recth; ++looph, looph2 += 2) { \
			for (loopw = 0, loopw2 = 0; loopw < rectw; ++loopw, loopw2 += 2) { \
				B = srcpix[(looph ? ts - rectw : 0) + loopw]; \
				D = srcpix[ts + (loopw ? loopw - 1 : 0)]; \
				E = srcpix[ts + loopw]; \
				F = srcpix[ts + (loopw < (rectw-2) ? loopw + 1 : (rectw-1))]; \
				H = srcpix[(looph < (recth-2) ? ts + rectw : (recth-1)*rectw) + loopw]; \
				dstpix[td + loopw2] = D == B && B != F && D != H ? D : E; \
				dstpix[td + loopw2 + 1] = B == F && B != D && F != H ? F : E; \
				dstpix[td + (rectw<<1) + loopw2] = D == H && D != B && H != F ? D : E; \
				dstpix[td + (rectw<<1) + loopw2 + 1] = H == F && D != H && B != F ? F : E; \
			} \
			ts += srcw; \
			td += rectw<<2; \
		} \
	}

#endif /* __scale2x_h__ */
