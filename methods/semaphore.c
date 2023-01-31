#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "./apue.h"

#define TRUE 1
#define FALSE 0

#define SHM_SIZE sizeof(int)
#define FILE_PATH "./buffer.txt" // Arquivo para escrever o conteúdo do item.

#define BUFFER_SIZE 20 // Tamanho do buffer
#define SHM_KEY 42

int *empty; // Semáforo
int *full; // Semáforo

int *buffer_count; // Contador do buffer

/* IDs dos seguimento de memória compartilhada */
int shmid_buffer_count;
int shmid_empty;
int shmid_full;

/**
 * Implementação de up para um semáforo, mantendo a atomicidade da instrução.
*/
int up(int *semaphore) {
  int ret;
  do {
    ret = *semaphore;
    if (ret == BUFFER_SIZE) {
      return -1;
    }
  } while (!__sync_bool_compare_and_swap(semaphore, ret, ret + 1));

  return 0;
}

/**
 * Implementação de down para um semáforo, mantendo a atomicidade da instrução.
*/
int down(int *semaphore) {
  int ret;
  do {
    ret = *semaphore;
    if (ret == 0) {
      return -1;
    }
  } while (!__sync_bool_compare_and_swap(semaphore, ret, ret - 1));

  return 0;
}

void insert_item() {
    FILE *file;
    file = fopen(FILE_PATH, "a");
    if (file == NULL) {
        perror("Error: fopens");
		exit(1);
    }

    fprintf(file, "#");
    fclose(file);
}

void remove_item() {
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

/**
 * Exibe as informaçõe na tela
*/
void print_info() {
	printf("PID: %d\n", getpid());
	printf("Empty: %d\n", *empty);
	printf("Full: %d\n", *full);
	printf("Buffer count: %d\n", *buffer_count);
	printf("\n\n");
}

void producer() {

	while (TRUE) {
		down(empty);

		if (*buffer_count < BUFFER_SIZE) {
			insert_item();
			*buffer_count += 1;
		}
		
		up(full);

    printf("Regiao Critica - Produtor\n");
		print_info();
		sleep(1);
	}
}

void consumer(void) {
	while (TRUE) {
		down(full);

		if (*buffer_count > 0) {
			remove_item();
			*buffer_count -= 1;
		}
		
		up(empty);

    printf("Regiao Critica - Consumidor\n");
		print_info();
		sleep(3);
	}
}

/**
 * Cria e zera o buffer
*/
void create_buffer() {
    FILE *file;
    file = fopen(FILE_PATH, "w");
    if (file == NULL) {
        perror("Error: fopens");
        exit(1);
    }

    fclose(file);
}

/**
 * Inicializa as vairáveis globais
*/
void init_global_variables() {
	shmid_buffer_count = shmget(SHM_KEY + 1, SHM_SIZE, IPC_CREAT | 0666);
	buffer_count = (int *) shmat(shmid_buffer_count, NULL, 0);
	*buffer_count = 0;

	shmid_empty = shmget(SHM_KEY + 2, SHM_SIZE, IPC_CREAT | 0666);
	empty = (int *) shmat(shmid_empty, NULL, 0);
	*empty = BUFFER_SIZE;

	shmid_full = shmget(SHM_KEY + 3, SHM_SIZE, IPC_CREAT | 0666);
	full = (int *) shmat(shmid_full, NULL, 0);
	*full = 0;
}

int main(int argc, char *argv[]) {

	init_global_variables();
    create_buffer();

    pid_t pid = fork();

    if (pid == 0) {
        producer();
    } else {
        consumer();
    }

	/* Desmapeia a memória compartilhada */
    shmdt(buffer_count);
    shmdt(full);
    shmdt(empty);

	/* Libera a memória alocada */
    shmctl(shmid_buffer_count, IPC_RMID, NULL);
    shmctl(shmid_full, IPC_RMID, NULL);
    shmctl(shmid_empty, IPC_RMID, NULL);

	return 0;
}