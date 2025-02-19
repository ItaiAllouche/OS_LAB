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

static struct file_operations my_fops = {
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .ioctl = my_ioctl,
    .write = my_write,
    .llseek = my_llseek,
};
 
int get_char_location(unsigned char charectar){
#ifdef DEBUG_MODE
    printk("get_char_location() in");
#endif // DEBUG_MODE

    int i;
    for(i = 0; i < ALPHA_BET_SIZE; i ++){
        if(g_alpha_bet[i] == charectar){
            return i;
        }
    }

#ifdef DEBUG_MODE
    printk("get_char_location() out");
#endif // DEBUG_MODE

    return -1;
}

char encrypt_char(unsigned char char_to_encrypt, int offset, int* key, int key_size){
#ifdef DEBUG_MODE
    printk("encrypt_char() in");
#endif // DEBUG_MODE

    int char_idx = get_char_location(char_to_encrypt);
    offset = offset%(key_size);
    
    if(char_idx != -1){
       int index = char_idx + key[offset];
        if(index >= ALPHA_BET_SIZE) { 
            index = index % ALPHA_BET_SIZE;
        }
        return g_alpha_bet[index];
    }

#ifdef DEBUG_MODE
    printk("encrypt_char() out");
#endif // DEBUG_MODE

    return char_to_encrypt;
}

char decrypt_char(unsigned char char_to_decrypt, int i, int* key, int key_size){
#ifdef DEBUG_MODE
    printk("decrypt_char() in");
#endif // DEBUG_MODE

    int char_idx = get_char_location(char_to_decrypt);
    i = i%(key_size);
    if(char_idx != -1){
       int index = char_idx - key[i];
        if(index < 0) { 
            index = char_idx - key[i] + ALPHA_BET_SIZE;
        }
        return g_alpha_bet[index];
    }
    
#ifdef DEBUG_MODE
    printk("decrypt_char() out");
#endif // DEBUG_MODE

    return char_to_decrypt;
}

void encrypt_buff(unsigned char* buff_to_encrypt, int buff_length, int* key, int key_size, int offset){
#ifdef DEBUG_MODE
    printk("encrypt_buff() in");
#endif // DEBUG_MODE

    int i;
    for(i = 0; i < buff_length; i++){
        buff_to_encrypt[i] = encrypt_char(buff_to_encrypt[i], i + offset, key, key_size);
    }

#ifdef DEBUG_MODE
    printk("encrypt_buff() out");
#endif // DEBUG_MODE

    return;
}

void decrypt_buff(unsigned char* buff_to_decrypt, int buff_length, int* key, int key_size, loff_t offset){
#ifdef DEBUG_MODE
    printk("decrypt_buff() in");
#endif // DEBUG_MODE

    int i;
    for(i = 0; i < buff_length; i ++){
        buff_to_decrypt[i] = decrypt_char(buff_to_decrypt[i], i + offset, key, key_size);
    }
#ifdef DEBUG_MODE
    printk("decrypt_buff() out");
#endif // DEBUG_MODE
    return;
}

int init_module(void){
#ifdef DEBUG_MODE
    printk("init_module() in");
#endif // DEBUG_MODE

    // This function is called when inserting the module using insmod
    my_major = register_chrdev(my_major, MY_DEVICE, &my_fops);

    if (my_major < 0){
        printk(KERN_WARNING "can't get dynamic major\n");
        return my_major;
    }

    //
    // do_init();

    int i;
    for(i=0; i<MINORS_NUM; i++){
        g_minors_array[i].buff = NULL;
        g_minors_array[i].buff_size = 0;
        g_minors_array[i].key = NULL;
        g_minors_array[i].key_length = 0;
        g_minors_array[i].debug = 0;
    }

#ifdef DEBUG_MODE
    printk("init_module() out");
#endif // DEBUG_MODE

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
#ifdef DEBUG_MODE
    printk("my_open() in");
#endif // DEBUG_MODE

    if ((inode == NULL) || (filp == NULL)){
        return -EFAULT;
    }
    int minor_number = MINOR(inode->i_rdev);
    filp->private_data = &(g_minors_array[minor_number]);

#ifdef DEBUG_MODE
    printk("my_open() out");
#endif // DEBUG_MODE

    return 0;
}

int my_release(struct inode *inode, struct file *filp){
    // handle file closing
    return 0;
}

ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){
 #ifdef DEBUG_MODE
    printk("my_read() in");
#endif // DEBUG_MODE

    VEGENERE_S *vegenre = (VEGENERE_S*)filp->private_data;
    if(filp == NULL || buf == NULL || count < 0 || f_pos == NULL){
        return -EFAULT;
    }
    if ((vegenre->key == NULL) && (vegenre->debug == 0) ){
         return -EINVAL;
    }
    if(count == 0){
        return count;
    }

    char* read_bytes = kmalloc(sizeof(char)*(count), GFP_KERNEL);
    if(read_bytes == NULL){
        return -ENOMEM;
    }
    int start = *f_pos;
    if(start == -1){
        start = 0;
    }
    int bytes_to_read = vegenre->buff_size - start;

    if (bytes_to_read <= count){
        count = bytes_to_read;
    }

    char* user_buff = (char*)kmalloc(sizeof(char)*count, GFP_KERNEL);
    int i;
    for(i = 0; i < count; i++){
        user_buff[i]= vegenre->buff[start+i];
    }
    
    loff_t offset = *f_pos;
    if(vegenre->debug == 0){
        decrypt_buff(user_buff,count, vegenre->key, vegenre->key_length, offset);
    }
    if(copy_to_user(buf, user_buff, sizeof(char)*count) != 0){
        kfree(user_buff);
        return -EBADF;
    }
    if (user_buff != NULL){
        kfree(user_buff);
    }
    
    *f_pos = *f_pos + count;

 #ifdef DEBUG_MODE
    printk("my_read() out");
#endif // DEBUG_MODE

    return count;
}

ssize_t my_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){
 #ifdef DEBUG_MODE
    printk("my_write() in");
#endif // DEBUG_MODE

    VEGENERE_S *vegenre = (VEGENERE_S*)filp->private_data;
    int new_offset = 0;
    int i;

    if(buf == NULL || filp == NULL ||f_pos == NULL || count < 0){
        return -EFAULT;
    }

    if ((vegenre->key == NULL) && (vegenre->debug == 0) ){
         return -EINVAL;
    }

    if(count == 0){
        return 0;
    }

    char *new_buff = (char*)kmalloc(sizeof(char)*(vegenre->buff_size+count), GFP_KERNEL);
    if(new_buff == NULL){
        return -ENOMEM;
    }

    for(i = 0; i < vegenre->buff_size; i++){
        new_buff[i] = vegenre->buff[i];
    }

    new_offset = i;
    char *user_data = (char*)kmalloc(sizeof(char)*count, GFP_KERNEL);

    if(user_data == NULL){
        return -ENOMEM;
    }

    if(copy_from_user(user_data, buf, sizeof(char)*count) != 0){
        return -EBADF;
    }

    if (vegenre->debug == 0){
        encrypt_buff(user_data, count, vegenre->key, vegenre->key_length, new_offset);
    }

    for (i = 0; i < count; i++){
        new_buff[new_offset+i] = user_data[i]; 
    }

    if(vegenre->buff != NULL){
        kfree(vegenre->buff);
    }

    if(user_data != NULL){
        kfree(user_data);
    }
    
    vegenre->buff = new_buff;
    vegenre->buff_size += count;

 #ifdef DEBUG_MODE
    printk("my_write() out");
#endif // DEBUG_MODE

    return count; 
}

loff_t my_llseek(struct file *filp, loff_t offset, int ignore){
    VEGENERE_S *vegenre = (VEGENERE_S*)filp->private_data;
    int buff_size = vegenre->buff_size;

    if((filp->f_pos + offset) < 0){
        filp->f_pos = 0;
        return 0;
    }
    else if((filp->f_pos + offset) >= buff_size){
        filp->f_pos = buff_size;
        return buff_size;
    }

    filp->f_pos += offset;

 #ifdef DEBUG_MODE
    printk("my_llseek() out");
#endif // DEBUG_MODE 

    return filp->f_pos;
}

int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg){
 #ifdef DEBUG_MODE
    printk("my_ioctl() in");
#endif // DEBUG_MODE 

    VEGENERE_S *vegenre = (VEGENERE_S*)filp->private_data;
    int i;
    int* key;
    int ley_length;
    char* key_to_encrypt = NULL;
    char* key_to_encrypt_user = NULL;
    int debug_mode;

    switch(cmd){
    case SET_KEY:
        key_to_encrypt_user = (char *)arg;
        if((key_to_encrypt_user) == NULL){
            return -EINVAL;
        }
        ley_length = strlen_user(key_to_encrypt_user) - 1;
        if (ley_length == 0){
             return -EINVAL;
        }
        key_to_encrypt = (char*)kmalloc(sizeof(char)*ley_length, GFP_KERNEL);
        key = (int*)kmalloc(sizeof(int)*ley_length, GFP_KERNEL);
        if(key_to_encrypt == NULL || key == NULL){
            return -ENOMEM;
        }
        if(copy_from_user(key_to_encrypt, key_to_encrypt_user, sizeof(char)*ley_length) != 0){
            kfree(key_to_encrypt);
            return -EBADF;
        }
        for(i=0; i<ley_length; i++){
            key[i] = get_char_location(key_to_encrypt[i]) + 1;
        }
        if (vegenre->key != NULL){
            kfree(vegenre->key);
        }
        vegenre->key = key;
        vegenre->key_length = ley_length;
	break;

    case RESET:
        if(vegenre->buff != NULL){
            kfree(vegenre->buff);
        }
        vegenre->buff = NULL;
        vegenre->buff_size = 0;
        if(vegenre->key != NULL){
            kfree(vegenre->key);
        }
        vegenre->key = NULL;
        vegenre->key_length = 0;
        vegenre->debug = 0;
        filp->f_pos = 0;
	 
	break;

    case DEBUG:
        debug_mode = (int)arg;
        if ((debug_mode != 1) && (debug_mode !=0)){
            return -EINVAL;
        }
        vegenre->debug = debug_mode;
        return 0;
	
	break;

    default:
	    return -ENOTTY;
    }

 #ifdef DEBUG_MODE
    printk("my_ioctl() out");
#endif // DEBUG_MODE 

    return 0;
}
