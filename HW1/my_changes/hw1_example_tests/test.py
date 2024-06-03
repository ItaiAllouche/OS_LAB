#!/usr/bin/python

import os
import pyMpi
import errno

import os

def get_parent_pid():
    pid = os.getpid()
    try:
        f = open('/proc/%d/status' % pid, 'r')
        try:
            for line in f:
                if line.startswith('PPid:'):
                    return int(line.split()[1])
        finally:
            f.close()
    except IOError:
        return None

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
    print("in test4")

    pid = os.fork()
    pyMpi.register()
    
    # parent process
    if pid > 0:
        # wait for child process to send message to parent
        os.wait()
        assert pyMpi.receive(pid, 13)== "Hello parent!"

    # child process
    else:
        pyMpi.send(get_parent_pid(), "Hello parent!")

    print("test4 passed!")

if __name__ == "__main__":
    # test1()
    # test2()
    # test3()
    test4()
