
#include "types.h"

#include "fft.c"

FILE* open_wave_file(S8* filename, S8* mod)
{
	FILE* fp = fopen(filename, mod);
	return fp;
}

S32 main(int argc, char* argv[])
{
	S32   res = -1;
	S8    filename[256];
	FILE* wavefile=NULL;

	struct RIFF_HEADER header;
	struct FMT_BLOCK fmt;
	struct WAVE_FORMAT newwavefmt;
	struct FACT_BLOCK  newdatafact;

	
	strcpy(filename, "teswpcm.wav");

	wavefile = open_wave_file(filename, "rb+");
	if (!wavefile)
		return res;


	newwavefmt.wChannels = 1;
	newwavefmt.wFormatTag= 1;
	newwavefmt.wBlockAlign = 2;
	newwavefmt.wBitsPerSample = 16;
	newwavefmt.dwSamplesPerSec = 8000;
	newwavefmt.dwAvgBytesPerSec = 16000;

	memcpy(newdatafact.szFactID, "data", 4);
	newdatafact.dwFactSize = 80000;

	HPRINTF("FORMAT:  %d\n", newwavefmt.wFormatTag);
	HPRINTF("CHANNEL: %d\n", newwavefmt.wChannels);
	HPRINTF("SAMPLE:  %d\n", newwavefmt.dwSamplesPerSec);
	HPRINTF("BITWIDTH:%d\n", newwavefmt.wBitsPerSample);
	HPRINTF("BLOCKALN:%d\n", newwavefmt.wBlockAlign);
	HPRINTF("BYTESPER:%d\n", newwavefmt.dwAvgBytesPerSec);
	{

		memcpy(fmt.szFmtID, "fmt ", 4);
		fmt.dwFmtSize = sizeof(newwavefmt);
		memcpy(&fmt.wavFormat, &newwavefmt, sizeof(newwavefmt));

		memcpy(&header.szRiffID, "RIFF", 4);
		memcpy(&header.szRiffFormat, "WAVE", 4);
		header.dwRiffSize = newdatafact.dwFactSize+sizeof(newdatafact)+sizeof(struct FMT_BLOCK)+4;

		fwrite(&header, 1, sizeof(header), wavefile);
		fwrite(&fmt, 1, sizeof(fmt), wavefile);
		fwrite(&newdatafact, 1, sizeof(newdatafact), wavefile);
	}

	fclose(wavefile);
	return res;
}
