#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "band_api.h"

// This should spend about 5s. Increase counts if not.
void spend_time() {
    int i, j;
    int sum = 0;
    for (i=0; i<5; i++) {
        for (j=0; j<15000000; j++) {
            sum++;
        }
    }
}

int main()
{
	// simple self test
    pid_t cpid = fork();
	int res = -1;
    time_t start_time, end_time;
            
    if (cpid == 0) {
        // In child: Create a band as the bass player, then block the parent
        band_create(2)
        spend_time();
        return 0;
    }
    // In parent: Give child time to create the band, then join and see if we're blocked
    sleep(1); // Wait for the child to create the band
    start_time = time(NULL);
    band_join(cpid, 0);
    end_time = time(NULL);
	// Make sure the process slept for at least 3 seconds because of its child blocking it
    // Ideally it would be 4, but we give some margin of error
	assert(end_time - start_time > 3);
	printf("Test Done\n");
	
	return 0;
}
