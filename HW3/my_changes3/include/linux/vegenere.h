#ifndef _VEGENERE_H_
#define _VEGENERE_H_

#include <linux/ioctl.h>
#include <linux/types.h>

#define MY_MAGIC 'r'
#define SET_KEY  _IOW(MY_MAGIC, 0, char*)
#define RESET  _IO(MY_MAGIC, 1)
#define DEBUG  _IOW(MY_MAGIC, 2, int)

#define DEBUG_MODE
#define ALPHA_BET_SIZE 62

//
// Function prototypes
//
int my_open(struct inode *, struct file *);

int my_release(struct inode *, struct file *);

ssize_t my_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)

ssize_t my_write(struct file *filp, const char *buff, size_t count, loff_t *f_pos);

int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

loff_t my_llseek(struct file *, loff_t, int);

// Functions for encryption-decryption mechanizem
char char_encryption(char elem, int key);

char char_decryption(char elem, int key);

char* message_encryption(char* message, size_t count);

char* message_decryption(char* message, size_t count);

// Custom implementation of memcpy
void *my_memcpy(void *dest, const void *src, size_t n);

typedef struct{
    char* buff;
    size_t buff_pos; // offest from start of buffer
    size_t buff_size;
    int* key;
    size_t key_size;
    char aplha_bet [62] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                           'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                           'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                           'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                           'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                           'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                           'w', 'x', 'y', 'z', '0', '1', '2', '3',
                           '4', '5', '6', '7', '8', '9'};
}MESSAGE_BUFFER_S;


#endif // _VEGENERE_H_
