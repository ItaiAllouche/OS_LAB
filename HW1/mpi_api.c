#include "mpi_api.h"

// relevant?
// struct mpi_message_s* find_msg_by_pid(pid_t pid){
//     struct mpi_process_s *mpi_process;
//     struct mpi_message_s *mpi_message;

//     mpi_process = curr_process;

//     list_for_each_prev(mpi_message, &mpi_process->msg_queue){
//         if(mpi_message->sender_id == pid){
//             return mpi_message
//         }
//     }
//     return NULL;
// }

int mpi_register(void){
    struct mpi_process_s *mpi_process = curr_process;

    // current process alreday registered
    if(mpi_process->registered){
        return SUCCESS;
    }

    mpi_process = kmalloc(sizeof(struct mpi_process_s), GFP_KERNEL);
    if(!mpi_process){
        errno = ENOMEM;
        return FAIL;
    }

    mpi_process->registered = true;
    INIT_LIST_HEAD(&mpi_process->msg_queue);
    curr_process = mpi_process;

    return SUCCESS
}

int mpi_send(pid_t pid, char *message, ssize_t message_size){
    // Message validation
    if(message == NULL || message_size < 1){
        errno = EINVAL;
        return FAIL;
    }

    struct mpi_process_s *mpi_process;
    struct mpi_message_s *mpi_message;
    struct task_struct * mpi_task;

    mpi_process = curr_process;

    // Check if sending process is valid and registered
    if(!mpi_process || !mpi_process->registered){
        errno = EPERM;
        return FAIL;
    }

    // Check if receiver process is exist
    mpi_task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if(!mpi_task){
        errno = ESRCH;
        return FAIL;
    }

    // Memory message allocation
    mpi_message = (struct mpi_message_s*)kmalloc(sizeof(struct mpi_message_s), GFP_KERNEL);
    if(!mpi_message){
        errno = ENOMEM;
        return FAIL;
    }

    // Copy message from user space to kernel space (in mpi_message)
    unsigned long res = copy_from_user(mpi_message->message, message, message_size);
    if(res != SUCCESS){
        errno = EFAULT;
        kfree(mpi_message);
        return FAIL;
    }

    // Add message to receiver process's queue
    mpi_message->message_size = message_size;
    INIT_LIST_HEAD(&mpi_message->head);
    list_add_tail(&mpi_message->head, &mpi_process->msg_queue);

    return SUCCESS;
}

int mpi_receive(pid_t pid, char* message, ssize_t message_size){
    // Message validation
    if(message == NULL || message_size < 1){
        errno = EINVAL;
        return FAIL;
    }

    struct mpi_process *mpi_process;
    struct mpi_message *mpi_message;

    mpi_process = curr_process;

    // Check if sending process is valid and registered
    if(!mpi_process || !mpi_process->registered){
        errno = EPERM;
        return FAIL;
    }

    // Find message from process pid
    list_for_each_prev(mpi_message, &mpi_process->msg_queue){
        if(mpi_message->sender_id == pid){
            break
        }
    }

    // No message was found
    if(!mpi_message){
        errno = EAGAIN;
        return FAIL;
    }

    // Copy message to user space (in message buffer)
    ssize_t copy_size = (message_size > mpi_message->message_size) ? mpi_message->message_size : message_size;
    if(copy_to_user(message, mpi_message->message, copy_size) != SUCCESS){
        errno = EFAULT;
        return FAIL;
    }

    // Remove message from process's queue
    list_del(&mpi_message->head)
    kfree(mpi_message);

    return copy_size;

}