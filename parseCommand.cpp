#include "init.hpp"

//PASS <password>
void handlePass(IrcServer& server, IrcClient& client, const std::vector<std::string>& args) 
{
    if (client.isRegistered()) {
        server.sendToClient(client, ":ircserv 462 " + client.getNick() +
            " :You may not reregister\r\n");
        return;
    }
    if (args[0].empty()) {
        server.sendToClient(client, ":ircserv 461 * PASS :Not enough parameters\r\n");
        return;
    }
    std::cout << "argument :" << args[0] << "\n" << "server :" << server.getpassword() << "\n";
    if (args[0] == server.getpassword()) {
        client.setPassAccepted(true);
    } else {
        server.sendToClient(client, ":ircserv 464 * :Password incorrect\r\n");
    }
    client.tryAuthenticate();
}


// NICK <nickname>
void handleNick(IrcServer& server, IrcClient& client, const std::vector<std::string>& args) {    
    if (args.empty()) {
        server.sendToClient(client, ":ircserv 431 * :No nickname given\r\n"); 
        return;
    }
    std::string nick = args[0];
    // std::cout << "nick : " << nick << "\n" << "argument: " << args[0] << "\n";
    if (server.isNickTaken(nick)) {
        server.sendToClient(client, ":ircserv 433 * " + nick + " :Nickname is already in use\r\n");
        return;
    }
    client.setNick(nick);
    client.tryAuthenticate();
}

// // USER <username> <mode> <unused> :<realname>
void handleUser(IrcServer& server, IrcClient& client, const std::vector<std::string>& args) 
{
    if (client.hasUser()) {
        server.sendToClient(client, ":ircserv 462 " + client.getNick() +
            " :You may not reregister\r\n");
        return;
    }
    if (args.size() < 4) {
        server.sendToClient(client, ":ircserv 461 " + client.getNick() +
            " USER :Not enough parameters\r\n");
        return;
    }
    std::string username = args[0];
    std::string realname = args[3];
    client.setUsername(username);
    client.setRealname(realname);
    client.tryAuthenticate();
}




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

void HandleCommand(IrcClient &client, const cmd &command, IrcServer &irc, Bot &bot, fileTransfer &fT){
    if(command.c== "NICK")
        handleNick(irc, client, command.args);
    else if (command.c == "PASS")
        handlePass(irc, client, command.args);
    else if (command.c == "USERNAME")
        handleUser(irc, client, command.args);
    else if (command.c == "JOIN")
        handleJoin(irc, client, command.args);
    else if(command.c == "BOT")
        bot.handelBotCommnads(irc, client, command.args);
    else if(fT.isFileTransferCmd(command.c))
        fT.handelfileTransferCmd(irc, client,command);
    else
        irc.sendToClient(client, "Unknow Command : " + command.c +"\n");
        
}
