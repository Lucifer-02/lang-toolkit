#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/common.h"
#include "lib/cvector.h"
#include "lib/cvector_utils.h"
#include "lib/trans.h"
#include "lib/tts.h"

void trans(char *translation, char text[]) {
  assert(strlen(text) < TRANS_BUFFER_SIZE); //@require enough memory

  char url[TRANS_BUFFER_SIZE];
  const Slice source = {.data = text, .size = strlen(text)};
  TransParams params = {.client = "gtx",
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

MemAudioData tts(char *text, const int limit) {
  assert(text != NULL);
  int text_len = strlen(text);
  assert(text_len < TOTAL_TEXT_BUFFER_SIZE);
  assert(text_len != 0);

  char url[TOTAL_TEXT_BUFFER_SIZE];
  const Slice source = {.data = text, .size = text_len};
  TTSParams params = {.client = "tw-ob", .ie = "UTF-8", .tl = "vi"};

  char *pointer = source.data + 0;
  int size = source.size - 0;
  char data[TOTAL_AUDIO_BUFFER_SIZE];
  Slice audio = {.data = data, .size = 0};

  // split text to chunk then handle one by one if reach limit length
  while (size > 0) {

    const Slice slice = tok(pointer, size, limit);
    // fwrite(slice.data, 1, slice.size, stdout);
    // printf("\n");
    // printf("\nlen: %d\n", slice.size);
    genarate_tts_url(url, params, slice);
    // printf("url: %s\n", url);
    pointer = slice.data + slice.size + 1;
    size -= slice.size + 1;

    // put all audio chunk together
    Slice resp = {.data = data + audio.size,
                  .size = 0}; // resp also a slice to audio
    request_api(&resp, url);
    audio.size += resp.size;
    // printf("Size: %d\n", resp.size);
    // printf("TTS: %s, size: %d\n", all.data, all.size);
  }

  MemAudioData mem = {.audio = audio.data, .size = audio.size, .pos = 0};
  return mem;
}

void split_text(char *text, const int limit,
                cvector_vector_type(Slice) * result) {
  assert(text != NULL);

  int text_len = strlen(text);
  assert(text_len < TOTAL_TEXT_BUFFER_SIZE);
  assert(text_len != 0);

  const Slice source = {.data = text, .size = text_len};

  char *pointer = source.data + 0;
  int remain_size = source.size - 0;

  // split text to chunk then handle one by one if reach limit length
  while (remain_size > 0) {

    const Slice slice = tok(pointer, remain_size, limit);
    cvector_push_back(*result, slice);
    pointer = slice.data + slice.size + 1;
    remain_size -= slice.size + 1;
  }
}
void *fetch_url(void *arg) {
  thread_data_t *data = (thread_data_t *)arg;
  // printf("URL: %s\n", data->url);
  request_api(&(data->response), data->url);
  return NULL;
}

MemAudioData fast_tts(char *text, const int limit) {
  assert(text != NULL);
  int text_len = strlen(text);
  assert(text_len < CHUNK_TEXT_BUFFER_SIZE);
  assert(text_len != 0);

  TTSParams params = {.client = "tw-ob", .ie = "UTF-8", .tl = "vi"};

  char data[TOTAL_AUDIO_BUFFER_SIZE];
  Slice audio = {.data = data, .size = 0};

  cvector_vector_type(Slice) text_chunks = NULL;
  split_text(text, limit, &text_chunks);

  // /* iterator over the vector standard indexing too! */
  // for (size_t i = 0; i < cvector_size(text_chunks); i++) {
  //   fwrite(text_chunks[i].data, 1, text_chunks[i].size, stdout);
  //   printf("\n");
  //   printf("\nlen: %d\n", text_chunks[i].size);
  // }

  int num_chunks = cvector_size(text_chunks);
  pthread_t threads[num_chunks];
  thread_data_t thread_data[num_chunks];
  char *urls[num_chunks];

  // Start threads
  for (int i = 0; i < num_chunks; i++) {
    urls[i] = malloc(CHUNK_TEXT_BUFFER_SIZE);
    genarate_tts_url(urls[i], params, text_chunks[i]);
    // fwrite(text_chunks[i].data, 1, text_chunks[i].size, stdout);

    thread_data[i].url = urls[i];
    Slice resp = {.data = malloc(CHUNK_AUDIO_BUFFER_SIZE), .size = 0};
    thread_data[i].response = resp;

    // printf("url: %s\n", url);
    pthread_create(&threads[i], NULL, fetch_url, &thread_data[i]);
  }

  // Wait for threads to finish
  for (int i = 0; i < num_chunks; i++) {
    pthread_join(threads[i], NULL);
  }

  // Combine responses
  for (int i = 0; i < num_chunks; i++) {
    // printf("Size: %d\n", thread_data[i].response.size);
    memmove(audio.data + audio.size, thread_data[i].response.data,
            thread_data[i].response.size);
    audio.size += thread_data[i].response.size;
  }

  MemAudioData mem = {.audio = audio.data, .size = audio.size, .pos = 0};

  // Free allocated memory, ignore for faster
  // for (int i = 0; i < num_chunks; i++) {
  //   free(urls[i]);
  //   free(thread_data[i].response.data);
  // }

  return mem;
}

const float SPEED = 2.0;
const int TEXT_LIMIT = 250;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Please run with arguments!!!\n");
    exit(0);
  }

  // char translation[TRANS_BUFFER_SIZE];
  // trans(translation, argv[1]);
  // printf("Translation: %s\n", translation);
  MemAudioData audio = fast_tts(argv[1], TEXT_LIMIT);
  // MemAudioData audio = tts(argv[1], 250);
  // play_audio(audio, SPEED);
  printf("Audio size: %d\n", audio.size);
  return 0;
}
