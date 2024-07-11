#include <linux/mpi.h>


int mpi_register(void){
#ifdef DEBUG
    printk("mpi_register function in\n");
#endif // DEBUG
    struct task_struct *current_process = current;

    if(!current_process){
        return -ENOMEM;
    }

    // current process alreday registered
    if(current_process->registered){
#ifdef DEBUG
        printk("current process alreday registerd\n");
#endif // DEBUG        
        return SUCCESS;
    }

    current_process->registered = 1;
    INIT_LIST_HEAD(&current_process->msg_queue);

#ifdef DEBUG
    printk("mpi_register function out\n");
#endif // DEBUG
    
    return SUCCESS;
}

int mpi_send(pid_t pid, char *message, ssize_t message_size){
#ifdef DEBUG
    printk("mpi_send function in\n");
#endif // DEBUG

    // Message validation
    if(!message || message_size < 1){
#ifdef DEBUG
        printk("invalid input. message size = %d\n", message_size);
#endif // DEBUG        
        return -EINVAL;
    }
#ifdef DEBUG
    printk("message is valid\n");
#endif // DEBUG       

    struct task_struct *sender_process = current;
    struct task_struct *recev_process;
    MPI_MESSAGE_S *mpi_message = NULL;

    // Check if receiver process is exist
    recev_process = find_task_by_pid(pid);

    // Check if sending process is valid and registered
    if(!recev_process){
#ifdef DEBUG
        printk("receving's pid process isn't vaild. pid = %d \n", recev_process);
#endif // DEBUG        
        return -ESRCH;
    }
#ifdef DEBUG
    printk("receving's pid is valid\n");
#endif // DEBUG       

    // Check if sending process and receivng process both registered.
    if(!sender_process->registered || !recev_process->registered){
#ifdef DEBUG
        printk("sender process or receiver process is not registered");
#endif // DEBUG        
        return -EPERM;
    }
#ifdef DEBUG
    printk("both sender process and receiver process are registered");
#endif // DEBUG   

    // mpi_message allocation
    mpi_message = (MPI_MESSAGE_S*)kmalloc(sizeof(MPI_MESSAGE_S), GFP_KERNEL);
    if(!mpi_message){
#ifdef DEBUG
        printk("failed on kamlloc() mpi_message allocation\n");
#endif // DEBUG        
        return -ENOMEM;
    }
    // message allocation
    mpi_message->message = (char*)kmalloc(message_size, GFP_KERNEL);
    if(!mpi_message->message){
#ifdef DEBUG
        printk("failed on kamlloc() message allocation\n");
#endif // DEBUG        
        return -ENOMEM;
    }    

#ifdef DEBUG
    printk("memory allocation went succesfully\n");
#endif // DEBUG       

    // Copy message from user space to kernel space (in mpi_message)
    if(copy_from_user(mpi_message->message, (const char *)message, (unsigned long)message_size)!= SUCCESS){
#ifdef DEBUG
        printk("failed on copy_from_user()\n");
#endif // DEBUG                
        kfree(mpi_message);
        return -EFAULT;
    }
#ifdef DEBUG
        printk("copy_from_user() succesed\n");
#endif // DEBUG      

    // Add message to receiver process's queue
    mpi_message->message_size = message_size;
    mpi_message->sender_pid = sender_process->pid;
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
#ifdef DEBUG
    printk("message is valid\n");
#endif // DEBUG    

    struct task_struct *recev_process = current;
    MPI_MESSAGE_S *recev_message;
    struct list_head *list_pos;
    int found = 0;

    // Check if receiver process is valid and registered
    if(!recev_process || !recev_process->registered){
#ifdef DEBUG
        printk("receiver process isn't valild or not registered. recev_process.registerd = %d \n", recev_process->registered);
#endif // DEBUG                
        return -EPERM;
    }
#ifdef DEBUG
    printk("receiver procces is valid\n");
#endif // DEBUG    

    // Find message from process pid
    list_for_each(list_pos, &recev_process->msg_queue){
        recev_message = list_entry(list_pos, MPI_MESSAGE_S, head);
        if(recev_message->sender_pid == pid){
            found = 1;
            break;
        }
    }

    // No message was found
    if(!found){
#ifdef DEBUG
        printk("no message was found from process = %d\n", pid);
#endif // DEBUG                
        return -EAGAIN;
    }
#ifdef DEBUG
    printk("essage was found. pid = %d\n",pid);
#endif // DEBUG    

    // Copy message to user space (in message buffer)
    ssize_t copy_size = (message_size > recev_message->message_size) ? recev_message->message_size : message_size;
    if(copy_to_user(message, recev_message->message, copy_size) != SUCCESS){
#ifdef DEBUG
        printk("failed on copy_to_user()\n");
#endif // DEBUG                  
        return -EFAULT;
    }
#ifdef DEBUG
        printk("copy_to_user() successed\n");
#endif // DEBUG       
    

    // Remove message from process's queue
    list_del(list_pos);
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
    struct list_head *pos, *q;
    MPI_MESSAGE_S* curr_message;

    if (!current_process){
        return;
    }

    // free memory allocated message queue
    list_for_each_safe(pos, q, &current_process->msg_queue){
        curr_message = list_entry(pos, MPI_MESSAGE_S, head);
        list_del(pos);
        kfree(curr_message->message);
        kfree(curr_message);
    }
    // INIT_LIST_HEAD(&current_process->msg_queue);

#ifdef DEBUG
    printk("free_process function out\n");
#endif // DEBUG    
    return;
}
