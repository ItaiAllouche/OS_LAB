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
const char aplha_bet [ALPHA_BET_SIZE]= {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                                   'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                                   'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                                   'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                                   'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                   'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                                   'w', 'x', 'y', 'z', '0', '1', '2', '3',
                                   '4', '5', '6', '7', '8', '9'};

void *my_memcpy(char *dest, const char *src, size_t n) {
    char *d = dest;
    const char *s = src;
    while (n--){
        *d++ = *s++;
    }
    return dest;
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

void message_encryption(char* message_to_encrypt, size_t count){
    for(size_t i=0; i<count; i++){
        message_to_encrypt[i] = char_encryption(message_to_encrypt[i], g_message_buffer->key[i % key_size]);
    }
    return;
}

void message_decryption(char* kernel_buffer, size_t count, loff_t pos){
    for(size_t i=0; i<g_message_buffer->buff_size; i++){
        //decrypt only between [pos, pos+count]
        if(i >= pos && i < pos + count){
            kernel_buffer[i-pos] = char_decryption(g_message_buffer->buff[i], g_message_buffer->key[i % key_size]);
        }
    }
    return;  
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
    return 0;
}

void cleanup_module(void){
    // This function is called when removing the module using rmmod

    unregister_chrdev(my_major, MY_DEVICE);
    return;
}

int my_open(struct inode *inode, struct file *filp){
#ifdef DEBUG_MODE
    printk("[DEBUG_MODE] my_open() in\n");
#endif // DEBUG_MODE
    MESSAGE_BUFFER_S* message_buffer = (MESSAGE_BUFFER_S*)kmalloc(sizeof(MESSAGE_BUFFER_S*),GFP_KERNEL);
    if(!message_buffer){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] kmalloc() faild\n");
#endif // DEBUG_MODE
        return -ENOMEM;        
    }

    message_buffer->buff = kmalloc(0, GFP_KERNEL);
    if(!message_buffer->buff){
        kfree(message_buffer);
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] kmalloc() faild\n");
#endif // DEBUG_MODE      
        return -ENOMEM;
    }

    message_buffer->buff_size = 0;
    message_buffer->key = NULL;
    message_buffer->key_size = 0;

    flip->private_data = message_buffer;

#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] my_open out\n");
#endif // DEBUG_MODE
    return 0;
}

int my_release(struct inode *inode, struct file *filp){
#ifdef DEBUG_MODE
    printk("[DEBUG_MODE] my_close() in\n");
#endif // DEBUG_MODE
    MESSAGE_BUFFER_S* message_buffer = (MESSAGE_BUFFER_S*)flip->private_data;

    if(message_buffer){
        if(message_buffer->key){
            kfree(message_buffer->key);
        }
        if(message_buffer->buff){
            kfree(message_buffer->buff);
        }
        kfree(message_buffer);
    }
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] my_release out\n");
#endif // DEBUG_MODE
    return 0;
}

ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){
#ifdef DEBUG_MODE
    printk("[DEBUG_MODE] my_read() in\n");
#endif // DEBUG_MODE
    MESSAGE_BUFFER_S* message_buffer = (MESSAGE_BUFFER_S*)filp->private_data;
    if(!buff || !message_buffer){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] buf arg invalid\n");
#endif // DEBUG_MODE
        return -EFAULT;
    }
    if(*f_pos > message_buffer->buff_size){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] no data to read\n");
#endif // DEBUG_MODE
        return 0;
    }
    if(!message_buffer->key){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] invalid key\n");
#endif // DEBUG_MODE  
        return -EINVAL;      
    }
    if(count <= 0 || *f_pos < 0){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] invalid count or f_pos\n");
#endif // DEBUG_MODE  
        return -EFAULT;          
    }

    // Determine the number of bytes to read
    if(*f_pos + count > message_buffer->buff_size){
        count = message_buffer->buff_size - *f_pos;
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] can't read %d bytes. %d bytes will be read \n",dummy, count);
#endif // DEBUG_MODE          
    }

    // Allocate kernel buffer for data to decrypt
    char* kernel_buffer = (char*)kmalloc(count, GFP_KERNEL);
    if(!kernel_buffer){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] kmalloc() faild\n");
#endif // DEBUG_MODE      
        return -ENOMEM;
    }

    message_decryption(kernel_buffer, count, *f_pos);
    // Copy data to user space
    if(copy_to_user(buf, kernel_buffer, count) != 0){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] failed on copy_to_user()\n");
#endif // DEBUG  
        return -EBADF;
    }
    kfree(kernel_buffer);

    // Update file position
    *f_pos += count;
    
    return count;

}

ssize_t my_write(struct file *filp, const char *buf, size_t count){
#ifdef DEBUG_MODE
    printk("[DEBUG_MODE] my_write() in\n");
#endif // DEBUG_MODE
    MESSAGE_BUFFER_S* message_buffer = (MESSAGE_BUFFER_S*)filp->private_data;
    if(!buff || !message_buffer){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] buf arg invalid\n");
#endif // DEBUG_MODE
        return -EFAULT;
    }
    if(!message_buffer->key){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] invalid key\n");
#endif // DEBUG_MODE  
        return -EINVAL;      
    }

    // Allocate larger buffer
    char* new_buff = (char*)kmalloc(message_buffer->buff_size + count, GFP_KERNEL);
    if(!new_buff){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] kmalloc() faild\n");
#endif // DEBUG_MODE      
        return -ENOMEM;
    }

     // Allocate kernel buffer for data to encrypt
    char* kernel_buffer = (char*)kmalloc(count, GFP_KERNEL);
    if(!kernel_buffer){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] kmalloc() faild\n");
        kfree(new_buff);
#endif // DEBUG_MODE      
        return -ENOMEM;
    }

    if(copy_from_user(kernel_buffer, buf, count)!= 0){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] failed on copy_from_user()\n");
#endif // DEBUG                
        kfree(kernel_buffer);
        kfree(new_buff);
        return -EBADF;        
    }

    message_decryption(message_buffer->buff, message_buffer->buff_size, 0);
    my_memcpy(new_buff, message_buffer->buff, message_buffer->buff_size);
    my_memcpy(new_buff + message_buffer->buff_size, kernel_buffer, count);
    kfree(kernel_buffer);
    kfree(message_buffer->buff);
    message_buffer->buff = new_buff;
    message_buffer->buff_size += count;
    message_encryption(message_buffer->buff, message_buffer->buff_size);

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

loff_t my_llseek(struct file *flip, loff_t offset){
    MESSAGE_BUFFER_S* message_buffer = (MESSAGE_BUFFER_S*)filp->private_data;
    loff_t curr_pos = *(flip->f_pos);

    // offset passes end of message
    // move offset to end of message
    if(curr_pos + offset >= message_buffer->buff_size){
        *(flip->f_pos) = message_buffer->buff_size;
        return *(flip->f_pos);
    }

    // offset passes start of message
    // move offset to start of message
    else if(curr_pos + offset < 0){
        *(flip->f_pos) = 0;
        return 0;
    }

    else{
        *(flip->f_pos) += offset;
        return *(flip->f_pos);
    }
}
