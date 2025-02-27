#ifndef _MPI_H
#define _MPI_H

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/list.h>
#include <asm/uaccess.h>    

#define SUCCESS 0
#define FAIL -1
#define DEBUG // flag for debug prints

//     bool registered;
//     struct list_head *msg_queue;
// }
typedef struct{
    ssize_t message_size;
    pid_t sender_pid;
    struct list_head head;
    char *message;    
} MPI_MESSAGE_S;

#endif // _MPI_H
