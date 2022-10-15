/*
 * Copyright 2019-2020 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef WAVE_FUNC_H
#define WAVE_FUNC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define WAVE_HEAD_SIZE 44 + 14 + 16

struct waveheader {
	int   riffchunkoffset;
	int   riffchunksize;
	int   fmtchunkoffset;
	int   fmtchunksize;
	short numchannels;
	int   samplerate;
	int   byterate;
	short blockalign;
	short bitspersample;
	int   datachunkoffset;
	int   datachunksize;

	char header[WAVE_HEAD_SIZE];
};

struct wavefile {
	FILE    *fp;
	struct  waveheader *header;
	uint8_t *buffer;
	size_t  size;
	long int pos;
};

int header_parser(FILE *src, struct waveheader *waveheader);
void header_update(FILE *dst, struct waveheader *waveheader);
void header_write(FILE *dst, struct waveheader *waveheader);
struct wavefile *wavefile_open(const char* filename, const char* mode);
struct wavefile *wavefile_new(const char* filename, const char* mode);
size_t wavefile_read(struct wavefile *wave, void **buffers, size_t count);
size_t wavefile_write(struct wavefile *wave, void **buffers, size_t count);
size_t wavefile_get_sample_size(struct wavefile *wave);
size_t wavefile_get_container_size(size_t sample_size);
void wavefile_close(struct wavefile *wave);
size_t pdmfile_read(struct wavefile *wave, uint32_t *buffer, size_t count);
size_t pcmfile_write(struct wavefile *wave, int32_t *buffer, size_t count);

#endif
