#!/usr/bin/python

import os
import pyMpi
import errno

def test1():
    """simple test to invoke the syscalls with no error."""
    pid = os.getpid()
    # Register
    pyMpi.register()
    
    # Send ourselves some messages. Python automatically sends the size.
    pyMpi.send(pid, "Message 1")
    pyMpi.send(pid, "Message 2")
    
    # Receive one message. Python allocates and returns the buffer.
    message = pyMpi.receive(pid, 100)

    # Make sure the first message was received
    assert (message == "Message 1")
    print("test1 passed!")

def test2():
    """Test receiving messages in the correct order."""
    pid = os.getpid()
    pyMpi.register()
    
    pyMpi.send(pid, "First Message")
    pyMpi.send(pid, "Second Message")
    
    message1 = pyMpi.receive(pid, 100)
    message2 = pyMpi.receive(pid, 100)

    assert (message1 == "First Message")
    assert (message2 == "Second Message")
    print("test2 passed!")

def test3():
    """Test receiving a message with buffer size smaller than the message."""
    pid = os.getpid()
    pyMpi.register()
    
    pyMpi.send(pid, "This is a long message")
    
    message = pyMpi.receive(pid, 10)

    assert (message == "This is a ")
    print("test3 passed!")

def test4():
    """Test sending and receiving with invalid inputs."""
    pid = os.getpid()
    pyMpi.register()
    
    try:
        pyMpi.send(-1, "Invalid PID")
    except OSError, e:
        assert e.errno == errno.ESRCH

    try:
        pyMpi.receive(pid, -1)
    except OSError, e:
        assert e.errno == errno.EINVAL

    print ("test4 passed!")

if __name__ == "__main__":
    test1()
    # test2()
    # test3()
    # test4()
