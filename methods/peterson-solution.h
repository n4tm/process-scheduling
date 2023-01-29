#ifndef PETERSON_SOLUTION_H
#define PETERSON_SOLUTION_H

#include "../types/cp_process_t.h"
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>

int turn;
bool interested[2];

void _enter_region(pid_t pid) {
  int other = pid + 1;
  interested[pid] = true;
  turn = pid;
  while (turn == pid && interested[other] == true) sleep(1);
}

void _leave_region(pid_t pid) {
  interested[pid] = false;
}

void peterson_solution(cp_process_t process) {
  _enter_region(process.id);
  process.act();
  commit_to_buffer(process);
  _leave_region(process.id);
}

#endif