/*
 *	File	: pc.c
 *
 *	Title	: Demo Producer/Consumer.
 *
 *	Short	: A solution to the producer consumer problem using
 *		pthreads.	
 *
 *	Long 	:
 *
 *	Author	: Andrae Muys
 *
 *	Date	: 18 September 1997
 *
 *	Revised	:
 */

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "prod_cons.h"
#include "csapp.h"

#define LOOP 5


/*int main ()
{
	queue *fifo;
	pthread_t pro, con;

	fifo = queueInit ();
	if (fifo ==  NULL) {
		fprintf (stderr, "main: Queue Init failed.\n");
		exit (1);
	}
	pthread_create (&pro, NULL, producer, fifo);
	pthread_create (&con, NULL, consumer, fifo);
	pthread_join (pro, NULL);
	pthread_join (con, NULL);
	queueDelete (fifo);

	return 0;
}

void *producer (void *q)
{
	queue *fifo;
	int i;

	fifo = (queue *)q;

	for (i = 0; i < LOOP; i++) {
		pthread_mutex_lock (fifo->mut);
		while (fifo->full) {
			printf ("producer: queue FULL.\n");
			pthread_cond_wait (fifo->notFull, fifo->mut);
		}
		queueAdd (fifo, i);
		pthread_mutex_unlock (fifo->mut);
		pthread_cond_signal (fifo->notEmpty);
		millisleep (200);
	}
	return (NULL);
}

void *consumer (void *q)
{
	queue *fifo;
	int i, d;

	fifo = (queue *)q;

	for (i = 0; i < LOOP; i++) {
		pthread_mutex_lock (fifo->mut);
		while (fifo->empty) {
			printf ("consumer: queue EMPTY.\n");
			pthread_cond_wait (fifo->notEmpty, fifo->mut);
		}
		queueDel (fifo, &d);
		pthread_mutex_unlock (fifo->mut);
		pthread_cond_signal (fifo->notFull);
		printf ("consumer: recieved %d.\n", d);
		millisleep(100);
	}
	return (NULL);
}

*/

void *producer (void *q)
{
	port_queue *tmp;
	queue *fifo;
	int i, clientlen, listenfd, port;
	struct sockaddr_in clientaddr;

	tmp = (port_queue *)q;
	fifo = (queue *)tmp->q;
	port = tmp->port;
	
	listenfd = Open_listenfd(port);
	printf("Producer: port open %d\n", port);

	for (;;) {
		clientlen = sizeof(clientaddr);	
		printf("Producer: Waiting for request....\n");
		i = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		printf("Producer: Request accepted %d\n", i);
		pthread_mutex_lock (fifo->mut);
		printf ("Producer: holds lock\n");
		while (fifo->full) {
			printf ("Producer: queue FULL.\n");
			pthread_cond_wait (fifo->notFull, fifo->mut);
		}
		queueAdd (fifo, i);
		printf("Producer: Added to queue %d\n", i);
		pthread_cond_signal (fifo->notEmpty);
		printf ("Producer: giving up lock\n");
		pthread_mutex_unlock (fifo->mut);
	}
	pthread_exit(NULL);
}


void *consumer (void *q)
{
	queue *fifo;
	int i, d;

	fifo = (queue *)q;

	for (;;) {
		pthread_mutex_lock (fifo->mut);
		//millisleep(5000);
		printf("Consumer %u: holds lock\n", (unsigned int) Pthread_self());
		while (fifo->empty) {
			printf ("Consumer %u: queue EMPTY.\n", (unsigned int) Pthread_self());
			printf ("Consumer %u: sleep. giving up lock\n", (unsigned int) Pthread_self());
			pthread_cond_wait (fifo->notEmpty, fifo->mut);
		}
		queueDel (fifo, &d);
		pthread_cond_signal (fifo->notFull);
		printf ("Consumer %u: Read data %d. giving up lock\n", (unsigned int) Pthread_self(), d);
		pthread_mutex_unlock (fifo->mut);
		printf ("Consumer %u: Request being processed.....\n", (unsigned int) Pthread_self());
		doit(d);
		printf ("Consumer %u: recieved %d.\n", (unsigned int) Pthread_self(), d);
		Close(d);
		//while(1);
	}
	pthread_exit(NULL);
}




queue *queueInit (void)
{
	queue *q;

	q = (queue *)malloc (sizeof (queue));
	if (q == NULL) return (NULL);

	q->empty = 1;
	q->full = 0;
	q->head = 0;
	q->tail = 0;
	q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
	pthread_mutex_init (q->mut, NULL);
	q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notFull, NULL);
	q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
	pthread_cond_init (q->notEmpty, NULL);
	
	return (q);
}

void queueDelete (queue *q)
{
	pthread_mutex_destroy (q->mut);
	free (q->mut);	
	pthread_cond_destroy (q->notFull);
	free (q->notFull);
	pthread_cond_destroy (q->notEmpty);
	free (q->notEmpty);
	free (q);
}

void queueAdd (queue *q, int in)
{
	q->buf[q->tail] = in;
	q->tail++;
	if (q->tail == QUEUESIZE)
		q->tail = 0;
	if (q->tail == q->head)
		q->full = 1;
	q->empty = 0;

	return;
}

void queueDel (queue *q, int *out)
{
	*out = q->buf[q->head];

	q->head++;
	if (q->head == QUEUESIZE)
		q->head = 0;
	if (q->head == q->tail)
		q->empty = 1;
	q->full = 0;

	return;
}

void millisleep(int milliseconds)
{
      usleep(milliseconds * 1000);
}
