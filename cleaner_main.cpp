#include "init.hpp"


int main(int argc, char **argv)
{
    try{
        if (argc != 3)
            throw std::runtime_error("Invalid number of arguments!");
        IrcServer irc;
        ft_init(irc, argv);
        bind_and_listen_accept(irc);
    
    
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(irc.getsocket_fd(), (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
            throw std::runtime_error("accept failed");
        close(irc.getsocket_fd());
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
