#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void subsec(void);
void second(void);
void minute(void);
void display(void);

pthread_mutex_t countmutex = PTHREAD_MUTEX_INITIALIZER;		/* Counter mutex */ 

/* condition variables */
pthread_cond_t count_threshold_subsec;			
pthread_cond_t count_threshold_second;

struct Watch {
	int minute;
	int second;
	int subsec;
} watch_counter = {0, 0, 0};

int main(void) {
	/* Initialise mutex and condition variables */
	pthread_mutex_init(&countmutex, NULL);
	pthread_cond_init (&count_threshold_subsec, NULL);
	pthread_cond_init (&count_threshold_second, NULL);

	pthread_t subsec_thread;
	pthread_t second_thread;
	pthread_t minute_thread;
	pthread_t display_thread;

	/* create the subsec thread */
	if(pthread_create(&subsec_thread, NULL, (void *)subsec, NULL)) {

		fprintf(stderr, "Error creating subsec thread\n");
		return 1;
	}

	/* create the second thread */
	if(pthread_create(&second_thread, NULL, (void *)second, NULL)) {

		fprintf(stderr, "Error creating second thread\n");
		return 1;
	}

	/* create the minute thread */
	if(pthread_create(&minute_thread, NULL, (void *)minute, NULL)) {

		fprintf(stderr, "Error creating minute thread\n");
		return 1;
	}

	/* create the display thread */
	if(pthread_create(&display_thread, NULL, (void *)display, NULL)) {

		fprintf(stderr, "Error creating display thread\n");
		return 1;
	}

	while(1);

	return 0;
}

/** 
 * This function is run by the subsec thread
 */
void subsec(void) {

	while(1) {
		usleep(10000);

		/* Lock Mutex */
		pthread_mutex_lock (&countmutex);

		/* critical section */
		/* increase subsec */
		watch_counter.subsec++;

		if (watch_counter.subsec == 100) {
			watch_counter.subsec = 0;
			pthread_cond_signal(&count_threshold_subsec);
		}
		
		/* unlock mutex */
		pthread_mutex_unlock (&countmutex);
	}
}

/** 
 * This function is run by the second thread
 */
void second(void) {

	while(1) {
		/* Lock Mutex */
		pthread_mutex_lock (&countmutex);

		/* critical section */
		/* wait for condition variable */
		pthread_cond_wait(&count_threshold_subsec, &countmutex);

		/* increase second */
		watch_counter.second++;
		watch_counter.subsec = 0;
		
		if (watch_counter.second == 60) {
			watch_counter.second = 0;
			pthread_cond_signal(&count_threshold_second);
		}
		/* unlock mutex */
		pthread_mutex_unlock (&countmutex);
	}
}

/** 
 * This function is run by the minute thread
 */
void minute(void) {

	while(1) {
		/* Lock Mutex */
		pthread_mutex_lock (&countmutex);

		/* critical section */
		/* wait for condition variable */
		pthread_cond_wait(&count_threshold_second, &countmutex);

		/* increase second */
		watch_counter.minute++;
		watch_counter.second = 0;
		watch_counter.subsec = 0;
		
		/* Wait for semaphore to occur */
		pthread_mutex_unlock (&countmutex);
	}
}

/** 
 * This function is run by the display thread
 */
void display(void) {

	while(1) {
		printf("\r%02d:%02d:%02d", watch_counter.minute, watch_counter.second,
			 watch_counter.subsec);
		fflush(NULL);

		/* wait for 0.01 sec */
		usleep(10000);
	}
}

