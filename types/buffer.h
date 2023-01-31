#ifndef BUFFER_H
#define BUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define ALLOCATE_SHARED_MEMORY(sz) mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)
#define BUFFER_CAPACITY 64
#define BUFFER_FILE_PATH "./buffer.txt"
#define PRODUCER 0

typedef struct buffer_t {
  void (*insert_item)();
  void (*remove_item)();
  void (*clear)();
} buffer_t;

static buffer_t buffer;

static char *_buffer;
static int *_current_buffer_size;
static FILE* _file;

void _insert_item() {
  _buffer[*_current_buffer_size] = '#';
  if (*_current_buffer_size < BUFFER_CAPACITY - 1) (*_current_buffer_size)++;
}

void _remove_item() {
  if (*_current_buffer_size <= 0) return;
  _buffer[--(*_current_buffer_size)] = '.';
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
  _buffer = ALLOCATE_SHARED_MEMORY(BUFFER_CAPACITY);
  buffer.clear();
  _current_buffer_size = ALLOCATE_SHARED_MEMORY(sizeof(int));
  if (access(BUFFER_FILE_PATH, F_OK) != 0) {
    printf("Error: file not found (path: %s).\n", BUFFER_FILE_PATH);
    exit(EXIT_FAILURE);
  }
}

#endif