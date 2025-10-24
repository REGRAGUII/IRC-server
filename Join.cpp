#include "ircCore.hpp"

// void handleJoin(IrcServer& server, IrcClient& client, const std::vector<std::string>& args){
//     (void)client;
//     // (void)server;
//     // (void)args;
//     if(args[0][0] == '#')
//     {
//         Channel& ch = server.getOrCreateChannel(args[0]);
//         ch.addMember(client.getClient());
//         server.sendToClient(client,"welcome to " + args[0] + " CHANNEL\n");
//     }
//     else
//         server.sendToClient(client,"Error : invalid argument for channels " + args[0]);
// }


// void IrcServer::handleJoin(Client& client, const std::string& channelName) {
//     Channel& channel = getOrCreateChannel(channelName);

//     // Add the user to the channel
//     channel.addMember(&client);

//     // Inform the client and others that he joined
//     std::string joinMsg = ":" + client.getFullName() + " JOIN :" + channelName + "\r\n";
//     broadcastToChannel(channel, joinMsg);

//     // Send topic (if exists)
//     if (!channel.getTopic().empty()) {
//         client.sendMsg("332 " + client.getNickname() + " " + channelName + " :" + channel.getTopic() + "\r\n");
//     }

//     // Send NAMES list
//     std::string names = channel.getNamesList();
//     client.sendMsg("353 " + client.getNickname() + " = " + channelName + " :" + names + "\r\n");
//     client.sendMsg("366 " + client.getNickname() + " " + channelName + " :End of /NAMES list.\r\n");
// }
