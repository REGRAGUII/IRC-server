#include "init.hpp"

int main(int argc, char **argv)
{
    try{
        if (argc != 3)
            throw std::runtime_error("Invalid number of arguments!");
        IrcServer irc;
        irc.setpassword(argv[2]);
        ft_init(irc, argv);
        bind_and_listen_accept(irc);
        run_server_loop(irc);
    }
    catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
