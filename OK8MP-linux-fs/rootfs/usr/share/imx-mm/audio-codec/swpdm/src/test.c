/*
 * Copyright 2020 NXP.
 *
 * PDM to PCM simple test application using libimxswpdm
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <imx-swpdm.h>
#include "wave_func.h"

static const char short_options[] = "hi:o:t:b:c:g:r:";
static const struct option long_options[] = {
	{ "help",	0, 0, 'h' },
	{ "type",	1, 0, 't' },
	{ "block",	1, 0, 'b' },
	{ "channels",	1, 0, 'c' },
	{ "gain",	1, 0, 'g' },
	{ "rate",	1, 0, 'r' },
	{ "outFile",	1, 0, 'o' },
	{ "inFile",	1, 0, 'i' },
	{ 0, 0, 0, 0 }
};

char *infile = NULL;
char *outfile = NULL;
int samplerate = 0;
unsigned blocksize = 40;
void print_help(const char *argv[]);
int parse_arguments(int argc, const char *argv[], afe_t *afe);
int update_wav_hdr(FILE* outfile, int32_t samplerate,
		   int32_t channels, int32_t nsamples);

int main (int argc, const char *argv[])
{
	size_t in_blocksize, out_blocksize, read, write = 0;
	struct wavefile *in_wav, *out_wav = NULL;
	afe_t afe;
	bool ret;

	if (parse_arguments(argc, argv, &afe) != 0) {
		perror("Fail to parse arguments");
		exit(1);
	}

	in_wav = wavefile_open(infile, "rb");
	if (in_wav == NULL) {
		perror("Fail to open wav file");
		exit(1);
	}

	out_wav = wavefile_new(outfile, "wb");
	if (out_wav == NULL) {
		perror("Fail to create wav file");
		exit(1);
	}

	if (in_wav->header->bitspersample != 32) {
		printf("unsupported bits per sample %d\n",
		       in_wav->header->bitspersample);
		goto err_header_parse;
	}

	if (afe.numberOfChannels != in_wav->header->numchannels)
		afe.numberOfChannels = (unsigned) in_wav->header->numchannels;

	if (afe.numberOfChannels != 4) {
		printf("unsupported number of channels %d\n",
		       afe.numberOfChannels);
		goto err_header_parse;
	}

	/* update output file header */
	memcpy(out_wav->header, in_wav->header, sizeof(struct waveheader));
	out_wav->header->samplerate = samplerate;
	header_write(out_wav->fp, out_wav->header);

	ret = constructAfeCicDecoder(afe.cicDecoderType, &afe,
		afe.outputGainFactor, blocksize);
	if (ret == false) {
		perror("Fail to create AfeCicDecoder");
		return SWPDM_ERR;
	}

	in_blocksize = afe.inputBufferSizePerChannel * afe.numberOfChannels;
	out_blocksize = afe.outputBufferSizePerChannel * afe.numberOfChannels * 4;

	do {
		read = pdmfile_read(in_wav, afe.inputBuffer, in_blocksize);
		if (read) {
			processAfeCic(&afe);
			write = pcmfile_write(out_wav, afe.outputBuffer, out_blocksize);
		}
	} while (read);

	deleteAfeCicDecoder(&afe);

	wavefile_close(out_wav);
	wavefile_close(in_wav);

	return 0;

err_header_parse:
	wavefile_close(out_wav);
	wavefile_close(in_wav);

	exit(1);
}

cic_t cic_decoder_type(unsigned long type)
{
	switch (type) {
	case 12: return CIC_pdmToPcmType_cic_order_5_cic_downsample_12;
	case 16: return CIC_pdmToPcmType_cic_order_5_cic_downsample_16;
	case 24: return CIC_pdmToPcmType_cic_order_5_cic_downsample_24;
	case 32: return CIC_pdmToPcmType_cic_order_5_cic_downsample_32;
	case 48: return CIC_pdmToPcmType_cic_order_5_cic_downsample_48;
	default: return CIC_pdmToPcmType_cic_order_5_cic_downsample_unavailable;
	}
}

int parse_arguments(int argc, const char *argv[], afe_t *afe)
{
	int c, option_index;
	unsigned long arg;

	/* afe defaults */
	afe->outputGainFactor = 0;
	afe->numberOfChannels = 4;

	while ((c = getopt_long(argc, (char * const*)argv, short_options,
		long_options, &option_index)) != -1) {
		switch (c) {
		case 't':
			arg = strtoul(optarg, NULL, 10);
			afe->cicDecoderType = cic_decoder_type(arg);
			if (afe->cicDecoderType ==
				CIC_pdmToPcmType_cic_order_5_cic_downsample_unavailable) {
				printf("CIC downsample not supported\n");
				exit(1);
			}
			break;
		case 'b':
			arg = strtoul(optarg, NULL, 10);
			blocksize = (unsigned) arg;
			break;
		case 'c':
			arg = strtoul(optarg, NULL, 10);
			afe->numberOfChannels = (unsigned) arg;
			break;
		case 'g':
			afe->outputGainFactor = strtof(optarg, NULL);
			break;
		case 'r':
			arg = strtoul(optarg, NULL, 10);
			samplerate = (int) arg;
			break;
		case 'i':
			infile = optarg;
			break;
		case 'o':
			outfile = optarg;
			break;
		case 'h':
			print_help(argv);
			exit(1);
		default:
			printf("Unknown command -%c \n", c);
			exit(1);
		}
	}

	return 0;
}

void print_help(const char *argv[])
{
	printf("Usage: %s \n"
		"  --help     (h): this screen\n"
		"  --type     (t): 5 order cic decoder type\n"
		"                  [12, 16, 24, 32, 48]\n"
		"  --block    (b): number of samples to output per channel\n"
		"                  divided by 16\n"
		"  --channels (c): number of channels\n"
		"  --gain     (g): output multipler scale factor\n"
		"                  if not set, value is calculated\n"
		"  --rate     (r): sample rate\n"
		"  --outFile  (o): output file\n"
		"  --inFile   (i): input file\n"
		,argv[0]);
	return;
}
