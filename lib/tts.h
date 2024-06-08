#ifndef TTS_H
#define TTS_H

#include "common.h"

#define TOTAL_AUDIO_BUFFER_SIZE 4194304 // 4Mb
#define CHUNK_AUDIO_BUFFER_SIZE 131072   // 131Kb

typedef struct {
  char *client;
  char *ie; // input encode
  char *tl; // output encode
} TTSParams;


typedef struct {
  const char *url;
  Slice response;
} thread_data_t;

void genarate_tts_url(char *url, const TTSParams params, Slice text);

#endif
