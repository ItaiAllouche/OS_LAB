#include <linux/mpi.h>


int mpi_register(void){
#ifdef DEBUG
    printk("mpi_register function in\n");
#endif // DEBUG
    struct task_struct *current_process = current;

    // current process alreday registered
    if(current_process->registered){
#ifdef DEBUG
        printk("current process alreday registerd\n");
#endif // DEBUG        
        return SUCCESS;
    }

//     current_process = kmalloc(sizeof(struct task_struct), GFP_KERNEL);
//     if(!current_process){
// #ifdef DEBUG
//         printk("failed on kmalloc()\n");
// #endif // DEBUG         
//         return -ENOMEM;
//     }

    current_process->registered = true;
    INIT_LIST_HEAD(&current_process->msg_queue);

#ifdef DEBUG
    printk("mpi_register function out\n");
#endif // DEBUG
    
    return SUCCESS
}

int mpi_send(pid_t pid, char *message, ssize_t message_size){
#ifdef DEBUG
    printk("mpi_send function in\n");
#endif // DEBUG

    // Message validation
    if(message == NULL || message_size < 1){
#ifdef DEBUG
        printk("invalid input. message size = %d\n", message_size);
#endif // DEBUG        
        return = -EINVAL;
    }

    struct task_struct *sender_process = current;
    struct task_struct *recev_process;
    MPI_MESSAGE_S *mpi_message;

    // Check if sending process is valid and registered
    if(!sender_process || !sender_process->registered){
#ifdef DEBUG
        printk("sending process isn't valild or not registered. sender_process.registerd = %d \n", sender_process->registered);
#endif // DEBUG        
        return -EPERM;
    }

    // Check if receiver process is exist
    recev_process = pid_task(find_vpid(pid), PIDTYPE_PID);
    if(!recev_process){
#ifdef DEBUG
        printk("receiver process isn't exist. invalid pid = %d\n", pid);
#endif // DEBUG        
        return -ESRCH;
    }

    // Memory message allocation
    mpi_message = (MPI_MESSAGE_S*)kmalloc(sizeof(MPI_MESSAGE_S), GFP_KERNEL);
    if(!mpi_message){
#ifdef DEBUG
        printk("failed on kamlloc()\n");
#endif // DEBUG        
        return -ENOMEM;
    }

    // Copy message from user space to kernel space (in mpi_message)
    unsigned long res = copy_from_user(mpi_message->message, message, message_size);
    if(res != SUCCESS){
#ifdef DEBUG
        printk("failed on copy_from_user()\n");
#endif // DEBUG                
        kfree(mpi_message);
        return -EFAULT;
    }

    // Add message to receiver process's queue
    mpi_message->message_size = message_size;
    mpi_message->sender_id = sender_process->pid;
    INIT_LIST_HEAD(&mpi_message->head);
    list_add_tail(&mpi_message->head, &recev_process->msg_queue);
#ifdef DEBUG
    printk("mpi_send function out\n");
#endif // DEBUG       

    return SUCCESS;
}

int mpi_receive(pid_t pid, char* message, ssize_t message_size){
#ifdef DEBUG
    printk("mpi_receive function in\n");
#endif // DEBUG

    // Message validation
    if(message == NULL || message_size < 1){
#ifdef DEBUG
        printk("invalid input. message size = %d\n", message_size);
#endif // DEBUG           
        return -EINVAL;
    }

    struct task_struct *recev_process = current;
    MPI_MESSAGE_S *recev_message;
    struct list_head *list_pos;
    bool found = false;

    // Check if receiver process is valid and registered
    if(!recev_process || !recev_process->registered){
#ifdef DEBUG
        printk("receiver process isn't valild or not registered. recev_process.registerd = %d \n", recev_process->registered);
#endif // DEBUG                
        return -EPERM;
    }

    // Find message from process pid
    list_for_each_prev(list_pos, &recev_process->msg_queue){
        recev_message = list_entry(list_pos)
        if(recev_message->sender_pid == pid){
            found = true;
            break
        }
    }

    // No message was found
    if(!found){
#ifdef DEBUG
        printk("no message was found from process = %d\n", pid);
#endif // DEBUG                
        return -EAGAIN;
    }

    // Copy message to user space (in message buffer)
    ssize_t copy_size = (message_size > recev_message->message_size) ? recev_message->message_size : message_size;
    if(copy_to_user(message, mpi_message->message, copy_size) != SUCCESS){
#ifdef DEBUG
        printk("failed on copy_to_user()\n");
#endif // DEBUG                  
        return -EFAULT;
    }

    // Remove message from process's queue
    list_del(list_pos)
    kfree(recev_message->message);
    kfree(recev_message);
#ifdef DEBUG
    printk("mpi_receive function out\n");
#endif // DEBUG    

    return copy_size;

}

void free_process(struct task_struct* current_process){
#ifdef DEBUG
    printk("free_process function in\n");
#endif // DEBUG    
    MPI_MESSAGE_S *curr_message, *temp;

    if (!current_process){
        return;
    }

    // free memory allocated message queue
    list_for_each_entry_safe(curr_message, temp, &task->mpi_message_queue, list){
        kfree(curr_message->message);
        kfree(curr_message);
    }
    INIT_LIST_HEAD(&current_process->msg_queue);

#ifdef DEBUG
    printk("free_process function out\n");
#endif // DEBUG    
    return;
}