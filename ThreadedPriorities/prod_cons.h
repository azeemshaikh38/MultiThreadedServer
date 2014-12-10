#ifndef HEADER_QUEUE
#define HEADER_QUEUE

#define QUEUESIZE 6
#define MAXTHREAD 3

void *producer (void *args);
void *consumer (void *args);

typedef struct {
	int buf[QUEUESIZE];
	long head, tail;
	int full, empty;
	pthread_mutex_t *mut;
	pthread_cond_t *notFull, *notEmpty;
} queue;

typedef struct {
	int port;
	queue *q;
} port_queue;

queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, int in);
void queueDel (queue *q, int *out);
void millisleep(int milliseconds);

#endif
