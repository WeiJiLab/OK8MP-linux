/*
 * Copyright 2019-2020 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "wave_func.h"


int header_parser(FILE *src, struct waveheader *waveheader)
{

	int format_size;
	char chunk_id[4];
	int chunk_size;
	char *header = &waveheader->header[0];

	/* check the "RIFF" chunk */
	fseek(src, 0, SEEK_SET);
	fread(chunk_id, 4, 1, src);
	while (strncmp(chunk_id, "RIFF", 4) != 0){
		fread(&chunk_size, 4, 1, src);
		fseek(src, chunk_size, SEEK_CUR);
		if(fread(chunk_id, 4, 1, src) == 0) {
			printf("Wrong wave file format \n");
			return -1;
		}
	}
	fseek(src, -4, SEEK_CUR);
	fread(&header[0], 1, 12, src);

	/* check the "fmt " chunk */
	fread(chunk_id, 4, 1, src);
	while (strncmp(chunk_id, "fmt ", 4) != 0){
		fread(&chunk_size, 4, 1, src);
		fseek(src, chunk_size, SEEK_CUR);
		if(fread(chunk_id, 4, 1, src) == 0) {
			printf("Wrong wave file format \n");
			return -1;
		}
	}
	/* fmt chunk size */
	fread(&format_size, 4, 1, src);

	fseek(src, -8, SEEK_CUR);
	fread(&header[12], 1, format_size + 8, src);

	/* AudioFormat(2) */

	/* NumChannel(2) */
	waveheader->numchannels = *(short *)&header[12 + 8 + 2];

	/* SampleRate(4) */
	waveheader->samplerate = *(int *)&header[12 + 8 + 2 + 2];

	/* ByteRate(4) */

	/* BlockAlign(2) */
	waveheader->blockalign = *(short *)&header[12 + 8 + 2 + 2 + 4 + 4];

	/* BitsPerSample(2) */
	waveheader->bitspersample = *(short *)&header[12 + 8 + 2 + 2 + 4 + 4 + 2];


	/* check the "data" chunk */
	fread(chunk_id, 4, 1, src);
	while (strncmp(chunk_id, "data", 4) != 0) {
		fread(&chunk_size, 4, 1, src);
		/* seek to next chunk if it is not "data" chunk */
		fseek(src, chunk_size, SEEK_CUR);
		if(fread(chunk_id, 4, 1, src) == 0) {
		    printf("No data chunk found \nWrong wave file format \n");
		    return -1;
		}
	}
	/* wave data length */
	fread(&waveheader->datachunksize, 4, 1, src);
	fseek(src, -8, SEEK_CUR);
	fread(&header[format_size + 20], 1, 8, src);

	return 0;
}

void header_update(FILE *dst, struct waveheader *waveheader)
{
	int format_size;
	char *header = &waveheader->header[0];

	format_size = *(int *)&header[16];

	*(int *)&header[4] = waveheader->datachunksize + 20 + format_size;
	*(int *)&header[24 + format_size] = waveheader->datachunksize;
	*(int *)&header[24] = waveheader->samplerate;
	*(int *)&header[28] =
		waveheader->samplerate * (waveheader->bitspersample / 8)
					* waveheader->numchannels;
	*(unsigned short *)&header[32] = waveheader->blockalign;
	*(unsigned short *)&header[34] = waveheader->bitspersample;

	fseek(dst, 4,  SEEK_SET);
	fwrite(&header[4], 4, 1, dst);

	fseek(dst, 24,  SEEK_SET);
	fwrite(&header[24], 12, 1, dst);

	fseek(dst, 24 + format_size,  SEEK_SET);
	fwrite(&header[24 + format_size], 4, 1, dst);

	fseek(dst, 0, SEEK_END);
}

void header_write(FILE *dst, struct waveheader *waveheader)
{
	int format_size;
	char *header = &waveheader->header[0];
	int i = 0;

	format_size = *(int *)&header[16];

	while (i < (format_size + 28)) {
		fwrite(&header[i], 1, 1, dst);
		i++;
	}
}

int header_update_datachunksize(struct waveheader *waveheader)
{
	int format_size;
	char *header = &waveheader->header[0];

	*(int *)&header[24 + format_size] = waveheader->datachunksize;
	*(int *)&header[4] = waveheader->datachunksize + 20 + format_size;

	return 0;
}

int header_update_blockalign(struct waveheader *waveheader)
{
	int format_size;
	char *header = &waveheader->header[0];

	*(unsigned short *)&header[32] = waveheader->blockalign;

	return 0;
}

int header_update_bitspersample(struct waveheader *waveheader)
{
	char *header = &waveheader->header[0];

	*(unsigned short *)&header[34] = waveheader->bitspersample;
	*(int *)&header[28] =
		waveheader->samplerate * (waveheader->bitspersample / 8)
					* waveheader->numchannels;
	return 0;
}

struct wavefile *wavefile_new(const char *filename, const char *mode)
{
	struct wavefile *wave = NULL;
	int ret;

	wave = malloc(sizeof(struct wavefile));
	if (wave == NULL)
		return NULL;

	wave->header = NULL;
	wave->header = malloc(sizeof(struct waveheader));
	if (wave->header == NULL)
		return NULL;

	wave->fp = NULL;
	wave->fp = fopen(filename, mode);
	if (wave->fp == NULL)
		goto open_error;

	wave->buffer = NULL;
	wave->size = 0;

	return wave;

open_error:
	free(wave->header);
	return NULL;
}

struct wavefile *wavefile_open(const char* filename, const char* mode)
{
	struct wavefile *wave = NULL;
	int ret;

	wave = malloc(sizeof(struct wavefile));
	if (wave == NULL)
		return NULL;

	wave->header = NULL;
	wave->header = malloc(sizeof(struct waveheader));
	if (wave->header == NULL)
		return NULL;

	wave->fp = NULL;
	wave->fp = fopen(filename, mode);
	if (wave->fp == NULL)
		goto parse_error;

	ret = header_parser(wave->fp, wave->header);
	if (ret < 0)
		goto parse_error;

	wave->pos = ftell(wave->fp);
	wave->buffer = NULL;
	wave->size = 0;

	return wave;

parse_error:
	free(wave->header);
	return NULL;
}

void wavefile_close(struct wavefile *wave) {
	if (wave->header)
		free(wave->header);
	if (wave->buffer)
		free(wave->buffer);
	fclose(wave->fp);
	free(wave);
}

long int wavefile_position(struct wavefile *wave) {
	long int pos = ftell(wave->fp);

	if (pos == -1L)
		return -1L;

	return ((pos - WAVE_HEAD_SIZE) / wave->header->blockalign);
}

static const int wave_container_lenght[5] = {0, 1, 2, 4, 4};
static inline size_t wave_container_len(size_t sample_len) {
	return wave_container_lenght[sample_len];
}

size_t wavefile_get_sample_size(struct wavefile *wave) {
	size_t sample_len;
	sample_len = wave->header->blockalign / wave->header->numchannels;

	return sample_len;
}

size_t wavefile_get_container_size(size_t sample_size) {
	size_t ret = 1;
	int i = 0;

	do {
		if (ret >= sample_size)
			return ret;
		++i;
		ret <<= 1;
	} while ((size_t) i < 8 * sizeof(size_t));

	return (size_t)(1 << (i -1));
}

size_t wavefile_read(struct wavefile *wave, void **buffers, size_t count) {
	size_t read, remain, buf_len, sample_len, container_len;
	struct waveheader *header = wave->header;
	size_t i, j, k, pos;

	pos = (size_t) wave->pos;
	if (pos == -1L )
		return 0;

	remain = (header->datachunksize / header->blockalign) - pos;
	count = (count <= remain) ? count : remain;
	if (count == 0)
		return 0;

	sample_len = wavefile_get_sample_size(wave);
	container_len = wave_container_len(sample_len);
	buf_len = header->numchannels * count * sample_len;

	if (wave->size < buf_len || wave->buffer == NULL) {
		if (wave->buffer)
			free(wave->buffer);
		wave->size = buf_len;
		wave->buffer = malloc(buf_len);
		if (wave->buffer == NULL) {
			wave->size = 0;
			return 0;
		}
	}

	read = fread(wave->buffer, sample_len,
		     header->numchannels * count, wave->fp);
	if (ferror(wave->fp))
		return 0;

	wave->pos = ftell(wave->fp);

	for (i = 0; i < header->numchannels; ++i) {
		for (j = 0; j < read / header->numchannels; ++j) {
			for (k = 0; k < sample_len; ++k) {
				((uint8_t*)buffers[i])[j * container_len + k] =
					wave->buffer[j * header->numchannels * sample_len + i * sample_len + k];
			}
			/* sing */
			if (((uint8_t*)buffers[i])[j * container_len + k - 1] & 0x80) {
				for (; k < container_len; ++k)
					((uint8_t*)buffers[i])[j * container_len + k] = 0xff;
			} else {
				for (; k < container_len; ++k)
					((uint8_t*)buffers[i])[j * container_len + k] = 0x00;
			}
		}
	}

	return read / header->numchannels;
}

size_t wavefile_write(struct wavefile *wave, void **buffers, size_t count) {
	struct waveheader *header = wave->header;
	size_t write, buf_len, sample_len, container_len;
	size_t i, j, k, t;
	long int pos;

	if (count == 0)
		return 0;

	/* update file position */
	pos = ftell(wave->fp);
	if (pos == -1L )
		return 0;

	sample_len = wavefile_get_sample_size(wave);
	container_len = wave_container_len(sample_len);
	buf_len = header->numchannels * count * sample_len;

	if (wave->size < buf_len || wave->buffer == NULL) {
		if (wave->buffer)
			free(wave->buffer);
		wave->size = buf_len;
		wave->buffer = malloc(buf_len);
		if (wave->buffer == NULL) {
			wave->size = 0;
			return 0;
		}
	}

	for (i = 0; i < header->numchannels; ++i) {
		for (j = 0; j < count; ++j) {
			for (k = 0; k < sample_len; ++k) {
				wave->buffer[j * header->numchannels * sample_len +
				i * sample_len + k] = ((uint8_t*)buffers[i])[j * container_len + k];
			}
		}
	}

	write = fwrite(wave->buffer, sample_len, header->numchannels * count, wave->fp);
	if (ferror(wave->fp))
		return 0;

	wave->header->datachunksize += write + sample_len;
	pos = ftell(wave->fp);
	if (pos == -1L)
		return 0;
	/* update header chunk_size */
	header_update(wave->fp, wave->header);
	if (fseek(wave->fp, pos, SEEK_SET) != 0)
		return 0;

	return (write / header->numchannels);
}

size_t pdmfile_read(struct wavefile *wave, uint32_t *buffer, size_t count) {
	struct waveheader *header = wave->header;
	size_t read, remain, sample_len, pos;

	pos = (size_t)wave->pos;
	if (pos == -1L )
		return 0;

	remain = (header->datachunksize / header->blockalign) - pos;
	count = (count <= remain) ? count : remain;
	if (count == 0)
		return 0;

	sample_len = wavefile_get_sample_size(wave);

	read = fread(buffer, sample_len, count, wave->fp);
	if (ferror(wave->fp))
		return 0;

	wave->pos = ftell(wave->fp);

	return read;
}

size_t pcmfile_write(struct wavefile *wave, int32_t *buffer, size_t count) {
	struct waveheader *header = wave->header;
	size_t write, sample_len, pos;

	sample_len = wavefile_get_sample_size(wave);

	write = fwrite(buffer, sample_len, count, wave->fp);
	if (ferror(wave->fp))
		return 0;

	wave->header->datachunksize += write + sample_len;
	pos = ftell(wave->fp);
	if (pos == -1L)
		return 0;
	/* update header chunk_size */
	header_update(wave->fp, wave->header);
	if (fseek(wave->fp, pos, SEEK_SET) != 0)
		return 0;

	return write;
}
