#include <assert.h>
#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <omp.h>

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

MemAudioData tts(char *text) {
  // assert(false);
  printf("This 1\n");
  assert(text != NULL);
  int text_len = strlen(text);
  assert(text_len < TOTAL_TEXT_BUFFER_SIZE);
  assert(text_len != 0);

  char url[TOTAL_TEXT_BUFFER_SIZE];
  const Slice source = {.data = text, .size = text_len};
  const TTSParams params = {.client = "tw-ob", .ie = "UTF-8", .tl = "vi"};

  char *pointer = source.data + 0;
  int size = source.size - 0;
  char data[TOTAL_AUDIO_BUFFER_SIZE];
  Slice audio = {.data = data, .size = 0};

  // split text to chunk then handle one by one if reach limit length
  while (size > 0) {

    const Slice slice = tok(pointer, size, TTS_TEXT_LIMIT);
    fwrite(slice.data, 1, slice.size, stdout);
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

  assert(mem.size > 0);
  assert(mem.size < TOTAL_AUDIO_BUFFER_SIZE);
  return mem;
}

void *fetch_url(void *arg) {
  thread_data_t *data = (thread_data_t *)arg;
  // printf("URL: %s\n", data->url);
  request_api(&(data->response), data->url);

  // display response data and size for debug
  // printf("Response: %s\n", data->response.data);
  // printf("Size: %d\n", data->response.size);

  assert(data->response.size > 0);
  return NULL;
}

MemAudioData fast_tts(char *text) {
    printf("This 1\n");

  assert(text != NULL);
  int text_len = strlen(text);
  assert(text_len < TOTAL_TEXT_BUFFER_SIZE);
  assert(text_len != 0);

  const TTSParams params = {.client = "tw-ob", .ie = "UTF-8", .tl = "vi"};

  char data[TOTAL_AUDIO_BUFFER_SIZE];
  Slice audio = {.data = data, .size = 0};

  cvector_vector_type(Slice) text_chunks = NULL;
  split_text(text, TTS_TEXT_LIMIT, &text_chunks);

  const int num_chunks = cvector_size(text_chunks);
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

  for (int i = 0; i < num_chunks; i++) {
    if ((void *)threads[i] == NULL) {
      printf("Thread %d was not created successfully.\n", i);
      continue;
    }
    int pthread_result = pthread_join(threads[i], NULL);
    if (pthread_result != 0) {
      printf("Error joining thread %d. Error code: %d\n", i, pthread_result);
    }
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

  assert(mem.size > 0);
  assert(mem.size < TOTAL_AUDIO_BUFFER_SIZE);
  return mem;
}

MemAudioData fast_tts_omp(char *text) {
    printf("This 1\n");

  assert(text != NULL);
  int text_len = strlen(text);
  assert(text_len < TOTAL_TEXT_BUFFER_SIZE);
  assert(text_len != 0);

  const TTSParams params = {.client = "tw-ob", .ie = "UTF-8", .tl = "vi"};

  cvector_vector_type(Slice) text_chunks = NULL;
  split_text(text, TTS_TEXT_LIMIT, &text_chunks);

  const int num_chunks = cvector_size(text_chunks);
  char **urls = malloc(num_chunks * sizeof(char *));
  thread_data_t *thread_data = malloc(num_chunks * sizeof(thread_data_t));
  Slice *responses = malloc(num_chunks * sizeof(Slice));

  char data[TOTAL_AUDIO_BUFFER_SIZE];

// Parallel region for generating URLs and fetching audio data
#pragma omp parallel for
  for (int i = 0; i < num_chunks; i++) {
    urls[i] = malloc(CHUNK_TEXT_BUFFER_SIZE);
    genarate_tts_url(urls[i], params, text_chunks[i]);

    responses[i] = (Slice){.data = malloc(CHUNK_AUDIO_BUFFER_SIZE), .size = 0};
    thread_data[i] = (thread_data_t){.url = urls[i], .response = responses[i]};
    request_api(&(thread_data[i].response), urls[i]);
  }

  Slice audio = {.data = data, .size = 0};
  // Combine responses sequentially to avoid race conditions
  for (int i = 0; i < num_chunks; i++) {
    memmove(audio.data + audio.size, thread_data[i].response.data,
            thread_data[i].response.size);
    audio.size += thread_data[i].response.size;

    // ignore for faster
    // free(urls[i]);
    // free(responses[i].data);
  }

  // ignore for faster
  // free(urls);
  // free(thread_data);
  // free(responses);

  MemAudioData mem = {.audio = audio.data, .size = audio.size, .pos = 0};

  assert(mem.size > 0);
  assert(mem.size < TOTAL_AUDIO_BUFFER_SIZE);
  return mem;
}
