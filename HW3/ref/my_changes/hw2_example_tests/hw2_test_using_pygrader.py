#!/usr/bin/python

import os
import errno
import pyBand
import time
from hwgrader.test_utils import *

def spend_time():
    for i in range(5):
        # This spends about 1s. If not, increase the range
        for j in xrange(15000000):
            pass

def test1():
    """ Verify a bass player blocks other processes """
    fork = tfork2()
    if fork.isChild:
        # In child: Create a band as the bass player, then block the parent
        pyBand.band_create(2)
        fork.relase() # Release the parent to join the band
        s = time.time()
        spend_time()
        e = time.time()
        fork.send(str(e-s)) # Not useful currently, but just to show how it's used
        fork.exit()
    # In parent: Give child time to create the band, then join and see if we're blocked
    fork.sync() # Wait until child has created the band
    s = time.time()
    pyBand.band_join(fork.cpid, 0)
    e = time.time()
    child_elapsed = float(fork.receive())
    print("Child ran for:", child_elapsed)
    fork.wait()
    elapsed = e - s
    assert (elapsed > 3), "Expected singer to yield time to bass player"

if __name__ == "__main__":
    test1()
