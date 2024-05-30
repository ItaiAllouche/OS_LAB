#!/usr/bin/python

import os
import pyBand

SINGING = 0
GUITAR = 1
BASS = 2
DRUMS = 3


def test1():
    """simple test to invoke the syscalls with no error."""

    # Create a new band
    pyBand.band_create(SINGING)

    # Add ourselves to every instrument (yeah!)
    pyBand.band_join(0, GUITAR)
    pyBand.band_join(0, BASS)
    pyBand.band_join(0, DRUMS)

    # Play something
    pyBand.band_play(SINGING, 0)
    pyBand.band_play(GUITAR, 10)
    pyBand.band_play(BASS, 20)
    pyBand.band_play(DRUMS, 30)

    # Listen to our groovy music
    chord = pyBand.band_listen(os.getpid())
    assert chord[0] == 0
    assert chord[1] == 10
    assert chord[2] == 20
    assert chord[3] == 30


if __name__ == "__main__":
    test1()
