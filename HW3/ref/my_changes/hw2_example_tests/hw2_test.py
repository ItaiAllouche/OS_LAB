#!/usr/bin/python

import os
import errno
import pyBand
import time

def spend_time():
    for i in range(5):
        # This spends about 1s. If not, increase the range
        for j in xrange(15000000):
            pass

def test1():
    """ Verify a bass player blocks other processes """
    cpid = os.fork()
    if cpid == 0:
        # In child: Create a band as the bass player, then block the parent
        pyBand.band_create(2)
        spend_time()
        os._exit(0)
    # In parent: Give child time to create the band, then join and see if we're blocked
    time.sleep(1) # force de-scheduling of the paren
    s = time.time()
    pyBand.band_join(cpid, 0)
    e = time.time()
    elapsed = e - s
    assert (elapsed > 3), "Expected singer to yield time to bass player"

if __name__ == "__main__":
    test1()
