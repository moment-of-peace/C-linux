/**
 *
 * Title: ex1_pthreads/main.c, Pthreads Example
 *
 * Description: This example shows how pthreads are created and used.
 *				Two pthreads are created. Each pthread increments a counter.
 *
 * Reference: https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/fork.html
 *
 * Modified by: MD & VS
 *
 * Date: 20/7/2016
 */

#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#include <event.h>

int stop = 0;
int stop2 = 0;

void catch(int snum);
void catch2(int snum);

void event_timer_callback(int fd, short event, void *arg) {

  printf("event timer callback called\n");
}

/** 
 * This function is run by the second thread 
 */
void *inc_x(void *x_void_ptr) {

	/* increment x to 100 */
	int *x_ptr = (int *)x_void_ptr;

	while(++(*x_ptr) < 61) {
		if((*x_ptr) == 60)
			(*x_ptr) = 0;

		//printf("x = %d: ", *x_ptr); 
		sleep(60);}

	printf("x increment finished\n");

	/* the function must return something - NULL will do */
	return NULL;

}

/** 
 * This function is run by the third thread 
 */
void *inc_z(void *z_void_ptr) {

	/* increment z to 100 */
	int *z_ptr = (int *)z_void_ptr;

	while(++(*z_ptr) < 61) {
		if((*z_ptr) == 60)
			*z_ptr = 0;
		//printf("z = %d\n", *z_ptr); 
		sleep(1);}

	printf("z increment finished\n");

	/* the function must return something - NULL will do */
	return NULL;

}

int main() {

	struct event ev;
  	struct timeval tv;

  	tv.tv_sec = 10;		/* Set timeout time to be 3 seconds */
  	tv.tv_usec = 0;

	int x = -1, y = -1, z = 0;

	signal(SIGUSR1, catch);
	signal(SIGUSR2, catch);

	printf("kill -s USR %d\n", getpid());
	
	pthread_t inc_x_thread;			/* this variable is our reference to the second thread */
	pthread_t inc_z_thread;	

	/* show the initial values of x and y */
	//printf("x: %d, y: %d, z %d\n", x, y, z);

	/* create a second thread which executes inc_x(&x) */
	if(pthread_create(&inc_x_thread, NULL, inc_x, &x)) {

		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	
	/* create a third thread which executes inc_(&x) */
	if(pthread_create(&inc_z_thread, NULL, inc_z, &z)) {

		fprintf(stderr, "Error creating thread\n");
		return 1;
	}

	/* Initialise event library, set/add timer event */
  	event_init();
  	evtimer_set(&ev, event_timer_callback, NULL);
  	evtimer_add(&ev, &tv);

	/* Wait for event to occur */
  	event_dispatch();
	/* increment y to 100 in the first thread */
	while(++y < 101) {
		if(y == 100) 
			y = 0;

		printf("%02d: %02d: %02d\r", x, z, y);
		fflush(NULL);
		usleep(10000);
		}
	

	printf("y increment finished\n");

	/* wait for the second thread to finish */
	if(pthread_join(inc_x_thread, NULL)) {

		fprintf(stderr, "Error joining thread\n");
		return 2;
	}

	/* show the results - x is now 100 thanks to the second thread */
	printf("x: %d, y: %d, z %d\n", x, y, z);

	return 0;

}

/**
 * Catch function - executed when signal is caught
 */
void catch(int snum) {
	
	printf("got signal %d\n", snum);
	stop = 1;

	/* on SunOS systems, we have to  reset the signal catcher */
	signal(SIGUSR1, catch);	/* don't need this on Linux o (but doesn't hurt) */
}

/**
 * Catch function - executed when signal is caught
 */
void catch2(int snum) {
	
	//printf("got signal %d\n", snum);
	stop2 = 1;

	/* on SunOS systems, we have to  reset the signal catcher */
	signal(SIGUSR2, catch);	/* don't need this on Linux o (but doesn't hurt) */
}
