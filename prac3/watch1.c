#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t semaphore_second;
sem_t semaphore_minute;

int subsec_count = 0;
int second_count = 0;
int minute_count = 0;

void subsec(void);
void second(void);
void minute(void);
void display(void);

int main(void) {

	pthread_t subsec_thread;
	pthread_t second_thread;
	pthread_t minute_thread;
	pthread_t display_thread;

	/* Initialise semaphore with initial value of 0.*/
	if (sem_init(&semaphore_second, 0, 0) == -1) {
    	printf("semaphore_second init error");
	}

	if (sem_init(&semaphore_minute, 0, 0) == -1) {
    	printf("semaphore_minute init error");
	}

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
		subsec_count++;

		if (subsec_count == 100) {
			subsec_count = 0;
			/* Post a semaphore */
			if (sem_post(&semaphore_second) == -1) {
		    	printf( "semaphore_second post failed\n");
			}

		}
		usleep(10000);
	}
}

/** 
 * This function is run by the second thread
 */
void second(void) {

	while(1) {
		/* Wait for semaphore to occur */
		if (sem_wait(&semaphore_second) == 0) {;
			second_count++;
		}

		if (second_count == 60) {
			second_count = 0;
			/* Post a semaphore */
			if (sem_post(&semaphore_minute) == -1) {
		    	printf( "semaphore_minute post failed\n");
			}
		}
	}
}

/** 
 * This function is run by the minute thread
 */
void minute(void) {

	while(1) {
		/* Wait for semaphore to occur */
		if (sem_wait(&semaphore_minute) == 0) {;
			minute_count++;
		}

		if (minute_count == 60) {
			minute_count = 0;
		}
	}
}

/** 
 * This function is run by the display thread
 */
void display(void) {

	while(1) {
		printf("\r%02d:%02d:%02d", minute_count, second_count, subsec_count);
		fflush(NULL);

		/* wait for 0.01 sec */
		usleep(10000);
	}
}

