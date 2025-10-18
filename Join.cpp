#include "ircCore.hpp"

void handleJoin(IrcServer& server, IrcClient& client, const std::vector<std::string>& args){
    (void)client;
    // (void)server;
    // (void)args;
    Channel& ch = server.getOrCreateChannel(args[0]);
    ch.addMember(client.getClient());
    server.sendToClient(client,"welcome to " + args[0] + " server\n");
}
