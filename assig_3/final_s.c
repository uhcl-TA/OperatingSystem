#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define MAX_CLIENTS 10
#define FIFO_NAME "./fifos/server_fifo"

int main() {
    int server_fd, client_fds[MAX_CLIENTS], num_clients = 0;
    char client_fifo[MAX_CLIENTS][20]; // Array to store client FIFO names
    char message[256];
    
    // Create server FIFO
    mkfifo(FIFO_NAME, 0666);
    
    printf("Server started. Waiting for clients...\n");
    
    // Open server FIFO for reading
    server_fd = open(FIFO_NAME, O_RDONLY);
    
    // Main server loop
    while(1) {
        // Read message from client
        read(server_fd, message, sizeof(message));
        printf("Received message from client: %s\n", message);
        
        // Check if message is from a new client
        if (strncmp(message, "CONNECT", 7) == 0) {
            // Extract client FIFO name
            char client_name[20];
            sscanf(message, "CONNECT ./fifos/%s", client_name);
            
            // Open client FIFO for writing
            sprintf(client_fifo[num_clients], "./fifos/%s_fifo", client_name);
            client_fds[num_clients] = open(client_fifo[num_clients], O_WRONLY);
            
            // Send welcome message to client
            char welcome_message[256];
            sprintf(welcome_message, "Welcome, %s!", client_name);
            write(client_fds[num_clients], welcome_message, strlen(welcome_message) + 1);
            
            printf("Client '%s' connected.\n", client_name);
            num_clients++;
        } else {
            printf("in else part \n");
            // Broadcast message to all connected clients
            for (int i = 0; i < num_clients; i++) {
                printf("boradcasting %d \n", client_fds[i]);
                write(client_fds[i], message, strlen(message) + 1);
            }
        }
    }
    
    // Close server FIFO
    close(server_fd);
    
    // Remove server FIFO
    unlink(FIFO_NAME);
    
    return 0;
}
