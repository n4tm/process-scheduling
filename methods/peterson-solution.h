#ifndef PETERSON_SOLUTION_H
#define PETERSON_SOLUTION_H

#include "../types/cp_process_t.h"
#include <stdbool.h>
#include <unistd.h>

static pid_t* turn;
static bool* interested;

void _enter_region(pid_t pid) {
  pid_t parent_id = getppid();                    // get parent id
  bool is_orphan = parent_id != pid-1;            // check if process is child or parent
  pid_t other = is_orphan ? pid + 1 : parent_id;  // child process will have parent pid + 1
  interested[pid%2] = true;
  *turn = pid;
  while (*turn == pid && interested[other%2] == true) {
    srand(time(NULL));
    sleep((rand()%3)+1);                          // sleep from 1 to 3 seconds
  }
}

void _leave_region(pid_t pid) {
  interested[pid%2] = false;
}

void peterson_solution(cp_process_t* process) {
  _enter_region(process->id);
  process->act();                                 // insert or remove from buffer
  commit_to_buffer(process);                      // commit changes to buffer file
  _leave_region(process->id);
}

void initialize_peterson_solution_shared_slots() {
  turn = ALLOCATE_SHARED_MEMORY(sizeof(pid_t));
  interested = ALLOCATE_SHARED_MEMORY(sizeof(bool)*2);
}

#endif