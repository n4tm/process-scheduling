#include "_peterson-solution.h"
#include "types/cp_process.h"
#include "types/cp_process_queue.h"

void peterson_solution(cp_process current_process, cp_process_queue* q) {
	enter_region(current_process, q);
	current_process.act();
	leave_region(q);	
}
