#include <unistd.h>

typedef struct cp_process {
  unsigned int id;
  void* act;
} cp_process;

void initialize_process(cp_process *p, unsigned int type) {
  cp_process temp = {
    (unsigned int)getpid(),
    type == 0 ? produce : consume
  };
  p = &temp;
}

void produce() {
  // Produce item and insert into the buffer
}

void consume() {
  // Consume item and remove from the buffer
}