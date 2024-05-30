#!/usr/bin/python


#############################################################################################
#Run cmd> python join_with_cretain_player_and_pid.py <pid_to_join> <instrument> <time_to_run>
#############################################################################################

import sys
import os
import errno
import pyBand
import time

def spend_time(time):
    for i in range(time):
        # This spends about 1s. If not, increase the range
        for j in xrange(15000000):
            pass
        print(i)

def test1():
    temp_string = "This process pid is: %d" % os.getpid()
    print(temp_string)
    arguments = sys.argv
    pid = int(arguments[1])
    player = int(arguments[2])
    time_to_spend = int(arguments[3])

    pyBand.band_join(pid, player)

    formatted_string = "Started spending time with %d-player: %d" % (player, time_to_spend)
    print(formatted_string)
    spend_time(time_to_spend)
    print("Done spending")


if __name__ == "__main__":
    test1()

