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
    cmd cmd;
    Bot bot;
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
                    memset(buffer, 0, sizeof(buffer));
                    int bytes_received = recv(fds[i].fd, buffer, sizeof(buffer), 0);

                    if (bytes_received <= 0)
                    {
                        std::cout << "Client disconnected: " << fds[i].fd << "\n";
                        close(fds[i].fd);
                        irc.remove_client(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        i--;
                    }
                    else
                    {

                            IrcClient* client = irc.getClient(fds[i].fd);
                            client->Buffering(std::string(buffer));
                            std::string line;
                            while(client->ExtractLine(line))
                            {
                                cmd = ft_parse(line);
                                HandleCommand(*client, cmd, irc, bot);
                                // std::cout << cmd.c << std::endl ;
                                // std::cout << cmd.prefix << std::endl ;
                            //
                            //  for (size_t i = 0; i < cmd.args.size(); i++)
                            // {
                                    // std::cout << cmd.args[i] << std::endl;
                            // }
                            }


                }
            }
        }
    }
}
}

