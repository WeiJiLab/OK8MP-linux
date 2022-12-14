/*****************************************************************************
 *
 * Motorola Inc.
 * (c) Copyright 2004 Motorola, Inc.
 * ALL RIGHTS RESERVED.
 *
 *****************************************************************************

 ***********************************************************************
 * Copyright (c) 2005-2010, 2012, Freescale Semiconductor, Inc.
 * All modifications are confidential and proprietary information
 * of Freescale Semiconductor, Inc.
 ***********************************************************************

 *
 * File Name: g726_dec_api.c
 *
 * Description: G.726 decoder test wrapper is defined in this file.
 *
 *
 ***************************** Change History*********************************
 *
 *   DD/MMM/YYYY     Code Ver     Author            Description
 *   -----------     --------     ------            -----------
 *   28/Jul/2004     0.1          Tommy Tang        File created
 *   19/Aug/2004     1.0          Tommy Tang        Review rework
 *   13/12/2006      1.2          Shyam Krishnan M  Added profiling code for RVDS
 *   03/06/2008      engr78773    Qiu Cunshou       Elinux build
 *   11/06/2008      engr79565    Qiu Cunshou       WinCE build,Add API for version
 *****************************************************************************/

/****************************<INCLUDE_FILES BEGIN>****************************/
#ifdef __WINCE
#include <windows.h>
#include <winbase.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef TIME_PROFILE
#ifdef __SYMBIAN32__
#include <sys/time.h>
#else //__SYMBIAN32__
#include <time.h>
#endif //__SYMBIAN32__

#endif

#include "g726_dec_api.h"
#include "g726_dectest.h"

#ifdef MEASURE_STACK_USAGE
#include "mm_codec_common.h"
#endif

#ifdef TIME_PROFILE_RVDS
#include "strfunc.h"
#endif

#define OUTPUT_DUMP
#if defined TIME_PROFILE || defined TIME_PROFILE_RVDS || defined ARM_MIPS_TEST_WINCE
#undef OUTPUT_DUMP
#endif
/******************************<INCLUDE_FILES END>****************************/

/***************************<GLOBAL_VARIABLES BEGIN>**************************/
G726_S8    *gps8HomingFileName;
G726_S8    *gps8InpFileName;
G726_S8    *gps8OutFileName;
G726_U32   gu32Homing;
G726_U32   gu32Quiet;
G726_S32   gs32Rate;
G726_S32   gs32Law;
FILE       *gpfFileIn;
FILE       *gpfFileOut;

#ifdef MEASURE_STACK_USAGE
    int           *ps32SP, *ps32BaseSP;
    int           s32StackCount, s32StackValue;
    int           s32PeakStack = 0;
#endif

#ifdef MEASURE_HEAP_USAGE
    int      s32TotalMallocBytes=0;
#endif

/***************************<GLOBAL_VARIABLES END>****************************/

/***************************<LOCAL FUNCTIONS>*********************************/
#ifdef TIME_PROFILE_RVDS_ARM9
void dummyCall1(void);
void dummyCall2(void);
G726_S32  uiframe = 0;
#endif

#ifdef ARM_MIPS_TEST_WINCE        		 /* for taking timing on wince platform*/
	extern void INTERRUPTS_SET(int k);
	unsigned int	Flag;
	LARGE_INTEGER	lpFrequency;
	LARGE_INTEGER	lpPerformanceCountBegin;
	LARGE_INTEGER	lpPerformanceCountEnd;
	__int64	Temp;
	FILE	*pfCycleCount;
	__int64	totalTime = 0;
	unsigned int	numFrame = 0;
	__int64	minFrameTime = 0x7FFFFFFFFFFFFFFF;
	__int64	maxFrameTime = -1;
	unsigned int	minFrameNumber = 0;
	unsigned int	maxFrameNumber = 0;
	unsigned char	chname[] = "[PROFILE-INFO]";
#endif
/***************************<LOCAL FUNCTIONS END>*********************************/

#ifndef BIT_MATCH_WITH_REF
/*******************************************************************************
 * Typedef : StreamBuf
 *
 * The Intel convention of least significant byte at lowest address necessitates
 * the introduction of additional complexity when reading the MLP bitstream.
 * 32 bits are read by 4 byte accesses.
 *
 * dataPtr  points to the input buffer from which the data are read.
 * nextBits caches the next data to be read.  The next bit is the msb of nextBits
 * index    points to the next word
 * bitsLeft is the number of bits (0..31) of the next word not yet in nextBits.
 *
 *******************************************************************************/

typedef struct
{
    const G726_U8 *dataPtr ;
    G726_U32 nextBits ;
    G726_U32 nextWord ;
    G726_S32  index ;
    G726_S32  bitsLeft ;
} StreamBuf ;

/*******************************************************************************
 * Function : initStreamBuf
 *
 * Initialize a bitstream buffer.
 * dataPtr is set to input
 * The first 32 bits of input are read (by four byte accesses) into nextWord,
 * and index is set to indicate the following byte.
 * nextWord is copied into nextBits, and bitsLeft initialized to 0.
 *
 * When n bits of data are read from the stream, they are taken from the msb end
 * of nextBits, which is then shifted left accordingly. bitsLeft is decremented
 * by n. When it becomes negative, nextBits is updated from nextWord, and a new
 * nextWord is read from the input.
 *
 *******************************************************************************/

void initStreamBuf (StreamBuf *buf, const G726_U8 *input) ;

/*******************************************************************************
 * StreamBuf Read functions.
 *
 * readU - read unigned value from bitstream (1 to 31 bits)
 *
 * Each of these functions calls consume to update Streambuf
 *
 *******************************************************************************/

G726_U32 readU (StreamBuf *buf, G726_S32 bits) ;

/*******************************************************************************
 * Function consume
 *
 * Updates StreamBuf structure following a read or skip.
 * nextBits is updated to contain the next 32 bits of the stream.
 * bitsLeft is adjusted, and nextWord and index are updated as required.
 *
 *******************************************************************************/

void consume (StreamBuf *buf, G726_S32 bits) ;

/*******************************************************************************
 * Function : initStreamBuf
 *
 * Description
 *   Initialize a bitstream buffer.
 *   dataPtr is set to input
 *   The first 32 bits of input are read into nextWord (by four byte accesses,
 *   to ensure that they are accessible as a 32-bit int, avoiding potential
 *   problems due to 'endianness'),
 *   and index is set to indicate the following byte.
 *   nextWord is copied into nextBits, and bitsLeft initialized to 0.
 *
 *   When n bits of data are read from the stream, they are taken from the msb end
 *   of nextBits, which is then shifted left accordingly. bitsLeft is decremented
 *   by n. When it becomes negative, nextBits is updated from nextWord, and a new
 *   nextWord is read from the input. The initial setting of bitsLeft to zero
 *   ensures that this updating will take place on the first read of the buffer.
 *
 * buf      pointer to bitstream buffer
 * input    pointer to input buffer
 *
 *******************************************************************************/

void initStreamBuf (StreamBuf *buf, const G726_U8 *input)
{
    buf->dataPtr  = input ;
    buf->nextWord = (buf->dataPtr[0] << 24) + (buf->dataPtr[1] << 16) +	(buf->dataPtr[2] << 8) + buf->dataPtr[3] ;
    buf->index    = 4 * sizeof (G726_U8) ;
    buf->nextBits = buf->nextWord ;
    buf->bitsLeft = 0 ;
}

/*******************************************************************************
 * Function readU
 *
 * Description
 *   Reads an unsigned value from bitstream (1 to 31 bits)
 *
 * buf    pointer to bitstream buffer
 * bits   number of bits to read (1..31)
 *
 * On Exit : StreamBuf structure is updated
 *
 *******************************************************************************/

G726_U32 readU(StreamBuf *buf, G726_S32 bits)
{
    G726_U32 val = buf->nextBits >> (32-bits) ;
    consume (buf, bits) ;

    return (val) ;
}

/*******************************************************************************
 * Function consume
 *
 * Description
 *   Updates StreamBuf structure following a read or skip.
 *   nextBits is shifted left by the number of bits read ; bitsLeft is adjusted.
 *   nextWord and index are updated if necessary.
 *   The lsbs of nextBits are filled in from nextWord
 *
 * buf    pointer to bitstream buffer
 * bits   number of bits 'consumed' from buf
 *
 * On Exit: StreamBuf structure is updated ; in particular, all 32 bits of
 *          nextBits are valid
 *
 *******************************************************************************/
void consume (StreamBuf *buf, G726_S32 bits)
{
    buf->nextBits = (buf->nextBits << bits) ;
    buf->bitsLeft -= bits ;

    if (buf->bitsLeft < 0)
    {
        buf->nextBits = buf->nextWord << -buf->bitsLeft ;
        buf->bitsLeft += 8*sizeof(G726_U32) ;

        buf->nextWord = (buf->dataPtr[buf->index] << 24) + (buf->dataPtr[buf->index+1] << 16) +
                        (buf->dataPtr[buf->index+2] << 8) + buf->dataPtr[buf->index+3] ;
        buf->index += 4 * sizeof (G726_U8) ;
    }

    buf->nextBits |= (buf->nextWord >> buf->bitsLeft) ;
}


#endif





/*****************************************************************************
 *
 * Function:  vDisplayUsage
 *
 * Description:  Display Usage
 *
 * Returns:      None
 *
 * Arguments:    None
 *
 * Global Variables: None
 *
 * Range Issues: None
 *
 * Special Issues: None
 *
 *****************************************************************************/

G726_Void vDisplayUsage()
{
#ifdef G726_DEBUG_PRINT
	fprintf(stderr, "G726 Codec Demo Program. Written by Motorola. All rights reserved.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: G726DecTest [-Options] <InpFile> <OutFile>\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Where:\n");
	fprintf(stderr, "  InpFile      is the name of the file to be processed.\n");
	fprintf(stderr, "  OutFile      is the name with the processed data.\n");
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -law <a|u|l> The letters A or a for G.711 A-law, letter u for G.711 m-law, or\n");
	fprintf(stderr, "               letter l for linear.\n");
	fprintf(stderr, "               Default is A-law.\n");
	fprintf(stderr, "  -rate #      is the bit-rate (in kbit/s): 40, 32, 24 or 16\n");
	fprintf(stderr, "               Default is 32 kbit/s.\n");
	fprintf(stderr, "  -homing <InitFile>  The file contains initialization (homing) sequence to\n");
	fprintf(stderr, "               drive the Decoder to a known initial state.\n");
	fprintf(stderr, "               Default is no init file and the Codec is in reset state.\n");
	fprintf(stderr, "  -?/-help     print help message\n");
#endif
	exit(1);
}

/*****************************************************************************
 *
 * Function:  eG726DProcessCmdLineOptions
 *
 * Description:  This function processes command line options.
 *
 * Returns: eG726DReturnType
 *
 * Arguments:    s32Argc -> number of command line arguments
 *               ps8Argv -> pointer to command line arguments
 *
 *
 * Global Variables:  gps8HomingFileName
 *                    gps8InpFileName
 *                    gps8OutFileName
 *                    gu32Homing;
 *                    gu32Quiet
 *                    gs32Rate
 *                    gs32Law
 *
 * Range Issues: None
 *
 * Special Issues: None
 *
 *****************************************************************************/

eG726DReturnType eG726DProcessCmdLineOptions(
                  G726_S32  s32Argc,
                  G726_S8  *ps8Argv[]
                  )
{

	/* Process decoding argument, if any */
	if (s32Argc < 3)
	{
		vDisplayUsage();
	}
	else
	{
		gps8HomingFileName = NULL; /* Homing file name */
		gps8InpFileName = NULL;    /* Input file name */
		gps8OutFileName = NULL;    /* Output file name */
		gu32Homing = G726_FALSE;   /* Default: Reset mode, no homing file */
		gu32Quiet = G726_FALSE;    /* Default: Verbose Mode */
		//gs32Rate = 4;              /* Default is 32 kbit/s */			//TLSbo59112
		gs32Rate = BIT_RATE_32KBPS;										//TLSbo59112
		gs32Law = G726_PCM_ALAW;   /* Default is A-law */

		while (s32Argc > 1 && ps8Argv[1][0] == '-')
		{
#ifdef TIME_PROFILE_RVDS
            if (mystrcmp(ps8Argv[1], "-homing") == 0)
#else
			if (strcmp(ps8Argv[1], "-homing") == 0)
#endif
			{
				if(s32Argc > 2)
				{
					gps8HomingFileName = ps8Argv[2];
					gu32Homing = G726_TRUE;
				}
				else
				{
					vDisplayUsage();
				}

				/* Update argc/argv to next valid option/argument */
				ps8Argv+=2;
				s32Argc-=2;
			}
#ifdef TIME_PROFILE_RVDS
			else if (mystrcmp(ps8Argv[1], "-law") == 0)
#else
			else if (strcmp(ps8Argv[1], "-law") == 0)
#endif
            		  {
				if(s32Argc > 2)
				{
					/* Define law for operation: A, u, or linear */
					switch (toupper(ps8Argv[2][0]))
					{
					case 'A':
						gs32Law = G726_PCM_ALAW;
						break;
					case 'U':
						gs32Law = G726_PCM_ULAW;
						break;
					case 'L':
						gs32Law = G726_PCM_LINEAR;
						break;
					default:
#ifdef G726_DEBUG_PRINT
						printf("Invalid law (A or u)! Aborted...\n");
#endif
						exit(1);
					}
				}
				else
				{
					vDisplayUsage();
				}

				/* Move argv over the option to the next argument */
				ps8Argv+=2;
				s32Argc-=2;
			}

#ifdef TIME_PROFILE_RVDS
			else if (mystrcmp(ps8Argv[1], "-rate") == 0)
#else
			else if (strcmp(ps8Argv[1], "-rate") == 0)
#endif
			{
				/*Define rate(s) for operation */
				//if (gs32Rate > 5)
				if (s32Argc > 2)
				{
				    gs32Rate = atoi(ps8Argv[2]);
					if (gs32Rate == 40)
						//gs32Rate = 5;							//TLSbo59112
						gs32Rate = BIT_RATE_40KBPS;				//TLSbo59112
					else if(gs32Rate == 32)
						//gs32Rate = 4;							//TLSbo59112
						gs32Rate = BIT_RATE_32KBPS;				//TLSbo59112
					else if(gs32Rate == 24)
						//gs32Rate = 3;							//TLSbo59112
						gs32Rate = BIT_RATE_24KBPS;				//TLSbo59112
					else if (gs32Rate == 16)
						//gs32Rate = 2;							//TLSbo59112
						gs32Rate = BIT_RATE_16KBPS;				//TLSbo59112
					else
					{
#ifdef G726_DEBUG_PRINT
						fprintf(stderr, "Invalid bit rate: %s\n", ps8Argv[2]);
						//vDisplayUsage();
#endif
						exit(1);
					}
				}
				else
				{
					vDisplayUsage();
				}

				/* Move argv over the option to the next argument */
				ps8Argv+=2;
				s32Argc-=2;
			}
#ifdef TIME_PROFILE_RVDS
			else if (mystrcmp(ps8Argv[1], "-q") == 0)
#else
			else if (strcmp(ps8Argv[1], "-q") == 0)
#endif
			{
				/* Don't print progress indicator */
				gu32Quiet = G726_TRUE;

				/* Move argv over the option to the next argument */
				ps8Argv++;
				s32Argc--;
			}
#ifdef TIME_PROFILE_RVDS
			else if (mystrcmp(ps8Argv[1], "-?") == 0 || mystrcmp(ps8Argv[1], "-help") == 0)
#else

			else if (strcmp(ps8Argv[1], "-?") == 0 ||
			         strcmp(ps8Argv[1], "-help") == 0)
#endif
			{
				/* Print help */
				vDisplayUsage();
			}
			else
			{
#ifdef G726_DEBUG_PRINT
				fprintf(stderr, "ERROR! Invalid option \"%s\" in command line\n\n",
					ps8Argv[1]);
#endif
				vDisplayUsage();
			}
		}
	}

	if(s32Argc == 3)
	{
		gps8InpFileName = ps8Argv[1];
		gps8OutFileName = ps8Argv[2];
	}
	else
	{
		vDisplayUsage();
	}

	return G726D_OK;
}

#ifdef G726_BIG_ENDIAN
/*****************************************************************************
 *
 * Function: vSwapBytes
 *
 * Description: This fucntions swap the high-byte and low-byte.
 *
 * Returns:    None
 *
 * Arguments:     ps16Buf -> pointer to memory address
 *                 s32Cnt -> the memmory size
 *
 *
 * Range Issues: None
 *
 * Special Issues: None
 *
 *****************************************************************************/

G726_Void vSwapBytes(G726_S16 *ps16Buf, G726_S32 s32Cnt)
{
    G726_S8   s8ch;
    G726_S8   *s8p;

    while(--s32Cnt >= 0)
    {
         s8p = (G726_S8 *)(ps16Buf+s32Cnt);
         s8ch = s8p[0];
         s8p[0] = s8p[1];
         s8p[1] = s8ch;
    }
}
#endif

/*****************************************************************************
 *
 * Function: eG726DDecodeExit
 *
 * Description: This fucntions deallocates all dynamically allocated memory
 *
 * Returns: eG726DReturnType
 *
 * Arguments: psDecConfig -> pointer to decoder config structure
 *              ps16InBuf -> pointer to input speech buffer
 *             ps16OutBuf -> pointer to output buffer
 *
 * Global Variables: gpfFileIn
 *                   gpfFileOut
 *
 * Range Issues: None
 *
 * Special Issues: None
 *
 *****************************************************************************/
eG726DReturnType eG726DDecodeExit(
					sG726DDecoderConfigType *psDecConfig,
					G726_S16  *ps16InBuf,
					G726_S16  *ps16OutBuf
					)
{
    G726_S16 s16Counter;

    if (ps16InBuf != NULL)
    {
		/* free input buffer */
        mem_free (ps16InBuf);
        ps16InBuf = NULL;
    }

    if (ps16OutBuf != NULL)
    {
		/* free output buffer */
        mem_free (ps16OutBuf);
        ps16OutBuf = NULL;
    }

    for (s16Counter=0; s16Counter<G726_MAX_NUM_MEM_REQS; s16Counter++)
    {
        if ((psDecConfig->sG726DMemInfo.asMemInfoSub[s16Counter].pvAPPDBasePtr) != NULL)
        {
            mem_free ((psDecConfig->sG726DMemInfo.asMemInfoSub[s16Counter].pvAPPDBasePtr));
            (psDecConfig->sG726DMemInfo.asMemInfoSub[s16Counter].pvAPPDBasePtr) = NULL;
        }
    }

    if (psDecConfig != NULL)
    {
        mem_free(psDecConfig);
        psDecConfig = NULL;
    }

	if(gpfFileIn)
	{
		fclose(gpfFileIn);
	}

	if(gpfFileOut)
	{
		fclose(gpfFileOut); /* Double free error fix */
	}

    return G726D_OK;
}

/*****************************************************************************
 *
 * Function:  u32G726Decoder
 *
 * Description:
 *
 * Returns:      G726_U32
 *
 * Arguments:    None
 *
 * Global Variables:   gps8HomingFileName
 *                     gps8InpFileName
 *                     gps8OutFileName
 *                     gu32Homing
 *                     gs32Rate
 *                     gs32Law
 *
 * Range Issues: None
 *
 * Special Issues: None
 *
 *****************************************************************************/
G726_U32 u32G726Decoder()
{
    eG726DReturnType          eRetVal;       /* local variable */
    sG726DDecoderConfigType   *psDecConfig;  /* Pointer to decoder config */
    G726_S16                  s16NumMemReqs;
    G726_S16                  s16i=0;
    sG726DMemAllocInfoSubType *psMem;        /* pointer to decoder sub-memory */
    G726_S16                  *ps16InBuf = NULL;  /* pointer to input speech samples*/
    G726_S16                  *ps16OutBuf = NULL; /* pointer to output (decoded) buffer */
	G726_S32                  s32Count;
    G726_S32                s32FrameNo = 0;

#ifndef BIT_MATCH_WITH_REF
    StreamBuf           bitStream;
    G726_U8             *pu8InBuf = NULL;  /* pointer to input speech samples*/
#endif

#ifdef TIME_PROFILE_RVDS_ARM11
    int     prev_clk, curr_clk, clk;
    int     total_clk = 0;
    int     min_clk = 0x7fffffff;
    int     max_clk = 0;
    int     min_no = 0;
    int     max_no = 0;
    FILE    *pfCycle;
	unsigned char chname[] = "[PROFILE-INFO]";
    pfCycle = fopen("g726_cycle_dec.txt", "a");

#endif

#ifdef TIME_PROFILE
    struct timeval tvProf;
    long timeBefore =0, timeAfter=0;
    int retVal = -1;
    int minFrameNumber=0, maxFrameNumber=0, numFrame =0;
    long minFrameTime=0x7FFFFFFF;
    long maxFrameTime =0, totalTime =0;
    long timeVal=0;
    unsigned char timeFlag =0;
    FILE    *fSysTime;
    unsigned char chname[] = "[PROFILE-INFO]";
#ifdef ARMPLAT
    int platform = ARMPLAT;
#else
    int platform = 12;
#endif
#ifdef CPUFREQ
    int cpu_freq = CPUFREQ;
#else    
    int cpu_freq = 1000;
#endif
    int bit_rate = 32000;
    int total_samples = 0;
    double performance = 0.0;
#ifdef __SYMBIAN32__
    fSysTime = fopen ("D:\\g726_dec_sys_time.txt", "a");
#else //__SYMBIAN32__
	fSysTime = fopen ("../audio_performance.txt", "a");
#endif //__SYMBIAN32__
#endif

#ifdef ARM_MIPS_TEST_WINCE
    FILE    *fSysTime;
    fSysTime = fopen ("g726_dec_sys_time.txt", "a");
    if(!fSysTime)
    {
		printf("\nUnable to open log file g726_dec_sys_time.txt\n");
		exit(1);
	}
#endif

    /* allocate fast memory for decoder config structure */
    psDecConfig = (sG726DDecoderConfigType *)alloc_fast(sizeof(sG726DDecoderConfigType));
    if (NULL == psDecConfig)
    {
        return G726_FAILURE;
    }

    /* allocate memory for decoder to use */
    psDecConfig->pvG726DDecodeInfoPtr = NULL;
	/* Not Use */
    psDecConfig->pu8APPDInitializedDataStart = NULL;

    /* initialize config structure memory to NULL */
    for(s16i = 0; s16i <G726_MAX_NUM_MEM_REQS; s16i++)
    {
        (psDecConfig->sG726DMemInfo.asMemInfoSub[s16i].pvAPPDBasePtr) = NULL;
    }

    /* Find decoder memory requiremet */
    eRetVal = eG726DQueryMem(psDecConfig);

    if (eRetVal != G726D_OK)
    {
        /* de-allocate memory allocated for decoder config */
        mem_free(psDecConfig);
        return G726_FAILURE;
    }

    /* Number of memory chunk requested by the decoder */
   	s16NumMemReqs = (G726_S16) psDecConfig->sG726DMemInfo.s32G726DNumMemReqs;
    /* allocate memory requested by the decoder*/
	for(s16i = 0; s16i <s16NumMemReqs; s16i++)
    {
        psMem = &(psDecConfig->sG726DMemInfo.asMemInfoSub[s16i]);
        if (G726_FAST_MEMORY == psMem->s32G726DMemTypeFs)
        {
            psMem->pvAPPDBasePtr = alloc_fast(psMem->s32G726DSize);
			#ifdef MEASURE_HEAP_USAGE
				s32TotalMallocBytes +=  psMem->s32G726DSize;
			#endif
            if (NULL == psMem->pvAPPDBasePtr)
            {
                mem_free(psDecConfig);
                return G726_FAILURE;
            }
        }
        else
        {
            psMem->pvAPPDBasePtr = alloc_slow(psMem->s32G726DSize);
			#ifdef MEASURE_HEAP_USAGE
				s32TotalMallocBytes +=  psMem->s32G726DSize;
			#endif
            if (NULL == psMem->pvAPPDBasePtr)
            {
                mem_free(psDecConfig);
                return G726_FAILURE;
            }
        }
    }

	/* Call decoder init routine */
	eRetVal = eG726DDecodeInit(psDecConfig);
	if (G726D_OK != eRetVal)
	{
		/* free all the dynamic memory and exit */
        eRetVal = eG726DDecodeExit(psDecConfig, ps16InBuf, ps16OutBuf);
        return G726_FAILURE;
	}

	/* allocate memory for input buffer */
	ps16InBuf = alloc_fast(G726_L_BUFFER * sizeof (G726_S16));

    if (NULL == ps16InBuf)
    {
		/* free all the dynamic memory and exit */
        eRetVal = eG726DDecodeExit(psDecConfig, ps16InBuf, ps16OutBuf);
        return G726_FAILURE;
    }

#ifndef BIT_MATCH_WITH_REF
	/* allocate memory for input buffer */
	pu8InBuf = alloc_fast(G726_L_BUFFER * sizeof (G726_U8));

    if (NULL == pu8InBuf)
    {
		/* free all the dynamic memory and exit */
        eRetVal = eG726DDecodeExit(psDecConfig, ps16InBuf, ps16OutBuf);
        return G726_FAILURE;
    }
#endif
	/* allocate memory for output buffer (unpacked) */
	ps16OutBuf = alloc_fast (G726_L_BUFFER * sizeof (G726_S16));

    if (NULL == ps16OutBuf)
    {
		/* free all the dynamic memory and exit */
        eRetVal = eG726DDecodeExit(psDecConfig, ps16InBuf, ps16OutBuf);
        return G726_FAILURE;
    }

	if(gu32Homing)
	{
		FILE    *pfFileHoming;

		pfFileHoming = fopen(gps8HomingFileName, "rb");

		if(pfFileHoming)
		{
			while(!feof(pfFileHoming))
            {
                /* Attempt to read in L_BUFFER bytes: */
#ifdef BIT_MATCH_WITH_REF
                s32Count = fread(ps16InBuf, sizeof(G726_S16), G726_L_BUFFER, pfFileHoming);
#else
                s32Count = fread(pu8InBuf, sizeof(G726_U8), 8*(gs32Rate+2), pfFileHoming);
                s32Count = (s32Count*8)/(gs32Rate+2);
#endif
                if(ferror(pfFileHoming) || (s32Count == 0))
                {
                    break;
                }

#ifdef G726_BIG_ENDIAN
                vSwapBytes(ps16InBuf, s32Count);
#endif
                /* Process s32Count smaples of actually read */
                psDecConfig->s32APPDSampleNum = s32Count;
                psDecConfig->s32APPDBitRate = gs32Rate;
                psDecConfig->s32APPDPcmFormat = gs32Law;

                for (s16i = 0; s16i < s32Count; s16i++)
                    ps16OutBuf[s16i] = 0;

#ifndef BIT_MATCH_WITH_REF
                initStreamBuf (&bitStream, pu8InBuf) ;
                for (s16i = 0; s16i < s32Count; s16i++)
                    ps16InBuf[s16i] = (G726_S16) readU (&bitStream, gs32Rate+2);
#endif

                s32FrameNo++;
#ifdef TIME_PROFILE_RVDS_ARM11
                __asm
                {
                    mrc p15, 0, prev_clk, c15, c12, 0;  /* Read the Monitor Control
                                                           Register in variable prev_clk */
                    orr prev_clk, prev_clk, 0xf;       /* Configure the Monitor conrol register
                                                          to reset the Cycle Count Register and
                                                          enable divide by 64 counter */
                    mcr p15, 0, prev_clk, c15, c12, 0;   /* write back to Monitor
                                                            control regsiter */
                    mrc p15, 0, prev_clk, c15, c12, 1;  /* Read Cycle Count Register into
                                                           variable prev_clk */
                }
#endif
#ifdef TIME_PROFILE_RVDS_ARM9
                dummyCall1();
#endif

#ifdef	ARM_MIPS_TEST_WINCE						/* for taking timing on wince platform*/
		INTERRUPTS_SET(0xDF);  						/*disable interrupt*/
		Flag=QueryPerformanceFrequency(&lpFrequency);
		Flag=QueryPerformanceCounter(&lpPerformanceCountBegin);
#endif

#ifdef TIME_PROFILE
                retVal = gettimeofday(&tvProf, 0);
                if (retVal == 0)
                {
                    timeBefore = tvProf.tv_sec * 1000000 + tvProf.tv_usec;
                    timeFlag = 1;
                }
#endif
                /* decode speech */
                eRetVal = eG726DDecode(psDecConfig, ps16InBuf, ps16OutBuf);

                if (eRetVal != G726D_OK)
                {
                    /* free all the dynamic memory and exit */
                    eRetVal=eG726DDecodeExit(psDecConfig, ps16InBuf,  ps16OutBuf);
                    exit (-1);
                }
#ifdef TIME_PROFILE_RVDS_ARM11
                __asm
                {
                    mrc p15, 0, curr_clk, c15, c12, 1;  /* Read Cycle Count Register in
                                                           variable curr_clk */
                }
                clk = (curr_clk-prev_clk);    /* clk gives the total core cycles per
                                                 frame call */
                total_clk += clk;

                if(clk > max_clk)
                {
                    max_clk = clk;
                    max_no = s32FrameNo;
                }

                if(clk < min_clk)
                {
                    min_clk = clk;
                    min_no = s32FrameNo;
                }

#endif
#ifdef TIME_PROFILE_RVDS_ARM9
            dummyCall2();
#endif
#ifdef TIME_PROFILE		  
                if (timeFlag == 1)
                {
                    retVal = gettimeofday(&tvProf, 0);
                    if (retVal == 0)
                    {
                        timeAfter = tvProf.tv_sec * 1000000 + tvProf.tv_usec;
                        timeVal = timeAfter - timeBefore;
                        total_samples += psDecConfig->s32APPDSampleNum;
                        numFrame++;
                        if (timeVal > maxFrameTime)
                        {
                            maxFrameTime = timeVal;
                            maxFrameNumber = s32FrameNo;
                        }
                        else if (timeVal < minFrameTime)
                        {
                            minFrameTime = timeVal;
                            minFrameNumber = s32FrameNo;
                        }
                        totalTime += timeVal;
                    }
                    timeFlag = 0;
                }
#endif

#ifdef ARM_MIPS_TEST_WINCE
                Flag=QueryPerformanceCounter(&lpPerformanceCountEnd);
                Temp=(((lpPerformanceCountEnd.QuadPart - lpPerformanceCountBegin.QuadPart)*1000000)/(lpFrequency.QuadPart));/*this is the duration*/
                INTERRUPTS_SET(0x1F) ;  					/*enable interrupt*/
                numFrame++;
                if (Temp > maxFrameTime)
                {
                    maxFrameTime = Temp;
                    maxFrameNumber = numFrame;
                }
                else if (Temp < minFrameTime)
                {
                    minFrameTime = Temp;
                    minFrameNumber = numFrame;
                }
                totalTime += Temp;
#endif

            }

			fclose(pfFileHoming);
		}
		else
		{
            eG726DDecodeExit(psDecConfig, ps16InBuf, ps16OutBuf);
#ifdef G726_DEBUG_PRINT
			fprintf(stderr, "Open homeing file %s error\n\n", gps8HomingFileName);
#endif
			vDisplayUsage();
		}
	}

	if(gps8InpFileName)
	{
		gpfFileIn = fopen(gps8InpFileName, "rb");
		if(gpfFileIn == NULL)
		{
#ifdef G726_DEBUG_PRINT
			fprintf(stderr, "Open input file %s error!\n", gps8InpFileName);
#endif
            eG726DDecodeExit(psDecConfig, ps16InBuf, ps16OutBuf);
			exit(1);
		}
	}
	else
	{
	    eG726DDecodeExit(psDecConfig, ps16InBuf, ps16OutBuf);
		vDisplayUsage();
	}
#ifdef OUTPUT_DUMP
	if(gps8OutFileName)
	{
		gpfFileOut = fopen(gps8OutFileName, "wb");
		if(gpfFileOut == NULL)
		{
#ifdef G726_DEBUG_PRINT
			fprintf(stderr, "Create output file %s error!\n", gps8OutFileName);
#endif
            eG726DDecodeExit(psDecConfig, ps16InBuf, ps16OutBuf);
			exit(1);
		}
	}
	else
	{
        eG726DDecodeExit(psDecConfig, ps16InBuf, ps16OutBuf);
		vDisplayUsage();
	}
#endif

	/*************************************************************
	 * Decode speech frame till end of frame
	 *************************************************************/

	/* Read input file and process */
    while(!feof(gpfFileIn))
    {
        /* Attempt to read in G726_L_BUFFER bytes: */
#ifdef BIT_MATCH_WITH_REF
        s32Count = fread(ps16InBuf, sizeof(G726_S16), G726_L_BUFFER, gpfFileIn);
#else
        s32Count = fread(pu8InBuf, sizeof(G726_U8), 8*(gs32Rate+2), gpfFileIn);
        s32Count = (s32Count*8)/(gs32Rate+2);
#endif
        if(ferror(gpfFileIn) || (s32Count == 0))
        {
            break;
        }

#ifdef G726_BIG_ENDIAN
        vSwapBytes(ps16InBuf, s32Count);
#endif
        /* Process s32Count smaples of actually read */
        psDecConfig->s32APPDSampleNum = s32Count;
        psDecConfig->s32APPDBitRate = gs32Rate;
        psDecConfig->s32APPDPcmFormat = gs32Law;

        for (s16i = 0; s16i < s32Count; s16i++)
            ps16OutBuf[s16i] = 0;

#ifndef BIT_MATCH_WITH_REF
        initStreamBuf (&bitStream, pu8InBuf) ;
        for (s16i = 0; s16i < s32Count; s16i++)
            ps16InBuf[s16i] = (G726_S16) readU (&bitStream, gs32Rate+2);
#endif

        s32FrameNo++;
#ifdef MEASURE_STACK_USAGE
	PAINT_STACK (ps32BaseSP, ps32SP, s32StackCount);
#endif
#ifdef TIME_PROFILE_RVDS_ARM11
        __asm
        {
            mrc p15, 0, prev_clk, c15, c12, 0;  /* Read the Monitor Control
                                                   Register in variable prev_clk */
            orr prev_clk, prev_clk, 0xf;       /* Configure the Monitor conrol register
                                                  to reset the Cycle Count Register and
                                                  enable divide by 64 counter */
            mcr p15, 0, prev_clk, c15, c12, 0;   /* write back to Monitor
                                                    control regsiter */
            mrc p15, 0, prev_clk, c15, c12, 1;  /* Read Cycle Count Register into
                                                   variable prev_clk */
        }
#endif
#ifdef TIME_PROFILE_RVDS_ARM9
            dummyCall1();
#endif
#ifdef TIME_PROFILE
        retVal = gettimeofday(&tvProf, 0);
        if (retVal == 0)
        {
            timeBefore = tvProf.tv_sec * 1000000 + tvProf.tv_usec;
            timeFlag = 1;
        }
#endif

#ifdef	ARM_MIPS_TEST_WINCE						/* for taking timing on wince platform*/
		INTERRUPTS_SET(0xDF);  						/*disable interrupt*/
		Flag=QueryPerformanceFrequency(&lpFrequency);
		Flag=QueryPerformanceCounter(&lpPerformanceCountBegin);
#endif

		/* decode speech */
        eRetVal = eG726DDecode(psDecConfig, ps16InBuf, ps16OutBuf);

        if (eRetVal != G726D_OK)
        {
            /* free all the dynamic memory and exit */
            eRetVal=eG726DDecodeExit(psDecConfig, ps16InBuf,  ps16OutBuf);
            exit (-1);
        }



#ifdef TIME_PROFILE_RVDS_ARM11
        __asm
        {
            mrc p15, 0, curr_clk, c15, c12, 1;  /* Read Cycle Count Register in
                                                   variable curr_clk */
        }
        clk = (curr_clk-prev_clk);    /* clk gives the total core cycles per
                                         frame call */
        total_clk += clk;

        if(clk > max_clk)
        {
            max_clk = clk;
            max_no = s32FrameNo;
        }

        if(clk < min_clk)
        {
            min_clk = clk;
            min_no = s32FrameNo;
        }
#endif
#ifdef TIME_PROFILE_RVDS_ARM9
            dummyCall2();
#endif
#ifdef MEASURE_STACK_USAGE
	GET_STACK_USAGE (ps32BaseSP, ps32SP, s32StackCount, s32StackValue);
	if (s32PeakStack < s32StackValue)
		s32PeakStack = s32StackValue;
#endif
#ifdef TIME_PROFILE        
        if (timeFlag == 1)
        {
            retVal = gettimeofday(&tvProf, 0);
            if (retVal == 0)
            {
                timeAfter = tvProf.tv_sec * 1000000 + tvProf.tv_usec;
                timeVal = timeAfter - timeBefore;
                total_samples += psDecConfig->s32APPDSampleNum;
                numFrame++;
                if (timeVal > maxFrameTime)
                {
                    maxFrameTime = timeVal;
                    maxFrameNumber = s32FrameNo;
                }
                else if (timeVal < minFrameTime)
                {
                    minFrameTime = timeVal;
                    minFrameNumber = s32FrameNo;
                }
                totalTime += timeVal;
            }
            timeFlag = 0;
        }
#endif

#ifdef ARM_MIPS_TEST_WINCE
        Flag=QueryPerformanceCounter(&lpPerformanceCountEnd);
        Temp=(((lpPerformanceCountEnd.QuadPart - lpPerformanceCountBegin.QuadPart)*1000000)/(lpFrequency.QuadPart));/*this is the duration*/
        INTERRUPTS_SET(0x1F) ;  					/*enable interrupt*/
		numFrame++;
		if (Temp > maxFrameTime)
		{
			maxFrameTime = Temp;
			maxFrameNumber = numFrame;
		}
		else if (Temp < minFrameTime)
		{
			minFrameTime = Temp;
			minFrameNumber = numFrame;
		}
		totalTime += Temp;
#endif

#ifdef OUTPUT_DUMP
        for(s16i = 0; s16i< s32Count; s16i++)
        {
            if(G726_PCM_LINEAR != gs32Law)
            {
                fprintf(gpfFileOut, "%c", ps16OutBuf[s16i] & 0xff);
#ifdef BIT_MATCH_WITH_REF
                fprintf(gpfFileOut, "%c", 0);
#endif
            }
            else
            {
                fprintf(gpfFileOut, "%c", ps16OutBuf[s16i] & 0xff);
                fprintf(gpfFileOut, "%c", ps16OutBuf[s16i] >> 8 );
            }
        }
#endif
    }

  	/*************************************************************
   	 *Closedown speech coder
   	************************************************************/
    /* free all the dynamic memory and exit */
#ifdef TIME_PROFILE_RVDS_ARM11
	#ifdef MEASURE_STACK_USAGE
    #ifdef MEASURE_HEAP_USAGE
 	fprintf(stderr,"\n\n%s\t%s\t%s\t%d\t%ld\t%ld\t%d\t%d\t%d\t%ld\t%ld\t%ld\t\n\n",chname,gps8InpFileName,gps8HomingFileName?gps8HomingFileName:"null",gs32Rate,\
				(max_clk *64),(min_clk *64),s32FrameNo,max_no,min_no,(total_clk*64),s32PeakStack,s32TotalMallocBytes);
 	#endif
 	#endif

    #ifndef MEASURE_STACK_USAGE
    #ifndef MEASURE_HEAP_USAGE
 		fprintf(stderr,"\n\n%s\t%s\t%s\t%d\t%ld\t%ld\t%d\t%d\t%d\t%ld\t\n\n",chname,gps8InpFileName,gps8HomingFileName?gps8HomingFileName:"null",gs32Rate,\
				(max_clk *64),(min_clk *64),s32FrameNo,max_no,min_no,(total_clk*64));
 	#endif
 	#endif
    fprintf(pfCycle, "decoder ");
    if (gs32Law == 0) fprintf(pfCycle, "-law l ");
    else if (gs32Law == 1) fprintf(pfCycle, "-law a ");
    else if (gs32Law == 2) fprintf(pfCycle, "-law u ");
    else fprintf(pfCycle, "-law err!!! ");

    if (gs32Rate == BIT_RATE_40KBPS) fprintf(pfCycle, "-rate 40 ");
    else if (gs32Rate == BIT_RATE_32KBPS) fprintf(pfCycle, "-rate 32 ");
    else if (gs32Rate == BIT_RATE_24KBPS) fprintf(pfCycle, "-rate 24 ");
    else if (gs32Rate == BIT_RATE_16KBPS) fprintf(pfCycle, "-rate 16 ");
    else fprintf(pfCycle, "-rate err!!! ");

    fprintf(pfCycle, " %s ", gps8InpFileName);
    if(gu32Homing)
    {
        fprintf(pfCycle, " %s ", gps8HomingFileName);
    } else
    {
        fprintf(pfCycle, " null ");
    }
    fprintf(pfCycle, " %s ", gps8OutFileName);
    /*
    fprintf(pfCycle, "-rate %d -homing %s -input %s -output %s\n", gs32Rate, gps8HomingFileName?gps8HomingFileName:"null", gps8InpFileName, gps8OutFileName);
    */
    /* number of frames in one sec = 8000/G726_L_BUFFER */
    /*
    fprintf(pfCycle, "Peak no   : %d,      Peak MCPS   : %4.2f\n", max_no, (float)((float)(max_clk *64*8)/(G726_L_BUFFER*1000)));
    fprintf(pfCycle, "Min  no   : %d,      Min MCPS    : %4.2f\n", min_no, (float)((float)(min_clk*64*8.0)/(G726_L_BUFFER*1000)));
    fprintf(pfCycle, "Total No of Sample    : %d,   average MCPS    : %4.2f\n", s32FrameNo*64, (float)((float)((float)((float)((total_clk*64)/s32FrameNo)*(8000/G726_L_BUFFER))/1000000)));
    */
    fprintf(pfCycle, " %d %4.2f", max_no, (float)((float)(max_clk *64*8)/(G726_L_BUFFER*1000)));
    fprintf(pfCycle, " %d %4.2f", min_no, (float)((float)(min_clk*64*8.0)/(G726_L_BUFFER*1000)));
    fprintf(pfCycle, " %d %4.2f", s32FrameNo*64, (float)((float)((float)((float)((total_clk*64)/s32FrameNo)*(8000/G726_L_BUFFER))/1000000)));

    fprintf(pfCycle, "\n");
    fclose(pfCycle);
#endif

#ifdef TIME_PROFILE
	printf("\nDecoder");
	printf("\n%s\t%s\t%s\t%d\t%ld\t%ld\t%d\t%d\t%d\t%ld\t ",chname,gps8InpFileName,gps8HomingFileName?gps8HomingFileName:"null",gs32Rate,
				maxFrameTime,minFrameTime,numFrame,maxFrameNumber,minFrameNumber,totalTime);
	
	printf("\npsDecConfig->s32APPDBitRate:%d\n", psDecConfig->s32APPDBitRate);
	switch(psDecConfig->s32APPDBitRate)
	{
	case BIT_RATE_16KBPS:
		bit_rate = 16000;
		break;
	case BIT_RATE_24KBPS:
		bit_rate = 24000;
		break;
	case BIT_RATE_32KBPS:
		bit_rate = 32000;
		break;
	case BIT_RATE_40KBPS:
		bit_rate = 40000;
		break;
	default:
		bit_rate = 32000;
		break;
	}
	fprintf (fSysTime,"\n|\tAudio Format\t|\tPlat\t|\tSamplerate\t|\tCh\t|\tBiterate\t|\tBit\t|\tPerf(MIPS)\t|\tAudio file name\n");
	performance = (double)totalTime * 0.000001*cpu_freq*8000/total_samples;
       fprintf(fSysTime, "|\tG.726 Decoder\t|\tARM%d\t|\t%9d\t|\t%d\t|\t%8d\t|\t%d\t|\t%.8f\t|%s\n", platform, 8000, 1, bit_rate, 16, performance, gps8InpFileName);

//	fprintf(fSysTime,"\n%s\t%s\t%s\t%d\t%ld\t%ld\t%d\t%d\t%d\t%ld\t ",chname,gps8InpFileName,gps8HomingFileName?gps8HomingFileName:"null",gs32Rate,
//				maxFrameTime,minFrameTime,numFrame,maxFrameNumber,minFrameNumber,totalTime);
    /*fprintf(fSysTime,"\tPeak Frame no: %ld,  Peak Time: %ld\n", maxFrameNumber, maxFrameTime);
    fprintf(fSysTime,"\tMin  Frame no: %ld,  Min Time: %ld\n", minFrameNumber, minFrameTime);
    fprintf(fSysTime,"\tTotal Frame : %ld, Average Time: %ld\n", (s32FrameNo*64), s32FrameNo?totalTime/numFrame:0);
    fprintf(fSysTime, "----------------------------------------\n\n");*/
    fclose(fSysTime);
#endif
#ifdef __SYMBIAN32__
#ifdef MEASURE_HEAP_USAGE
			printf("\n Stack and Heap are  %d\t%d\t",s32PeakStack,s32TotalMallocBytes);
#endif
#endif

#ifdef	ARM_MIPS_TEST_WINCE
	printf("\nDencoder");
	printf("\n%s\t%s\t%ld",chname,gps8InpFileName,maxFrameTime);
	printf("\t%ld",minFrameTime);
	printf("\t%d\t%d\t%d\t%ld\t",numFrame,maxFrameNumber,minFrameNumber,totalTime);
	fprintf(fSysTime,"\n%s\t%s\t%ld",chname,gps8InpFileName,maxFrameTime);
	fprintf(fSysTime,"\t%ld",minFrameTime);
	fprintf(fSysTime,"\t%d\t%d\t%d\t%ld\t",numFrame,maxFrameNumber,minFrameNumber,totalTime);
	fclose(fSysTime);
#endif

    eRetVal = eG726DDecodeExit(psDecConfig, ps16InBuf,  ps16OutBuf);
    return G726_SUCCESS;
		}

/*****************************************************************************
 * Function:  main
 *
 * Description: main function. Function is defined in application space.
 *
 * Returns: G726_SUCCESS/G726_FAILURE
 *
 * Arguments:   s32Argc
 *              ps8Argv
 *
 * Global Variables:   gps8HomingFileName
 *                     gps8InpFileName
 *                     gps8OutFileName
 *                     gu32Quiet
 *                     gu32Homing
 *                     gs32Rate
 *                     gs32Law
 *
 * Range Issues: None
 *
 * Special Issues: None
 *
 *****************************************************************************/
#ifdef __WINCE
G726_S32 _tmain(G726_S32 s32Argc, G726_S8 *ps8Argv[])
#else
G726_S32 main(G726_S32 s32Argc, G726_S8 *ps8Argv[])
#endif
{

eG726DReturnType  eRetVal;       /* local variable */

#ifdef __WINCE
	_TCHAR *arg_word;
	char *arg_byte;
	int count;
#endif

#ifdef __WINCE
	for (count = 1; count < s32Argc; count++)
	{
		arg_word =(_TCHAR *)ps8Argv[count];
		arg_byte = ps8Argv[count];

		while(*(arg_word) != '\0')
		{
			*arg_byte++=(char)*arg_word++;
		}
			*arg_byte=(char)'\0';
	}
#endif

    printf("Running %s\n", G726D_get_version_info());

    eRetVal = eG726DProcessCmdLineOptions(s32Argc, ps8Argv);

    if (eRetVal != G726D_OK)
    {
        exit(G726_FAILURE);
    }

#ifdef G726_DEBUG_PRINT
	if(!gu32Quiet)
	{
		if(gs32Law == G726_PCM_ALAW)
			printf("A-law\n");
		else if(gs32Law == G726_PCM_ULAW)
			printf("mu-law\n");
		else if(gs32Law == G726_PCM_LINEAR)
			printf("Linear\n");

		switch(gs32Rate)
		{
		//case 2:						//TLSbo59112
		case BIT_RATE_16KBPS:			//TLSbo59112
			printf("16 kbps\n");
			break;
		//case 3:						//TLSbo59112
		case BIT_RATE_24KBPS:			//TLSbo59112
			printf("24 kbps\n");
			break;
		//case 4:						//TLSbo59112
		case BIT_RATE_32KBPS:			//TLSbo59112
			printf("32 kbps\n");
			break;
		//case 5:						//TLSbo59112
		case BIT_RATE_40KBPS:			//TLSbo59112
			printf("40 kbps\n");
			break;
		default:
			break;
		}

		if(gu32Homing)
		{
			printf("Homing file: %s\n", gps8HomingFileName);
		}
		else
		{
			printf("No homing file, using reset state.\n");
		}

		printf("%s %s\n", gps8InpFileName, gps8OutFileName);
	}
#endif

	u32G726Decoder();

	return G726_SUCCESS;
}

#ifdef TIME_PROFILE_RVDS_ARM9
void dummyCall1(void)
{
	uiframe++;
}

void dummyCall2(void)
{
	uiframe++;
}
#endif

/*************************<END OF THE FILE>**********************************/
