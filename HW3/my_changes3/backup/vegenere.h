#ifndef _VEGENERE_H_
#define _VEGENERE_H_

#include <linux/ioctl.h>
#include <linux/types.h>

#define MY_MAGIC 'r'
#define SET_KEY  _IOW(MY_MAGIC, 0, char*)
#define RESET  _IO(MY_MAGIC, 1)
#define DEBUG  _IOW(MY_MAGIC, 2, int)

#define MINORS_NUM 256
#define ALPHA_BET_SIZE 62
// #define DEBUG_MODE

typedef struct{		
	char *buff;
    ssize_t buff_size;
    int* key;
    int key_length;
    int debug;
    
}VEGENERE_S;

/* globals */
int my_major = 0; /* will hold the major # of my device driver */
const char g_alpha_bet[ALPHA_BET_SIZE] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
static VEGENERE_S g_minors_array[MINORS_NUM] = {};

int my_open(struct inode *, struct file *);

int my_release(struct inode *, struct file *);

ssize_t my_write(struct file *, const char *, size_t, loff_t *);

ssize_t my_read(struct file *, char *, size_t, loff_t *);

int my_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

loff_t my_llseek(struct file *, loff_t, int);

#endif // _VEGENERE_H_
