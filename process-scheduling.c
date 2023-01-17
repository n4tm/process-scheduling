#include "peterson-solution.h"
#include <getopt.h>

#define SPECIFIED_PRODUCERS_QTY 0
#define SPECIFIED_CONSUMERS_QTY 1

#define AS_A_PRODUCER 0
#define AS_A_CONSUMER 1

void handle_args(int argc, char *argv[], int* producers_qty, int* consumers_qty);

int main(int argc, char *argv[]) {
    int producers_qty = 1;
    int consumers_qty = 1;

    handle_args(argc, argv, &producers_qty, &consumers_qty);

    cp_process current_process;
    construct_process(&current_process, AS_A_PRODUCER);

    cp_process_queue q = construct_process_queue(current_process, producers_qty, consumers_qty);

    while (true) {
        peterson_solution(current_process, q);
        sleep(1);
    }
    return 0;
}

void handle_args(int argc, char *argv[], int* producers_qty, int* consumers_qty) {
    static struct option long_options[] = {
        { "producers", required_argument, 0, SPECIFIED_PRODUCERS_QTY },
        { "consumers", required_argument, 0, SPECIFIED_CONSUMERS_QTY },
        { 0,           0,                 0, 0                       }
    };
    int opt = 0;
    while ((opt = getopt_long_only(argc, argv, "", long_options, 0)) != -1) {
        switch (opt) {
            case SPECIFIED_PRODUCERS_QTY:
                *producers_qty = optarg;
                break;
            case SPECIFIED_CONSUMERS_QTY:
                *consumers_qty = optarg;
                break;
            default:
                break;
        }
    }
};
