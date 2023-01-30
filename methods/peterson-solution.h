#ifndef PETERSON_SOLUTION_H
#define PETERSON_SOLUTION_H

#include "../types/cp_process_t.h"
#include <stdbool.h>
#include <unistd.h>

static pid_t* turn;
static bool* interested;

void _enter_region(pid_t pid) {
  int parent_id = getppid();
  bool is_orphan = parent_id != pid-1;
  int other = is_orphan ? pid + 1 : parent_id;
  interested[pid] = true;
  *turn = pid;
  while (*turn == pid && interested[other] == true) sleep(1);
}

void _leave_region(pid_t pid) {
  interested[pid] = false;
}

void peterson_solution(cp_process_t* process) {
  _enter_region(process->id);
  process->act();
  commit_to_buffer(process);
  _leave_region(process->id);
}

void initialize_peterson_solution_shared_slots() {
  turn = ALLOCATE_SHARED_MEMORY(sizeof(pid_t));
  interested = ALLOCATE_SHARED_MEMORY(sizeof(bool)*2);
}

#endif