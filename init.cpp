#include "init.hpp"

void ft_init(Ircclient& irc, char **argv)
{
    irc.setport(atoi(argv[1]));
    irc.setsocket(socket(AF_INET, SOCK_STREAM,0));
    int opt = 1;
    if (setsockopt(irc.getsocket_fd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed"); // throw !!
        return ;
    }
}


void bind_and_listen(Ircclient& irc)
{
    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_port = htons(irc.getport());
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(irc.getsocket_fd(), (struct sockaddr *)&address,sizeof(address))< 0)
    {
        perror("bind failed");
        close(irc.getsocket_fd());
        return ;
    }

    if(listen(irc.getsocket_fd(), SOMAXCONN) < 0)
    {
        perror("listne failed");
        return  ;
    }

    std::cout << "serveur listening on port " << irc.getport() << "\n";
}