// Nicholas Carrasquilla

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>

#include "buffer.h"

#define SEM_MUTEX "/buffer_mutex"
#define SEM_EMPTY "/buffer_empty"
#define SEM_FULL  "/buffer_full"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./consumer <id> <num_items>\n");
        return 1;
    }

    int consumer_id = atoi(argv[1]);
    int num_items = atoi(argv[2]);

    int shmid = shmget(SHM_KEY, sizeof(shared_buffer_t), 0666);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    shared_buffer_t *shared_buffer = (shared_buffer_t *) shmat(shmid, NULL, 0);
    if (shared_buffer == (void *) -1) {
        perror("shmat");
        return 1;
    }

    sem_t *mutex = sem_open(SEM_MUTEX, 0);
    sem_t *empty = sem_open(SEM_EMPTY, 0);
    sem_t *full = sem_open(SEM_FULL, 0);

    if (mutex == SEM_FAILED || empty == SEM_FAILED || full == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    for (int i = 0; i < num_items; i++) {
        sem_wait(full);
        sem_wait(mutex);

        item_t item = shared_buffer->buffer[shared_buffer->tail];
        shared_buffer->tail = (shared_buffer->tail + 1) % BUFFER_SIZE;
        shared_buffer->count--;

        printf("Consumer %d: Consumed value %d from Producer %d\n",
               consumer_id, item.value, item.producer_id);

        sem_post(mutex);
        sem_post(empty);
    }

    shmdt(shared_buffer);

    sem_close(mutex);
    sem_close(empty);
    sem_close(full);

    return 0;
}
