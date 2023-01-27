/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "SDL.h"
#include "sound\fx.h"

struct FXBase FXBase = { NULL, NULL, 0, 0, 0, 0 };

static SDL_AudioSpec audioSpec;

int InitSBlaster(void)
{
	return(SDL_InitSubSystem(SDL_INIT_AUDIO));
}

void RemoveSBlast(void)
{
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void sndInitFX(void)
{
	int RetVal;
	ubyte fileName[TXT_KEY_LENGTH];

	FXBase.ul_MaxSize = 64<<10;
	FXBase.p_SoundBuffer = MemAlloc(FXBase.ul_MaxSize);
}

void sndDoneFX(void)
{
	RemoveSBlast();

	if (FXBase.p_SoundBuffer)
		MemFree(FXBase.p_SoundBuffer, FXBase.ul_MaxSize);

	FXBase.p_SoundBuffer 	 = NULL;
	FXBase.us_SoundBlasterOk = 0;
}

#define VOC_FREQ(SR)	(1000000L/(256L-(ulong)(SR)))

static ubyte *GetVocData(ubyte *buffer, ulong *datasize)
{
	ubyte *ptr = buffer;

	if (!strncmp(ptr, "Creative Voice File", 19) &&
		(ptr[0x13] == 0x1A) &&
		(ptr[0x16] == 0x0A) &&
		(ptr[0x17] == 0x01) )
	{
		uword temp = (uword)ptr[0x14] | ((uword)ptr[0x15] << 8);
		ptr += temp;
		ulong size;
		ubyte type;

		while (type = ptr[0])
		{
			size = (ulong)ptr[1] | ((ulong)ptr[2] << 8) | ((ulong)ptr[3] << 16);
			ptr += 4;
			switch (type)
			{
				case 0x01:
					audioSpec.freq = VOC_FREQ(ptr[0]);
					audioSpec.format = AUDIO_U8;
					audioSpec.channels = 1;
					*datasize = size - 2;
					ptr += 2;
					return(ptr);
				break;
				default:
					ptr += size;
				break;
			}
		}
	}

	return(NULL);
}

/**
 * The AudioCallback runs in a seperate thread and is
 * called by the SDL audio system when it needs more data.
 */
static void AudioCallback(void *userdata, Uint8 *stream, int len)
{
	if (FXBase.ul_DataPlayed >= FXBase.ul_DataSize) return;

	ulong ul_StillToPlay = FXBase.ul_DataSize - FXBase.ul_DataPlayed;
	len = (len > ul_StillToPlay ? ul_StillToPlay : len);

	memcpy(stream, &FXBase.p_Data[FXBase.ul_DataPlayed], len);

	FXBase.ul_DataPlayed += len;
}

void sndPrepareFX(ubyte *puch_Name)
{
	ubyte fileName[TXT_KEY_LENGTH];
	long i;

	if (FXBase.p_SoundBuffer && FXBase.us_SoundBlasterOk)
	{
		dskBuildPathName(SAMPLES_DIRECTORY, puch_Name, fileName);
		dskLoad(fileName, FXBase.p_SoundBuffer, 0);
		FXBase.p_Data = GetVocData((ubyte*)FXBase.p_SoundBuffer, &FXBase.ul_DataSize);
	}
}

void sndPlayFX(void)
{
	if (FXBase.p_SoundBuffer && FXBase.p_Data && FXBase.us_SoundBlasterOk)
	{
		audioSpec.samples = 4096;	// recommended (SDL Docs)
		audioSpec.callback = AudioCallback;
		audioSpec.userdata = NULL;
		if (!SDL_OpenAudio(&audioSpec, NULL))
		{
			FXBase.ul_DataPlayed = 0;
			SDL_PauseAudio(0);
			while (FXBase.ul_DataPlayed < FXBase.ul_DataSize)
			{
				SDL_Delay(100);
			}
			SDL_CloseAudio();
		}
	}
}
