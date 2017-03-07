
#include "types.h"


FILE* open_wave_file(S8* filename, S8* mod)
{
	FILE* fp = fopen(filename, mod);
	return fp;
}

S32 read_wave_header(FILE* fp, struct WAVE_FORMAT* wavefmt, struct FACT_BLOCK* data_fact)
{
	S32 res = -1;
	if (fp)
	{
		struct RIFF_HEADER header;
		struct FMT_BLOCK   fmt;
		struct FACT_BLOCK  fact;

		fread(&header, 1, sizeof(header), fp);
		fread(&fmt, 1, sizeof(fmt), fp);

		fseek(fp, sizeof(header)+sizeof(fmt)-sizeof(struct WAVE_FORMAT)+fmt.dwFmtSize, SEEK_SET);
		if (!strncmp(fmt.szFmtID, "fmt", 3))
		{
			memcpy(wavefmt, &fmt.wavFormat, sizeof(*wavefmt));
			for (;;)
			{
				if (fread(&fact, 1, sizeof(fact), fp) != sizeof(fact))
					break;
				HPRINTF("ID:%c%c%c%c\n", fact.szFactID[0], fact.szFactID[1], fact.szFactID[2], fact.szFactID[3]);
				if(!strncmp(fact.szFactID, "data", 4))
				{
					res = 0;
					memcpy(data_fact, &fact, sizeof(fact));
					break;
				}
				fseek(fp, fact.dwFactSize, SEEK_CUR);
			}
		}
	}
	return res;
}

S32 write_wave_pcm(FILE* fp, struct WAVE_FORMAT* wavefmt, COMPLEX* comx, U32 num)
{
	U8* buffer;
	S32 res = -1,i;
	U32 len = wavefmt->wBlockAlign*num;

	buffer = malloc(len);
	if (!buffer)
		return res;

	memset(buffer, 0, len);
	for (i=0; i<len/wavefmt->wBlockAlign; i++)
	{
		comx[i].real = comx[i].real*2;
		if (wavefmt->wChannels == 1)
		{
			if (wavefmt->wBlockAlign == 1)
				 buffer[i]=comx[i].real;
			else if (wavefmt->wBlockAlign == 2)
			{
				U16 x=comx[i].real;
				buffer[i*2] = x&0xff;
				buffer[i*2+1] = (x>>8)&0xff;
			}
		}
		else if (wavefmt->wChannels == 2)
		{
			if (wavefmt->wBlockAlign == 2)
				buffer[i*2]=comx[i].real;
			else if (wavefmt->wBlockAlign == 4)
			{
				U16 x=comx[i].real;
				buffer[i*4]=x&0xff;
				buffer[i*4+1]=(x>>8)&0xff;
			}
		}
	}
	len = fwrite(buffer, 1, len, fp);
	free(buffer);
	return res;
}

S32 read_wave_pcm(FILE* fp, struct WAVE_FORMAT* wavefmt, COMPLEX* comx, U32 num)
{
	U8* buffer;
	S32 res = -1,i;
	U32 len = wavefmt->wBlockAlign*num;

	buffer = malloc(len);
	if (!buffer)
		return res;

	len = fread(buffer, 1, len, fp);
	for (i=0; i<len/wavefmt->wBlockAlign; i++)
	{
		if (wavefmt->wChannels == 1)
		{
			if (wavefmt->wBlockAlign == 1)
				comx[i].real = buffer[i];
			else if (wavefmt->wBlockAlign == 2)
			{
				S16 x;
				x = buffer[i*2+1];
				x = (x<<8)|buffer[i*2];
				comx[i].real = x;
			}
		}
		else if (wavefmt->wChannels == 2)
		{
			if (wavefmt->wBlockAlign == 2)
				comx[i].real = buffer[i*2];
			else if (wavefmt->wBlockAlign == 4)
			{
				S16 x;
				x = buffer[i*4+1];
				x = (x<<8)|buffer[i*4];
				comx[i].real = x;
			}
		}
	}
	free(buffer);
	return res;
}

S32 read_wave_pcm1(FILE* fp, struct WAVE_FORMAT* wavefmt, COMPLEX* comx, U32 num)
{
	S32 res = -1;
	U8* buffer;
	U32 len = wavefmt->dwAvgBytesPerSec;
	U32 readlen;
	buffer = malloc(len);
	if (!buffer)
		return res;
	do
	{
		U32 i;
		readlen = fread(buffer, 1, len, fp);
		//HPRINTF("readlen:%d\n", readlen);
		for (i=0; i<wavefmt->dwSamplesPerSec && i< readlen/wavefmt->wBlockAlign; i++)
		{
			if (wavefmt->wChannels == 1)
			{
				if (wavefmt->wBlockAlign == 1)
				{
					HPRINTF("%d", buffer[i]);
				}
				else if (wavefmt->wBlockAlign == 2)
				{
					S16 x;
					x = buffer[i*2+1];
					x = (x<<8)|buffer[i*2];
					HPRINTF("%d", x);
				}
			}
			else if (wavefmt->wChannels == 2)
			{
				if (wavefmt->wBlockAlign == 2)
				{
					HPRINTF("%d,%d", buffer[i*2], buffer[i*2+1]);
				}
				else if (wavefmt->wBlockAlign == 4)
				{
					S16 x,y;
					x = buffer[i*4+1];
					x = (x<<8)|buffer[i*4];
					y = buffer[i*4+3];
					y = (y<<8)|buffer[i*4+2];
					HPRINTF("%d,%d", x,y);
				}
			}
			HPRINTF("\n");
		}
	}while (readlen == len);
	free(buffer);
}

S32 main(int argc, char* argv[])
{
	S32   res = -1;
	S8    filename[256];
	FILE* wavefile=NULL;
	FILE* newfile =NULL;
	struct WAVE_FORMAT wavefmt;
	struct FACT_BLOCK  datafact;

	memset(&wavefmt, 0, sizeof(wavefmt));

	if (argc == 2)
		strcpy(filename, argv[1]);
	else
		strcpy(filename, "test.wav");

	wavefile = open_wave_file(filename, "rb");
	if (!wavefile)
		return res;

	newfile  = open_wave_file("res.wav", "wb");
	if (!newfile)
	{
		HPRINTF("open res.wav failed!\n");
		fclose(wavefile);
		return res;
	}

	if (read_wave_header(wavefile, &wavefmt, &datafact))
		goto wave_exit;

	HPRINTF("FORMAT:  %d\n", wavefmt.wFormatTag);
	HPRINTF("CHANNEL: %d\n", wavefmt.wChannels);
	HPRINTF("SAMPLE:  %d\n", wavefmt.dwSamplesPerSec);
	HPRINTF("BITWIDTH:%d\n", wavefmt.wBitsPerSample);
	HPRINTF("BLOCKALN:%d\n", wavefmt.wBlockAlign);
	HPRINTF("BYTESPER:%d\n", wavefmt.dwAvgBytesPerSec);

	{
		struct RIFF_HEADER header;
		struct FMT_BLOCK fmt;

		memcpy(fmt.szFmtID, "fmt ", 4);
		fmt.dwFmtSize = sizeof(wavefmt);
		memcpy(&fmt.wavFormat, &wavefmt, sizeof(wavefmt));

		memcpy(&header.szRiffID, "RIFF", 4);
		memcpy(&header.szRiffFormat, "WAVE", 4);
		header.dwRiffSize = datafact.dwFactSize+sizeof(datafact)+sizeof(struct FMT_BLOCK)+4;

		fwrite(&header, 1, sizeof(header), newfile);
		fwrite(&fmt, 1, sizeof(fmt), newfile);
		fwrite(&datafact, 1, sizeof(datafact), newfile);
	}

	
	{
		U32 len;
		const U32 NUM = 128;
		COMPLEX comx[NUM];
		for (len=0; len<datafact.dwFactSize; len+=wavefmt.wBlockAlign*NUM)
		{
			memset(comx, 0, sizeof(comx));
			read_wave_pcm(wavefile, &wavefmt, comx, NUM);
			fft(comx, NUM);
			freq_small(comx, NUM, 8);
			ifft(comx, NUM);
			write_wave_pcm(newfile, &wavefmt, comx, NUM);
		}
	}
wave_exit:
	fclose(newfile);
	fclose(wavefile);
	return res;
}
