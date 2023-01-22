#include "_peterson-solution.h"
#include "types/cp_process.h"
#include "types/cp_process_queue.h"

void peterson_solution(cp_process current_process, int producers_qty, int consumers_qty) {
	construct_process_queue(&q, current_process, producers_qty, consumers_qty);

	enter_region(current_process);
	current_process.act();
	leave_region(q);	
}
