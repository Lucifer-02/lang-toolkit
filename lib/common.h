#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>

#include "cvector.h"

#define TOTAL_TEXT_BUFFER_SIZE 32768 // 33Kb
#define CHUNK_TEXT_BUFFER_SIZE 1024  // 1Kb

// Struct to hold translated data and its size
typedef struct {
  char *data;
  int size;
} Slice;

typedef struct {
  char *audio; // pointer to audio in memory
  size_t size;
  size_t pos; // for callback
} MemAudioData;

// Warning: May can't handle all cases. See https://www.url-encode-decode.com/
void url_encode(const Slice text, char *output);
bool is_end_sentence(const char *text);
bool is_interrupt_sentence(const char *text);
Slice tok(char *text, int len, int limit);
// Function to perform translation request using Google Translate API
void request_api(Slice *output, const char *url);
void split_text(char *text, const int limit,
                cvector_vector_type(Slice) * result);
void print_slice(Slice slice);

#endif
