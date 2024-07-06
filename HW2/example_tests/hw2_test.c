#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "mpi_api.h"

int main()
{
	// simple self test
    pid_t cpid, ppid;
    struct mpi_poll_entry poll_entry;
	int res = -1;
    time_t start_time, end_time;
    
    ppid = getpid();
    mpi_register();
    cpid = fork();
    if (cpid == 0) {
        // In child: wait a bit and send a message
        sleep(3);
        mpi_send(ppid, "Hello!", 6);
        return 0;
    }
    // In parent: poll for new messages from child
    poll_entry.pid = cpid;
    start_time = time(NULL);
    res = mpi_poll(&poll_entry, 1, 10);
    end_time = time(NULL);

    // Make sure return value and polling time was as expected
    assert(res == 1);
	assert(end_time - start_time > 2);
	assert(end_time - start_time < 4);
	printf("Test Done\n");
	
	return 0;
}
