#include "ircCore.hpp"
#include "fileTransfer.hpp"
// #include "channel.hpp"

IrcServer::IrcServer() {
    fT = new fileTransfer();
}

Channel* IrcServer::findChannel(const std::string& cname) {
    std::map<std::string, Channel>::iterator it = _channels.find(cname);
    return (it == _channels.end()) ? 0 : &it->second;
}

//
// Channel& IrcServer::getOrCreateChannel(const std::string& cname) {
    // std::map<std::string, Channel>::iterator it = _channels.find(cname);
    // if (it != _channels.end()) return it->second;
    // Channel ch(cname);
    // _channels[cname] = ch;
    // return _channels[cname];
// }

Channel& IrcServer::getOrCreateChannel(const std::string& cname) {
    std::map<std::string, Channel>::iterator it = _channels.find(cname);
    if (it != _channels.end())
        return it->second;

    _channels.insert(std::make_pair(cname, Channel(cname)));
    return _channels.find(cname)->second;
}
Channel* IrcServer::GetChannel(const std::string& cname) 
{
    std::map<std::string, Channel>::iterator it = _channels.find(cname);
    if (it != _channels.end())
        return it->second;
}
// std::string Channel::getNamesList() const {
    // std::string list;
    // for (size_t i = 0; i < _members.size(); ++i) {
        // if (i > 0) list += " ";
        // list += _members[i]->getNick();
    // }
    // return list;
// }

void IrcServer::broadcastToChannel(const Channel& channel, const std::string& msg, IrcClient* exclude) {
    for (size_t i = 0; i < channel.GetMembers().size(); ++i) {
        IrcClient* member = channel.GetMembers()[i];
        if (member != exclude)
            member->sendMessage(msg);
    }
}


void IrcServer::handleJoin(IrcClient& client, const std::string& channelName) {
    Channel& channel = getOrCreateChannel(channelName);

    // Add the user to the channel
    channel.addMember(&client);

    // Build the JOIN message
    std::string host = client.getHost().empty() ? "localhost" : client.getHost();
    std::string joinMsg = ":" + client.getNick() + "!" + client.getUsername() + "@" + host +
                      " JOIN :" + channelName + "\r\n";


    // 1️⃣ Send JOIN message to the joining client (so HexChat sees the channel)
    client.sendMessage(joinMsg);

    // 2️⃣ Broadcast JOIN message to other users in the same channel
    broadcastToChannel(channel, joinMsg, &client);

    // 3️⃣ Send topic (optional)
    // if (!channel.getTopic().empty()) {
    //     client.sendMessage("332 " + client.getNick() + " " + channelName + " :" + channel.getTopic() + "\r\n");
    // }

    // 4️⃣ Send NAMES list
    std::string names = channel.getNamesList();
    client.sendMessage("353 " + client.getNick() + " = " + channelName + " :" + names + "\r\n");
    client.sendMessage("366 " + client.getNick() + " " + channelName + " :End of /NAMES list.\r\n");
}



IrcServer::~IrcServer() {
    delete fT;
}