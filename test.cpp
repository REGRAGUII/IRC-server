// #include <iostream>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <unistd.h>
// #include <cstdlib>

// int main(int argc, char **argv)
// {
//     if (argc != 2)
//     {
//         std::cerr << "Usage: ./server <port>\n";
//         return 1;
//     }

//     int port = std::atoi(argv[1]);

//     // Create socket
//     int server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd == -1)
//     {
//         perror("socket");
//         return 1;
//     }

//     // Bind socket to port
//     sockaddr_in address;
//     address.sin_family = AF_INET;
//     address.sin_addr.s_addr = INADDR_ANY;
//     address.sin_port = htons(port);

//     if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
//     {
//         perror("bind");
//         close(server_fd);
//         return 1;
//     }

//     // Listen
//     if (listen(server_fd, 5) < 0)
//     {
//         perror("listen");
//         close(server_fd);
//         return 1;
//     }

//     std::cout << "Server listening on port " << port << std::endl;

//     // Keep server running
//     while (1)
//     {
//         std::cout << "asd"; 
//         sleep(1);
//     }

//     close(server_fd);
//     return 0;
// }
