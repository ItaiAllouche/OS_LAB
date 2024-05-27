#ifndef _MPI_H
#define _MPI_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/list.h>
#include <unistd.h>


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

int mpi_register(void);
int mpi_send(pid_t pid, char *message, ssize_t message_size);
int mpi_receive(pid_t pid, char* message, ssize_t message_size);
void terminate_process(pid_t curr_process);

#endif // _MPI_H
