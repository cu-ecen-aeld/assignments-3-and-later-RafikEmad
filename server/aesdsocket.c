#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <signal.h>
#include <fcntl.h>
#include <getopt.h>

#define PORT 9000
#define BUFFER_SIZE 1024
#define DATA_FILE "/var/tmp/aesdsocketdata"

volatile sig_atomic_t stop = 0;

// Signal handler for SIGINT and SIGTERM
void handle_signal(int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        stop = 1;
        syslog(LOG_INFO, "Caught signal, exiting");
        remove(DATA_FILE); // Delete the data file on exit
    }
}

// Function to handle client connections
void handle_client(int client_socket, struct sockaddr_in *client_addr) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    FILE *data_file;

    // Log the client connection
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr->sin_addr), client_ip, INET_ADDRSTRLEN);
    syslog(LOG_INFO, "Accepted connection from %s", client_ip);

    // Open the data file for appending
    data_file = fopen(DATA_FILE, "a+");
    if (!data_file) {
        syslog(LOG_ERR, "Failed to open data file");
        close(client_socket);
        return;
    }

    // Receive data from the client
    while ((bytes_read = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_read] = '\0';
        fputs(buffer, data_file);

        // Check if the packet ends with a newline
        if (buffer[bytes_read - 1] == '\n') {
            fflush(data_file); // Ensure data is written to the file

            // Send the entire file content back to the client
            rewind(data_file); // Reset file pointer to the beginning
            while (fgets(buffer, BUFFER_SIZE, data_file) != NULL) {
                send(client_socket, buffer, strlen(buffer), 0);
            }
        }
    }

    // Close the data file and client socket
    fclose(data_file);
    close(client_socket);

    // Log the client disconnection
    syslog(LOG_INFO, "Closed connection from %s", client_ip);
}

// Function to daemonize the process
void daemonize() {
    pid_t pid = fork();

    if (pid < 0) {
        syslog(LOG_ERR, "Failed to fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        // Parent process exits
        exit(EXIT_SUCCESS);
    }

    // Create a new session
    if (setsid() < 0) {
        syslog(LOG_ERR, "Failed to create new session");
        exit(EXIT_FAILURE);
    }

    // Change the working directory to root
    if (chdir("/") < 0) {
        syslog(LOG_ERR, "Failed to change working directory");
        exit(EXIT_FAILURE);
    }

    // Redirect standard file descriptors to /dev/null
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    open("/dev/null", O_RDONLY); // stdin
    open("/dev/null", O_WRONLY); // stdout
    open("/dev/null", O_WRONLY); // stderr
}

int main(int argc, char *argv[]) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int daemon_mode = 0;

    // Parse command-line arguments
    int opt;
    while ((opt = getopt(argc, argv, "d")) != -1) {
        switch (opt) {
            case 'd':
                daemon_mode = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-d]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Open syslog
    openlog("aesdsocket", LOG_PID | LOG_NDELAY, LOG_USER);

    // Clear the data file on startup
    remove(DATA_FILE);

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        syslog(LOG_ERR, "Failed to create socket");
        return -1;
    }

    // Bind the socket to port 9000
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        syslog(LOG_ERR, "Failed to bind socket");
        close(server_socket);
        return -1;
    }

    // Listen for connections
    if (listen(server_socket, 5) == -1) {
        syslog(LOG_ERR, "Failed to listen on socket");
        close(server_socket);
        return -1;
    }

    // Daemonize if requested
    if (daemon_mode) {
        daemonize();
    }

    // Set up signal handling
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    // Main loop to accept connections
    while (!stop) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            if (stop) {
                // Exit if a signal was received
                break;
            }
            syslog(LOG_ERR, "Failed to accept connection");
            continue;
        }

        // Handle the client connection
        handle_client(client_socket, &client_addr);
    }

    // Cleanup
    close(server_socket);
    remove(DATA_FILE); // Ensure the file is deleted on exit
    syslog(LOG_INFO, "Server stopped");

    // Close syslog
    closelog();

    return 0;
}