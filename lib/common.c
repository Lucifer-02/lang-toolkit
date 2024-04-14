#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <curl/curl.h>
#include <string.h>

#include "common.h"

// Warning: May can't handle all cases. See https://www.url-encode-decode.com/
void url_encode(const Slice text, char *output) {
  assert(output != NULL);
  assert(text.size != 0);
  assert(text.data != NULL);

  int pos = 0;

  for (int i = 0; i < text.size; i++) {

    unsigned char ch = text.data[i];
    // printf("char: %02x\n", ch);

    if (('A' <= ch && ch <= 'Z') || ('a' <= ch && ch <= 'z') ||
        ('0' <= ch && ch <= '9')) {
      output[pos++] = ch;
    } else if (ch == ' ') {
      output[pos++] = '+';
    } else if (ch == '-' || ch == '_' || ch == '.' || ch == '!' || ch == '~' ||
               ch == '*' || ch == '\'' || ch == '(' || ch == ')') {
      output[pos++] = ch;
    } else {
      sprintf(output + pos, "%%%02X", ch);
      pos += 3;
    }
  }

  output[pos++] = '\0';
}

bool is_end_sentence(const char *text) {
  if ((text[0] == '?' || text[0] == '.') && isspace(text[1])) {
    return true;
  }
  return false;
}

bool is_interrupt_sentence(const char *text) {
  if ((text[0] == ',' || text[0] == ';') && isspace(text[1])) {
    return true;
  }
  return false;
}

Slice tok(char *text, int len, int limit) {

  int blank_pos = 0;
  int end_sentence_pos = 0;
  int interrupt_sentence_pos = 0;

  int count = 0;
  const int start = 0;
  int end = 0;
  int pos = 0;

  // printf("Origin:\n");
  // fwrite(text, 1, len, stdout);
  // // printf("\nlen: %d\nEnd\n", len);
  // printf("\n");

  if (len <= limit) {
    return (Slice){.data = text, .size = len};
  }

  while (true) {

    if (isspace(text[pos])) {
      blank_pos = pos;
    }

    if (is_end_sentence(text + pos)) {
      end_sentence_pos = pos;
    }

    if (is_interrupt_sentence(text + pos)) {
      interrupt_sentence_pos = pos;
    }

    if (count == limit) {
      // end = fmax(interrupt_sentence_pos, end_sentence_pos);
      end =
          end_sentence_pos == 0
              ? interrupt_sentence_pos == 0 ? blank_pos : interrupt_sentence_pos
              : end_sentence_pos;
      // printf("End: %d and %d\n", interrupt_sentence_pos,
      // end_sentence_pos); end = interrupt_sentence_pos;
      assert(start < end);
      return (Slice){.data = text, .size = end - start + 1};
    }

    if (pos == len - 1) {
      end = pos;
      return (Slice){.data = text, .size = end - start + 1};
    }

    count++;
    pos++;
  }
}

// Callback function to handle received data
static size_t write_data(const void *ptr, size_t size, size_t nmemb,
                         Slice *resp) {
  assert(resp->data != NULL);

  size_t new_size = resp->size + size * nmemb;
  memmove(resp->data + resp->size, ptr, size * nmemb);
  resp->size = new_size;
  resp->data[resp->size] = '\0';
  return size * nmemb;
}

// Function to perform translation request using Google Translate API
void request_api(Slice *output, const char *url) {
  assert(url != NULL);
  assert(output->data != NULL);

  CURL *curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output);
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
    }
    curl_easy_cleanup(curl);
  }

  assert(output->data != NULL);
  assert(output->size != 0);
}
