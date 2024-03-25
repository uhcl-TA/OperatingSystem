#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 256

int main() {
    printf("inside\n");
    int processIDCounter = 1;
    int clientCount = 0;
    int clientFifos[MAX_CLIENTS];
    int clientPIDs[MAX_CLIENTS];

    // Create well-known receive FIFO
    mkfifo("server_fifo", 0666);
    printf("Fifo created \n");

    // Open well-known receive FIFO in READ/WRITE mode
    int serverFifo = open("server_fifo", O_RDWR);
    printf("server ready and listening.....\n");

    while (1) {
        // Read request from clients
        char buffer[MAX_BUFFER_SIZE];
        read(serverFifo, buffer, sizeof(buffer));

        // Parse request
        int processID = atoi(strtok(buffer, ","));
        int systemCallNumber = atoi(strtok(NULL, ","));
        int parameterCount = atoi(strtok(NULL, ","));
        int parameterSize = atoi(strtok(NULL, ","));
        char* parameters = strtok(NULL, ",");

        // Print system call received
        printf("Client pid: %d\n", processID);
        printf("System Call Requested: %d with %d parameters which are:\n", systemCallNumber, parameterCount);
        printf("Parameters: %s\n", parameters);

        // Process system call
        switch (systemCallNumber) {
            case 1: // Connect
                // Open client-specific FIFO in WRITE mode
                printf("in case 1 \n");
                char clientFifoName[MAX_BUFFER_SIZE];
                printf("client number %d \n", processIDCounter);
                sprintf(clientFifoName, "./Client%dfifo", processIDCounter);
                int clientFifo = open(clientFifoName, O_WRONLY);
                clientFifos[clientCount] = clientFifo;
                clientPIDs[clientCount] = processID;
                clientCount++;
                // Increment processID counter and return next available simulated ClientID/processID
                sprintf(buffer, "%s", "abc");
                write(clientFifo, buffer, sizeof(buffer));
                processIDCounter++;
                break;
            case 2: // Number
                // Process number system call
                // ...

                // Return the number received
                sprintf(buffer, "%s", parameters);
                write(clientFifos[clientCount], buffer, sizeof(buffer));
                break;
            case 3: // Text
                // Process text system call
                // ...

                // Return the string
                sprintf(buffer, "%s", parameters);
                write(clientFifos[clientCount], buffer, sizeof(buffer));
                break;
            case 4: // Store
                // Process store system call
                // ...

                // Return stored value
                sprintf(buffer, "%s", parameters);
                write(clientFifos[clientCount], buffer, sizeof(buffer));
                break;
            case 5: // Recall
                // Process recall system call
                // ...

                // Return stored/recalled value
                sprintf(buffer, "%s", parameters);
                write(clientFifos[clientCount], buffer, sizeof(buffer));
                break;
            case 0: // Exit
                // Close client-specific FIFO
                close(clientFifos[clientCount]);
                break;
            case -1: // Shutdown
                // Close well-known FIFO
                close(serverFifo);
                // Delete well-known FIFO
                unlink("server_fifo");
                // Terminate server
                exit(0);
        }
    }

    return 0;
}
