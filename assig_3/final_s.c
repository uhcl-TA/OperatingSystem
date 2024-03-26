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
    int processIDCounter = 0;
    int processID, server_fd, client_fds[MAX_CLIENTS], num_clients = 0;
    int stored_value[MAX_CLIENTS];
    char client_fifo[MAX_CLIENTS][20]; // Array to store client FIFO names
    
    // Create server FIFO
    mkfifo(FIFO_NAME, 0666);
    
    printf("Server started. Waiting for clients...\n");
    
    // Open server FIFO for reading
    server_fd = open(FIFO_NAME, O_RDONLY);
    
    // Main server loop
    while(1) {
        // char buffer[256];
        // Read message from client
        char message[256];
        read(server_fd, message, sizeof(message));
        printf("%s \n", message);

        if (strncmp(message, "SHUTDOWN", 7) == 0) {
          // Close server FIFO
          close(server_fd);
          
          // Remove server FIFO
          unlink(FIFO_NAME);
          return(0);
        }
        // Check if message is from a new client
        else if (strncmp(message, "CONNECT", 7) == 0) {
            // Extract client FIFO name
            char client_name[20];
            sscanf(message, "CONNECT ./fifos/%s", client_name);
            
            // Open client FIFO for writing
            sprintf(client_fifo[num_clients], "./fifos/client%sfifo", client_name);
            client_fds[num_clients] = open(client_fifo[num_clients], O_WRONLY);

            // Send welcome message to client
            char welcome_message[256];
            sprintf(welcome_message, "Welcome, %s!", client_name);
            write(client_fds[num_clients], welcome_message, strlen(welcome_message) + 1);
            
            num_clients++;
        }
      else {
        int processID = atoi(strtok(message, ","));
        int systemCallNumber = atoi(strtok(NULL, ","));
        int parameterCount = atoi(strtok(NULL, ","));
        int parameterSize = atoi(strtok(NULL, ","));
        char* parameters;
        if (systemCallNumber != 5) {
          parameters = strtok(NULL, ",");
        }

        // printf("process ID %d \n", processID);
        // printf("systemCallNumber %d \n", systemCallNumber);
        // printf("parameter count %d \n", parameterCount);
        // printf("parameter size %d \n", parameterSize);
        // printf("parameters %s \n", parameters);

        switch (systemCallNumber) {
            case 1: // Connect
              printf("in case 1 \n");
              printf("client number %d \n", processIDCounter);
              // sprintf(clientFifoName, "./fifos/Client%d_fifo", processIDCounter);
              // printf(clientFifoName);
              char* client_name = parameters;
              
              char welcome_message[256];
              // Sending the PID
              sprintf(welcome_message, "%d", processIDCounter);
              write(client_fds[processIDCounter], welcome_message, strlen(welcome_message) + 1);
              processIDCounter++;
            break;
            case 2: // display number
              printf("in case 2 \n");
              char case2_message[256];
              sprintf(case2_message, "received the number %s", parameters);
              write(client_fds[processID], case2_message, strlen(case2_message) + 1);
            break;
            case 3:
              printf("in case 3 \n");
              char case3_message[256];
              sprintf(case3_message, "received text %s", parameters);
              write(client_fds[processID], case3_message, strlen(case3_message) + 1);
            break;
            case 4:
              printf("in case 4 \n");
              stored_value[processID] = atoi(parameters);
              char *case4_message = "stored";
              write(client_fds[processID], case4_message, strlen(case4_message) + 1);
            break;
            case 5:
              printf("in case 5 \n");
              char case5_message[256];
              printf("%d \n", processID);
              printf("%d \n", stored_value[processID]);
              sprintf(case5_message, "Stored the value %d", stored_value[processID]);
              printf("%s", case5_message);
              write(client_fds[processID], case5_message, strlen(case5_message) + 1);
            break;
            case -1: // Shutdown
              // Close well-known FIFO
              close(server_fd);
              // Delete well-known FIFO
              unlink("FIFO_NAME");
              // Terminate server
              exit(0);
            break;
        printf("Client Pid : %d\n", processID + 1);
        printf("System Call Received: %d with %d parameters which are: \n", systemCallNumber, parameterCount);
        for (int i=0; i<parameterSize; i++) {
          printf("Param%d=%d", i+1, parameters[i]);
        }
      }
        }
    }
    
    // Close server FIFO
    close(server_fd);
    
    // Remove server FIFO
    unlink(FIFO_NAME);
    
    return 0;
}
