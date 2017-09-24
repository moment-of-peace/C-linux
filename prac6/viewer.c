#include <libmemcached/memcached.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	char type[10] = {'\0'};
	memcached_server_st *servers = NULL;
  	memcached_st *memc;
  	memcached_return rc;
  	
  	char *retrieved_value;
  	size_t value_length;
  	uint32_t flags;
	int display;

	strcpy(type, argv[1]);
  	memc = memcached_create(NULL);
	servers = memcached_server_list_append(servers, "localhost", 11211, &rc);

	// Connect to memcached server
	rc = memcached_server_push(memc, servers);
	if (rc == MEMCACHED_SUCCESS) {
		
	} else {
		printf("ERROR Couldn't add server: %s\n", memcached_strerror(memc, rc));
	}

	puts(type);

	while(1) {
		// Get a key value from memcached
		retrieved_value = memcached_get(memc, type, strlen(type), &value_length, &flags, &rc);
	
		if (rc == MEMCACHED_SUCCESS) {
			if (retrieved_value[0] == 60)
				display = 0;
			else
				display = retrieved_value[0];

			printf("\r%s: %d  ", type, display);
			fflush(NULL);
		} else {
			printf("Couldn't retrieve key: %s\n", memcached_strerror(memc, rc));
		}
		usleep(100000);
	}

	return 0;
}