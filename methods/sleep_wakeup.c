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

int sleep_process() {
    pid_t pid = getpid();
    kill(pid, SIGSTOP);
}

int wakeup_process(pid_t process_id) {
    int status = kill(process_id, SIGCONT);

    if (status == -1) {
        perror("kill");
        exit(1);
    }

    return 0;
}

char* produce_item() { 
	return "|";
}

void consume_item(char *item) {
    printf(strcat("Item consumed: %s\n", item));
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

int remove_item() {
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
}

void producer() {
	char *item;

	while (TRUE) {
		item = produce_item();
		if (*count == N) sleep_process();
		insert_item(item);
		*count += 1;
        sleep(1);
		if (*count == 1) wakeup_process(getpid() - 1);
    }
}

void consumer(void) {
	char item;
	
	while (TRUE) {
		if (*count == 0) sleep_process();
        item = remove_item();
        *count -= 1;
        sleep(1);
        if (*count == N-1) wakeup_process(getpid() - 1);
        consume_item(item);
	}
}


int *init_count(char *shm_name) {
	int shm = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
	if (shm == -1) {
		perror("Error: shm_open");
		exit(1);
	}

	if (ftruncate(shm, SHM_SIZE) == -1) {
		perror("Error: ftruncate");
		exit(1);
	}
	
	int *ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
	if (ptr == MAP_FAILED) {
		perror("Error: mmap");
		exit(1);
	}

	close(shm);

    if (access(FILE_PATH, F_OK) == 0) {
        FILE *file;
        file = fopen(FILE_PATH, "r");
        if (file == NULL) {
            perror("Error: fopens");
            exit(1);
        }

        if (feof(file)) {
            *ptr = 0;
        }

        fclose(file);
    } else {
        *ptr = 0;
    }

    if (*ptr < 0) {
        *ptr = 0;
    }

	return ptr;
}

int main(int argc, char *argv[]) {
	count = init_count("COUNT");

	producer();
    // consumer();
	

	munmap(count, SHM_SIZE);
	shm_unlink("COUNT");

	return 0;
}