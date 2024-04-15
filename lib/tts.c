#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "tts.h"


void genarate_tts_url(char *url, const TTSParams params, Slice text) {
  assert(params.client != NULL && params.ie != NULL && params.tl != NULL);

  // const char base[] = "https://translate.googleapis.com/translate_tts";
  const char base[] = "https://translate.google.com/translate_tts";

  sprintf(url, "%s?client=%s&ie=%s&tl=%s&q=", base, params.client, params.ie,
          params.tl);

  int len = strlen(url);
  url_encode(text, url + len);
}

