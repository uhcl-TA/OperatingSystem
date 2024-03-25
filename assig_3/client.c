#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define MAX_BUFFER_SIZE 256

int main() {
    int processID;
    int serverFifo;
    char clientFifoName[MAX_BUFFER_SIZE];
    int clientFifo;
    int clientID = -1;

    // Open well-known server FIFO in WRITE mode
    serverFifo = open("server_fifo", O_WRONLY);
    printf("opened server fifo \n");

    // Create client-specific FIFO
    sprintf(clientFifoName, "./Client%dfifo", getpid());
    mkfifo(clientFifoName, 0666);
    printf("created client fifo \n");

    // Send connect system call to server
    // char connectRequest[MAX_BUFFER_SIZE];
    // sprintf(connectRequest, "%d,%d,%d,%d,%s", getpid(), 1, 1, strlen(clientFifoName), clientFifoName);
    // write(serverFifo, connectRequest, sizeof(connectRequest));
    // printf("Connected to server now \n");

    printf("text %s \n", clientFifoName);
    // Open client-specific FIFO in READ mode
    // clientFifo = open(clientFifoName, O_RDONLY);
    // printf("opened the client fifo \n");

    while (1) {
        // Ask user for input
        printf("What do you want to do?\n");
        printf("1. Send request to server\n");
        printf("2. Exit\n");
        printf("3. Shutdown\n");
        int choice;
        scanf("%d", &choice);

        if (choice == 1) {
            // Send request to server
            printf("Enter system call number: ");
            int systemCallNumber;
            scanf("%d", &systemCallNumber);

            printf("Enter number of parameters: ");
            int parameterCount;
            scanf("%d", &parameterCount);

            char parameters[MAX_BUFFER_SIZE];
            for (int i = 0; i < parameterCount; i++) {
                printf("Enter parameter %d: ", i + 1);
                scanf("%s", parameters);
            }
            // Format system call request
            char request[MAX_BUFFER_SIZE];
            sprintf(request, "%d,%d,%d,%d,%s", processID, systemCallNumber, parameterCount, strlen(parameters), parameters);

            // Send request to server
            write(serverFifo, request, sizeof(request));

            // Read reply from server
            char reply[MAX_BUFFER_SIZE];
            if (systemCallNumber == 1) {
             printf("reply is %s\n", reply);
            }
            read(clientFifo, reply, sizeof(reply));

            // Print reply
            printf("Reply from server: %s\n", reply);
        } else if (choice == 2) {
            // Send exit system call to server
            char exitRequest[MAX_BUFFER_SIZE];
            sprintf(exitRequest, "%d,%d,%d,%d,%s", processID, 0, 0, 0, "");
            write(serverFifo, exitRequest, sizeof(exitRequest));

            // Close client-specific FIFO
            close(clientFifo);

            // Delete client-specific FIFO
            unlink(clientFifoName);

            // Exit client program
            exit(0);
        } else if (choice == 3) {
            // Send shutdown system call to server
            char shutdownRequest[MAX_BUFFER_SIZE];
            sprintf(shutdownRequest, "%d,%d,%d,%d,%s", processID, -1, 0, 0, "");
            write(serverFifo, shutdownRequest, sizeof(shutdownRequest));

            // Close client-specific FIFO
            close(clientFifo);

            // Delete client-specific FIFO
            unlink(clientFifoName);

            // Exit client program
            exit(0);
        }
    }

    return 0;
}
