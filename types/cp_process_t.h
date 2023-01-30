#ifndef CP_PROCESS_T_H
#define CP_PROCESS_T_H

#include "buffer.h"
#include <sys/types.h>

typedef struct cp_process_t { // Consumer/Producer process
  pid_t id;
  void (*act)();
  int type;
} cp_process_t;

void construct_process(cp_process_t* const p, int type) {
  p->id = getpid();
  p->act = type == PRODUCER ? buffer.insert_item : buffer.remove_item;
  p->type = type;
}

void commit_to_buffer(cp_process_t* p) {
  const char* process_action_desc = p->type == PRODUCER ? "Producer inserted" : "Consumer removed";

  _file = fopen(BUFFER_FILE_PATH, "w+");
  if (fprintf(_file, "-- (pid: %d) %s an item --\n\n[%s]\n", p->id, process_action_desc, _buffer) == -1) {
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