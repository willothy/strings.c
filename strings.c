#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEFAULT_CAP 4

size_t next_power_2(size_t x) {
  x--;
  for (int i = 0; i < 8 * sizeof(size_t); i++) {
    x |= x >> (1 << i);
  }
  return x + 1;
}

typedef struct {
  char *buffer;
  size_t len;
  size_t cap;
} String;

String string_new() {
  String result;
  result.buffer = calloc(DEFAULT_CAP, sizeof(char));
  result.len = 0;
  result.cap = DEFAULT_CAP;

  return result;
}

/// Assumes that str is a null-terminated cstring
String string_from(const char *str) {
  String result;
  result.len = strlen(str);
  result.cap = next_power_2(result.len);
  result.buffer = calloc(result.cap, sizeof(char));

  strncpy(result.buffer, str, result.len);

  return result;
}

void string_push(String *dst, const char *src) {
  size_t srclen = strlen(src);
  size_t newlen = dst->len + srclen;
  if (newlen > dst->cap) {
    dst->cap = next_power_2(newlen);
    dst->buffer = realloc(dst->buffer, dst->cap);
  }

  strncpy(dst->buffer + dst->len, src, srclen);
  dst->len = newlen;
}

void string_push_char(String *dst, char c) {
  if (dst->len + 1 > dst->cap) {
    dst->cap = next_power_2(dst->len + 1);
    dst->buffer = realloc(dst->buffer, dst->cap);
  }

  dst->buffer[dst->len] = c;
  dst->len++;
}

void string_extend(String *dst, const String *src) {
  size_t newlen = dst->len + src->len;
  if (newlen > dst->cap) {
    dst->cap = next_power_2(newlen);
    dst->buffer = realloc(dst->buffer, dst->cap);
  }

  strncpy(dst->buffer + dst->len, src->buffer, src->len);
  dst->len = newlen;
}

char string_char_at(const String *str, size_t idx) {
  if (idx >= str->len) {
    return 0;
  }
  return str->buffer[idx];
}

void string_free(String *str) {
  free(str->buffer);
  str->buffer = NULL;
  str->len = 0;
  str->cap = 0;
}

void string_print(const String *str, const char *fmt) {
  if (fmt == NULL) {
    fmt = "%s";
  }
  printf(fmt, str->buffer);
}

void string_println(const String *str, const char *fmt) {
  string_print(str, fmt);
  printf("\n");
}

String string_copy(const String *str) {
  String result;
  result.len = str->len;
  result.cap = str->cap;
  result.buffer = calloc(result.cap, sizeof(char));

  strncpy(result.buffer, str->buffer, str->len);

  return result;
}

/// string_checksum and string_corrupt are from
/// https://www.youtube.com/watch?v=q-BvQgZVRGA.
/// string_corrupt only exists to test string_checksum.
String string_corrupt(const String *str, size_t bits) {
  String result = string_copy(str);

  size_t len = result.len;

  // bool *visited = calloc(len, sizeof(bool));
  bool visited[len];
  // Ensure bits < nbits to avoid infinite loop if all are visited
  if (bits > (len * 8)) {
    bits = len * 8;
  }

  for (int i = 0; i < bits; i++) {
    size_t rbyte;
    do {
      rbyte = rand() % len;
    } while (visited[rbyte]);
    visited[rbyte] = true;
    int rbit = rand() % 8;
    result.buffer[rbyte] ^= 1 << rbit;
  }

  // free(visited);
  return result;
}

size_t string_checksum(const String *str) {
  size_t result = 0;
  for (size_t i = 0; i < str->len; i++) {
    result ^= string_char_at(str, i);
  }
  return result;
}

int main() {
  srand(time(NULL));

  String msg = string_from("Hello world");
  string_push(&msg, ", this is a test of the thingy.");

  printf("BEFORE:\n");
  string_print(&msg, "  MESSAGE: %s\n");
  size_t checksum = string_checksum(&msg);
  printf("  CHECKSUM: %zu\n", checksum);

  printf("AFTER:\n");
  String cmsg = string_corrupt(&msg, 10);
  string_print(&cmsg, "  MESSAGE: %s\n");
  size_t verification = string_checksum(&cmsg);
  printf("  CHECKSUM: %zu\n", verification);

  string_free(&msg);
  string_free(&cmsg);

  return EXIT_SUCCESS;
}
