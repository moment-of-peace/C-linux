#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

int converter(char string[]);
void subsec(void);
void second(void);
void display(void);

struct Watch {
	int minute;
	int second;
	int subsec;
} watch_counter = {0, 0, 0};

int main(int argc, char *argv[]) {
	watch_counter.minute = converter(argv[1]);
	watch_counter.second = converter(argv[2]);
	watch_counter.subsec = converter(argv[3]);

	
	pthread_t display_thread;

	/* create the display thread */
	if(pthread_create(&display_thread, NULL, (void *)display, NULL)) {

		fprintf(stderr, "Error creating display thread\n");
		return 1;
	}

	while (watch_counter.minute >= 0) {
		pthread_t second_thread;

		/* create the second thread */
		if(pthread_create(&second_thread, NULL, (void *)second, NULL)) {

			fprintf(stderr, "Error creating second thread\n");
			return 1;
		}

		/* wait for the second thread to finish */
		if(pthread_join(second_thread, NULL)) {
			fprintf(stderr, "Error joining thread\n");
			return 2;
		}

		/* critical section */
		watch_counter.minute--;
		watch_counter.second = 59;
	}

	char chara;
	printf("\npress any key to quit\n");
	scanf("%c", &chara);

	return 0;
}

/**
 * This function is used to convert a string to a number
 */
int converter(char string[]) {
	int i;
	int sum = 0;

	for (i = 0; i < strlen(string); i++) {
		sum = sum * 10 + string[i] - 48;
	}

	return sum;
}

/** 
 * This function is run by the second thread
 */
void second(void) {
	

	while (watch_counter.second >= 0) {
		pthread_t subsec_thread;

		/* create the subsec thread */
		if(pthread_create(&subsec_thread, NULL, (void *)subsec, NULL)) {
			fprintf(stderr, "Error creating subsec thread\n");
		}

		/* wait for the second thread to finish */
		if(pthread_join(subsec_thread, NULL)) {
			fprintf(stderr, "Error joining thread\n");
		}

		/* critical section */
		watch_counter.second--;
		watch_counter.subsec = 99;
	}
}

/** 
 * This function is run by the subsec thread
 */
void subsec(void) {

	while(watch_counter.subsec >= 0) {
		usleep(10000);

		/* critical section */
		watch_counter.subsec--;
	}
}

/** 
 * This function is run by the display thread
 */
void display(void) {

	while(watch_counter.minute >= 0) {
		printf("\r%02d:%02d:%02d", watch_counter.minute, watch_counter.second,
			 watch_counter.subsec);
		fflush(NULL);

		/* wait for 0.01 sec */
		usleep(10000);
	}
}

