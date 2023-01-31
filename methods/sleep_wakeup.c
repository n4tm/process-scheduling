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
#define FILE_PATH "./buffer.txt" // Arquivo para escrever o conteúdo do item.

#define BUFFER_SIZE 20 // Tamanho do buffer
#define SHM_KEY 42


int shmid_count; // Id do seguimento de memória
int *count; // Memoria compartilhada

pid_t producer_pid;
pid_t consumer_pid;

/**
 * Coloca o processo para dormir
*/
int sleep_process() {
    pid_t pid = getpid();
    kill(pid, SIGSTOP);
}

/**
 * Acorda o processo
*/
int wakeup_process(pid_t process_id) {
    int status = kill(process_id, SIGCONT);

    if (status == -1) {
        perror("Error: kill");
        exit(1);
    }

    return 0;
}

/**
 * Escreve no arquivo de buffer
*/
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

/**
 * Deleta do arquivo de buffer
*/
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
	printf("Buffer count: %d\n", *count);
	printf("\n\n");
}

void producer() {
	while (TRUE) {
		if (*count == BUFFER_SIZE) sleep_process();
		insert_item();
		*count += 1;

        printf("Regiao Critica - Produtor\n");
        print_info();
        sleep(1);
		if (*count == 1) wakeup_process(consumer_pid);

    }
}

void consumer() {
	while (TRUE) {
		if (*count == 0) sleep_process();
        remove_item();
        *count -= 1;

        printf("Regiao Critica - Consumidor\n");
        print_info();
        sleep(3);
        if (*count == BUFFER_SIZE - 1) wakeup_process(producer_pid);
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
	shmid_count = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
	count = (int *) shmat(shmid_count, NULL, 0);
	*count = 0;
}

int main(int argc, char *argv[]) {
    init_global_variables();
    create_buffer();

    pid_t pid = fork();

    if (pid == 0) {
        consumer_pid = getppid();
        producer();
    } else {
        producer_pid = pid;
        consumer();
    }

	/* Desmapeia a memória compartilhada */
    shmdt(count);

	/* Libera a memória alocada */
    shmctl(shmid_count, IPC_RMID, NULL);

	return 0;
}