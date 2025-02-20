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

// global PID for pid management
bool *pid_map = nullptr;

// creates and initializes the bitmap for PID management
int allocate_map() {
    pid_map = (bool*)malloc(sizeof(bool) * (MAX_PID + 1));

    if (pid_map == nullptr) {
        return -1;
    }

    // marking all pid's as free
    for (int i = 0; i < MAX_PID; i++) {
        pid_map[i] = false;
    }

    return 1;
}

// Allocates and returns a new PID from the bitmap
int allocate_pid() {
    int random_number = rand() % (MAX_PID - MIN_PID + 1) + MIN_PID;
    int pid = random_number;

    while (pid_map[pid]) {
        pid = rand() % (MAX_PID - MIN_PID + 1) + MIN_PID;
    }

    pid_map[pid] = true;
    return pid;
}

void release_pid(int pid) {
    if (pid >= MIN_PID && pid <= MAX_PID && pid_map[pid]) {
        pid_map[pid] = false;
        printf("Release PID %d \n", pid);
    } else {
        printf("PID %d is not in use \n", pid);
    }
}

int main(void) {
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

    // initializing the pid map 
    if (allocate_map() == -1) {
        fprintf(stderr, "Failed to allocate PID map");
        return 1;
    }

    /* fork a child process */
    pid = fork();

    // if the fork fails, then the pid is negative 
    if (pid < 0) { /* error occurred */
        fprintf(stderr, "Fork Failed");
        return 1;
    }

    if (pid > 0) { /* parent process */
        /* close the unused end of the pipe */
        close(fd[READ_END]);

        while (1) {
            read(fd[READ_END], read_msg, BUFFER_SIZE);
            
            if (strcmp(read_msg, "Request PID") == 0) {
                // allocating new pid 
                int new_pid = allocate_pid();
                printf("Parent allocated PID: %d \n", new_pid);

                // sending the new pid to child 
                sprintf(write_msg, "%d", new_pid);
                write(fd[WRITE_END], write_msg, BUFFER_SIZE);

            } else if (strncmp(read_msg, "Release PID:", 12) == 0) {
                int pid_to_release = atoi(read_msg + 12);
                release_pid(pid_to_release);

                printf("Parent received request to release PID: %d \n", pid_to_release);
            } else if (strcmp(read_msg, "Done") == 0) {
                break;
            }
        }

        // Closing after done
        close(fd[WRITE_END]);
        
    } else { // child process
        close(fd[WRITE_END]);

        for (int i = 0; i < 5; i++) { // releasing 5 pids 
            strcpy(write_msg, "Request PID");
            write(fd[WRITE_END], write_msg, BUFFER_SIZE);
            
            // read the allocated pid from the Parent
            read(fd[READ_END], read_msg, BUFFER_SIZE);
            int received = atoi(read_msg);

            printf("\nHello from child, received PID # %d", received);

            sprintf(write_msg, "Release PID: #%d", received);
            write(fd[WRITE_END], write_msg, BUFFER_SIZE);
        }

        // sending the Done message
        strcpy(write_msg, "Done");
        write(fd[WRITE_END], write_msg, BUFFER_SIZE);

        close(fd[READ_END]);
    }

    return 0;
}
