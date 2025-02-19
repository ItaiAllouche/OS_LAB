#!/usr/bin/python

from __future__ import division
from struct import *
import fcntl
import struct
import os
import errno

#
# Globals
#
DEVICE_PATH = '/dev/vegenere1'

#
# Utilities for calculating the IOCTL command codes.
#
sizeof = {
    'byte': calcsize('c'),
    'signed byte': calcsize('b'),
    'unsigned byte': calcsize('B'),
    'short': calcsize('h'),
    'unsigned short': calcsize('H'),
    'int': calcsize('i'),
    'unsigned int': calcsize('I'),
    'long': calcsize('l'),
    'unsigned long': calcsize('L'),
    'long long': calcsize('q'),
    'unsigned long long': calcsize('Q')
}

_IOC_NRBITS = 8
_IOC_TYPEBITS = 8
_IOC_SIZEBITS = 14
_IOC_DIRBITS = 2

_IOC_NRMASK = ((1 << _IOC_NRBITS)-1)
_IOC_TYPEMASK = ((1 << _IOC_TYPEBITS)-1)
_IOC_SIZEMASK = ((1 << _IOC_SIZEBITS)-1)
_IOC_DIRMASK = ((1 << _IOC_DIRBITS)-1)

_IOC_NRSHIFT = 0
_IOC_TYPESHIFT = (_IOC_NRSHIFT+_IOC_NRBITS)
_IOC_SIZESHIFT = (_IOC_TYPESHIFT+_IOC_TYPEBITS)
_IOC_DIRSHIFT = (_IOC_SIZESHIFT+_IOC_SIZEBITS)

_IOC_NONE = 0
_IOC_WRITE = 1
_IOC_READ = 2

def _IOC(dir, _type, nr, size):
    if type(_type) == str:
        _type = ord(_type)
        
    cmd_number = (((dir)  << _IOC_DIRSHIFT) | \
        ((_type) << _IOC_TYPESHIFT) | \
        ((nr)   << _IOC_NRSHIFT) | \
        ((size) << _IOC_SIZESHIFT))

    return cmd_number

def _IO(_type, nr):
    return _IOC(_IOC_NONE, _type, nr, 0)

def _IOR(_type, nr, size):
    return _IOC(_IOC_READ, _type, nr, sizeof[size])

def _IOW(_type, nr, size):
    return _IOC(_IOC_WRITE, _type, nr, sizeof[size])

def add_null(key):
    return key + "\0"

def test1():
    """Test the device driver"""
    
    #
    # Calculate the ioctl cmd number
    #
    print("in test1")
    MY_MAGIC = 'r'
    SET_KEY = _IOW(MY_MAGIC, 0, 'int')
    RESET = _IO(MY_MAGIC, 1)
    DEBUG = _IOW(MY_MAGIC, 2, 'int')

    # Open the device file
    f = os.open(DEVICE_PATH, os.O_RDWR)
    
    # Set a key
    fcntl.ioctl(f, SET_KEY, add_null("abc"))

    # Write a message
    message = 'Hello'
    os.write(f, message)

    # Read back the same message. Notice that we request more bytes than we must.
    read_message = os.read(f, 10)
    
    # Messages should be identical
    assert (message == read_message)

    # Finaly close the device file
    os.close(f)
    print("test1 passed")

def test2():
    """Test the device driver"""
    
    #
    # Calculate the ioctl cmd number
    #
    print("in test2")
    MY_MAGIC = 'r'
    SET_KEY = _IOW(MY_MAGIC, 0, 'int')
    RESET = _IO(MY_MAGIC, 1)
    DEBUG = _IOW(MY_MAGIC, 2, 'int')

    # Open the device file
    f = os.open(DEVICE_PATH, os.O_RDWR)
    
    # Set a key
    fcntl.ioctl(f, SET_KEY, add_null("11111111111111111111111111"))

    # Write a message
    message = 'Hello'
    os.write(f, message)

    message2 = " World!"
    os.write(f, message2)

    # Read back the same message. Notice that we request more bytes than we must.
    read_message = os.read(f, 11)
    
    # Messages should be identical
    if read_message != "Hello World":
        print("message ",read_message, "was read != Hello World")
        assert False

    message3 = "i cant hear you!"
    os.write(f, message3)

    read_message2 = os.read(f, 7)

    if read_message2 != "!i cant":
        print("message ",read_message2, "was read != !i cant")
        assert False

    # Move offset by 1
    os.read(f, 1)

    read_message3 = os.read(f, 9)
    if read_message3 != "hear you!":
        print("message ",read_message3, "was read != hear you!")
        assert False       

    # Finaly close the device file
    os.close(f)
    print("test2 passed")

def test3():
    """Test the device driver"""
    
    #
    # Calculate the ioctl cmd number
    #
    print("in test3")
    MY_MAGIC = 'r'
    SET_KEY = _IOW(MY_MAGIC, 0, 'int')
    RESET = _IO(MY_MAGIC, 1)
    DEBUG = _IOW(MY_MAGIC, 2, 'int')

    # Open the device file
    f = os.open(DEVICE_PATH, os.O_RDWR)
    
    # Set a key
    fcntl.ioctl(f, SET_KEY, add_null("ABC"))

    # Write a message
    message = 'Hello'
    os.write(f, message)

    # Enable deubg
    # No encryption-decryption mechanizem
    fcntl.ioctl(f, DEBUG, 1)

    # Write a message
    message = ' you'
    os.write(f, message)

    read_message = os.read(f, 9)

    # Messages should be identical
    if read_message != "Igomq you":
        print("message ",read_message, "was read != Igomq you")
        assert False
    
    # Finaly close the device file
    os.close(f)
    print("test3 passed")

def test4():
    """Test the device driver"""
    
    #
    # Calculate the ioctl cmd number
    #
    print("in test4")
    MY_MAGIC = 'r'
    SET_KEY = _IOW(MY_MAGIC, 0, 'int')
    RESET = _IO(MY_MAGIC, 1)
    DEBUG = _IOW(MY_MAGIC, 2, 'int')

    # Open the device file
    f = os.open(DEVICE_PATH, os.O_RDWR)
    
    # Set a key
    fcntl.ioctl(f, SET_KEY, add_null("ABC"))

    # Write a message
    message = 'KAPARA'
    os.write(f, message)

    # RESET -  free message buffer and key
    fcntl.ioctl(f, RESET, 1)

    fcntl.ioctl(f, SET_KEY, add_null("DEF"))

    # Write a message
    message1 = 'SHELI'
    os.write(f, message1)

    read_message = os.read(f, 5)

    # Messages should be identical
    if read_message != "SHELI":
        print("message ",read_message, "was read != SHELI")
        assert False
    
    # Finaly close the device file
    os.close(f)
    print("test4 passed")

def test5():
    """Test the device driver"""
    
    #
    # Calculate the ioctl cmd number
    #
    print("in test5")
    MY_MAGIC = 'r'
    SET_KEY = _IOW(MY_MAGIC, 0, 'int')
    RESET = _IO(MY_MAGIC, 1)
    DEBUG = _IOW(MY_MAGIC, 2, 'int')

    # Open the device file
    f = os.open(DEVICE_PATH, os.O_RDWR)
    
    # Set a key
    fcntl.ioctl(f, SET_KEY, add_null("ABCDEFGH"))

    # Write a message
    message = 'Solution to HW3'
    os.write(f, message)

    # Enable Debug
    fcntl.ioctl(f, DEBUG, 1)

    # Write a message

    read_message = os.read(f, 8)

    # Messages should be identical
    if read_message != "Tqoyyovv":
        print("message ",read_message, "was read != Tqoyyovv")
        assert False
    
    # Move offset to start of message
    os.lseek(f, -30, 0)
    read_message1 = os.read(f, 15)

    # Messages should be identical
    if read_message1 != "Tqoyyovv vr McA":
        print("message ",read_message1, "was read != Tqoyyovv vr McA")
        assert False

    read_message2 = os.read(f, 17)
    if read_message2 != "":
        print("message ",read_message2, "was read != ")
        assert False

   # Disable deubg
    fcntl.ioctl(f, DEBUG, 0) 

    # Move offset to start of message
    os.lseek(f, -97, 0)

    read_message3 = os.read(f, 35)

    # Messages should be identical
    if read_message3 != "Solution to HW3":
        print("message ",read_message3, "was read != Solution to HW3")
        assert False            

    # Finaly close the device file
    os.close(f)
    print("test5 passed")

def test6():
    """Test the device driver"""
    
    #
    # Calculate the ioctl cmd number
    #
    print("in test6")
    MY_MAGIC = 'r'
    SET_KEY = _IOW(MY_MAGIC, 0, 'int')
    RESET = _IO(MY_MAGIC, 1)
    DEBUG = _IOW(MY_MAGIC, 2, 'int')

    # Open the device file
    f = os.open(DEVICE_PATH, os.O_RDWR)
    
    # Set a key
    fcntl.ioctl(f, SET_KEY, add_null("abc"))

    # Write a message
    message = 'Hell'
    os.write(f, message)

    # Read back the same message. Notice that we request more bytes than we must.
    read_message = os.read(f, 100)
    
    # Messages should be identical
    # print("msg = "+message)
    # print("read msg = "+read_message)
    assert (message == read_message)

    print("test6 passed")

    # Finaly close the device file
    os.close(f)  
if __name__ == '__main__':
    # test1()
    # test2()
    test3()
    # test4()
    # test5()
    # test6()
