#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080

int main(int argc, char const* argv[])
{
    int server_fd, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = { 0 };

    FILE *fptr;

    // Open a file in read mode
    fptr = fopen("index.html", "r");
    if (fptr == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    // Store the content of the file
    char myString[100] = { 0 };

    if (fgets(myString, sizeof(myString), fptr) == NULL) {
        perror("Failed to read from file");
        fclose(fptr);
        exit(EXIT_FAILURE);
    }

    fclose(fptr);

    char* hello = myString;

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Accept a connection
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Read from the socket
    valread = read(new_socket, buffer, sizeof(buffer) - 1);
    if (valread < 0) {
        perror("read error");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    
    buffer[valread] = '\0'; // Null-terminate the buffer
    printf("Received: %s\n", buffer);

    // Send the content of the file to the client
    send(new_socket, hello, strlen(hello), 0);
    printf("Message sent: %s\n", hello);

    // Closing the connected socket and the listening socket
    close(new_socket);
    close(server_fd);
    return 0;
}
