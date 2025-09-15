#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>



int main(int argc, char **argv)
{
    if(argc != 3)
    {
        std::cout << "invalid number of arguments !";
        return (1);
    }
    int port = atoi(argv[1]);
    int socket_fd = socket(AF_INET, SOCK_STREAM,0);
    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        return(1);
    }
    sockaddr_in address;
    address.sin_port = htons(port);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_fd, (struct sockaddr *)&address,sizeof(address))< 0)
    {
        perror("bind failed");
        close(socket_fd);
        return(1);
    }

    if(listen(socket_fd, 10) < 0)
    {
        perror("listne failed");
        return (1);
    }

    std::cout << "serveur listening on port " << port << "\n";

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
    {
        perror("accept failed");
        close(socket_fd);
        return 1;
    }

    std::cout << "New client connected\n";

    char buffer[1024];
    while(1)
    {
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received < 0)
        {
            perror("recv failed");
        }
        else
        {
            std::cout << "Received from client: " << buffer;
            const char *response = "Message sent correctly\n";
            int bytes_sent = send(client_fd, response, strlen(response), 0);
            if (bytes_sent < 0)
            {
                perror("send failed");
            }
            else
            {
            }
                std::cout << "Sent response to client." << std::endl;
        }

    }

    close(client_fd);
    close(socket_fd);
}
