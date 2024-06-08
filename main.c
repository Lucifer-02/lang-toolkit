#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/common.h"
#include "lib/cvector.h"
#include "lib/cvector_utils.h"
#include "lib/player.h"
#include "lib/trans.h"
#include "lib/tts.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Please run with arguments: ./main <speed> <mode> <text>\n");
    printf("\tMode 0 - translation -> text-to-speech -> play-audio\n");
    printf("\tMode 1 - translation -> text-to-speech -> stdout\n");
    printf("\tMode 2 - text-to-speech -> stdout\n");
    printf("\tMode 3 - text-to-speech -> play_audio\n");
    printf("\tMode 4 - translation -> stdout\n");
    exit(0);
  }

  const float speed = atof(argv[1]);
  const int mode = atoi(argv[2]);
  char *text = argv[3];

  if (mode == 0) {
    char translation[TRANS_BUFFER_SIZE];
    trans(translation, text);
    MemAudioData audio = fast_tts_omp(translation);
    play_audio(audio, speed);
    exit(0);
  }
  if (mode == 1) {
    char translation[TRANS_BUFFER_SIZE];
    trans(translation, text);
    MemAudioData audio = fast_tts_omp(translation);
    fwrite(audio.audio, 1, audio.size, stdout);
    exit(0);
  }
  if (mode == 2) {
    MemAudioData audio = fast_tts_omp(text);
    fwrite(audio.audio, 1, audio.size, stdout);
    exit(0);
  }
  if (mode == 3) {
    MemAudioData audio = fast_tts_omp(text);
    play_audio(audio, speed);
    exit(0);
  }
  if (mode == 4) {
    char translation[TRANS_BUFFER_SIZE];
    trans(translation, text);
    fwrite(translation, 1, strlen(translation), stdout);
    exit(0);
  }

  return 0;
}
