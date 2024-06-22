#!/usr/bin/python

import os
import pyMpi
import errno
import time

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

def test5():
    """simple test to invoke the syscalls with no error."""
    
    parent_pid = os.getpid()
    print("parent_pid: ")
    print(parent_pid)
    # Register
    pyMpi.register()
    child_pid = os.fork()
    if (child_pid == 0): #child process running
        child_pid = os.getpid()
        print("child_pid from child POV: ")
        print(child_pid)
        pyMpi.send(parent_pid, "Message 1")
    else: #parent
        print("child_pid from parent POV: ")
        print(child_pid)
        os.waitpid(child_pid, 0) #wait for child to finish sending
        message = pyMpi.receive(child_pid, 100)
        print("message: ")
        print(message)
        # Make sure the first message was received
        assert (message == "Message 1")
        print("test5 pass!!")

#sent to a not existing pid
def our_test1():
    """simple test to invoke the syscalls with no error."""
    print 'our test - 1 -  sent to a not existing pid'
    pid = os.getpid()

    # Register
    pyMpi.register()
    
    # Send ourselves some messages
    pyMpi.send(pid+1, "Message 1")

#receive with no msg in qeue 
def our_test1_1():
    """simple test to invoke the syscalls with no error."""
    print 'our test - 1.1 -  receive with no msg in qeue'
    pid = os.getpid()

    # Register
    pyMpi.register()
    
    # Send ourselves some messages
    pyMpi.receive(pid, 3)

#receive without being registered 
def our_test1_2():
    """simple test to invoke the syscalls with no error."""
    print 'our test - 1.2 -  receive without being registered'
    pid = os.getpid()
    pyMpi.receive(pid, 4)


	#sent to a registered pid from a not registered pid 
def our_test3_1():
    print 'our test - 3_1 -  sent to a registered pid from a not registered pid'
    ppid = os.getpid()
  
    cpid = os.fork() # cpid contains the child's PID for the parent and zero for the child
    if cpid == 0:
	    # Register
        pyMpi.register()
     
     
    else: 
        os.wait() # Make the parent wait until the child exists
        pyMpi.send(cpid, "Message from not registered proc")	



	#sent to a not registered pid from a registered pid 
def our_test3_2():
    print 'our test - 3_2 -  sent to a not registered pid from a registered pid'
    ppid = os.getpid()
    cpid = os.fork() # cpid contains the child's PID for the parent and zero for the child
    if cpid == 0:
	    # Register
        pyMpi.register()
        pyMpi.send(ppid, "Message from registered proc to unregistered")

 
    else: 
        os.wait() # Make the parent wait until the child exists
		


	#sent from a registered pid to a unregistered pid , and tring to receive
def our_test3_3():
    print 'our test - 3_3 -  sent from a registered pid to a unregistered pid , and tring to receive'
    ppid = os.getpid()
    cpid = os.fork() # cpid contains the child's PID for the parent and zero for the child
    if cpid == 0:
	    # Register
        pyMpi.register()
        pyMpi.send(ppid, "Message from registered proc to unregistered")

 
    else: 
        os.wait() # Make the parent wait until the child exists
        pyMpi.receive(cpid, 3)
		
#user's buffer is smaller than the msg in queue
def our_test6_7():
    print 'our test - 6_7'
    pid = os.getpid()

    # Register
    pyMpi.register()
    
    # Send ourselves some messages
    pyMpi.send(pid, "Message 1")
    #pyMpi.send(pid, "Message 2")
    
    # Receive one message
    message = pyMpi.receive(pid, 3)
    # Make sure the first message was received
    assert (message == "Mes")
    print message


	# multiple msgs- check if teh first one is the one to be deleted
def our_test11():
    print 'our test - 11'
    pid = os.getpid()

    # Register
    pyMpi.register()
    
    # Send ourselves some messages
    pyMpi.send(pid, "Message 1")
    pyMpi.send(pid, "Message 2")
    pyMpi.send(pid, "shani")
    pyMpi.send(pid, "adi")
    pyMpi.send(pid, "aaaaaaaaa")
    pyMpi.send(pid, "bbbbbbbbb")
    
    # Receive one message
    message = pyMpi.receive(pid, 3)
    # Make sure the first message was received
    assert (message == "Mes")
    print(message)

	#multiple Register is ok 
def our_test14():
    print 'our test - 14'
    pid = os.getpid()

    # Register
    pyMpi.register()
    pyMpi.register()
    pyMpi.register()

	# Send ourselves some messages
    pyMpi.send(pid, "Message 1")
    #pyMpi.send(pid, "Message 2")
    
    # Receive one message
    message = pyMpi.receive(pid, 3)
    # Make sure the first message was received
    assert (message == "Mes")

def test_basics():
    pid = os.getpid()

    # Check failures before registration
    try:
        pyMpi.send(pid+1000, "Failed Message")
    except:
        print("Error: Process pid doesn't exist.")
    try:
        pyMpi.send(pid, "Failed Message")
    except:
        print("Error: Process pid isn't registered.")
    try:
        message = pyMpi.receive(pid, 100)
    except:
        print("Error: Process pid isn't registered.")
    
    # Register
    pyMpi.register()
    # Double registration
    pyMpi.register()

    # Check failures after registration and before sending messasges
    try:
        pyMpi.send(pid, "")
    except:
        print("Error: message_size < 1")
    try:
        message = pyMpi.receive(pid, 100)
    except:
        print("Error: No message found from pid.")

    # Sending messages:
    # Sending one message -> Trying to receive two
    pyMpi.send(pid, "Message 1")
    try: 
        message = pyMpi.receive(pid, 0)
    except:
        print("Error: message_size < 1")
    message = pyMpi.receive(pid, 100)
    assert (message == "Message 1")
    try:
        message = pyMpi.receive(pid, 100)
    except:
        print("Error: No message found from pid.")

    # Send->Send->Receive->Send->Receive->Receive->Receive
    pyMpi.send(pid, "Message 2")
    pyMpi.send(pid, "Message 3")
    message = pyMpi.receive(pid, 100)
    try:
        assert (message == "Message 1")
    except:
        print("Error: message != 'Message 1'.")
    assert (message == "Message 2")
    pyMpi.send(pid, "Message 4")
    message = pyMpi.receive(pid, 100)
    assert (message == "Message 3")
    message = pyMpi.receive(pid, 100)
    assert (message == "Message 4")
    try:
        message = pyMpi.receive(pid, 100)
    except:
        print("Error: No message found from pid.")
    
    # Receive a part of a message
    pyMpi.send(pid, "Half Message")
    message = pyMpi.receive(pid, 6)
    assert (message == "Half M")
    try:
        message = pyMpi.receive(pid, 6)
    except:
        print("Error: No message found from pid.")

    print("test_basics pass!!")
    return 0

def test_fork1():

    child_pid = os.fork()
    if child_pid > 0:
        # Parent process
        print("parent process!!")
        pyMpi.register()
        try:
            pyMpi.send(child_pid, "Hello Child!")
        except:
            print("Error: Child process pid isn't registered.")
        time.sleep(1)
    elif child_pid == 0:
        print("child process!!")
        # Child process
        parent_pid = os.getppid()
        pyMpi.register()     
        try:
            message = pyMpi.receive(parent_pid, 100)
        except:
            print("Error: No message from parent.")
    else:
        # Fork failed
        print("Fork failed!")

def test_fork2():

    child_pid = os.fork()
    if child_pid > 0:
        # Parent process
        time.sleep(1)
        pyMpi.register()
        try:
            message = pyMpi.receive(parent_pid, 100)
        except:
            print("Error: No message from child.")
    elif child_pid == 0:
        # Child process
        parent_pid = os.getppid()
        pyMpi.register()     
        try:
            pyMpi.send(parent_pid, "Hello Parent!")
        except:
            print("Error: Parent process pid isn't registered.")
        time.sleep(2)
    else:
        # Fork failed
        print("Fork failed!")

def test_fork3():

    pyMpi.register()
    child_pid = os.fork()
    if child_pid > 0:
        # Parent process
        pyMpi.send(child_pid, "Hello Child!")
        print("Parent sent message to Child.")
        time.sleep(1)
        message_a = pyMpi.receive(child_pid, 100)
        message_b = pyMpi.receive(child_pid, 100)
        try:
            message_c = pyMpi.receive(child_pid, 100)
        except:
            print("Error: No message from Child.")
        print("Parent received messages from Child:")
        print(message_a)
        print(message_b)
    elif child_pid == 0:
        # Child process
        parent_pid = os.getppid()
        message = pyMpi.receive(parent_pid, 100)
        print("Child received message from Parent:")
        print(message)
        assert (message == "Hello Child!")
        pyMpi.send(parent_pid, "Hello Parent!")
        pyMpi.send(parent_pid, "How are you?")
        print("Child sent messages to Parent.")
        time.sleep(2)
    else:
        # Fork failed
        print("Fork failed!")

def test_fork4():

    grandparent_pid = os.getpid()
    pyMpi.register()
    child_pid = os.fork()
    if child_pid > 0:
        # Parent process
        time.sleep(1)
        my_grandchild_pid = pyMpi.receive(child_pid, 100)
        print("Grandparent received Grandchild's pid from Child:")
        print(my_grandchild_pid)
        message = pyMpi.receive(int(my_grandchild_pid), 100)
        print("Grandparent received message from Grandchild:")
        print(message)
        message = pyMpi.receive(child_pid, 100)
        print("Grandparent received message from Child:")
        print(message)
        pyMpi.send(int(my_grandchild_pid), "I am your Grandparent!")
        print("Grandparent sent message to Grandchild.")
        time.sleep(4)
    elif child_pid == 0:
        # Child process
        parent_pid = os.getpid()
        grandchild_pid = os.fork()
        if grandchild_pid > 0:
            # Child process
            pyMpi.send(grandparent_pid, str(grandchild_pid))
            print("Child sent Grandchild's pid to Parent.")
            pyMpi.send(grandparent_pid, "I am your Child!")
            print("Child sent message to Parent.")
            time.sleep(3)
        elif grandchild_pid == 0:
            # Grandchild process
            pyMpi.send(grandparent_pid, "I am your Grandchild!")
            print("Grandchild sent message to Grandparent.")
            time.sleep(2)
            message = pyMpi.receive(grandparent_pid, 100)
            print("Grandchild received message from Grandparent:")
            print(message)
        else:
            # Fork failed
            print("Fork failed!") 
    else:
        # Fork failed
        print("Fork failed!")

def test_fork5():

    parent_pid = os.getpid()
    pyMpi.register()
    first_child_pid = os.fork()
    if first_child_pid > 0:
        # Parent process
        second_child_pid = os.fork()
        if second_child_pid > 0:
            # Parent process
            pyMpi.send(first_child_pid, str(second_child_pid))
            print("Parent sent Second Child's pid to First Child.")
            pyMpi.send(first_child_pid, "I am your Parent!")
            print("Parent sent message to First Child.")
            time.sleep(3)
        elif second_child_pid == 0:
            # Second child process
            pyMpi.send(first_child_pid, "I am your Younger Sibling!")
            print("Younger Sibling sent message to Older Sibling.")
            time.sleep(2)
            message = pyMpi.receive(first_child_pid, 100)
            print("Younger Sibling received message from Older Sibling:")
            print(message)
        else:
            # Fork failed
            print("Fork failed!")
    elif first_child_pid == 0:
        # First child process
        time.sleep(1)
        younger_sibling_pid = pyMpi.receive(parent_pid, 100)
        print("First Child received Younger Sibling's pid from Parent:")
        print(younger_sibling_pid)
        message = pyMpi.receive(int(younger_sibling_pid), 100)
        print("Older Sibling received message from Younger Sibling:")
        print(message)
        message = pyMpi.receive(parent_pid, 100)
        print("First Child received message from Parent:")
        print(message)
        pyMpi.send(int(younger_sibling_pid), "I am your Older Sibling!")
        print("Older Sibling sent message to Younger Sibling.")
    else:
        # Fork failed
        print("Fork failed!")

if __name__ == "__main__":
    # test1()
    # test2()
    # test3()
    # test4()
    # test5()
    # our_test1()
    # our_test1_1()
    # our_test1_2()
    # our_test3_1()
    #our_test3_2()
    # our_test3_3()
    # our_test6_7()
    # our_test11()
    # our_test14() 
    # test_basics() 
    # test_fork1()
    # test_fork2()
    # test_fork3()
    test_fork4()
    # test_fork5()
