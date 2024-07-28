#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <curl/curl.h>
#include <jansson.h>

#include "trans.h"

void genarate_trans_url(char *url, const TransParams params, Slice text) {
  assert(params.client != NULL && params.ie != NULL && params.oe != NULL &&
         params.dt != NULL && params.sl != NULL && params.tl != NULL);

  const char base[] = "https://translate.googleapis.com/translate_a/single";

  sprintf(url, "%s?client=%s&ie=%s&oe=%s&dt=%s&sl=%s&tl=%s&q=", base,
          params.client, params.ie, params.oe, params.dt, params.sl, params.tl);

  int len = strlen(url);
  url_encode(text, url + len);
  // printf("url: %s\n", url);
}

void parse_resp(char *parsed, const char *json_string) {

  json_t *root = json_loads(json_string, 0, NULL);
  json_t *array = json_array_get(root, 0);

  size_t offset = 0;

  for (size_t i = 0; i < json_array_size(array); i++) {

    json_t *dialogue = json_array_get(array, i);
    json_t *chunk = json_array_get(dialogue, 0);

    const char *paragaph = json_string_value(chunk);
    size_t para_size = json_string_length(chunk);
    memcpy(parsed + offset, paragaph, para_size);
    printf("paragaph: %s\n", paragaph);
    offset += para_size;
  }

  parsed[offset] = '\0';
  json_decref(root);
}

void trans(char *translation, char text[]) {
  assert(strlen(text) < TRANS_BUFFER_SIZE); //@require enough memory

  char url[TRANS_BUFFER_SIZE];
  const Slice source = {.data = text, .size = strlen(text)};
  const TransParams params = {.client = "gtx",
                              .ie = "UTF-8",
                              .oe = "UTF-8",
                              .dt = "t",
                              .sl = "auto",
                              .tl = "vi"};

  genarate_trans_url(url, params, source);
  // printf("url: %s\n", url);

  char data[TRANS_BUFFER_SIZE];
  Slice trans = {.data = data, .size = 0};
  request_api(&trans, url);
  // printf("Output: %s, size: %ld\n", trans.data, trans.size);
  parse_resp(translation, trans.data);
}
