#ifndef MPI_API_H_
#define MPI_API_H_

#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

/*
Register for MPI communication. After this system call returns, the calling process can send
and receive MPI messages. Calling this function more than once does nothing.
system call number 243
returns 0 on success, -1 on failure.
*/
int mpi_register(void){
	int res;
	__asm__
	(
		"pushl %%eax;"
		"movl $243, %%eax;"
		"int $0x80;"
		"movl %%eax,%0;"
		"popl %%eax;"
		: "=m" (res)
	);
	if (res < 0)
	{
		errno = -res;
		res = -1;
	}
	return res;

}

/*
Send a message of size message_size to the process identified by pid.
system call number 244
returns 0 on success, -1 on failure.
*/
int mpi_send(pid_t pid, char *message, ssize_t message_size){
	int res;
	__asm__
	(
		"pushl %%eax;"
		"pushl %%ebx;"
		"pushl %%ecx;"
        "pushl %%edx;"
		"movl $244, %%eax;"
		"movl %1, %%ebx;"
		"movl %2, %%ecx;"
        "movl %3, %%edx;"
		"int $0x80;"
		"movl %%eax,%0;"
        "popl %%edx;"        
		"popl %%ecx;"
		"popl %%ebx;"
		"popl %%eax;"
		: "=m" (res)
		: "m" (pid) ,"m" (message), "m" (message_size)
	);
	if (res < 0)
	{
		errno = -res;
		res = -1;
	}
	return res;
}

/*
Check if the current process has a message from process pid. If there is, copy it to the buffer
message with size message_size and delete it from the incoming messages queue. Messages
are processed in the order they were received. If the message is longer than message_size,
copy only the first message_size bytes and delete the message from the queue.
system call number 245
returns 0 on success, -1 on failure.
*/
int mpi_receive(pid_t pid, char* message, ssize_t message_size){
	int res;
	__asm__
	(
		"pushl %%eax;"
		"pushl %%ebx;"
		"pushl %%ecx;"
        "pushl %%edx;"
		"movl $245, %%eax;"
		"movl %1, %%ebx;"
		"movl %2, %%ecx;"
        "movl %3, %%edx;"
		"int $0x80;"
		"movl %%eax,%0;"
        "popl %%edx;"        
		"popl %%ecx;"
		"popl %%ebx;"
		"popl %%eax;"
		: "=m" (res)
		: "m" (pid) ,"m" (message), "m" (message_size)
	);
	if (res < 0)
	{
		errno = -res;
		res = -1;
	}
	return res;    
}
#endif // MPI_API_H
