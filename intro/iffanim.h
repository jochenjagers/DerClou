/*
  IFF ANIM decoder by Markus Wolf http://murkymind.de (v1.01 2007-03-30)
  Ported and adapted by Oliver Gantert (2014-06-28)
*/
#ifndef IFFANIM_H
#define IFFANIM_H

/*
Internal frame Management:
-----------------
 The Animation data is read to memory completely, into a frame list:
   -> frames are decoded faster (delta compression is kept in memory)
   -> verification of data before playing -> no file reading errors during play (except compression errors)
   - I haven't seen an iff animation with more than 10MB, so this shouldn't be a problem.

 Frame data:
   There is the "current frame" and the "previous frame" in bitplanar format representing a kind of double buffer
   When a frame is decoded, the delta frame information modifies the previous frame always. After that the "previous frame"
   is swapped with the "current frame".
   The "current frame" can be converted to a useful output format (chunky), to a display suitable frame
   When opening a file, the first bit planar frame is decoded (frame 0).
   A call to "NextFrame" decodes the next frame (frame 1).


Output formats
--------------
The old bitplanar format isn't useable for today's graphic systems, so the animation frames can be converted to chunky formats.
 - 1..8 bit are converted to 8 bit, with a "r,g,b,0" palette (byte order exactly as mentioned)
 - HAM and 24 bit frames are converted to 24 bit "r,g,b" images (byte order as mentioned)
 - pitch for a scanline can be defined


Notes for a player:
---------------------
 While waiting the delay of a frame, it is useful to decode the next frame already.
 Add the used time for the decoding to your "already waited time".
 You'll need a timer with a resolution of at least 1/60 seconds.


File format support:
--------------------

ANIM file is currently only supported in it's standart structure:
 - I haven't seen any other structure so far

 FORM ANIM
  FORM ILBM        first frame (frame 0)
    BMHD             normal type IFF data
    ANHD             optional animation header (chunk for timing of 1st frame)
    ...            
    CMAP             optional cmap
    BODY             normal iff pixel data
  FORM ILBM        frame 1
    ANHD             animation header chunk
    DLTA             delta mode data
  FORM ILBM        frame 2
    ANHD
    DLTA
    ....

known delta compression modes (current decoding support is marked with '*'):
  - ANIM-0 * ILBM BODY (no delta compression)
  - ANIM-1   ILBM XOR
  - ANIM-2   Long Delta mode
  - ANIM-3   Short Delta mode
  - ANIM-4   General Delta mode
  - ANIM-5 * Byte Vertical Delta mode (most common)
  - ANIM-6   Stereo Byte Delta mode
  - ANIM-7 * Anim-5 compression using LONG/WORD data
  - ANIM-8 * Anim-5 compression using LONG/WORD data
  - ANIM-J * Eric Grahams compression format (Sculpt 3D / Sculpt 4D, by "Byte by Byte")
  
 "dctv" animations aren't supported, due to lacking format information.
*/

typedef struct IffAnim IffAnim;

extern IffAnim *IffAnim_Open(char *fname);
extern void IffAnim_Close(IffAnim *anim);
extern int IffAnim_CurrentFrameIndex(IffAnim *anim);
extern int IffAnim_SetLoopAnim(IffAnim *anim, int state);
extern char *IffAnim_GetInfoText(IffAnim *anim);
extern char *IffAnim_GetFramePlanar(IffAnim *anim, int *framesize_);
extern void *IffAnim_GetFrame(IffAnim *anim);
extern void *IffAnim_GetCmap(IffAnim *anim);
extern void *IffAnim_GetPrevFrame(IffAnim *anim);
extern void *IffAnim_GetPrevCmap(IffAnim *anim);
extern int IffAnim_Reset(IffAnim *anim);
extern int IffAnim_NextFrame(IffAnim *anim);
extern int IffAnim_ConvertFrame(IffAnim *anim);
extern int IffAnim_GetInfo(IffAnim *anim, int *w_, int *h_, int *bpp_, int *pitch_, int *nframes_, int *mslentime_);
extern int IffAnim_GetDelayTime(IffAnim *anim);
extern int IffAnim_GetDelayTimeOriginal(IffAnim *anim);
extern int IffAnim_SetLoop(IffAnim *anim, int state);
extern int IffAnim_GetAudioFormat(IffAnim *anim, int *nch, int *bps, int *freq);
extern char *IffAnim_GetAudioData(IffAnim *anim, int *size);
extern int IffAnim_GetAudioOffset(IffAnim *anim, int index);
extern int IffAnim_GetAudioOffsetMS(IffAnim *anim, int index, int msoffs);
extern int IffAnim_GetAudioFrameSize(IffAnim *anim, int index);

#endif /* IFFANIM_H */
