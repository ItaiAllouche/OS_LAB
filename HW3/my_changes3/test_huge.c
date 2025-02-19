#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>

#define MY_MAGIC 'r'
#define SET_KEY  _IOW(MY_MAGIC, 0, char*)
#define RESET  _IO(MY_MAGIC, 1)
#define DEBUG  _IOW(MY_MAGIC, 2, int)

int main()
{
   int j=0;
   // Open first minor
   int fd1  = open("/dev/vegenere1", O_RDWR);
   if (fd1 < 0) printf("Failed openning device file\n");

   int fd3  = open("/dev/vegenere1", O_RDWR);
   if (fd3 < 0) printf("Failed openning device file\n");

   // open second minor
   int fd2  = open("/dev/vegenere2", O_RDWR);
   if (fd3 < 0) printf("Failed openning device file\n");

   int fd4  = open("/dev/vegenere2", O_RDWR);
   if (fd4 < 0) printf("Failed openning device file\n");
   int res = 0;
   //int res2 = 0;

   //check write and read fails if key is not defined
   char msg1[128] = {0};
   char msg2[128] = {0};
   char read1[128] = {0};
   char read2[128] = {0};
   char key1[128] = {0};
   char key2[128] = {0};
   char key3[128] = {0};
   char key4[128] = {0};

   // reset both minors
   ioctl(fd1, RESET);
   ioctl(fd2, RESET);
   strcpy(msg1, "Helloooo");
   strcpy(msg2, "Solution to HW3");
   ioctl(fd1, DEBUG, 0);
   ioctl(fd2, DEBUG, 0);
   strcpy(key1, "ABC");
   strcpy(key2, "ABCDEFGH");
   strcpy(key3, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
   strcpy(key4, "");



//______________________________________ ERRNO TESTS + basic test many times _______________________________________________________________



   //EFAULT, null buffer write()
   res = write(fd1, NULL, 8);
   if(res != -1){
      printf("PROBLEM - write() should have returned an error\n");
      printf("returned value = '%d'    should be -1\n", res);
      printf("use 'return -EFAULT;'\n");
   }
   assert(res == -1);

   if(errno != EFAULT){
      printf("PROBLEM - write() should have raised EFAULT error\n");
      printf("errno value = '%d'    should be '%d'\n", errno,-EFAULT);
   }
   assert(errno == EFAULT);



   //EINVAL, no defined key write()
   res = write(fd1, msg1, 8);
   if(res != -1){
      printf("PROBLEM - write() should have returned an error\n");
      printf("returned value = '%d'    should be -1\n", res);
      printf("use 'return -EINVAL;'\n");
   }
   assert(res == -1);

   if(errno != EINVAL){
      printf("PROBLEM - write() should have raised EINVAL error\n");
      printf("errno value = '%d'    should be '%d'\n", errno,-EINVAL);
   }
   assert(errno == EINVAL);



   //EFAULT, null buffer read()
   res = read(fd1, NULL, 8);
   if(res != -1){
      printf("PROBLEM - read() should have returned an error\n");
      printf("returned value = '%d'    should be -1\n", res);
      printf("use 'return -EFAULT;'\n");
   }
   assert(res == -1);

   if(errno != EFAULT){
      printf("PROBLEM - read() should have raised EFAULT error\n");
      printf("errno value = '%d'    should be '%d'\n", errno,-EFAULT);
   }
   assert(errno == EFAULT);



   //EINVAL, no defined key read()
   res = read(fd1, read1, 8);
   if(res != -1){
      printf("PROBLEM - read() should have returned an error\n");
      printf("returned value = '%d'    should be -1\n", res);
      printf("use 'return -EINVAL;'\n");
   }
   assert(res == -1);

   if(errno != EINVAL){
      printf("PROBLEM - read() should have raised EINVAL error\n");
      printf("errno value = '%d'    should be '%d'\n", errno,-EINVAL);
   }
   assert(errno == EINVAL);



   //EINVAL, debug command value is not 0 or 1
   res = ioctl(fd1, DEBUG, 5);
   if(res != -1){
      printf("PROBLEM - ioctl(debug) should have returned an error\n");
      printf("returned value = '%d'    should be -1\n", res);
      printf("use 'return -EINVAL;'\n");
   }
   assert(res == -1);

   if(errno != EINVAL){
      printf("PROBLEM - ioctl(debug) should have raised EINVAL error\n");
      printf("errno value = '%d'    should be '%d'\n", errno,-EINVAL);
   }
   assert(errno == EINVAL);



   //basic read write test many times
   ioctl(fd1, SET_KEY, key1);
   ioctl(fd2, SET_KEY, key2);
   unsigned int maxBytes = (16384)*8; //16384 = (0b1 << 14)
   for(j=0; j<(maxBytes/8); j++){
      res = write(fd1, msg1, 8);
      if(res != 8){
         printf("wrote %d bytes (should be 8)\n", res);
         printf("tried to send msg = '%s')\n", msg1);
         printf("failed on iteration...  %d\n", j);
      }
      assert(res == 8);

      res = read(fd1, read1, 8);
      if(res!=8){
         printf("read %d bytes (should be 8)\n", res);
         printf("failed on iteration...  %d\n", j);
      }
      assert(res == 8);

      res = strcmp(read1,"Helloooo");
      if(res!=0){
         printf("read '%s'    should be '%s')\n", read1, msg1);
      }
      assert(res == 0);
   }



   //ENOMEM, no memory write()
   res = write(fd1, msg1, 8);
   if(res != -1){
      printf("PROBLEM - write() should have returned an error\n");
      printf("returned value = '%d'    should be -1\n", res);
      printf("use 'return -ENOMEM;'\n");
   }
   assert(res == -1);

   if(errno != ENOMEM){
      printf("PROBLEM - write() should have raised ENOMEM error\n");
      printf("errno value = '%d'    should be '%d'\n", errno,-ENOMEM);
   }
   assert(errno == ENOMEM);


/* ------------------------- get help to implement
   //ENOMEM, no memory SET_KEY
   res = ioctl(fd1, SET_KEY, key3);
   if(res != -1){
      printf("PROBLEM - ioctl(SET_KEY) should have returned an error\n");
      printf("returned value = '%d'    should be -1\n", res);
      printf("use 'return -ENOMEM;'\n");
   }
   assert(res == -1);

   if(errno != ENOMEM){
      printf("PROBLEM - ioctl(SET_KEY) should have raised ENOMEM error\n");
      printf("errno value = '%d'    should be '%d'\n", errno,-ENOMEM);
   }
   assert(errno == ENOMEM);
   */



   //EINVAL, SET_KEY key value is NULL
   res = ioctl(fd1, SET_KEY, NULL);
   if(res != -1){
      printf("PROBLEM - ioctl(SET_KEY) should have returned an error\n");
      printf("returned value = '%d'    should be -1\n", res);
      printf("use 'return -EINVAL;'\n");
   }
   assert(res == -1);

   if(errno != EINVAL){
      printf("PROBLEM - ioctl(SET_KEY) should have raised EINVAL error\n");
      printf("errno value = '%d'    should be '%d'\n", errno,-EINVAL);
   }
   assert(errno == EINVAL);



   //EINVAL, SET_KEY key value is empty
   res = ioctl(fd1, SET_KEY, key4);
   if(res != -1){
      printf("PROBLEM - ioctl(SET_KEY) should have returned an error\n");
      printf("returned value = '%d'    should be -1\n", res);
      printf("use 'return -EINVAL;'\n");
   }
   assert(res == -1);

   if(errno != EINVAL){
      printf("PROBLEM - ioctl(SET_KEY) should have raised EINVAL error\n");
      printf("errno value = '%d'    should be '%d'\n", errno,-EINVAL);
   }
   assert(errno == EINVAL);
//_________________________________________________________________________________________________________________________________________



   // try to write without a key through the other files of the same minors
   /*
   res = write(fd2, buffer, 8);
   assert(res == -1);
   assert(errno == EINVAL);

   //res = read(fd4, buffer2, 8);
   //assert(res == -1);
   //assert(errno == EINVAL);
   

   //char key[10] = {};
   char key2[10] = {};
   //strcpy(key, "ABCDEFGH");
   strcpy(key2, "ABC");

   //ioctl(fd1, SET_KEY, key);
   ioctl(fd4, SET_KEY, key2);

   
   // write "Hello" to 2nd minor
   res = write(fd3, buffer3, 6);
   //printf("write %d bytes\n", res);
   //printf("write %s\n", buffer2);
   ioctl(fd4, DEBUG, 1);
   res = read(fd3, readb3, 6);
   //printf("read %d bytes - should be Igomq\n", res);
   //printf("read %s\n", readb2);
   assert(res == 6);
   res = strcmp(readb3,"Igomq");
   assert(res == 0);
*/
   
   //test if ioctl(RESET) frees inner buffer and changes read pointer fpos = 0
   ioctl(fd1, RESET);
   ioctl(fd1, SET_KEY, key1);
   res = write(fd1, msg1, 8);
   if(res != 8){
      if(res==-1 && errno == ENOMEM){
         printf("RESET failed to free inner buffer space\n");
      }
      else{
         printf("wrote %d bytes (should be 8)\n", res);
         printf("tried to send msg = '%s')\n", msg1);
      }
   }
   assert(res == 8);

   res = read(fd1, read1, 8);
   if(res!=8){
      printf("read %d bytes (should be 8)\n", res);
      printf("failed on iteration...  %d\n", (maxBytes/8));
   }
   assert(res == 8);

   res = strcmp(read1,"Helloooo");
   if(res!=0){
      printf("read '%s'    should be '%s')\n", read1, msg1);
   }
   assert(res == 0);
   
   
   
   //test if ioctl(RESET) frees key
   ioctl(fd1, RESET);
   res = write(fd1, msg1, 8);
   if(res != -1){
      printf("PROBLEM - write() should have returned an error\n");
      printf("returned value = '%d'    should be -1\n", res);
      printf("use 'return -EINVAL;'\n");
   }
   assert(res == -1);

   if(errno != EINVAL){
      printf("PROBLEM - write() should have raised EINVAL error\n");
      printf("errno value = '%d'    should be '%d'\n", errno,-EINVAL);
   }
   assert(errno == EINVAL);



   // test debug mode 1 read()
   ioctl(fd2, SET_KEY, key2);//'ABCDEFGH'
   res = write(fd2, msg2, 16);//'Solution to HW3'
   if(res != 16){
      printf("wrote %d bytes (should be 16)\n", res);
      printf("tried to send msg = '%s')\n", msg2); //
   }
   assert(res == 16);

   ioctl(fd2, DEBUG, 1);
   res = read(fd2, read2, 16);
   if(res != 16){
      printf("read %d bytes (should be 16)\n", res);
   }
   assert(res == 16);

   //A B C D E F G H I J K L M N O P Q R S T U V W X Y Z a b c d e f g h i j k l m n o p q r s t u v w x y z 0 1 2 3 4 5 6 7 8 9
   res = strcmp(read2, "Tqoyyovv vr McA");
   if(res != 0){
      printf("read '%s'    should be 'Tqoyyovv vr McA')\n", read2);
   }
   assert(res == 0);



   // test debug mode 1 write() and read()
   res = write(fd2, msg2, 16);
   if(res != 16){
      printf("wrote %d bytes (should be 16)\n", res);
      printf("tried to send msg = '%s')\n", msg2);
   }
   assert(res == 16);

   res = read(fd2, read2, 16);
   if(res != 16){
      printf("read %d bytes (should be 16)\n", res);
   }
   assert(res == 16);

   res = strcmp(read2, "Solution to HW3");
   if(res != 0){
      printf("read '%s'    should be 'Solution to HW3')\n", read2);
   }
   assert(res == 0);



   // test debug mode 1 write()
   res = write(fd2, msg2, 16);//'Solution to HW3'
   if(res != 16){
      printf("wrote %d bytes (should be 16)\n", res);
      printf("tried to send msg = '%s')\n", msg2);
   }
   assert(res == 16);

   res = ioctl(fd2, DEBUG, 0); //notice DEBUG is per minor
   if(res != 0){
      printf("res = %d (should be 0)\n", res);
   }
   assert(res == 0);

   res = read(fd2, read2, 16);
   if(res != 16){
      printf("read %d bytes (should be 16)\n", res);
   }
   assert(res == 16);

   //A B C D E F G H I J K L M N O P Q R S T U V W X Y Z a b c d e f g h i j k l m n o p q r s t u v w x y z 0 1 2 3 4 5 6 7 8 9
   res = strcmp(read2, "Rmiqochf rl CQw");
   if(res != 0){
      printf("read '%s'    should be 'Rmiqochf rl CQw')\n",read2);
   }
   assert(res == 0);




   // test lseek
   ioctl(fd1, RESET);
   ioctl(fd1, SET_KEY, key1);//ABC
   ioctl(fd1, DEBUG, 0);
   memset(read1, 0 ,128);
   res = write(fd1, msg1, 8);//Helloooo => Igomqrpq
   if(res != 8){
      printf("wrote %d bytes (should be 8)\n", res);
      printf("tried to send msg = '%s')\n", msg1);
      printf("errno = '%d')\n", errno);
   }
   assert(res == 8);

   res = lseek(fd1, 2, SEEK_CUR);
   if(res != 2){
      printf("new index %d (should be 2)\n", res);
   }
   assert(res == 2);

   res = read(fd1, read1, 8); //omqrpq => lloooo
   if(res!=6){
      printf("read %d bytes (should be 6)\n", res);
   }
   assert(res == 6);

   //A B C D E F G H I J K L M N O P Q R S T U V W X Y Z a b c d e f g h i j k l m n o p q r s t u v w x y z 0 1 2 3 4 5 6 7 8 9
   res = strcmp(read1,"lloooo");
   if(res!=0){
      printf("read '%s'    should be 'lloooo')\n", read1);
   }
   assert(res == 0);
   memset(read1, 0 ,128); 

   res = lseek(fd1, -5, SEEK_CUR);
   if(res != 3){
      printf("moved to index %d (should be 3)\n", res);
   }
   assert(res == 3);

   res = read(fd1, read1, 8);
   if(res!=5){
      printf("read %d bytes (should be 5)\n", res);
   }
   assert(res == 5);

   res = strcmp(read1,"loooo");
   if(res!=0){
      printf("read '%s'    should be 'loooo')\n", read1);
   }
   assert(res == 0);  



   // test lseek out of bounds
   memset(read1, 0 ,128);
   ioctl(fd1, RESET);
   ioctl(fd1, SET_KEY, key1);//ABC
   ioctl(fd1, DEBUG, 0);
   res = lseek(fd1, 2, SEEK_CUR);
   if(res != 0){
      printf("moved to junk part of empty array, index %d (should be 0)\n", res);
   }
   assert(res == 0);

   res = write(fd1, msg1, 8); //Helloooo => Igomqrpq
   if(res != 8){
      printf("wrote %d bytes (should be 8)\n", res);
      printf("tried to send msg = '%s')\n", msg1);
   }
   assert(res == 8);

   res = lseek(fd1, 2, SEEK_CUR);
   if(res != 2){
      printf("new reading position index %d (should be 2)\n", res);
   }
   assert(res == 2);

   res = lseek(fd1, -4, SEEK_CUR);
   if(res != 0){
      printf("moved to index %d (should be 0)\n", res);
      if(res == -2){
         printf("did you return delta index instead of new index?\n");
      }
   }
   assert(res == 0);

   res = lseek(fd1, 1, SEEK_CUR);
   if(res != 1){
      printf("moved to index %d (should be 1)\n", res);
   }
   assert(res == 1);

   res = write(fd1, msg1, 1); //H=>I   HellooooH => IgomqrpqI
   if(res != 1){
      printf("wrote %d bytes (should be 1)\n", res);
      printf("tried to send msg = '%c')\n", msg1[0]);
   }
   assert(res == 1);

   res = read(fd1, read1, 8);
   if(res!=8){
      printf("read %d bytes (should be 8)\n", res);
   }
   assert(res == 8);

   //A B C D E F G H I J K L M N O P Q R S T U V W X Y Z a b c d e f g h i j k l m n o p q r s t u v w x y z 0 1 2 3 4 5 6 7 8 9
   res = strcmp(read1,"ellooooH");
   if(res!=0){
      printf("read '%s'    should be 'ellooooH')\n", read1);
   }
   assert(res == 0);  



   // Now close it
   ioctl(fd1, RESET);
   ioctl(fd2, RESET);
   close(fd1);
   close(fd2);
   close(fd3);
   close(fd4);
   printf("Test is successfull!\n");
   return 0;
}
