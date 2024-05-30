/* vegenere.c: Example char device module.
 *
 */
/* Kernel Programming */
#define MODULE
#define LINUX
#define __KERNEL__

#include <linux/kernel.h>  	
#include <linux/module.h>
#include <linux/fs.h>       		
#include <asm/uaccess.h>
#include <linux/errno.h>  
#include <asm/segment.h>
#include <asm/current.h>

#include "vegenere.h"

#define MY_DEVICE "vegenere"

MODULE_AUTHOR("Anonymous");
MODULE_LICENSE("GPL");

/* globals */
int my_major = 0; /* will hold the major # of my device driver */
MESSAGE_BUFFER_S* g_message_buffer;

void *my_memcpy(char *dest, const char *src, size_t n) {
    char *d = dest;
    const char *s = src;
    while (n--){
        *d++ = *s++;
    }
    return d;
}

char char_encryption(char elem, int key){
    for(int i=0; i<ALPHA_BET_SIZE; i++){
        if(elem == g_message_buffer->aplha_bet[i]){
            return g_message_buffer->aplha_bet[(i+key) % ALPHA_BET_SIZE];
        }
    }

    // elem not in alpha bet. therefore not need to be encrypted
    return elem;
}

char char_decryption(char elem, int key){
    for(int i=0; i<ALPHA_BET_SIZE; i++){
        if(elem == g_message_buffer->aplha_bet[i]){
            return g_message_buffer->aplha_bet[(i-key) % ALPHA_BET_SIZE];
        }
    }

    // elem not in alpha bet. therefore not need to be decrypted
    return elem;   
}

char* message_encryption(char* message, size_t count){
    for(size_t i=0; i<count; i++){
        message[i] = char_encryption(message[i], g_message_buffer->key[i % key_size]);
    }
    return message
}

char* message_decryption(char* message, size_t count){
    for(size_t i=0; i<count; i++){
        message[i] = char_decryption(message[i], g_message_buffer->key[i % key_size]);
    }
    return message    

}

struct file_operations my_fops = {
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .ioctl = my_ioctl,
};

int init_module(void){
    // This function is called when inserting the module using insmod

    my_major = register_chrdev(my_major, MY_DEVICE, &my_fops);

    if (my_major < 0){
        printk(KERN_WARNING "can't get dynamic major\n");
        return my_major;
    }

    //
    // do_init();
    //
    return 0;
}

void cleanup_module(void){
    // This function is called when removing the module using rmmod

    unregister_chrdev(my_major, MY_DEVICE);

    //
    // do clean_up();
    //
    return;
}

int my_open(struct inode *inode, struct file *filp){
    // handle open

    return 0;
}

int my_release(struct inode *inode, struct file *filp){
    // handle file closing

    return 0;
}

ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){
    //
    // Do read operation.
    // Return number of bytes read.
    return 0; 
}

ssize_t my_write(struct file *filp, const char *buf, size_t count){
#ifdef DEBUG_MODE
    printk("[DEBUG_MODE] my_write() in\n");
#endif // DEBUG_MODE
    if(!buff){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] buf arg invalid\n");
#endif // DEBUG_MODE
        return -EFAULT;
    }
    if(!g_message_buffer->key){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] invalid key\n");
#endif // DEBUG_MODE  
        return -EINVAL;      
    }

    // Allocate larger buffer
    char* new_buff = (char*)kmalloc(g_message_buffer->buff_size + count, GFP_KERNEL);
    if(!new_buff){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] kmalloc() faild\n");
#endif // DEBUG_MODE      
        return -ENOMEM;
    }

    char* dummy = my_memcpy(new_buff, g_message_buffer->buff, g_message_buffer->buff_size);
    kfree(g_message_buffer->buff);
    g_message_buffer->buff = dummy;
    if(copy_from_user(g_message_buffer->buff+g_message_buffer->buff_size, message_encryption(buf, count), count)!= 0){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] failed on copy_from_user()\n");
#endif // DEBUG                
        kfree(g_message_buffer->buff);
        return -EBADF;        
    }
    g_message_buffer->buff_size += count;

#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] my_write() out\n");
#endif // DEBUG
    return count;
}

int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg){
    switch(cmd)
    {
    case SET_KEY:
	//
	// handle 
	//
	break;
    case RESET:
	//
	// handle 
	//
	break;
    case DEBUG:
	//
	// handle 
	//
	break;
    default:
	return -ENOTTY;
    }

    return 0;
}
