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
    char message[MAX_BUF_SIZE];

    // Create server FIFO
    mkfifo(SERVER_FIFO, 0666);

    // Open server FIFO for reading
    server_fd = open(SERVER_FIFO, O_RDONLY);
    if (server_fd == -1) {
        perror("Server: Unable to open server FIFO");
        exit(EXIT_FAILURE);
    }

    // Read client FIFO name from the client
    if (read(server_fd, client_fifo, MAX_BUF_SIZE) == -1) {
        perror("Server: Error reading client FIFO name");
        exit(EXIT_FAILURE);
    }

    // Open client FIFO for writing
    client_fd = open(client_fifo, O_WRONLY);
    if (client_fd == -1) {
        perror("Server: Unable to open client FIFO");
        exit(EXIT_FAILURE);
    }

    // Display client's name on screen
    printf("Server: Received client FIFO name: %s\n", client_fifo);

    // Wait for requests from the client
    while (1) {
        printf("Server: Waiting for requests from the client...\n");

        // Read request from the client
        if (read(server_fd, message, MAX_BUF_SIZE) == -1) {
            perror("Server: Error reading request from the client");
            break;
        }

        // Display the received request on the screen
        printf("Server: Received request from client: %s\n", message);

        // Echo the request back to the client
        write(client_fd, message, strlen(message) + 1);
    }

    // Clean up
    close(server_fd);
    close(client_fd);
    unlink(SERVER_FIFO);

    return 0;
}
