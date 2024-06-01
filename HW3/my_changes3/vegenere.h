#ifndef _VEGENERE_H_
#define _VEGENERE_H_

#include <linux/ioctl.h>
#include <linux/types.h>

#define MY_MAGIC 'r'
#define SET_KEY  _IOW(MY_MAGIC, 0, char*)
#define RESET  _IO(MY_MAGIC, 1)
#define DEBUG  _IOW(MY_MAGIC, 2, int)

#define DEBUG_MODE // flag for debug prints
#define ALPHA_BET_SIZE 62

int my_major = 0;
int g_debug_mode = 0;
const char g_aplha_bet[ALPHA_BET_SIZE] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

// struct for message buffer
typedef struct{
    char* buff;
    size_t buff_size;
    int* key;
    size_t key_size;
} MESSAGE_BUFFER_S;

int find_pos_in_alpha_bet(char elem);

// Custom implementation of memcpy
void *my_memcpy(char *dest, const char *src, size_t n);

// Functions for encryption-decryption mechanizem
char char_encryption(char elem, int key);

char char_decryption(char elem, int key);

void message_encryption(MESSAGE_BUFFER_S* message_buffer, char* message_to_encrypt, size_t count);

void message_decryption(MESSAGE_BUFFER_S* message_buffer, char* kernel_buffer, size_t count, loff_t pos);

int my_open(struct inode *, struct file *);

int my_release(struct inode *, struct file *);

ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);

ssize_t my_write(struct file *filp, const char *buf, size_t count);

int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

loff_t my_llseek(struct file *flip, loff_t offset);

#endif // _VEGENERE_H_
