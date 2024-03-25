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
        // Read user input
        printf("Enter message: ");
        fgets(server_message, sizeof(server_message), stdin);

        // Send message to server
        write(server_fd, server_message, strlen(server_message) + 1);

        // Check for exit command
        if (strncmp(server_message, "exit", 4) == 0) {
            break;
        }

        // Read server response
        read(client_fd, server_message, sizeof(server_message));
        printf("Server: %s\n", server_message);
    }

    // Close FIFOs
    close(server_fd);
    close(client_fd);

    // Remove client FIFO
    unlink(client_fifo);

    return 0;
}
