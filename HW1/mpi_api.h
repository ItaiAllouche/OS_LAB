#ifndef MPI_API_H_
#define MPI_API_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <sys/types.h>
#include <stddef.h>
#include <errno.h>
#include <string.h>
/*
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/spinlock.h>
*/


#define MPI_REGISTER_SYSCALL 243
#define MPI_SEND_SYSCALL 244
#define MPI_RECEIVE_SYSCALL 245
#define SUCCESS 0
#define FAIL -1

struct mpi_process_s{
    bool registered;
    struct list_head *msg_queue;
};

struct mpi_message_s{
    char *message;
    ssize_t message_size;
    pid_t sender_id;
    struct list_head head;
};

// void list_add_tail(struct list_head *new_node, struct list_head *head);

// void list_del(struct list_head *entry);

// int list_empty(struct list_head *head);

// void list_for_each(struct list_head *pos, struct list_head *head);

// void list_for_each_prev(struct list_head *pos, struct list_head *head);

/*
Register for MPI communication. After this system call returns, the calling process can send
and receive MPI messages. Calling this function more than once does nothing.
system call number 243
returns 0 on success, -1 on failure.
*/
int mpi_register(void);

/*
Send a message of size message_size to the process identified by pid.
system call number 244
returns 0 on success, -1 on failure.
*/
int mpi_send(pid_t pid, char *message, ssize_t message_size);

/*
Check if the current process has a message from process pid. If there is, copy it to the buffer
message with size message_size and delete it from the incoming messages queue. Messages
are processed in the order they were received. If the message is longer than message_size,
copy only the first message_size bytes and delete the message from the queue.
system call number 245
returns 0 on success, -1 on failure.
*/
int mpi_receive(pid_t pid, char* message, ssize_t message_size);

// Warpper function for mpi_register
int communication_register(void);

// Warpper function for mpi_send
int send_message(pid_t pid, const char *message, ssize_t message_size);

// Warpper function for mpi_receive
int receive_message(pid_t pid, char* message, ssize_t message_size);

struct mpi_message_s* find_msg_by_pid(pid_t pid);

#endif // MPI_API_H