#include "methods/peterson-solution.h"
#include "types/cp_process_t.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define AS_A_PRODUCER 0
#define AS_A_CONSUMER 1
#define METHODS_QTY 4
#define GET_VAR_NAME(var) #var

const void (*methods[])(cp_process_t) = {
    peterson_solution,
    // sleep_wakeup_action,
    // semaphore_action,
    // monitor_action
};

void handle_args(int argc, char *argv[], char* method_name);
void set_chosen_method(void (*chosen_method)(cp_process_t), char* method_name);

int main(int argc, char *argv[]) {
    char* method_name;
    handle_args(argc, argv, method_name);

    void (*apply_chosen_method)(cp_process_t);
    set_chosen_method(apply_chosen_method, method_name);

    initialize_buffer();

    cp_process_t producer;          // parent process is a producer
    construct_process(&producer, getpid(), AS_A_PRODUCER);

    cp_process_t consumer;
    pid_t consumer_pid = fork();    // child process is a consume
    construct_process(&consumer, consumer_pid, AS_A_CONSUMER);

    while (true) {
        apply_chosen_method(producer);
        apply_chosen_method(consumer);
        sleep(1);
    }

    close_buffer_file();

    return 0;
}

void show_help(char *name) {
    fprintf(stderr, "\
            [usage] %s <options>\n\
            -m METHOD_NAME  method to be used for the process scheduling\n\
            \n\
            METHOD_NAME options:\n\
              \"peterson_solution\"\n\
              \"sleep_wakeup\"\n\
              \"semaphore\"\n\
              \"monitor\"\n", name);
    exit(-1);
}

void handle_args(int argc, char *argv[], char* method_name) {
    int opt;

    if (argc < 2) show_help(argv[0]);

    while ((opt = getopt(argc, argv, "hm:")) != -1) {
        switch (opt) {
            case 'h': // help
                show_help(argv[0]);
                break;
            case 'm': // method name:
                method_name = optarg;
                break;
            default:
                break;
        }
    }
};

void set_chosen_method(void (*chosen_method)(cp_process_t), char* method_name) {
    for (int i = 0; i < METHODS_QTY; ++i) {
        if (strcmp(method_name, GET_VAR_NAME(methods[i])) == 0) {
            chosen_method = methods[i];
            break;
        }
    }
}