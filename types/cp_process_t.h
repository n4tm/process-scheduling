#ifndef CP_PROCESS_T_H
#define CP_PROCESS_T_H

#include "buffer.h"
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

typedef struct cp_process_t { // Consumer/Producer process
  pid_t id;
  void (*act)();
  int type;                   // 0 if producer / 1 if consumer
} cp_process_t;

void construct_process(cp_process_t* const p, int type) {
  p->id = getpid();
  p->act = type == PRODUCER ? buffer.insert_item : buffer.remove_item;
  p->type = type;
}

void commit_to_buffer(cp_process_t* p) {
  const char* process_action_desc = p->type == PRODUCER ? "Producer inserted" : "Consumer removed";

  struct timeval tp;
  if (gettimeofday(&tp, 0) == -1) {
    printf("Error: gettimeofday failed.\n");
    exit(-1);
  }

  time_t curtime = tp.tv_sec;
  struct tm *t = localtime(&curtime);

  _file = fopen(BUFFER_FILE_PATH, "w+");
  if (fprintf(_file, "[%02d:%02d:%02d:%03d] %s an item (pid: %d)\n\n[%s]\n", t->tm_hour-3, t->tm_min, t->tm_sec,
              (int)tp.tv_usec/1000, process_action_desc, p->id, _buffer) == -1) {
    printf("Error: failed to write in buffer content.\n");
    exit(EXIT_FAILURE);
  }
  
  if (ferror(_file) != 0) {
    printf("Error: failed to write into buffer file.\n");
    exit(EXIT_FAILURE);
  }

  fclose(_file);
}

#endif