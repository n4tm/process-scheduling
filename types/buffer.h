#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#define ALLOCATE_SHARED_MEMORY(sz) mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)
#define BUFFER_CAPACITY 64
#define BUFFER_FILE_PATH "./buffer.txt"
#define PRODUCER 0

typedef struct buffer_t {
  void (*insert_item)();
  void (*remove_item)();
  void (*clear)();
} buffer_t;

buffer_t *buffer;
char *buffer_content;

char *_buffer;
int *_current_buffer_size;
FILE **_file;

void _insert_item() {
  _buffer[*_current_buffer_size] = '#';
  if (*_current_buffer_size < BUFFER_CAPACITY - 1) (*_current_buffer_size)++;  
}

void _remove_item() {
  _buffer[*_current_buffer_size];
  if (*_current_buffer_size > 0) (*_current_buffer_size)--;
}

void _clear() {
  memset(_buffer, '.', BUFFER_CAPACITY);
}

void initialize_buffer() {
  buffer = ALLOCATE_SHARED_MEMORY(sizeof(buffer_t));
  *buffer = (buffer_t) {
    _insert_item,
    _remove_item,
    _clear
  };
  _current_buffer_size = ALLOCATE_SHARED_MEMORY(sizeof(int));
  buffer_content = ALLOCATE_SHARED_MEMORY(128);
  _buffer = ALLOCATE_SHARED_MEMORY(BUFFER_CAPACITY);
  buffer->clear();
  _file = ALLOCATE_SHARED_MEMORY(sizeof(**_file));
  *_file = fopen(BUFFER_FILE_PATH, "w");
  if (*_file == NULL) {
    printf("Error: failed to open file (path: %s).\n", BUFFER_FILE_PATH);
    exit(EXIT_FAILURE);
  }
}

void watch_buffer_file() {
  const char *watch_command = "watch -n 0.1 cat buffer.txt";
  if (system(watch_command) == -1) {
    printf("Error: failed when trying to watch for buffer file (used command: %s).\n", watch_command);
    exit(EXIT_FAILURE);
  }
}

void close_buffer_file() {
  munmap(buffer, sizeof(buffer_t));
  munmap(_current_buffer_size, sizeof(int));
  munmap(buffer_content, 128);
  munmap(_buffer, BUFFER_CAPACITY);
  munmap(_file, sizeof(**_file));

  if (fclose(*_file) == -1) {
    printf("Error: failed to close file (path: %s).\n", BUFFER_FILE_PATH);
    exit(EXIT_FAILURE);
  };
}

#endif