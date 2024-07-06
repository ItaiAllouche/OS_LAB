#include <linux/mpi.h>
#include <linux/sched.h>
#include <linux/list.h>
#include <stdbool.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#define TASK_INTERRUPTIBLE	1
#define FOUND_P 1
struct mpi_poll_entry {
    pid_t pid;
    char incoming;
};

int sys_mpi_register(void){
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

int sys_mpi_send(pid_t pid, char *message, ssize_t message_size){
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

    // Check if sending process is valid and registered
    if(!sender_process || !sender_process->registered){
#ifdef DEBUG
        printk("sending process isn't valild or not registered. sender_process.registerd = %d \n", sender_process->registered);
#endif // DEBUG        
        return -EPERM;
    }
#ifdef DEBUG
    printk("sendig process is valid\n");
#endif // DEBUG       

    // Check if receiver process is exist
    recev_process = find_task_by_pid(pid);
    if(!recev_process){
#ifdef DEBUG
        printk("receiver process isn't exist. invalid pid = %d\n", pid);
#endif // DEBUG        
        return -ESRCH;
    }
#ifdef DEBUG
    printk("receiver process exists pid =%d\n", pid);
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
	wake_up_process(recev_process);

    return SUCCESS;
}

int sys_mpi_receive(pid_t pid, char* message, ssize_t message_size){
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


int sys_mpi_poll(struct mpi_poll_entry * poll_pids, int npids, int timeout){

	#ifdef DEBUG
		printk("Poll started\n");
	#endif // DEBUG 
    if ( npids<1 ){
        
        return -EINVAL;
    }
	if ( timeout<0 ){
		return -EINVAL;
	}
	#ifdef DEBUG
		printk("npids and timeout passed\n");
	#endif // DEBUG 
    struct mpi_poll_entry* poll_pids_kspace = (struct mpi_poll_entry*)kmalloc(sizeof(struct mpi_poll_entry)*npids, GFP_KERNEL);
    if(poll_pids_kspace == NULL)
    {
    #ifdef DEBUG
		printk("allocation failed\n");
	#endif // DEBUG 
        return -ENOMEM;
    }
    #ifdef DEBUG
		printk("allocation passed\n");
	#endif // DEBUG 
    if(copy_from_user(poll_pids_kspace,poll_pids,(sizeof(struct mpi_poll_entry))*npids)){
        if (poll_pids_kspace != NULL){
			kfree((poll_pids_kspace));
        } 
	#ifdef DEBUG
		printk("failed copy\n");
	#endif // DEBUG 
        return -EFAULT;
    }

    struct task_struct* reciver_process = current;
    if(reciver_process->registered == 0){
        if (poll_pids_kspace != NULL){
			kfree((poll_pids_kspace));
        }
			#ifdef DEBUG
		printk("failed copy\n");
	#endif // DEBUG 
        return -EPERM;
    }   

    set_current_state(TASK_INTERRUPTIBLE);
    int found_pid = 0;
    int num_of_found_pids = 0;
    int i=0;
	#ifdef DEBUG
	printk("init pol ids\n");
	#endif // DEBUG 
    for (i=0; i<npids; i++){
          poll_pids_kspace[i].incoming = 0;
    }

    struct list_head *temp;
	struct list_head *pos;
	#ifdef DEBUG
	printk("search pids\n");
	#endif // DEBUG 
    list_for_each_safe(pos,temp,&reciver_process->msg_queue) {
		MPI_MESSAGE_S *node;
		node = list_entry(pos,MPI_MESSAGE_S, head);
        for (i=0; i<npids; i++){
            
            if(node->sender_pid == poll_pids_kspace[i].pid){
                poll_pids_kspace[i].incoming = FOUND_P;
               
                found_pid = FOUND_P; 
            }
        }
    }
	#ifdef DEBUG
	printk("not found pids section\n");
	#endif // DEBUG 
    if (found_pid==0){
        signed long remaining_time;
        remaining_time = timeout;
        while(remaining_time > 0 && found_pid == 0){
            remaining_time = (schedule_timeout(HZ * remaining_time)/HZ);
            if(remaining_time == 0){
                if (poll_pids_kspace != NULL){
			        kfree((poll_pids_kspace));
                }
                
                return -ETIMEDOUT;
            }
            else{
                //search again
               
                list_for_each_safe(pos,temp,&reciver_process->msg_queue) {
                    MPI_MESSAGE_S *node;
                    node = list_entry(pos,MPI_MESSAGE_S, head);
                    for (i=0; i<npids; i++){
                        if(node->sender_pid == poll_pids_kspace[i].pid){ 
                            poll_pids_kspace[i].incoming = 1;
                            found_pid = 1; 
                        }
                    }
                }
            }
        }
    }
	#ifdef DEBUG
	printk("calculating number of found pids \n");
	#endif // DEBUG 
    for (i=0; i<npids; i++){
        if(poll_pids_kspace[i].incoming == FOUND_P){
            num_of_found_pids++;
        }
    }

    if (copy_to_user(poll_pids,poll_pids_kspace,(sizeof(struct mpi_poll_entry))*npids)){
        if (poll_pids_kspace != NULL){
			    kfree((poll_pids_kspace));
			}
			#ifdef DEBUG
			printk("failed copy \n");
			#endif // DEBUG 
            return -EFAULT;
    }
	#ifdef DEBUG
	printk("freeing poll kspace \n");
	#endif // DEBUG 
    if (poll_pids_kspace != NULL)
	{
		kfree((poll_pids_kspace));
	}
	#ifdef DEBUG
	printk("Finishing poll \n");
	#endif // DEBUG 
    return num_of_found_pids;

}
