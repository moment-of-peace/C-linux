#include <stdio.h>
#include <libmemcached/memcached.h>

int main(int argc, char* argv[]) {
	memcached_server_st *servers = NULL;
	memcached_st *memc;
	memcached_return rc;
	char* key_sec = "second";
	char value_sec[2] = {'\0'};

	memc = memcached_create(NULL);
	servers = memcached_server_list_append(servers, "localhost", 11211, &rc);

	// Connect to memcached server
	rc = memcached_server_push(memc, servers);

	value_sec[0] = argv[1][0]-48;
	rc = memcached_set(memc, key_sec, strlen(key_sec), value_sec, strlen(value_sec), (time_t)0, (uint32_t)0);

	if (rc == MEMCACHED_SUCCESS) {
		
	} else {
		printf("Couldn't store key: %s\n", memcached_strerror(memc, rc));
	}

	memcached_flush(memc,0);
	memcached_free(memc);
	return 0;
}
