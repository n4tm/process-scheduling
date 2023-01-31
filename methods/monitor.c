#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "./apue.h"

#define TRUE 1
#define FALSE 0

#define SHM_SIZE sizeof(ProducerConsumerMonitor)
#define FILE_PATH "./buffer.txt" // Arquivo para escrever o conteúdo do item.

#define BUFFER_SIZE 20 // Tamanho do buffer
#define SHM_KEY 42

void insert_item(void);
void remove_item(void);
int up(int *);
int down(int *);

pid_t other;

struct ProducerConsumerMonitor {
    int full;
    int empty;
    int mutex;
    int count;

    void (*insert) (struct ProducerConsumerMonitor *monitor);
    void (*remove) (struct ProducerConsumerMonitor *monitor);
};

void insert_monitor(struct ProducerConsumerMonitor *monitor) {
    down(&monitor->empty);
    down(&monitor->mutex);

    if (monitor->count < BUFFER_SIZE) {
        insert_item();
        monitor->count += 1;
    }
            
    up(&monitor->mutex);
    up(&monitor->full);
}

void remove_monitor(struct ProducerConsumerMonitor *monitor) {
    down(&monitor->full);
    down(&monitor->mutex);

    if (monitor->count > 0) {
		remove_item();
		monitor->count -= 1;
	}

    up(&monitor->mutex);
    up(&monitor->empty);
}

void init_monitor(struct ProducerConsumerMonitor *monitor) {
    monitor->full = 0;
    monitor->empty = BUFFER_SIZE;
    monitor->mutex = 1;
    monitor->count = 0;
    monitor->insert = insert_monitor;
    monitor->remove = remove_monitor;
}

typedef struct ProducerConsumerMonitor ProducerConsumerMonitor;

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



/* IDs dos seguimento de memória compartilhada */
int shmid;

/* Ponteiro para o monitor */
ProducerConsumerMonitor *shm_monitor;

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

/**
 * Exibe as informaçõe na tela
*/
void print_info() {
	printf("PID: %d\n", getpid());
	printf("Empty: %d\n", shm_monitor->empty);
	printf("Full: %d\n", shm_monitor->full);
	printf("Buffer count: %d\n", shm_monitor->count);
	printf("\n\n");
}

void producer() {
	while (TRUE) {
		shm_monitor->insert(shm_monitor);

        printf("Regiao Critica - Produtor\n");
		print_info();
		sleep(1);
	}
}

void consumer(void) {
	while (TRUE) {
		shm_monitor->remove(shm_monitor);
        
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
void create_monitor() {
	shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
	shm_monitor = (ProducerConsumerMonitor *) shmat(shmid, NULL, 0);
	init_monitor(shm_monitor);
}

/**
 * Desmapeia e libera a memória compartilhada
*/
void dispose_monitor() {
    shmdt(shm_monitor);
    shmctl(shmid, IPC_RMID, NULL);
}

int main(int argc, char *argv[]) {
    create_buffer();
	create_monitor();

    pid_t pid = fork();

    if (pid == 0) { // Filho
        other = getppid();
        producer();
    } else { // Pai
        other = pid;
        consumer();
    }

	dispose_monitor();

	return 0;
}