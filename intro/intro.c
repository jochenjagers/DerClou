/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "SDL.h"
#include "base\base.h"
#include "intro\intro.h"
#include "intro\iffanim.h"
#include "disk\disk.h"
#include "gfx\gfx.h"
#include "inphdl\inphdl.h"

#define NUM_ANIM_FILES	5
static const char *anim_files[NUM_ANIM_FILES] = {
	"AN1_1.ANM",
	"AN2_4.ANM",
	"AN3_11.ANM",
	"AN4_11.ANM",
	"AN5_11.ANM"
};
static const int anim_delays[NUM_ANIM_FILES] = {
	/* speed of animation expressed as delay between frames */
	/* in ticks (1 tick = 1/60 second) */
	60,
	17,
	7,
	7,
	7
};
#define NUM_ANIM_SYNC	6
static const int anim_sync[NUM_ANIM_SYNC*2] = {
	/* anim #, frame # */
	/* when to play the church bell sound */
	0,1,
	0,5,
	1,2,
	1,27,
	1,39,
	1,53
};
#define NUM_CD_TRACKS	24
static const int anim_cdframes[NUM_CD_TRACKS*6] = {
	2,	100,	0,	 3,	0,		0,
	2,	143,	0,	 4,	0,		0,
	3,	  5,	0,	23,	0,		0,
	3,	 74,	0,	 8,	0,		0,
	3,	 90,	0,	24,	0,		0,
	3,	118,	0,	 9,	0,		0,
	3,	137,	0,	14,	0,		0,
	3,	177,	0,	19,	0,		0,
	3,	205,	0,	10,	0,		0,
	3,	222,	0,	15,	0,		0,
	3,	242,	0,	20,	0,		0,
	3,	260,	0,	16,	0,		0,
	3,	293,	0,	21,	0,		0,
	3,	327,	0,	11,	0,		0,
	3,	344,	0,	17,	0,		0,
	3,	375,	0,	 5,	0,		0,
	3,	427,	1,	12,	0,		2*75,
	3,	445,	1,	12,	3*75,	5*75,
	3,	467,	1,	12,	8*75,	11*75,
	4,	  5,	0,	18,	0,		0,
	4,	 44,	0,	22,	0,		0,
	4,	 66,	0,	 6,	0,		0,
	4,	115,	0,	13,	0,		0,
	4,	153,	0,	 7,	0,		0
};

static int Anim_Play(IffAnim *anim, int nfile, int delayticks)
{
	ubyte *cmap, *frame;
	ubyte *dst;
	int q;
	int a;
	int action;
	int numframes, nframe;

	int ntrack;
	char wavPath[256];
	char wavName[16];

	/* start music with anim #3 */
	if (nfile == 2) {
		sndPlaySound("title.bk", 0);
	}

	IffAnim_SetLoop(anim, 0);
	IffAnim_SetLoopAnim(anim, 0);

	dst = gfxGetGfxBoardBase();

	inpSetWaitTicks(delayticks);
	q = 0;

	numframes = 0;
	IffAnim_GetInfo(anim, NULL, NULL, NULL, NULL, &numframes, NULL);

	/* don't play last 2 frames, as they're identical to the
	   first 2 frames (at least in looping animations) */
	for (nframe = 1; nframe < numframes - 1; nframe++) {

		if (IffAnim_NextFrame(anim)) {
			IffAnim_ConvertFrame(anim);
			frame = (ubyte *)IffAnim_GetFrame(anim);
			if (!frame) break;

			cmap = (ubyte *)IffAnim_GetCmap(anim);
			if (cmap) {
				gfxSetPalette32(NULL, 0, 255, cmap);
			}

			memcpy(dst, frame, 320*200);
			gfxInvalidate();

			for (a = 0; a < NUM_ANIM_SYNC; a++) {
				if ((nfile == anim_sync[(a<<1)]) && (nframe == anim_sync[(a<<1)+1])) {
					sndPlayFX("church.voc");
				}
			}

			if (bCDRom) {
				if (CDRomInstalled) {
					for (a = 0; a < NUM_CD_TRACKS; a++) {
						if ((nfile == anim_cdframes[a*6]) && (nframe == anim_cdframes[(a*6)+1])) {
							sndFading(16);
							CDROM_StopAudioTrack();

							if (!anim_cdframes[(a*6)+2]) {
								CDROM_PlayAudioTrack(anim_cdframes[(a*6)+3]);
							} else {
								CDROM_PlayAudioSequence(anim_cdframes[(a*6)+3], anim_cdframes[(a*6)+4], anim_cdframes[(a*6)+5]);
							}

							break;
						}
					}
				} else {
					for (a = 0; a < NUM_CD_TRACKS; a++) {
						if ((nfile == anim_cdframes[a*6]) && (nframe == anim_cdframes[(a*6)+1])) {
							ntrack = 0;
							if (!anim_cdframes[(a*6)+2]) {
								ntrack = anim_cdframes[(a*6)+3];
							} else {
								/* PlaySequence not supported for WAV */
								/* "intro_11.wav" is just played as a whole */
								if (!anim_cdframes[(a*6)+4]) {
									ntrack = anim_cdframes[(a*6)+3];
								}
							}
							if (ntrack) {
								sprintf(wavName, "intro_%02d.wav", ntrack - 1);
								dskBuildPathName(AUDIO_DIRECTORY, wavName, wavPath);

								sndFading(16);

								MXR_SetInput(pAudioMixer, MXR_INPUT_VOICE, MXR_CreateInputWAV(wavPath));
							}

							break;
						}
					}
				}
			}

			while (1) {
				action = inpWaitFor(INP_TIME|INP_ESC|INP_QUIT);
				if (action & (INP_ESC|INP_QUIT)) {
					q = 1;
					break;
				}
				if (action & INP_TIME) {
					break;	// next frame
				}
			}
			if (q) {
				return(0);	// break intro
			}
		}
	}
	return(1);	// next anim
}

void tcIntro(void)
{
	char anim_path[DSK_PATHNAMELENGTH];
	int n_file, bContinue;
	IffAnim *iff_anim;

	inpTurnESC(1);
	gfxSetVideoMode(GFX_VIDEO_MCGA);

	for (n_file = 0; n_file < NUM_ANIM_FILES; n_file++) {
		dskBuildPathName("intropix", anim_files[n_file], anim_path);
		iff_anim = IffAnim_Open(anim_path);
		if (iff_anim) {
			bContinue = Anim_Play(iff_anim, n_file, anim_delays[n_file]);
			IffAnim_Close(iff_anim);
		} else {
			Log("tcIntro failed to open file: %s", anim_path);
		}
		if (!bContinue) {
			break;
		}
	}

	if (bCDRom && CDRomInstalled) {
		CDROM_StopAudioTrack();
		sndFading(0);
	}

	memset(gfxGetGfxBoardBase(), 0, 320*200);
	gfxInvalidate();
}
