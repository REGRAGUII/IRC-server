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

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(irc.getsocket_fd(), (struct sockaddr *)&client_addr, &client_len);
    irc.add_client(client_fd);
    if (client_fd < 0)
    {
        throw std::runtime_error("accept failed");
        return ;
    }
}