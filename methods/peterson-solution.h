#ifndef PETERSON_SOLUTION_H
#define PETERSON_SOLUTION_H

#include "../types/cp_process_t.h"
#include <unistd.h>

static pid_t* turn;
static bool* interested;

void _enter_region(pid_t pid) {
  pid_t other = get_other_pid(pid);
  interested[pid%2] = true;
  *turn = pid;
  while (*turn == pid && interested[other%2] == true) sleep(1);
}

void _leave_region(pid_t pid) {
  interested[pid%2] = false;
}

void peterson_solution(cp_process_t* process) {
  _enter_region(process->id);
  process->act();             // insert or remove from buffer
  printf("[pid: %d] %s an item. Buffer: %s\n", process->id, 
         process->type == PRODUCER ? "Producer inserted" : "Consumer removed", _buffer);
  commit_to_buffer(process);  // commit changes to buffer file
  sleep(process->type == PRODUCER ? 1 : 3);
  _leave_region(process->id);
}

void initialize_peterson_solution_shared_slots() {
  turn = ALLOCATE_SHARED_MEMORY(sizeof(pid_t));
  interested = ALLOCATE_SHARED_MEMORY(sizeof(bool)*2);
}

#endif