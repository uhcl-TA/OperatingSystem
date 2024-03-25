#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SERVER_FIFO "server_fifo"
#define MAX_BUF_SIZE 256

int main() {
    int server_fd, client_fd;
    char client_fifo[MAX_BUF_SIZE];
    char request[MAX_BUF_SIZE];

    // Create client FIFO
    sprintf(client_fifo, "client_fifo_%d", getpid());
    mkfifo(client_fifo, 0666);

    // Open server FIFO for writing
    server_fd = open(SERVER_FIFO, O_WRONLY);
    if (server_fd == -1) {
        perror("Client: Unable to open server FIFO");
        exit(EXIT_FAILURE);
    }

    // Send client FIFO name to the server
    write(server_fd, client_fifo, strlen(client_fifo) + 1);

    // Open client FIFO for reading
    client_fd = open(client_fifo, O_RDONLY);
    if (client_fd == -1) {
        perror("Client: Unable to open client FIFO");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Read input from the user to form a request
        printf("Client: Enter your request (or 'exit' to quit): ");
        fgets(request, MAX_BUF_SIZE, stdin);
        request[strcspn(request, "\n")] = '\0'; // Remove newline character

        // Send the request to the server
        write(server_fd, request, strlen(request) + 1);

        // Check if the user wants to exit
        if (strcmp(request, "exit") == 0) {
            break;
        }

        // Wait for the server's response
        printf("Client: Waiting for the server's response...\n");
        read(client_fd, request, MAX_BUF_SIZE);
        printf("Client: Received response from the server: %s\n", request);
    }

    // Clean up
    close(server_fd);
    close(client_fd);
    unlink(client_fifo);

    return 0;
}
