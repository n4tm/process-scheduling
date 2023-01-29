#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_CAPACITY 64
#define BUFFER_FILE_PATH "./buffer.txt"
#define PRODUCER 0

typedef struct buffer_t {
  void (*insert_item)();
  void (*remove_item)();
  void (*clear)();
} buffer_t;

buffer_t buffer;
char *buffer_content;

static char _buffer[BUFFER_CAPACITY];
static int _current_buffer_size;
static FILE *_file;

void _insert_item() {
  _buffer[_current_buffer_size] = '#';
  if (_current_buffer_size < BUFFER_CAPACITY - 1) _current_buffer_size++;  
}

void _remove_item() {
  _buffer[_current_buffer_size];
  if (_current_buffer_size > 0) _current_buffer_size--;
}

void _clear() {
  memset(_buffer, '.', BUFFER_CAPACITY);
}

void initialize_buffer() {
  buffer = (buffer_t) {
    _insert_item,
    _remove_item,
    _clear
  };
  buffer_content = (char*)malloc(128);
  buffer.clear();
  _file = fopen(BUFFER_FILE_PATH, "w");
  if (_file == NULL) {
    printf("Error: failed to open file (path: %s).\n", BUFFER_FILE_PATH);
    exit(-1);
  }
}

void watch_buffer_file() {
  const char *watch_command = "watch -n 0.1 cat buffer.txt";
  if (system(watch_command) == -1) {
    printf("Error: failed when trying to watch for buffer file (used command: %s).\n", watch_command);
    exit(-1);
  }
}

void close_buffer_file() {
  free(buffer_content);
  if (fclose(_file) == -1) {
    printf("Error: failed to close file (path: %s).\n", BUFFER_FILE_PATH);
    exit(-1);
  };
}

#endif