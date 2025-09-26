#include "init.hpp"

void ft_init(IrcServer& irc, char **argv)
{
    irc.setport(atoi(argv[1]));
    irc.setsocket(socket(AF_INET, SOCK_STREAM,0));
    int opt = 1;
    if (setsockopt(irc.getsocket_fd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("setsockopt failed");
}


void bind_and_listen_accept(IrcServer& irc)
{
    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_port = htons(irc.getport());
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(irc.getsocket_fd(), (struct sockaddr *)&address,sizeof(address))< 0)
        throw std::runtime_error("bind failed");
    if(listen(irc.getsocket_fd(), SOMAXCONN) < 0)
        throw std::runtime_error("listne failed");

    std::cout << "serveur listening on port " << irc.getport() << "\n";
}



int accept_new_client(IrcServer& irc)
{
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(irc.getsocket_fd(),(struct sockaddr *)&client_addr,&client_len);
    if (client_fd < 0)
        throw std::runtime_error("accept failed");

    std::cout << "New client connected: " << client_fd - 3 << "\n";
    irc.add_client(client_fd);
    return client_fd;
}

void run_server_loop(IrcServer& irc)
{
    std::vector<pollfd> fds;

    pollfd server_poll;
    server_poll.fd = irc.getsocket_fd();
    server_poll.events = POLLIN;
    fds.push_back(server_poll);

    while (true)
    {
        int activity = poll(fds.data(), fds.size(), -1);
        if (activity < 0)
            throw std::runtime_error("poll failed");

        for (size_t i = 0; i < fds.size(); i++)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == irc.getsocket_fd())
                {
                    int client_fd = accept_new_client(irc);
                    pollfd client_poll;
                    client_poll.fd = client_fd;
                    client_poll.events = POLLIN;
                    fds.push_back(client_poll);
                }
                else
                {
                    char buffer[8042];
                    memset(buffer, 0, 8042);
                    int bytes_received = recv(fds[i].fd, buffer, 8042 - 1, 0);

                    if (bytes_received <= 0)
                    {
                        std::cout << "Client disconnected: " << fds[i].fd << "\n";
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        i--;
                    }
                    else
                    {
                        // std::cout << "Message from client " << fds[i].fd - 3 << ": " << buffer << "\n";
                        // std::string response = "Server received: " + std::string(buffer);
                        // send(fds[i].fd, response.c_str(), response.size(), 0);
                        processMessage(buffer,fds[i].fd, irc);
                    }
                }
            }
        }
    }
}

void processMessage(const std::string &message, int clientFd, IrcServer &server)
{
    (void)server;
    std::string botResponse;

    if(message[0] == '!')
    {
        if(message.find("!hello") != std::string::npos)
        {
            botResponse = "Bot : hey this IRC Bot can i help you ?\n";
            send(clientFd, botResponse.c_str(), botResponse.size(),0);
        }
        else if(message.find("!help") != std::string::npos)
        {
            botResponse = "Bot : Available commands: !hello, !help, !time,\n";
            send(clientFd, botResponse.c_str(), botResponse.size(), 0);
        }
        else if(message.find("!time") != std::string::npos)
        {
            botResponse = "Bot : Current time feature coming soon!\n";
            send(clientFd, botResponse.c_str(), botResponse.size(), 0);
        } 
        else
        {
            botResponse = "Bot : invalid commande try <!help> to see all commandes\n";
            send(clientFd, botResponse.c_str(), botResponse.size(), 0);
        }
    }
    else if(message[0] == '/')
    {
        if(message.find("/send") != std::string::npos)
        {
            botResponse = "start file transfer\n";
            send(clientFd, botResponse.c_str(), botResponse.size(),  0);
            // i have to start the file transfer logic here
        }
        else if(message.find("/accept") != std::string::npos)
        {
            botResponse = "file transfer accepted\n";
            send(clientFd, botResponse.c_str(), botResponse.size(), 0);
        }
        else if(message.find("/decline") != std::string::npos)
        {
            botResponse = "file transfer declined\n";
            send(clientFd, botResponse.c_str(), botResponse.size(), 0);
        }
        else
        {
            botResponse = "invalid file command\n";
            send(clientFd, botResponse.c_str(), botResponse.size(), 0);
        }

    }
    else
    {
        std::cout << "Message from client " << clientFd << ": " << message << "\n";
        std::string response = "Server received: " + message;
        send(clientFd, response.c_str(), response.size(), 0);
    }
    
}