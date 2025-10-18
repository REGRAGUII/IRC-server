#include "init.hpp"



void handlePrivmsg(IrcServer& server, IrcClient& client, const std::vector<std::string>& args)
{
    if(args.size() < 2)
    {
        std::string err = "Usage: PRIVMSG <recipient> <message>\n";
        send(client.getClient(), err.c_str(), err.size(), 0);
        return;
    }

    std::string targetNick = args[0];
    std::string message = args[1];

    IrcClient *targetClient = server.findClientByNick(targetNick);
    if(!targetClient)
    {
        std::string err = "Error: User not found\n";
        send(client.getClient(), err.c_str(), err.size(), 0);
        return;
    }

    // forward the message (for DCC or normal chat)
    std::ostringstream oss;
    oss << ":" << client.getNick() << " PRIVMSG " << targetNick << " :" << message << "\r\n";
    send(targetClient->getClient(), oss.str().c_str(), oss.str().size(), 0);
}


