#include <linux/mpi.h>


int mpi_register(void){
    struct mpi_process_s *mpi_process = curr_process;

    // current process alreday registered
    if(mpi_process->registered){
        return SUCCESS;
    }

    mpi_process = kmalloc(sizeof(struct mpi_process_s), GFP_KERNEL);
    if(!mpi_process){
        return -ENOMEM;
    }

    mpi_process->registered = true;
    INIT_LIST_HEAD(&mpi_process->msg_queue);
    curr_process = mpi_process;

    return SUCCESS
}

int mpi_send(pid_t pid, char *message, ssize_t message_size){
    // Message validation
    if(message == NULL || message_size < 1){
        return = -EINVAL;
    }

    struct mpi_process_s *mpi_process;
    struct mpi_message_s *mpi_message;
    struct task_struct *dest_task;

    mpi_process = curr_process;

    // Check if sending process is valid and registered
    if(!mpi_process || !mpi_process->registered){
        return -EPERM;
    }

    // Check if receiver process is exist
    dest_task = pid_task(find_vpid(pid), PIDTYPE_PID);
    if(!dest_task){
        return -ESRCH;
    }

    // Memory message allocation
    mpi_message = (struct mpi_message_s*)kmalloc(sizeof(struct mpi_message_s), GFP_KERNEL);
    if(!mpi_message){
        return -ENOMEM;
    }

    // Copy message from user space to kernel space (in mpi_message)
    unsigned long res = copy_from_user(mpi_message->message, message, message_size);
    if(res != SUCCESS){
        kfree(mpi_message);
        return -EFAULT;
    }

    // Add message to receiver process's queue
    mpi_message->message_size = message_size;
    mpi_message->sender_id = getpid();
    list_add_tail(&mpi_message->head, &mpi_process->msg_queue);

    return SUCCESS;
}

int mpi_receive(pid_t pid, char* message, ssize_t message_size){
    // Message validation
    if(message == NULL || message_size < 1){
        return -EINVAL;
    }

    struct mpi_process *mpi_process;
    struct mpi_message *mpi_message;

    mpi_process = curr_process;

    // Check if sending process is valid and registered
    if(!mpi_process || !mpi_process->registered){
        return -EPERM;
    }

    // Find message from process pid
    list_for_each_prev(mpi_message, &mpi_process->msg_queue){
        if(mpi_message->sender_id == pid){
            break
        }
    }

    // No message was found
    if(!mpi_message){
        return -EAGAIN;
    }

    // Copy message to user space (in message buffer)
    ssize_t copy_size = (message_size > mpi_message->message_size) ? mpi_message->message_size : message_size;
    if(copy_to_user(message, mpi_message->message, copy_size) != SUCCESS){
        return -EFAULT;
    }

    // Remove message from process's queue
    list_del(&mpi_message->head)
    kfree(mpi_message);

    return copy_size;

}