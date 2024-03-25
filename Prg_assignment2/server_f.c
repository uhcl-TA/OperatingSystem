#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SERVER_FIFO "server_fifo"
#define MAX_BUF_SIZE 256

// Function to process different types of requests
void processRequest(const char *request, char *result, int *client_specific_fd, pid_t *client_pid, int *stored_value) {
    int sys_call_number, num_parameters, param_size;
    char param_value[MAX_BUF_SIZE];
    // void processRequest(request, result) {
    sscanf(request, "%d %d", &sys_call_number, &num_parameters);

    // Print the system call received message
    printf("Server: Client pid: %d\n", *client_pid);
    printf("Server: System Call Requested: %d with %d parameters which are:\n", sys_call_number, num_parameters);

    for (int i = 0; i < num_parameters; ++i) {
        // Read parameter value
        read(client_specific_fd[i], param_value, MAX_BUF_SIZE);
        printf("Param%d=%s ", i + 1, param_value);
    }

    // Additional information in the format
    printf("Result=%s\n", result);

    switch (sys_call_number) {
        case 0: // EXIT
            // Close the client-specific FIFO
            for (int i = 0; i < num_parameters; ++i) {
                close(client_specific_fd[i]);
            }
            break;

            case 1: // Connect System Call
            if (num_parameters == 1) {
                // Read the client-specific FIFO name
                read(client_specific_fd[0], result, MAX_BUF_SIZE);

                // Open the client-specific FIFO in WRITE mode
                int fd = open(result, O_WRONLY);
                if (fd == -1) {
                    perror("Server: Unable to open client-specific FIFO");
                    exit(EXIT_FAILURE);
                }

                client_specific_fd[0] = fd;
                *client_pid = atoi(result);

                // No need to return anything for the connect system call
                result[0] = '\0';
            }
            break;

            case 2: // Number System Call
            if (num_parameters == 1) {
                // Read the parameter size and value
                read(client_specific_fd[0], &param_size, sizeof(int));
                read(client_specific_fd[0], param_value, param_size);

                // Convert the parameter value to an integer
                int number = atoi(param_value);

                // Return the number received
                snprintf(result, MAX_BUF_SIZE, "%d", number);
            }
            break;

        case 3: // Text System Call
            if (num_parameters == 1) {
                // Read the parameter size and value
                read(client_specific_fd[0], &param_size, sizeof(int));
                read(client_specific_fd[0], param_value, param_size);

                // Return the string received
                snprintf(result, MAX_BUF_SIZE, "%s", param_value);
            }
            break;

        case 4: // Store System Call
            if (num_parameters == 1) {
                // Read the parameter size and value
                read(client_specific_fd[0], &param_size, sizeof(int));
                read(client_specific_fd[0], param_value, param_size);

                // Convert the parameter value to an integer and store it
                *stored_value = atoi(param_value);

                // Return stored value
                snprintf(result, MAX_BUF_SIZE, "%d", *stored_value);
            }
            break;

        case 5: // Recall System Call
            if (num_parameters == 0) {
                // Return stored/recalled value
                snprintf(result, MAX_BUF_SIZE, "%d", *stored_value);
            }
            break;

        case -1: // Shutdown System Call
            // Close the well-known FIFO and terminate
            close(client_specific_fd[0]);
            unlink(SERVER_FIFO);
            exit(EXIT_SUCCESS);
            break;
    }
}

int main() {
    int server_fd, client_fd;
    char client_fifo[MAX_BUF_SIZE];
    char request[MAX_BUF_SIZE];
    char result[MAX_BUF_SIZE];
    int client_specific_fd[MAX_BUF_SIZE];
    int stored_value = 0;
    pid_t client_pid;

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
        if (read(server_fd, request, MAX_BUF_SIZE) == -1) {
            perror("Server: Error reading request from the client");
            break;
        }

        // Process the request
        processRequest(request, result, client_specific_fd, &client_pid, &stored_value);
        // processRequest(request, result);

        // Display the result on the screen
        printf("Server: %s\n", result);

        // Send the result back to the client
        write(client_fd, result, strlen(result) + 1);
    }

    // Clean up
    close(server_fd);
    close(client_fd);
    unlink(SERVER_FIFO);

    return 0;
}
