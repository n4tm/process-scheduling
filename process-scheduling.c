#include "methods/peterson-solution.h"
#include <getopt.h>

#define AS_A_PRODUCER 0
#define AS_A_CONSUMER 1
#define METHODS_QTY 1
#define GET_VAR_NAME(var) #var

const void (*methods[])(cp_process_t*) = {
    peterson_solution,
    // sleep_wakeup,
    // semaphore,
    // monitor
};

const char* method_names[] = {
    "peterson_solution",
    // "sleep_wakeup",
    // "semaphore",
    // "monitor"
};

const void (*method_shared_slots_initializers[])() = {
    initialize_peterson_solution_shared_slots
    // initialize_sleep_wakeup_shared_slots,
    // initialize_semaphore_shared_slots,
    // initialize_monitor_shared_slots
};

void show_help(char* name);
void handle_args(int argc, char* argv[], char* method_name);
int get_method_index(char* method_name);

int main(int argc, char* argv[]) {
    char method_name[18];
    handle_args(argc, argv, method_name);

    int method_index = get_method_index(method_name);
    if (method_index == -1) {
        printf("Error: Invalid method name.\n");
        show_help(argv[0]);
    };

    const void (*apply_chosen_method)(cp_process_t*) = methods[method_index];
    const void (*initialize_chosen_method)() = method_shared_slots_initializers[method_index];
    
    initialize_buffer();
    (*initialize_chosen_method)();                      // allocate shared memory

    cp_process_t producer;
    cp_process_t consumer;
    pid_t producer_pid;

    if ((producer_pid = fork()) == -1) {
        printf("Error: Failed to create child process (parent pid: %d).\n", getpid());
        exit(EXIT_FAILURE);
    } else if (producer_pid > 0) {
        construct_process(&producer, AS_A_PRODUCER);    // parent process is a producer
        while (true) {
            (*apply_chosen_method)(&producer);
            sleep(1);
        }
    } else if (producer_pid == 0) {
        construct_process(&consumer, AS_A_CONSUMER);    // child process is a consumer
        while (true) {
            (*apply_chosen_method)(&consumer);
            sleep(1);
        }
    }

    return 0;
}

void show_help(char* name) {
    fprintf(stderr, "[usage] %s <options>\n\
    -m METHOD_NAME method to be used for the process scheduling\n\
    \n\
    METHOD_NAME options:\n\
        \"peterson_solution\"\n\
        \"sleep_wakeup\"\n\
        \"semaphore\"\n\
        \"monitor\"\n", name);
    exit(EXIT_FAILURE);
}

void handle_args(int argc, char* argv[], char* method_name) {
    if (argc < 2) show_help(argv[0]);

    int opt;
    opt = getopt(argc, argv, "m:");

    if (opt == 'm') strcpy(method_name, optarg);
    else show_help(argv[0]);
};

int get_method_index(char* method_name) {
    for (int i = 0; i < METHODS_QTY; ++i) {
        if (strcmp(method_name, method_names[i]) == 0) {
            return i;
        }
    }
    return -1;
}