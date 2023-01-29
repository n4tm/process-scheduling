#include <stdlib.h>

typedef struct cp_process_queue {
  cp_process (*front)();
  void (*push)(cp_process);
  void (*pop)();
  unsigned int (*size)();
} cp_process_queue;

cp_process* _processes_array;
unsigned int _array_free_slots_amount;

void construct_process_queue(cp_process_queue *q, cp_process current_process, int producers_qty, int consumers_qty) {
  _processes_array = (cp_process*)malloc(sizeof(cp_process) * (producers_qty + consumers_qty));
  cp_process_queue queue = {
    _front,
    _push,
    _pop,
    _size
  };
  q = &queue;
}

cp_process _front() {
  return _processes_array[0];
}

void _push(cp_process p) {
  size_t processes_array_capacity = sizeof(_processes_array) / sizeof(cp_process);

  unsigned int index_to_put_new_element = _size();

  if (_array_free_slots_amount == 0) {
    _processes_array = realloc(_processes_array, processes_array_capacity + sizeof(p));
  }

  processes_array[index_to_put_new_element] = p;

  if (_array_free_slots_amount != 0) {
    _array_free_slots_amount--;
  }
}

void _pop() { 
  size_t processes_array_capacity = sizeof(_processes_array) / sizeof(cp_process);
  _processes_array = realloc(_processes_array, processes_array_capacity);
  _processes_array++;
  _array_free_slots_amount++;
}

unsigned int _size() {
  return sizeof(_processes_array) / sizeof(cp_process) - _array_free_slots_amount;
}