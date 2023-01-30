#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

void show_help(char* name);
void handle_args(int argc, char* argv[], char* method_name);

int main(int argc, char* argv[]) {
  char method_name[18];
  handle_args(argc, argv, method_name);
  char command[96];
  snprintf(command, 96, "./_process-scheduling -m \"%s\" | watch -n 0.1 cat buffer.txt", method_name);
  system(command);

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