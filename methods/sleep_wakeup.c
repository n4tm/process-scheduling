#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "apue.h"

#define TRUE 1
#define FALSE 0

#define SHM_SIZE sizeof(int)
#define FILE_PATH "./buffer.txt"

#define N 20

int *count;

pid_t producer_pid;
pid_t consumer_pid;

int sleep_process() {
    pid_t pid = getpid();
    kill(pid, SIGSTOP);
}

int wakeup_process(pid_t process_id) {
    int status = kill(process_id, SIGCONT);

    if (status == -1) {
        perror("Error: kill");
        exit(1);
    }

    return 0;
}

char* produce_item() { 
	return "#";
}

void consume_item(char *item) {
    
}

void insert_item(const char *item) {
    FILE *file;
    file = fopen(FILE_PATH, "a");
    if (file == NULL) {
        perror("Error: fopens");
		exit(1);
    }

    fprintf(file, item);
    fclose(file);
}

char *remove_item() {
    FILE *file;
    file = fopen(FILE_PATH, "a");
    if (file == NULL) {
        perror("Error: fopens");
		exit(1);
    }

    fseeko(file,-1,SEEK_END);
    int position = ftello(file);
    ftruncate(fileno(file), position);

    fclose(file);

    return "#";
}

void producer() {
	char *item;

	while (TRUE) {
		item = produce_item();
		if (*count == N) sleep_process();
		insert_item(item);
		*count += 1;
        sleep(1);
		if (*count == 1) wakeup_process(consumer_pid);
    }
}

void consumer() {
	char *item;
	
	while (TRUE) {
		if (*count == 0) sleep_process();
        item = remove_item();
        *count -= 1;
        sleep(3);
        if (*count == N-1) wakeup_process(producer_pid);
        consume_item(item);
	}
}

void create_buffer() {
    FILE *file;
    file = fopen(FILE_PATH, "w");
    if (file == NULL) {
        perror("Error: fopens");
        exit(1);
    }

    fclose(file);
}

void init_count() {
	int shm = shm_open("COUNT", O_CREAT | O_RDWR, 0666);
	if (shm == -1) {
		perror("Error: shm_open");
		exit(1);
	}

	if (ftruncate(shm, SHM_SIZE) == -1) {
		perror("Error: ftruncate");
		exit(1);
	}
	
	count = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
	if (count == MAP_FAILED) {
		perror("Error: mmap");
		exit(1);
	};

    *count = 0;
    close(shm);
}

int main(int argc, char *argv[]) {
    init_count();
    create_buffer();

    pid_t pid = fork();

    if (pid == 0) {
        consumer_pid = getppid();
        printf("Producer with PID: %d\n", getpid());
        producer();
    } else {
        producer_pid = pid;
        printf("Consumer with PID: %d\n", getpid());
        consumer();
    }

	munmap(count, SHM_SIZE);
	shm_unlink("COUNT");

	return 0;
}