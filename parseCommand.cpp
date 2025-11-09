#include "init.hpp"

cmd ft_parse(const std::string& msg)
{
    cmd cmd;

    std::string command = std::string(msg);

    if(command.size() >= 2 && command.substr(command.size() - 2) == "\r\n")
        command = command.substr(0, command.size() - 2);

    std::istringstream ss(command);
    std::string tk;

    if(!command.empty() && command[0] == '/'){
        ss >> tk;
        cmd.prefix = tk.substr(1);
    }
    if(!(ss >> cmd.c))
        return cmd;
    while(ss >> tk)
    {
        if (tk[0] == ':'){
            std::string trail = tk.substr(1);
            std::string rest;
            std::getline(ss, rest);

            trail +=rest;
            cmd.args.push_back(trail);
            break;
        }
        else
        {
            cmd.args.push_back(tk);
        }
    }
    return cmd;
}

void HandleCommand(IrcClient &client, const cmd &command, IrcServer &irc, Bot &bot, fileTransfer &fT)
{
    if(command.c== "NICK")
        irc.handleNick(client, command.args);
    else if (command.c == "PASS")
        irc.handlePass(client, command.args);
    else if (command.c == "USER")
        irc.handleUser(client, command.args);
    else if (client.isRegistered() && command.c == "PRIVMSG")
        irc.handlePrivmsg(client, command.args);
    else if(client.isRegistered() && command.c == "KICK")
        irc.handelKick(client, command.args);
    else if(client.isRegistered() && command.c == "INVITE")
        irc.handelInvite(client, command.args);
    else if(client.isRegistered() &&  command.c == "TOPIC")
        irc.handelTopic(client, command.args);
    else if(client.isRegistered() && command.c == "MODE")
        irc.handleModes(client, command.args);
    else if (client.isRegistered() && command.c == "JOIN")
        irc.handleJoin( client, command.args);
    else if(client.isRegistered() && command.c == "BOT")
        bot.handelBot(irc, client, command.args);
    else if(client.isRegistered() && command.c == "/send")
        fT.handelSend(irc, client,command.args);
    else if (!client.isRegistered())
        irc.sendToClient(client, "You are  Not Authentificated : " + command.c +"\n");
    else
        irc.sendToClient(client, "Unknow Command : " + command.c +"\n");
}

