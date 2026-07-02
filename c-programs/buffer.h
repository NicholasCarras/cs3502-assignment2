#ifndef BUFFER_H
#define BUFFER_H

#define BUFFER_SIZE 10
#define SHM_Key 0x1234

typedef struct {
	int value;
	int producer_id;
} item_t;

typedef struct {
	item_t buffer[BUFFER_SIZE];
	int head;
	int tail;
	int count;
} shared_buffer_t

#endif
