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
#include <linux/slab.h>

#include "vegenere.h"

#define MY_DEVICE "vegenere"

MODULE_AUTHOR("Anonymous");
MODULE_LICENSE("GPL");

#ifdef DEBUG_MODE
void print_int_array(int* arr, size_t len){
    int i;
    for(i=0; i<len; i++){
        printk("%d,",arr[i]);
    }
    printk("\n");
}

void print_char_array(char* arr, size_t len){
    int i;
    for(i=0; i<len; i++){
        printk("%c,",arr[i]);
    }
    printk("\n");
}
#endif // DEBUG_MODE

// Operates a mod b. (assuming b > 0)
int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

struct file_operations my_fops = {
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
    .ioctl = my_ioctl,
    .llseek = my_llseek
};

int find_pos_in_alpha_bet(char elem){
    int i;
    for(i=0; i<ALPHA_BET_SIZE; i++){
        if (elem == g_aplha_bet[i]){
            return (i + 1);
        }
    }
    return -1;
}

void *my_memcpy(char *dest, const char *src, size_t n){
    char *d = dest;
    const char *s = src;
    while (n--){
        *d++ = *s++;
    }
    return dest;
}

char char_encryption(char elem, int key){
    int i;
    for(i=0; i<ALPHA_BET_SIZE; i++){
        if(elem == g_aplha_bet[i]){
            return g_aplha_bet[mod(i+key, ALPHA_BET_SIZE)];
        }
    }

    // elem not in alpha bet. therefore not need to be encrypted
    return elem;
}

char char_decryption(char elem, int key){
    int i;
    for(i=0; i<ALPHA_BET_SIZE; i++){
        if(elem == g_aplha_bet[i]){
            return g_aplha_bet[mod(i-key, ALPHA_BET_SIZE)];
        }
    }

    // elem not in alpha bet. therefore not need to be decrypted
    return elem;   
}

void message_encryption(MESSAGE_BUFFER_S* message_buffer, char* message_to_encrypt, size_t count){
    size_t i;
    for(i=0; i<count; i++){
        message_to_encrypt[i] = char_encryption(message_to_encrypt[i], message_buffer->key[i % message_buffer->key_size]);
    }
    return;
}

void message_decryption(MESSAGE_BUFFER_S* message_buffer, char* kernel_buffer, size_t count, loff_t pos){
    size_t i;
    size_t j = 0;

#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] message_decryption() in\n");
        printk("[DEBUG_MODE] buff_size = %d\n", message_buffer->buff_size);
        printk("[DEBUG_MODE] count = %d. pos = %d\n", count, (int)pos);
#endif // DEBUG_MODE

    for(i=0; i<message_buffer->buff_size; i++){
        //decrypt only between [pos, pos+count]
        if(i >= pos && i < pos + count){
            kernel_buffer[j] = char_decryption(message_buffer->buff[i], message_buffer->key[i % message_buffer->key_size]);
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] iteration = %d. char_decryption() output = %c\n",i, kernel_buffer[j]);
#endif // DEBUG_MODE    
            j++;
        }
    }

#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] message_decryption() out\n");
#endif // DEBUG_MODE    
    return;  
}

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

    filp->private_data = message_buffer;

#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] my_open out\n");
#endif // DEBUG_MODE
    return 0;
}

int my_release(struct inode *inode, struct file *filp){
#ifdef DEBUG_MODE
    printk("[DEBUG_MODE] my_release() in\n");
#endif // DEBUG_MODE
    MESSAGE_BUFFER_S* message_buffer = (MESSAGE_BUFFER_S*)filp->private_data;

    if(message_buffer){
        if(message_buffer->key){
            kfree(message_buffer->key);
        }
        if(message_buffer->buff){
            kfree(message_buffer->buff);
        }
        kfree(message_buffer);
    }
    g_debug_mode = 0;
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
    if(!buf || !message_buffer){
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

#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] f_pos before reading = %d\n", (int)*f_pos);
        printk("[DEBUG_MODE] all message buffer before reading = ");
        print_char_array(message_buffer->buff, message_buffer->buff_size);
#endif // DEBUG_MODE

    // Determine the number of bytes to read
    if(*f_pos + count > message_buffer->buff_size){
        size_t dummy = count;
        count = message_buffer->buff_size - *f_pos;
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] can't read %d bytes. %d bytes will be read \n",dummy, count);
#endif // DEBUG_MODE          
    }

    // Allocate kernel buffer for data to decrypt
    char* kernel_buffer = (char*)kmalloc(sizeof(char) * count, GFP_KERNEL);
    if(!kernel_buffer){
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] kmalloc() faild\n");
#endif // DEBUG_MODE      
        return -ENOMEM;
    }

    if(!g_debug_mode){
        message_decryption(message_buffer, kernel_buffer, count, *f_pos);

        // Copy data to user space
        if(copy_to_user(buf, kernel_buffer, count) != 0){
#ifdef DEBUG_MODE
            printk("[DEBUG_MODE] failed on copy_to_user()\n");
#endif // DEBUG  
            return -EBADF;
        }
        kfree(kernel_buffer);
    }
    else{
        if(copy_to_user(buf, message_buffer->buff + (int)*f_pos, count) != 0){
#ifdef DEBUG_MODE
            printk("[DEBUG_MODE] failed on copy_to_user()\n");
#endif // DEBUG  
            return -EBADF;
        }
        kfree(kernel_buffer);
    }

    // Update file offset
    *f_pos += count;

#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] f_pos after reading = %d\n", (int)*f_pos);
        printk("[DEBUG_MODE] readed data = ");
        print_char_array(buf, count);
        printk("[DEBUG_MODE] my_read() out\n");
#endif // DEBUG_MODE    
    return count;
}

ssize_t my_write(struct file *filp, const char *buf, size_t count){
#ifdef DEBUG_MODE
    printk("[DEBUG_MODE] my_write() in\n");
#endif // DEBUG_MODE
    MESSAGE_BUFFER_S* message_buffer = (MESSAGE_BUFFER_S*)filp->private_data;
    if(!buf || !message_buffer){
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

#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] message buffer before writing = ");
        print_char_array(message_buffer->buff, message_buffer->buff_size);
#endif // DEBUG_MODE

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

    if(!g_debug_mode){
        message_decryption(message_buffer, message_buffer->buff, message_buffer->buff_size, 0);
    }
    my_memcpy(new_buff, message_buffer->buff, message_buffer->buff_size);
    my_memcpy(new_buff + message_buffer->buff_size, kernel_buffer, count);
    kfree(kernel_buffer);
    kfree(message_buffer->buff);
    message_buffer->buff = new_buff;
    message_buffer->buff_size += count;
    if(!g_debug_mode){
        message_encryption(message_buffer, message_buffer->buff, message_buffer->buff_size);
    }
#ifdef DEBUG_MODE
    printk("[DEBUG_MODE] message buffer after writing = ");
    print_char_array(message_buffer->buff, message_buffer->buff_size);
#endif // DEBUG_MODE

#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] my_write() out\n");
#endif // DEBUG
    return count;
}

int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg){
#ifdef DEBUG_MODE
    printk("[DEBUG_MODE] my_ioctl() in\n");
#endif // DEBUG_MODE    
    MESSAGE_BUFFER_S* message_buffer = (MESSAGE_BUFFER_S*)filp->private_data;   
    int* key;
    size_t key_size;
    char* user_key;

    switch(cmd){
    case SET_KEY:
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] case SET_KEY\n");
#endif // DEBUG_MODE     
        key_size = strlen_user((const char *)arg) - 1;
        if(key_size <= 0 || !(const char *)arg){          
            return -EINVAL;
        }

        user_key  = (char*)kmalloc(key_size, GFP_KERNEL);
        if(!user_key){
#ifdef DEBUG_MODE
            printk("[DEBUG_MODE] kmalloc() failed\n");
#endif // DEBUG_MODE
            return -ENOMEM;
        }
        if(copy_from_user(user_key, (char *)arg, key_size) != 0){
            kfree(user_key);
#ifdef DEBUG_MODE
            printk("[DEBUG_MODE] copy_from_user() failed\n");
#endif // DEBUG_MODE
            return -EBADF;          
        }

        key = (int*)kmalloc(key_size * sizeof(int), GFP_KERNEL);
        if(!key){
#ifdef DEBUG_MODE
            kfree(user_key);
            printk("[DEBUG_MODE] kmalloc() failed\n");
#endif // DEBUG_MODE
            return -ENOMEM;
        }
        size_t i;
        for(i=0; i<key_size; i++){
            int pos = find_pos_in_alpha_bet(user_key[i]);
#ifdef DEBUG_MODE
            printk("[DEBUG_MODE] user_key[%i] = %c\n", i, user_key[i]);
            printk("[DEBUG_MODE] pos = %d\n", pos);
#endif // DEBUG_MODE                 
            if(pos == -1){
                kfree(user_key);
                kfree(key);
                return -EINVAL;
            }
            key[i] = pos;
        }
        kfree(user_key);

        if(message_buffer->key){
            kfree(message_buffer->key);
        }
        message_buffer->key = key;
        message_buffer->key_size = key_size;
#ifdef DEBUG_MODE
            printk("[DEBUG_MODE] my_ioctl out. key_size = %d. key = ",message_buffer->key_size);
            print_int_array(message_buffer->key, message_buffer->key_size);
#endif // DEBUG_MODE             
        return 0;
    case RESET:
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] case RESET\n");
#endif // DEBUG_MODE
        if(message_buffer->buff){
            kfree(message_buffer->buff);
            message_buffer->buff = NULL;
        }
        message_buffer->buff_size = 0;
        
        if(message_buffer->key){
            kfree(message_buffer->key);
            message_buffer->key = NULL;
        }
        message_buffer->key_size = 0;
        return 0;
    case DEBUG:
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] case DEBUG\n");
#endif // DEBUG_MODE
        if(arg == 1){
            g_debug_mode = 1;
#ifdef DEBUG_MODE
            printk("[DEBUG_MODE] debug mode enabled\n");
#endif // DEBUG_MODE            
        }
        else if (arg == 0 ){
            g_debug_mode = 0;
#ifdef DEBUG_MODE
            printk("[DEBUG_MODE] debug mode disabled\n");
#endif // DEBUG_MODE               
        }
        else{
            return -EINVAL;
        }
#ifdef DEBUG_MODE
            printk("[DEBUG_MODE] my_ioctl() out\n");
#endif // DEBUG_MODE 
        return 0;
    default:
#ifdef DEBUG_MODE
        printk("[DEBUG_MODE] case default\n");
        printk("[DEBUG_MODE] my_ioctl() out\n");

#endif // DEBUG_MODE     
	return -ENOTTY;
    }  
}

loff_t my_llseek(struct file *flip, loff_t offset){
    MESSAGE_BUFFER_S* message_buffer = (MESSAGE_BUFFER_S*)flip->private_data;
    loff_t curr_pos = flip->f_pos;

    // offset passes end of message
    // move offset to end of message
    if(curr_pos + offset >= message_buffer->buff_size){
        flip->f_pos = message_buffer->buff_size;
    }

    // offset passes start of message
    // move offset to start of message
    else if(curr_pos + offset < 0){
        flip->f_pos = 0;
    }

    else{
        flip->f_pos += offset;
    }

    return flip->f_pos;
}
