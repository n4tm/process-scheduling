#include <stdbool.h>
#include "types/cp_process_queue.h"
#include "types/cp_process.h"

cp_process_queue q;

void enter_region(cp_process current_process) {
	
	while (q->front().id != current_process.id) {
		sleep(1);	// Se não sou o primeiro da fila, durmo
	}
}

void leave_region(cp_process ) {
	q->pop();		// Saio da fila e o próximo da fila se torna o primeiro
}
