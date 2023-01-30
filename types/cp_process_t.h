#ifndef CP_PROCESS_T_H
#define CP_PROCESS_T_H

#include "buffer.h"
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

typedef struct cp_process_t { // Consumer/Producer process
  pid_t id;
  void (*act)();
  int type;
} cp_process_t;

void construct_process(cp_process_t *p, pid_t pid, int type) {
  cp_process_t temp = {
    pid,
    type == PRODUCER ? buffer->insert_item : buffer->remove_item,
    type
  };
  p = &temp;
}

void commit_to_buffer(cp_process_t p) {
  const char *process_action_desc = p.type == PRODUCER ? "Producer inserted" : "Consumer removed";

  struct timeval tp;

  if (gettimeofday(&tp, 0) == -1) {
    printf("Error: gettimeofday failed.\n");
    exit(EXIT_FAILURE);
  }

  time_t curtime = tp.tv_sec;
  struct tm *t = localtime(&curtime);

  const char *buffer_content_format = "[%02d:%02d:%02d:%03d] %s an item (pid: %d)\n\n[%s]\n";

  if (snprintf(buffer_content, 128, buffer_content_format, t->tm_hour-3, t->tm_min, t->tm_sec,
               (int)tp.tv_usec/1000, process_action_desc, p.id, _buffer) < 0) {
    printf("Error: failed to write in buffer content.\n");
    exit(EXIT_FAILURE);
  }

  fwrite(buffer_content, 1, sizeof(buffer_content), *_file);
  
  if (ferror(*_file) != 0) {
    printf("Error: failed to write into buffer file.\n");
    exit(EXIT_FAILURE);
  }
}

#endif