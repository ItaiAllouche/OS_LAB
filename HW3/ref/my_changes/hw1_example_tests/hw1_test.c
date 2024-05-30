#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

#include "band_api.h"

#define SINGING 0
#define GUITAR 1
#define BASS 2
#define DRUMS 3

int main()
{
	int res = -1;
    unsigned char chord[4];
    pid_t pid = getpid()

	// Create a new band
	res = band_create(SINGING);
	assert(res == 0);

	// Add ourselves to every instrument (yeah!)
	res = band_join(0, GUITAR);
	assert(res == 0);
	res = band_join(0, BASS);
	assert(res == 0);
	res = band_join(0, DRUMS);
	assert(res == 0);

    // Play something
    res = band_play(SINGING, 0);
    assert(ret == 0)
    res = band_play(GUITAR, 10);
    assert(ret == 0)
    res = band_play(BASS, 20);
    assert(ret == 0)
    res = band_play(DRUMS, 30);
    assert(ret == 0)

    // Listen to our groovy music
    res = band_listen(pid, &chord);
    assert(res == 0);
    assert(chord[0] == 0);
    assert(chord[1] == 10);
    assert(chord[2] == 20);
    assert(chord[3] == 30);

	return 0;
}
