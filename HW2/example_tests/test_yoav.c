#include <stdio.h>
#include <sys/time.h>
#include <errno.h>


#include <sys/types.h>
#include <unistd.h>

#include "mpi_api.h"

int main()
{
    int pid_t = getpid();
    const char help[5] = "help";
    const char me[3] = "me";
    const char pls[8] = "please!";
    int ret;
    char get_msg[100];
    struct mpi_poll_entry poll_entry [3];
    poll_entry[0].pid = 1;
    poll_entry[1].pid = 1;
    poll_entry[2].pid = pid_t;

    poll_entry[0].incoming = 0;
    poll_entry[1].incoming = 0;
    poll_entry[2].incoming = 0;

    printf("trying to polling before register\n");
    ret = mpi_poll(poll_entry, 3, 705032404);
    perror("error polling without register:");
    printf("got %d from polling without register\n", ret);

//    ret = mpi_send(pid_t, help, 5);
//    printf("got %d from send without register\n", ret);
//    perror("error of send without");
//    ret = mpi_receive(pid_t, get_msg, 5);
//    printf("got %d from receive without register\n", ret);

    mpi_register();
    printf("registered pid: %d\n", pid_t);


    printf("trying to polling before there is message\n");
    ret = mpi_poll(poll_entry, 3, 1);
    printf("done waiting\n");
    perror("polling without message");

    ret = mpi_send(pid_t, help, 5);
    printf("got %d from send\n", ret);
//    ret = mpi_send(pid_t, me, 3);
//    printf("got %d from send\n", ret);

    printf("trying to polling after there is message\n");
    ret = mpi_poll(poll_entry, 3, 1);
    printf("done waiting\n");


    ret = mpi_receive(pid_t, get_msg, 100);
    printf("got %d from receive\n", ret);
//    printf("got message: %s from pid: %d\n", get_msg, pid_t);
//    ret = mpi_receive(pid_t, get_msg, 100);
//    printf("got %d from receive\n", ret);
//    printf("got message: %s from pid: %d\n", get_msg, pid_t);
//    ret = mpi_send(pid_t, help, 5);
    printf("got %d from send\n", ret);

    int pid = fork();
    if(pid == 0){
        //son proccess
        printf("----son process----\n");
        //ret = mpi_send(pid_t, pls, 8);
        //printf("got %d from send without register but should be ok (son process )\n", ret);

        //ret = mpi_receive(pid_t, get_msg, 100);
        //printf("got %d from receive\n", ret);
        //perror("son tried to recv but the list is empty");

        int my_pid = getpid();
        printf("son starting polling\n");
        ret = mpi_poll(poll_entry, 3, 10);
        printf("son the income values are: %d, %d, %d, \n",poll_entry[0].incoming,poll_entry[1].incoming,poll_entry[2].incoming);
        printf("son done waiting \n");
        ret = mpi_send(pid_t, help, 5);



        //ret = mpi_send(my_pid, pls, 8);
        //printf("got %d from send without register\n", ret);

    }
    else{
        printf("----father process----\n");
        printf("father waiting 3 sec\n");
        ret = mpi_poll(poll_entry, 3, 3);
        printf("father send son a message\n");
        ret = mpi_send(pid, help, 5);
        wait();
        ret = mpi_send(pid_t, help, 5);
        poll_entry[1].pid = pid;
        ret = mpi_poll(poll_entry, 3, 10);
        printf("father the income values are: %d, %d, %d, \n",poll_entry[0].incoming,poll_entry[1].incoming,poll_entry[2].incoming);
        ret = mpi_send(pid, help, 5);


//        printf("son pid is: %d  \n", pid);
//        ret = mpi_receive(pid, get_msg, 100);
//        printf("got %d from receive\n", ret);
//        printf("got message: %s from pid: %d\n", get_msg, pid);
//
//        ret = mpi_receive(pid_t, get_msg, 100);
//        printf("got %d from receive\n", ret);
//        printf("got message: %s from pid: %d\n", get_msg, pid_t);

    }
    return 0;
}
