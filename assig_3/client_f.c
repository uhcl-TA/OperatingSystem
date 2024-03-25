#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>

#define MAX_BUFFER_SIZE 256

int processID;
int serverFifo;
char clientFifoName[MAX_BUFFER_SIZE];
int clientFifo;

void *fifo_reader(void *arg) {
    char reply[MAX_BUFFER_SIZE];
    while (1) {
        // Read reply from server
        int bytes_read = read(clientFifo, reply, sizeof(reply));
        if (bytes_read > 0) {
            // Print reply
            printf("Reply from server: %s\n", reply);
        }
    }
}

int main() {
    processID = getpid();

    // Open well-known server FIFO in WRITE mode
    serverFifo = open("server_fifo", O_RDWR);
    if (serverFifo == -1) {
        perror("Failed to open server FIFO");
        exit(EXIT_FAILURE);
    }
    printf("Opened server fifo\n");

    // Create client-specific FIFO
    sprintf(clientFifoName, "./fifos/Client%d_fifo", processID);
    if (mkfifo(clientFifoName, 0666) == -1) {
        perror("Failed to create client FIFO");
        exit(EXIT_FAILURE);
    }
    printf("Created client fifo\n");

    // Open client-specific FIFO in READ mode
    clientFifo = open(clientFifoName, O_RDONLY | O_NONBLOCK);
    if (clientFifo == -1) {
        perror("Failed to open client FIFO");
        exit(EXIT_FAILURE);
    }
    printf("Opened the client fifo\n");

    // Create thread for FIFO reading
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, fifo_reader, NULL);

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
            read(clientFifo, reply, sizeof(reply));
            int bytes_read = read(clientFifo, reply, sizeof(reply));
            if (bytes_read > 0) {
                // Print reply
                printf("Reply from server: %s\n", reply);
            }

            // Print reply
            // printf("Reply from server: %s\n", reply);
        } else if (choice == 2) {
            // Send exit system call to server
            // (Code for sending exit request to server...)
            break;
        } else if (choice == 3) {
            // Send shutdown system call to server
            // (Code for sending shutdown request to server...)
            break;
        }
    }

    // Close client-specific FIFO
    close(clientFifo);

    // Delete client-specific FIFO
    unlink(clientFifoName);

    // Wait for the FIFO reader thread to finish
    pthread_join(thread_id, NULL);

    return 0;
}
