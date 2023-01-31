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
int *full;  // Semáforo
int *mutex; // Semáforo

int *buffer_count; // Contador do buffer

/* IDs dos seguimento de memória compartilhada */
int shmid_buffer_count;
int shmid_empty;
int shmid_full;
int shmid_mutex;

char items[] = {'!', '@', '#', '$', '%'};
int items_number = 5;

pid_t other;

void sleep_process()
{
  pid_t pid = getpid();
  kill(pid, SIGSTOP);
}

void wakeup_process(pid_t process_id)
{
  int status = kill(process_id, SIGCONT);

  if (status == -1)
  {
    perror("Error: kill");
    exit(1);
  }
}

/**
 * Implementação de up para um semáforo, mantendo a atomicidade da instrução.
 */
void up(int *semaphore)
{
  int lock;
  do
  {
    lock = *semaphore;
    if (lock == BUFFER_SIZE)
    {
      sleep_process();
    }
  } while (!__sync_bool_compare_and_swap(semaphore, lock, lock + 1));

  wakeup_process(other);
}

/**
 * Implementação de down para um semáforo, mantendo a atomicidade da instrução.
 */
void down(int *semaphore)
{
  int lock;
  do
  {
    lock = *semaphore;
    if (lock == 0)
    {
      sleep_process();
    }
  } while (!__sync_bool_compare_and_swap(semaphore, lock, lock - 1));
}

/**
 * Escreve no arquivo de buffer
 */
void insert_item(char item)
{
  FILE *file;
  file = fopen(FILE_PATH, "a");
  if (file == NULL)
  {
    perror("Error: fopens");
    exit(1);
  }

  fputc(item, file);
  fclose(file);
}

/**
 * Deleta do arquivo de buffer
 */
void remove_item()
{
  FILE *file;
  file = fopen(FILE_PATH, "a");
  if (file == NULL)
  {
    perror("Error: fopens");
    exit(1);
  }

  fseeko(file, -1, SEEK_END);
  int position = ftello(file);
  ftruncate(fileno(file), position);

  fclose(file);
}

/**
 * Exibe as informaçõe na tela
 */
void print_info()
{
  printf("PID: %d\n", getpid());
  printf("Empty: %d\n", *empty);
  printf("Full: %d\n", *full);
  printf("Buffer count: %d\n", *buffer_count);
  printf("\n\n");
}

char produce_item()
{
  return items[rand() % items_number];
}

void producer()
{
  char item;

  while (TRUE)
  {
    item = produce_item();

    down(empty);
    down(mutex);

    printf("Regiao Critica - Produtor\n");

    if (*buffer_count < BUFFER_SIZE)
    {
      insert_item(item);
      *buffer_count += 1;
    }

    up(mutex);
    up(full);

    printf("Item produzido: %c\n", item);
    print_info();
    sleep(1);
  }
}

void consumer(void)
{
  while (TRUE)
  {
    down(full);
    down(mutex);

    printf("Regiao Critica - Consumidor\n");

    if (*buffer_count > 0)
    {
      remove_item();
      *buffer_count -= 1;
    }

    up(mutex);
    up(empty);

    printf("Um item foi consumido\n");
    print_info();
    sleep(3);
  }
}

/**
 * Cria e zera o buffer
 */
void create_buffer()
{
  FILE *file;
  file = fopen(FILE_PATH, "w");
  if (file == NULL)
  {
    perror("Error: fopens");
    exit(1);
  }

  fclose(file);
}

/**
 * Inicializa as vairáveis globais
 */
void create_shm_variables()
{
  shmid_buffer_count = shmget(SHM_KEY + 1, SHM_SIZE, IPC_CREAT | 0666);
  buffer_count = (int *)shmat(shmid_buffer_count, NULL, 0);
  *buffer_count = 0;

  shmid_empty = shmget(SHM_KEY + 2, SHM_SIZE, IPC_CREAT | 0666);
  empty = (int *)shmat(shmid_empty, NULL, 0);
  *empty = BUFFER_SIZE;

  shmid_full = shmget(SHM_KEY + 3, SHM_SIZE, IPC_CREAT | 0666);
  full = (int *)shmat(shmid_full, NULL, 0);
  *full = 0;

  shmid_mutex = shmget(SHM_KEY + 4, SHM_SIZE, IPC_CREAT | 0666);
  mutex = (int *)shmat(shmid_mutex, NULL, 0);
  *mutex = 1;
}

/**
 *  Desmapeia a memória compartilhada e libera a memória alocada
 */
void dispose_shm_variables()
{
  shmdt(buffer_count);
  shmdt(full);
  shmdt(empty);
  shmdt(mutex);

  shmctl(shmid_buffer_count, IPC_RMID, NULL);
  shmctl(shmid_full, IPC_RMID, NULL);
  shmctl(shmid_empty, IPC_RMID, NULL);
  shmctl(shmid_mutex, IPC_RMID, NULL);
}

int main(int argc, char *argv[])
{
  create_buffer();
  create_shm_variables();

  pid_t pid = fork();

  if (pid == 0)
  {
    other = getppid();
    producer();
  }
  else
  {
    other = pid;
    consumer();
  }

  dispose_shm_variables();

  return 0;
}