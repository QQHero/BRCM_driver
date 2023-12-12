#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>


int main(int argc, char* argv[])
{
pid_t pid;
char *default_argv[] = {"mp_server","20009",  NULL};

int status;
while(1) {
    status = posix_spawn(&pid,"./mp_server",NULL,NULL,default_argv,NULL);
    if (status==0) {
        printf("Child pid:%d\n",pid);
        if (waitpid(pid,&status,0)!=-1) {
            printf("Child exited with status %d \n",status);
        } else {
            printf("waitpid error");
        }
    } else {
        printf("posix spawn error with status %d \n",status);
    }
    sleep(30);
}

}