/*
 * ===================================================================
 *  TS 26.104
 *  R99   V3.5.0 2003-03
 *  REL-4 V4.5.0 2003-06
 *  REL-5 V5.2.0 2003-06
 *  3GPP AMR Floating-point Speech Codec
 * ===================================================================
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "interf_dec.h"
#include "typedef.h"
#include "types.h"

#ifndef ETSI
#ifndef IF2
#include <string.h>
#define AMR_MAGIC_NUMBER "#!AMR\n"
#include "sp_dec.h"
#endif
#endif

void Copyright(void){
AMRPRINTF (
"===================================================================\n"
" TS 26.104                                                         \n"
" R99   V3.5.0 2003-03                                              \n"
" REL-4 V4.4.0 2003-03                                              \n"
" 3GPP AMR Floating-point Speech Decoder                            \n"
"===================================================================\n"
);
}

void write_wave_header(FILE* wavefile, int framecnt)
{
	struct RIFF_HEADER header;
	struct FMT_BLOCK   fmt;
	struct FACT_BLOCK  datafact;

	memcpy(fmt.szFmtID, "fmt ", 4);
	fmt.dwFmtSize = sizeof(fmt.wavFormat);
	fmt.wavFormat.wFormatTag = 1; //PCM
	fmt.wavFormat.wChannels  = 1; //Channel
	fmt.wavFormat.dwSamplesPerSec  = 8000; //8KHZ
	fmt.wavFormat.dwAvgBytesPerSec = 16000; //
	fmt.wavFormat.wBlockAlign      = 2; //PCM
	fmt.wavFormat.wBitsPerSample   = 16; //

	memcpy(datafact.szFactID, "data", 4);
	datafact.dwFactSize = framecnt*fmt.wavFormat.wBlockAlign*160;

	memcpy(&header.szRiffID, "RIFF", 4);
	header.dwRiffSize = datafact.dwFactSize+sizeof(datafact)+sizeof(struct FMT_BLOCK)+4;
	memcpy(&header.szRiffFormat, "WAVE", 4);

	fwrite(&header, 1, sizeof(header), wavefile);
	fwrite(&fmt, 1, sizeof(fmt), wavefile);
	fwrite(&datafact, 1, sizeof(datafact), wavefile);
}

/*
 * main
 *
 *
 * Function:
 *    Speech decoder main program
 *
 *    Usage: decoder bitstream_file synthesis_file
 *
 *    Format for ETSI bitstream file:
 *       1 word (2-byte) for the TX frame type
 *       244 words (2-byte) containing 244 bits.
 *          Bit 0 = 0x0000 and Bit 1 = 0x0001
 *       1 word (2-byte) for the mode indication
 *       4 words for future use, currently written as zero
 *
 *    Format for 3GPP bitstream file:
 *       Holds mode information and bits packed to octets.
 *       Size is from 1 byte to 31 bytes.
 *
 *    Format for synthesis_file:
 *       Speech is written to a 16 bit 8kHz file.
 *
 *    ETSI bitstream file format is defined using ETSI as preprocessor
 *    definition
 * Returns:
 *    0
 */
int main (int argc, char * argv[]){

   FILE * file_speech, *file_analysis;

   short synth[160];
   int frames = 0;
   int * destate;
   int read_size;
#ifndef ETSI
   unsigned char analysis[32];
   enum Mode dec_mode;
#ifdef IF2
   short block_size[16]={ 12, 13, 15, 17, 18, 20, 25, 30, 5, 0, 0, 0, 0, 0, 0, 0 };
#else
   char magic[8];
   short block_size[16]={ 12, 13, 15, 17, 19, 20, 26, 31, 5, 0, 0, 0, 0, 0, 0, 0 };
#endif
#else
   short analysis[250];
#endif

   /* Process command line options */
   if (argc == 3){

      file_speech = fopen(argv[2], "wb");
      if (file_speech == NULL){
         AMRPRINTF (  "%s%s%s\n","Use: ",argv[0], " input.file output.file " );
         return 1;
      }

      file_analysis = fopen(argv[1], "rb");
      if (file_analysis == NULL){
         AMRPRINTF (  "%s%s%s\n","Use: ",argv[0], " input.file output.file " );
         fclose(file_speech);
         return 1;
      }

   }
   else {
      AMRPRINTF (  "%s%s%s\n","Use: ",argv[0], " input.file output.file " );
      return 1;
   }
   Copyright();
   /* init decoder */
   destate = Decoder_Interface_init();

#ifndef ETSI
#ifndef IF2
   /* read and verify magic number */
   fread( magic, sizeof( char ), strlen( AMR_MAGIC_NUMBER ), file_analysis );
   if ( strncmp( magic, AMR_MAGIC_NUMBER, strlen( AMR_MAGIC_NUMBER ) ) ) {
	   AMRPRINTF(  "%s%s\n", "Invalid magic number: ", magic );
	   fclose( file_speech );
	   fclose( file_analysis );
	   return 1;
   }
#endif
#endif

	write_wave_header(file_speech, 0);

#ifndef ETSI

   /* find mode, read file */
   while (fread(analysis, sizeof (unsigned char), 1, file_analysis ) > 0)
   {
#ifdef IF2
      dec_mode = analysis[0] & 0x000F;
#else
      dec_mode = (analysis[0] >> 3) & 0x000F;
#endif
	  read_size = block_size[dec_mode];
		//printf("%2x\t", analysis[0]);
		//if ((frames%5) == 0)
		//	printf("\n");
      fread(&analysis[1], sizeof (char), read_size, file_analysis );
#else

   read_size = 250;
   /* read file */
   while (fread(analysis, sizeof (short), read_size, file_analysis ) > 0)
   {
#endif

      frames ++;

      /* call decoder */
	  memset(synth, 0, sizeof(synth));
      Decoder_Interface_Decode(destate, analysis, synth, 0);

	  if (0){
		int i;
		for (i=0; i<160; i++){
			synth[i] = ((synth[i]&0xFF)<<8) | ((synth[i]>>8)&0xff);
		}
	  }
      fwrite( synth, sizeof (short), 160, file_speech );
   }

	Decoder_Interface_exit(destate);
	printf("\n");
	fclose(file_analysis);

	fseek(file_speech, 0, SEEK_SET);

	write_wave_header(file_speech, frames);
	fflush(file_speech);
	fclose(file_speech);

	AMRPRINTF (  "\n%s%i%s\n","Decoded ", frames, " frames.");

	return 0;
}
