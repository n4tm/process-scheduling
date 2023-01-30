#ifndef CP_PROCESS_T_H
#define CP_PROCESS_T_H

#include "buffer.h"
#include <sys/types.h>
#include <unistd.h>

typedef struct cp_process_t { // Consumer/Producer process
  pid_t id;
  void (*act)();
  int type;
} cp_process_t;

void construct_process(cp_process_t* const p, int type) {
  p->id = getpid();
  p->act = type == PRODUCER ? buffer->insert_item : buffer->remove_item;
  p->type = type;
}

void commit_to_buffer(cp_process_t* p) {
  const char* process_action_desc = p->type == PRODUCER ? "Producer inserted" : "Consumer removed";

  const char* buffer_content_format = "-- (pid: %d) %s an item --\n\n[%s]\n";

  if (snprintf(buffer_content, 128, buffer_content_format,
               p->id, process_action_desc, _buffer) < 0) {
    printf("Error: failed to write in buffer content.\n");
    exit(EXIT_FAILURE);
  }

  printf("%s\n", buffer_content);

  fwrite("test", 1, 5, *_file);
  
  if (ferror(*_file) != 0) {
    printf("Error: failed to write into buffer file.\n");
    exit(EXIT_FAILURE);
  }
}

#endif