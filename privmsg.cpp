#include "init.hpp"



void handlePrivmsg(IrcServer& server, IrcClient& client, const std::vector<std::string>& args)
{
    if (args.size() != 2)
    {
        std::cout << "Error: privmsg must have 2 arguments" << std::endl;
        return;
    }

    const std::string& targetNick = args[0];
    const std::string& message = args[1];
    
    IrcClient* target = server.findClientByNick(targetNick);
    std::cout << "targer = " << target << "\n" << std::endl;
    if (!target)
    {
        client.sendMessage("No such nick\n");
        return;
    }

    std::string msg =  " " + client.getNick() + " PRIVMSG " + targetNick + " :" + message + "\r\n";
    target->sendMessage(msg);
}

