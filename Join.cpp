#include "ircCore.hpp"

void handleJoin(IrcServer& server, IrcClient& client, const std::vector<std::string>& args){
    if(client.isRegistered())
        server.sendToClient();
}
