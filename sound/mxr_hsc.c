/*
** MXR - A Simple Audio Mixer
** Copyright (c) 2017 by Oliver Gantert.
**
** Permission is given by the author to use this version of MXR freely,
** provided that the message "Copyright (c) 2017 by Oliver Gantert" in this
** and associated files is not changed or removed.
**
** See "mxr.h" for details.
*/
#include "mxr.h"
#include "fmopl.h"

/******************************************************************************/

static const int index_car[] =
    { 0x03, 0x04, 0x05, 0x0B, 0x0C, 0x0D, 0x13, 0x14, 0x15 };

static const int index_mod[] =
    { 0x00, 0x01, 0x02, 0x08, 0x09, 0x0A, 0x10, 0x11, 0x12 };

static const int freq_table[] =
    { 0x016B, 0x0181, 0x0198, 0x01B0, 0x01CA, 0x01E5, 0x0202, 0x0220, 0x0241, 0x0263, 0x0287, 0x02AE };

typedef struct {
    ubyte car_am_vib_eg_ksr_multi;
    ubyte mod_am_vib_eg_ksr_multi;

    ubyte car_ksl_volume;
    ubyte mod_ksl_volume;

    ubyte car_attack_decay;
    ubyte mod_attack_decay;

    ubyte car_sustain_release;
    ubyte mod_sustain_release;

    ubyte feedback_fm;

    ubyte car_waveform;
    ubyte mod_waveform;

    ubyte slide;
} HSC_Instrument;	// 12

typedef struct {
    ubyte note[9][64];
    ubyte effect[9][64];
} HSC_Pattern;		// 1152

typedef enum {
    on, off, slide
} voice_type;

#define HSC_BUFFER_SIZE		((unsigned)(OPL_FREQUENCY*10 / 182))

typedef struct {
	HSC_Instrument instrument[128];
	HSC_Pattern pattern[50];

	ubyte pattern_table[51];
	ubyte reserved_padding;

	int hsc_ch_keyoff[9]; // bool
	int hsc_ch_octave[9];
	int hsc_ch_inst[9];
	int hsc_ch_note[9];

	int hsc_index;
	int hsc_line;
	int hsc_speed;
	int hsc_ticks;

	int hsc_effect03; // bool
	int hsc_effect03_line;

	int hsc_valid_data; // bool

	int hsc_in_process; // bool

	int hsc_buffer_samples;
	short hsc_buffer[HSC_BUFFER_SIZE];
} HSC_File;

/******************************************************************************/

static void ym3812_write(int reg, int val)
{
    YM3812Write(OPL_CHIP0, 0x0388, reg);
    YM3812Write(OPL_CHIP0, 0x0389, val);
}
/******************************************************************************/

static void hsc_set_volume(HSC_File *pHSC, int channel, int volume_car, int volume_mod)
{
    if (volume_car != 0xFF) {
	volume_car |= pHSC->instrument[pHSC->hsc_ch_inst[channel]].car_ksl_volume & 0xC0;

	ym3812_write(0x40 + index_car[channel], volume_car);
    }

    if (volume_car == 0xFF && volume_mod != 0xFF) {
	volume_mod |= pHSC->instrument[pHSC->hsc_ch_inst[channel]].mod_ksl_volume & 0xC0;

	ym3812_write(0x40 + index_mod[channel], volume_mod);
    }

    if (volume_car != 0xFF && volume_mod != 0xFF
	&& (pHSC->instrument[pHSC->hsc_ch_inst[channel]].feedback_fm & 1)) {
	volume_mod |= pHSC->instrument[pHSC->hsc_ch_inst[channel]].mod_ksl_volume & 0xC0;

	ym3812_write(0x40 + index_mod[channel], volume_mod);
    }
}

/******************************************************************************/

static void hsc_set_instrument(HSC_File *pHSC, int channel, int new_inst)
{
    pHSC->hsc_ch_inst[channel] = new_inst;

    hsc_set_volume(pHSC, channel, 0x3F, 0x3F);

    ym3812_write(0x20 + index_car[channel], pHSC->instrument[new_inst].car_am_vib_eg_ksr_multi);
    ym3812_write(0x20 + index_mod[channel], pHSC->instrument[new_inst].mod_am_vib_eg_ksr_multi);

    ym3812_write(0x40 + index_car[channel], pHSC->instrument[new_inst].car_ksl_volume);
    ym3812_write(0x40 + index_mod[channel], pHSC->instrument[new_inst].mod_ksl_volume);

    ym3812_write(0x60 + index_car[channel], pHSC->instrument[new_inst].car_attack_decay);
    ym3812_write(0x60 + index_mod[channel], pHSC->instrument[new_inst].mod_attack_decay);

    ym3812_write(0x80 + index_car[channel], pHSC->instrument[new_inst].car_sustain_release);
    ym3812_write(0x80 + index_mod[channel], pHSC->instrument[new_inst].mod_sustain_release);

    ym3812_write(0xC0 + channel, pHSC->instrument[new_inst].feedback_fm);

    ym3812_write(0xE0 + index_car[channel], pHSC->instrument[new_inst].car_waveform);
    ym3812_write(0xE0 + index_mod[channel], pHSC->instrument[new_inst].mod_waveform);
}

/******************************************************************************/

static void hsc_set_voice(HSC_File *pHSC, int channel, voice_type vt)
{
    int note_high =	(pHSC->hsc_ch_note[channel] >> 0x08) | (pHSC->hsc_ch_octave[channel] << 0x02);
    int note_low = pHSC->hsc_ch_note[channel] & 0xFF;

    switch (vt) 
	{
    case on:
	{
	    ym3812_write(0xB0 + channel, 0x00);

	    ym3812_write(0xA0 + channel, note_low);
	    ym3812_write(0xB0 + channel, note_high | 0x20);

	    pHSC->hsc_ch_keyoff[channel] = FALSE;
	}
	break;

    case off:
	{
	    ym3812_write(0xB0 + channel, note_high);

	    pHSC->hsc_ch_keyoff[channel] = TRUE;
	}
	break;

    case slide:
	{
	    ym3812_write(0xA0 + channel, note_low);

	    if (pHSC->hsc_ch_keyoff[channel]) {
		ym3812_write(0xB0 + channel, note_high);
	    } else {
		ym3812_write(0xB0 + channel, note_high | 0x20);
	    }
	}
	break;
    }
}

/******************************************************************************/

static void hsc_process_row(HSC_File *pHSC)
{
    int i, channel, cur_pattern, note, effect;

    int pat_break = FALSE; // bool


    if (!pHSC->hsc_valid_data || pHSC->hsc_in_process) {
	return;
    }

    pHSC->hsc_in_process = TRUE;


    if (pHSC->hsc_ticks < pHSC->hsc_speed) {
	pHSC->hsc_ticks++;

	pHSC->hsc_in_process = FALSE;

	return;
    }

    pHSC->hsc_ticks = 1;


    cur_pattern = pHSC->pattern_table[pHSC->hsc_index];

    if (pHSC->hsc_effect03) {
	pHSC->hsc_effect03_line++;

	if (pHSC->hsc_effect03_line == 32) {
	    pHSC->hsc_effect03 = FALSE;
	    /*hsc_set_volume(4, 0x03 * 4, 0x03 * 4);*/
	    /*hsc_set_volume(3, 0x03 * 4, 0x03 * 4);*/
	} else {
	    for (i = 0; i < 9; i++) {
		hsc_set_volume(pHSC, i, 0x3F - pHSC->hsc_effect03_line * 2 + 1,
			       0x3F - pHSC->hsc_effect03_line * 2 + 1);
	    }
	}
    }



    for (channel = 0; channel < 9; channel++) {
	note = pHSC->pattern[cur_pattern].note[channel][pHSC->hsc_line];
	effect = pHSC->pattern[cur_pattern].effect[channel][pHSC->hsc_line];

	switch (note) {
	case 0:
	    {
	    }
	    break;

	case 127:
	    {
		hsc_set_voice(pHSC, channel, off);
	    }
	    break;

	case 128:
	    {
		hsc_set_instrument(pHSC, channel, effect);
	    }
	    break;

	default:
	    {
		note -= 1;

		pHSC->hsc_ch_octave[channel] = note / 12;
		pHSC->hsc_ch_note[channel] = freq_table[note % 12];

		if (effect >= 0x10 && effect <= 0x1F) {
		    pHSC->hsc_ch_note[channel] += (int) ((effect - 0x10) * 1.31);
		}

		if (effect >= 0x20 && effect <= 0x2F) {
		    pHSC->hsc_ch_note[channel] -= (int) ((effect - 0x20) * 1.31);
		}

		if (pHSC->instrument[pHSC->hsc_ch_inst[channel]].slide >> 0x04) {
		    pHSC->hsc_ch_note[channel] +=
			(int) ((pHSC->instrument[pHSC->hsc_ch_inst[channel]].
				slide >> 0x04) * 1.31);
		}

		hsc_set_voice(pHSC, channel, on);
	    }
	    break;
	}


	if (effect != 0x00) {
	    /* Pattern break */

	    if (effect == 0x01 && note != 0x80) {
		pat_break = TRUE;	/*cur_line = 0x3F;*/
	    }


	    if (effect == 0x03 && note != 0x80) {
		pHSC->hsc_effect03 = TRUE;

		pHSC->hsc_effect03_line = 0;

		for (i = 0; i < 9; i++) {
		    hsc_set_volume(pHSC, i, 0x3F, 0x3F);
		}
	    }
	    /* Slide note up */

	    if (effect >= 0x10 && effect <= 0x1F && note == 0x00) {
		pHSC->hsc_ch_note[channel] += (int) ((effect - 0x10) * 1.31);

		hsc_set_voice(pHSC, channel, slide);
	    }
	    /* Slide note down */

	    if (effect >= 0x20 && effect <= 0x2F && note == 0x00) {
		pHSC->hsc_ch_note[channel] -= (int) ((effect - 0x20) * 1.31);

		hsc_set_voice(pHSC, channel, slide);
	    }
	    /* Set carrier volume */

	    if (effect >= 0xA0 && effect <= 0xAF) {
		hsc_set_volume(pHSC, channel, (effect - 0xA0) * 0x04, 0xFF);
	    }
	    /* Set modulator volume */

	    if (effect >= 0xB0 && effect <= 0xBF) {
		hsc_set_volume(pHSC, channel, 0xFF, (effect - 0xB0) * 0x04);
	    }
	    /* Set instrument volume */

	    if (effect >= 0xC0 && effect <= 0xCF) {
		hsc_set_volume(pHSC, channel, (effect - 0xC0) * 0x4,
			       (effect - 0xC0) * 0x4);
	    }
	    /* Set speed */

	    if (effect >= 0xF0 && effect <= 0xFF) {
		pHSC->hsc_speed = effect - 0xEF;
	    }
	}
    }


    pHSC->hsc_line = pat_break ? 0x40 : pHSC->hsc_line + 0x01;


    if (pHSC->hsc_line == 0x40) {
	pHSC->hsc_line = 0x00;
	pHSC->hsc_index += 0x01;

	if (pHSC->pattern_table[pHSC->hsc_index] == 0xFF) {
	    pHSC->hsc_index = 0x00;

	    ym3812_write(0x01, 0x20);

	    ym3812_write(0x08, 0x40);


	    for (i = 0; i < 9; i++) {
		ym3812_write(0xB0 + i, 0);
		ym3812_write(0xA0 + i, 0);

		hsc_set_instrument(pHSC, i, i);

		pHSC->hsc_ch_note[i] = 0;
		pHSC->hsc_ch_octave[i] = 0;
		pHSC->hsc_ch_keyoff[i] = FALSE;
	    }

	    pHSC->hsc_line = 0;
	    pHSC->hsc_index = 0;
	    pHSC->hsc_ticks = 1;
	    pHSC->hsc_speed = 2;
	} else if (pHSC->pattern_table[pHSC->hsc_index] >= 0x80) {
	    pHSC->hsc_index = pHSC->pattern_table[pHSC->hsc_index] - 0x80;

	    ym3812_write(0x01, 0x20);
	    ym3812_write(0x08, 0x40);

	    for (i = 0; i < 9; i++) {
		ym3812_write(0xB0 + i, 0);
		ym3812_write(0xA0 + i, 0);

		hsc_set_instrument(pHSC, i, i);

		pHSC->hsc_ch_note[i] = 0;
		pHSC->hsc_ch_octave[i] = 0;
		pHSC->hsc_ch_keyoff[i] = FALSE;
	    }

	    pHSC->hsc_line = 0;
	    pHSC->hsc_index = 0;
	    pHSC->hsc_ticks = 1;
	    pHSC->hsc_speed = 2;
	}
    }

    pHSC->hsc_in_process = FALSE;
}

/******************************************************************************/

static void hscReset(HSC_File *pHSC)
{
    while (pHSC->hsc_in_process);

    pHSC->hsc_in_process = TRUE;
    pHSC->hsc_valid_data = FALSE;

    YM3812ResetChip(OPL_CHIP0);

    pHSC->hsc_in_process = FALSE;
}

/******************************************************************************/

static long HSCOpen(HSC_File *pHSC, const char *pszFileName)
{
    int i, line, channel, num_pattern;
	long hsc_size;
    ubyte *hsc_data, *hsc_file;
	FILE *pFile;

    hscReset(pHSC);

    pHSC->hsc_in_process = TRUE;

	pFile = fopen(pszFileName, "rb");
	if (!pFile) {
		return(0);
	}

	fseek(pFile, 0, SEEK_END);
	hsc_size = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	hsc_file = MXR_MemAlloc(hsc_size);
	if (!hsc_file) {
		fclose(pFile);
		return(0);
	}

	fread(hsc_file, 1, hsc_size, pFile);
	fclose(pFile);

	hsc_data = hsc_file;

    for (i = 0; i < 128; i++) {
		pHSC->instrument[i].car_am_vib_eg_ksr_multi = *hsc_data++;
		pHSC->instrument[i].mod_am_vib_eg_ksr_multi = *hsc_data++;

		pHSC->instrument[i].car_ksl_volume = *hsc_data++;
		pHSC->instrument[i].mod_ksl_volume = *hsc_data++;

		pHSC->instrument[i].car_attack_decay = *hsc_data++;
		pHSC->instrument[i].mod_attack_decay = *hsc_data++;

		pHSC->instrument[i].car_sustain_release = *hsc_data++;
		pHSC->instrument[i].mod_sustain_release = *hsc_data++;

		pHSC->instrument[i].feedback_fm = *hsc_data++;

		pHSC->instrument[i].car_waveform = *hsc_data++;
		pHSC->instrument[i].mod_waveform = *hsc_data++;

		pHSC->instrument[i].slide = *hsc_data++;
    }

    for (i = 0; i < 51; i++) {
		pHSC->pattern_table[i] = *hsc_data++;
	}

    num_pattern = (hsc_size - 1587) / 1152;

    for (i = 0; i < num_pattern; i++) {
		for (line = 0; line < 64; line++) {
			for (channel = 0; channel < 9; channel++) {
				pHSC->pattern[i].note[channel][line] = *hsc_data++;
				pHSC->pattern[i].effect[channel][line] = *hsc_data++;
			}
		}
    }

    MXR_MemFree(hsc_file, hsc_size);

    ym3812_write(0x01, 0x20);
    ym3812_write(0x08, 0x40);

    for (i = 0; i < 9; i++) {
		ym3812_write(0xB0 + i, 0);
		ym3812_write(0xA0 + i, 0);

		hsc_set_instrument(pHSC, i, i);

		pHSC->hsc_ch_note[i] = 0;
		pHSC->hsc_ch_octave[i] = 0;
		pHSC->hsc_ch_keyoff[i] = FALSE;
    }

    pHSC->hsc_line = 0;
    pHSC->hsc_index = 0;
    pHSC->hsc_ticks = 1;
    pHSC->hsc_speed = 2;

    pHSC->hsc_valid_data = TRUE;
    pHSC->hsc_in_process = FALSE;
	
	return(1);
}

/******************************************************************************/

typedef struct {
	MXR_Input	mxrInput;
	HSC_File	hscFile;
} MXR_InputHSC;

/******************************************************************************/

static unsigned long MXR_ProcessInputHSC(MXR_InputHSC *pInput, void *pStream, unsigned long nNumSamples)
{
	short *out;
	long nSamples, nWrite, i;
	out = (short *)pStream;
	nSamples = nNumSamples;
	while (nSamples > 0) {
		if (pInput->hscFile.hsc_buffer_samples) {
			nWrite = pInput->hscFile.hsc_buffer_samples;
			if (nWrite > nSamples) {
				nWrite = nSamples;
			}
			for (i = 0; i < nWrite; i++) {
				*out++ = pInput->hscFile.hsc_buffer[(HSC_BUFFER_SIZE - pInput->hscFile.hsc_buffer_samples) + i];
			}
			pInput->hscFile.hsc_buffer_samples -= nWrite;
			nSamples -= nWrite;
		} else {
			hsc_process_row(&pInput->hscFile);
			YM3812UpdateOne(OPL_CHIP0, pInput->hscFile.hsc_buffer, HSC_BUFFER_SIZE);
			pInput->hscFile.hsc_buffer_samples = HSC_BUFFER_SIZE;
		}
	}
	return(nNumSamples);
}

/******************************************************************************/

static void MXR_DestroyInputHSC(MXR_InputHSC *pInput)
{
	MXR_MemFree(pInput, sizeof(MXR_InputHSC));
}

/******************************************************************************/

MXR_Input *MXR_CreateInputHSC(const char *pszFileName)
{
	MXR_InputHSC *pInput;
	pInput = (MXR_InputHSC *)MXR_MemAlloc(sizeof(MXR_InputHSC));
	if (!pInput) {
		return(NULL);
	}
	pInput->mxrInput.pProcess = (MXR_ProcessInputFunc)MXR_ProcessInputHSC;
	pInput->mxrInput.pDestroy = (MXR_DestroyInputFunc)MXR_DestroyInputHSC;
	pInput->mxrInput.nVolume = 128;

	pInput->mxrInput.fmt.nSamplesPerSec = OPL_FREQUENCY;
	pInput->mxrInput.fmt.nBitsPerChannel = 16;
	pInput->mxrInput.fmt.nNumChannels = 1;
	pInput->mxrInput.fmt.nSampleSize = (pInput->mxrInput.fmt.nNumChannels * pInput->mxrInput.fmt.nBitsPerChannel) >> 3;

	if (!HSCOpen(&pInput->hscFile, pszFileName)) {
		MXR_MemFree(pInput, sizeof(MXR_InputHSC));
		return(NULL);
	}

	return((MXR_Input *)pInput);
}

/******************************************************************************/
