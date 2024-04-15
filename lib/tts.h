#ifndef TTS_H
#define TTS_H

#include "common.h"

#include <vlc/vlc.h>

#define TOTAL_AUDIO_BUFFER_SIZE 1048576 // 1Mb
#define CHUNK_AUDIO_BUFFER_SIZE 65536   // 65Kb

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
void play_audio(MemAudioData mem, float speed);

#endif
