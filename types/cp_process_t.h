#ifndef CP_PROCESS_T_H
#define CP_PROCESS_T_H

#include "buffer.h"
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

#define PRODUCER 0

typedef struct cp_process_t { // Consumer/Producer process
  pid_t id;
  void (*act)();
  int type;
} cp_process_t;

void construct_process(cp_process_t *p, pid_t pid, int type) {
  cp_process_t temp = {
    pid,
    type == PRODUCER ? buffer.insert_item : buffer.remove_item,
    type
  };
  p = &temp;
}

void commit_to_buffer(cp_process_t p) {
  char *buffer_content = (char*)malloc(128);

  const char *process_action_desc = p.type == PRODUCER ? "Producer inserted" : "Consumer removed";

  struct timeval tp;
  gettimeofday(&tp, 0);
  time_t curtime = tp.tv_sec;
  struct tm *t = localtime(&curtime);

  const char *buffer_content_format = "[%02d:%02d:%02d:%03d] %s an item (pid: %d)\n\n[%s]\n";

  if (snprintf(buffer_content, 128, buffer_content_format, t->tm_hour-3, t->tm_min, t->tm_sec,
               (int)tp.tv_usec/1000, process_action_desc, p.id, _buffer) < 0) {
    // throw exception
  }
  fwrite(buffer_content, 1, sizeof(buffer_content), _file);
}

#endif