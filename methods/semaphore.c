#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "aupe.h"

#define TRUE 1
#define FALSE 0

#define SHM_SIZE sizeof(int)
#define FILE_PATH "./buffer.txt"

#define N 20

int *count;

pid_t producer_pid;
pid_t consumer_pid;


int main(int argc, char *argv[]) {
	count = init_count("COUNT");

    pid_t pid = fork();

    if (pid == 0) {
        consumer_pid = getppid();
        producer();
    } else {
        producer_pid = pid;
        consumer();
    }

	munmap(count, SHM_SIZE);
	shm_unlink("COUNT");

	return 0;
}