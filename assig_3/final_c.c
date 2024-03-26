#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define SERVER_FIFO "./fifos/server_fifo"
#define MAX_MESSAGE_SIZE 256

int processID;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <client_number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char client_fifo[20];
    char server_message[MAX_MESSAGE_SIZE];

    // Create client FIFO
    printf("Creating client fifo \n");
    sprintf(client_fifo, "./fifos/client%sfifo", argv[1]);
    printf("creating client fifo %s \n", client_fifo);
    mkfifo(client_fifo, 0666);

    // Open server FIFO for writing
    int server_fd = open(SERVER_FIFO, O_WRONLY);
    if (server_fd == -1) { // Error handling for server fifo
        perror("Failed to open server FIFO");
        exit(EXIT_FAILURE);
    }

    // Connect to server
    char connect_message[50];
    sprintf(connect_message, "CONNECT ./fifos/%s", argv[1]);
    write(server_fd, connect_message, strlen(connect_message) + 1);

    // Open client FIFO for reading
    int client_fd = open(client_fifo, O_RDONLY);

    // Read welcome message from server
    read(client_fd, server_message, sizeof(server_message));
    printf("Server: %s\n", server_message);

    // Main client loop
    while(1) {
        // Read user input
        printf("What do you want to do?\n");
        printf("1. Send request to server\n");
        printf("2. Exit\n");
        printf("3. Shutdown\n");
        int choice;
        scanf("%d", &choice);

        if (choice == 1) {
          printf("Enter system call number: ");
            int systemCallNumber;
            scanf("%d", &systemCallNumber);

            printf("Enter number of parameters: ");
            int parameterCount;
            scanf("%d", &parameterCount);

            char parameters[MAX_MESSAGE_SIZE];
            for (int i = 0; i < parameterCount; i++) {
                printf("Enter parameter %d: ", i + 1);
                scanf("%s", parameters);
            }

            // Format system call request
            char request[MAX_MESSAGE_SIZE];
            sprintf(request, "%d,%d,%d,%d,%s \n", processID, systemCallNumber, parameterCount, strlen(parameters), parameters);
            // Send message to server
            write(server_fd, request, strlen(request) + 1);

            read(client_fd, server_message, sizeof(server_message));
            if (systemCallNumber == 1) {
              // Save the PID
              printf("process ID is %s \n", server_message);
              processID = atoi(strtok(server_message, ","));
              printf("saved PID %d \n", processID);
            } else {
              printf("%s \n", server_message);
            }   
        } else if (choice == 2) {
          // Exit
          close(client_fd);
          // Remove client FIFO
          unlink(client_fifo);
          return 0;
        } else if (choice == 3) {
          // Shutdown
          char SHUT_DOWN_MESSAGE[50];
          sprintf(SHUT_DOWN_MESSAGE, "SHUTDOWN");
          write(server_fd, SHUT_DOWN_MESSAGE, strlen(SHUT_DOWN_MESSAGE) + 1);
          // Close FIFOs
          close(server_fd);
          close(client_fd);

          // Remove client FIFO
          unlink(client_fifo);
          return 0;
        }
    }

    // Close FIFOs
    close(server_fd);
    // close(client_fd);

    // Remove client FIFO
    unlink(client_fifo);

    return 0;
}
