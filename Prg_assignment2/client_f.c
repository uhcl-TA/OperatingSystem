#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define SERVER_FIFO "server_fifo"
#define MAX_BUF_SIZE 256

void sendRequestToServer(int server_fd, const char *client_fifo) {
    char request[MAX_BUF_SIZE];
    char reply[MAX_BUF_SIZE];

    int syscallNumber, numParameters;
    printf("Client: Enter system call number: ");
    scanf("%d", &syscallNumber);

    printf("Client: How many parameters? ");
    scanf("%d", &numParameters);

    snprintf(request, MAX_BUF_SIZE, "SYS_CALL %d %d", syscallNumber, numParameters);

    write(server_fd, request, strlen(request) + 1);

    for (int i = 0; i < numParameters; ++i) {
        char parameter[MAX_BUF_SIZE];
        printf("Client: Enter value for parameter %d: ", i + 1);
        scanf("%s", parameter);

        write(server_fd, parameter, strlen(parameter) + 1);
    }

    read(server_fd, reply, MAX_BUF_SIZE);

    printf("Client: Received reply from the server: %s\n", reply);
}

int main() {
    int server_fd, client_fd;
    char client_fifo[MAX_BUF_SIZE];
    char request[MAX_BUF_SIZE];

    sprintf(client_fifo, "client_fifo_%d", getpid());
    mkfifo(client_fifo, 0666);

    server_fd = open(SERVER_FIFO, O_WRONLY);
    if (server_fd == -1) {
        perror("Client: Unable to open server FIFO");
        exit(EXIT_FAILURE);
    }

    write(server_fd, client_fifo, strlen(client_fifo) + 1);

    client_fd = open(client_fifo, O_RDONLY);
    if (client_fd == -1) {
        perror("Client: Unable to open client FIFO");
        exit(EXIT_FAILURE);
    }

    int choice;
    do {
        printf("\nClient Menu:\n");
        printf("1 - Send request to server\n");
        printf("2 - EXIT\n");
        printf("3 - SHUTDOWN\n");
        printf("Enter your choice (1-3): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("server %d", server_fd);
                printf("client %s", client_fifo);
                sendRequestToServer(server_fd, client_fifo);
                break;
            case 2:
                close(client_fd);
                break;
            case 3:
                write(server_fd, "SHUTDOWN", strlen("SHUTDOWN") + 1);
                break;
            default:
                printf("Invalid choice. Please enter a number between 1 and 3.\n");
        }
    } while (choice != 2 && choice != 3);

    close(server_fd);
    close(client_fd);
    unlink(client_fifo);

    return 0;
}
