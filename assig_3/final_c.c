#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define SERVER_FIFO "./fifos/server_fifo"
#define MAX_MESSAGE_SIZE 256

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <client_name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char client_fifo[20];
    char server_message[MAX_MESSAGE_SIZE];

    // Create client FIFO
    sprintf(client_fifo, "./fifos/%s_fifo", argv[1]);
    mkfifo(client_fifo, 0666);

    // Open server FIFO for writing
    int server_fd = open(SERVER_FIFO, O_WRONLY);

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




        // // Read user input
        // printf("Enter message: ");
        // fgets(server_message, sizeof(server_message), stdin);

        // // Send message to server
        // write(server_fd, server_message, strlen(server_message) + 1);

        // // Check for exit command
        // if (strncmp(server_message, "exit", 4) == 0) {
        //     break;
        // }

        // // Read server response
        // read(client_fd, server_message, sizeof(server_message));
        // printf("Server: %s\n", server_message);
    }

    // Close FIFOs
    close(server_fd);
    close(client_fd);

    // Remove client FIFO
    unlink(client_fifo);

    return 0;
}
