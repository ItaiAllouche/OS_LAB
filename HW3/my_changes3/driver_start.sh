#!/bin/bash

# gcc –I/usr/src/linux-2.4.18-14/include -c vegenere.c
make
insmod ./vegenere.o
major=`cat /proc/devices | grep vegenere | awk '{print $1}'`
echo $major
mknod /dev/vegenere1 c $major 1
mknod /dev/vegenere2 c $major 2
#mknod /dev/vegenere3 c $major 3
