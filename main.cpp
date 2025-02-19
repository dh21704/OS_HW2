#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 25
#define READ_END 0
#define WRITE_END 1

#define MIN_PID 100
#define MAX_PID 1000

#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <ctime>


#include <vector>

using namespace std;

//global PID for pid management
bool *pid_map = nullptr;

//creates and initializes the bitmap for PID management
int allocate_map()
{
     pid_map = (bool*)malloc(sizeof(bool) * MAX_PID);
     
     if(pid_map == nullptr)
     {
         return -1;
     }
    
    //marking all pid's as free
    for (int i = 0; i < MAX_PID; i++)
    {
        pid_map[i] = false;
    }
    
    return 1;
}

//Allocates and returns a new PID from the bitmap
int allocate_pid()
{
    bool *pid_map;
    
    int random_number = rand()%(MAX_PID-MIN_PID + 1) + MIN_PID;
    
    int pid = random_number;
    
    while(pid_map[pid])
    {
        pid = rand()%(MAX_PID-MIN_PID + 1) + MIN_PID;
    }
    
    pid_map[pid] = true;
    
    return pid;
}

int main(void)
{
    char write_msg[BUFFER_SIZE] = "Greetings";
    char read_msg[BUFFER_SIZE];

    /* create the pipe */
    int fd[2];
    pid_t pid;

    /* error checking */
    if (pipe(fd) == -1) {
        fprintf(stderr, "Pipe failed");
        return 1;
    }

    /* fork a child process */
    pid = fork();

//if the fork fails then the pid is negative 
    if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed");
        return 1;
    }

    if (pid > 0) { /* parent process */
        /* close the unused end of the pipe */
        close(fd[READ_END]);

        /* write to the pipe */
        write(fd[WRITE_END], write_msg, strlen(write_msg) + 1);

        /* close the write end of the pipe */
        close(fd[WRITE_END]);
    }
    else { /* child process */
        /* close the unused end of the pipe */
        close(fd[WRITE_END]);

        /* read from the pipe */
        read(fd[READ_END], read_msg, BUFFER_SIZE);
        printf("read %s", read_msg);

        /* close the read end of the pipe */
        close(fd[READ_END]);
    }

    return 0;
}
