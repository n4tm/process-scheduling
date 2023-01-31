#ifndef SLEEP_WAKEUP_H
#define SLEEP_WAKEUP_H

#include "../types/cp_process_t.h"
#include "../libs/apue.h"

/**
 * Coloca o processo para dormir
*/
void sleep_process() {
    pid_t pid = getpid();
    kill(pid, SIGSTOP);
}

/**
 * Acorda o processo
*/
void wakeup_process(pid_t process_id) {
    int status = kill(process_id, SIGCONT);

    if (status == -1) {
        perror("Error: kill");
        exit(1);
    }
}

void sleep_wakeup(cp_process_t* process) {
  bool need_to_sleep;
  bool need_to_wakeup_other;
  int time_to_sleep;

  // Define as condições de SLEEP/WAKEUP
  if (process->type == PRODUCER) {
    need_to_sleep = *_current_buffer_size == BUFFER_CAPACITY;
    need_to_wakeup_other = *_current_buffer_size == 1;
    time_to_sleep = 1;
  } else {
    need_to_sleep = *_current_buffer_size == 0;
    need_to_wakeup_other = *_current_buffer_size == BUFFER_CAPACITY - 1;
    time_to_sleep = 3;
  }

  // Produtor/Consumidor
  if (need_to_sleep) sleep_process();
  process->act();
  printf("[pid: %d] %s an item. Buffer: %s\n", process->id, 
      process->type == PRODUCER ? "Producer inserted" : "Consumer removed", _buffer);
  commit_to_buffer(process);
  sleep(time_to_sleep);

  // Verifica se precisa acordar o outro processo
  if (need_to_wakeup_other) wakeup_process(get_other_pid(process->id));
}

// OVERRIDE
void initialize_sleep_wakeup_shared_slots() { }

#endif