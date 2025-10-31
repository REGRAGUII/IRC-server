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
        return &it->second;
    return NULL;
}

void IrcServer::broadcastToChannel(const Channel& channel, const std::string& msg, IrcClient* exclude) {
    for (size_t i = 0; i < channel.GetMembers().size(); ++i) {
        IrcClient* member = channel.GetMembers()[i];
        if (member != exclude)
            member->sendMessage(msg);
    }
}


// void IrcServer::handleJoin(IrcClient& client, const std::string& channelName) {
//     Channel& channel = getOrCreateChannel(channelName);

//     // Add the user to the channel
//     channel.addMember(&client);

//     // Build the JOIN message
//     std::string host = client.getHost().empty() ? "localhost" : client.getHost();
//     std::string joinMsg = ":" + client.getNick() + "!" + client.getUsername() + "@" + host +
//                       " JOIN :" + channelName + "\r\n";


//     // 1️⃣ Send JOIN message to the joining client (so HexChat sees the channel)
//     client.sendMessage(joinMsg);

//     // 2️⃣ Broadcast JOIN message to other users in the same channel
//     broadcastToChannel(channel, joinMsg, &client);

//     // 3️⃣ Send topic (optional)
//     // if (!channel.getTopic().empty()) {
//     //     client.sendMessage("332 " + client.getNick() + " " + channelName + " :" + channel.getTopic() + "\r\n");
//     // }

//     // 4️⃣ Send NAMES list
//     std::string names = channel.getNamesList();
//     client.sendMessage("353 " + client.getNick() + " = " + channelName + " :" + names + "\r\n");
//     client.sendMessage("366 " + client.getNick() + " " + channelName + " :End of /NAMES list.\r\n");
// }

void IrcServer::handleJoin(IrcClient& client, const std::vector<std::string>& args) {
    if (args.empty()) {
        client.sendMessage("461 " + client.getNick() + " JOIN :Not enough parameters\r\n");
        return;
    }

    std::string channelName = args[0];
    std::string password = (args.size() > 1) ? args[1] : "";

    // get or create channel
    Channel& channel = getOrCreateChannel(channelName);
    // check if already in channel
    if (channel.isMember(&client))
        return;  // Already in channel
    // check invite-only mode
    if (channel.isInviteOnly() && !channel.isInvited(&client))
    {
        client.sendMessage("473 " + client.getNick() + " " + channelName + " :Cannot join channel (+i)\r\n");
        return;
    }
    // check password
    if (channel.GetChannelKey() != password)
    {
        client.sendMessage("475 " + client.getNick() + " " + channelName + " :Cannot join channel (+k) - bad key\r\n");
        return;
    }
    // check user limit
    if (channel.GetUserLimit() && (int)channel.GetMembers().size() >= channel.GetUserLimit())
    {
        client.sendMessage("471 " + client.getNick() + " " + channelName + " :Cannot join channel (+l)\r\n");
        return;
    }
    // add user to channel
    channel.addMember(&client);
    // remove from invite list if was invited
    if (channel.isInvited(&client))
        channel.removeInvite(&client);
    // build JOIN message
    std::string host = client.getHost().empty() ? "localhost" : client.getHost();
    std::string joinMsg = ":" + client.getNick() + "!" + client.getUsername() + "@" + host + " JOIN :" + channelName + "\r\n";
    // send JOIN to the joining client
    client.sendMessage(joinMsg);
    // broadcast JOIN to other users in channel
    broadcastToChannel(channel, joinMsg, &client);
    // send topic if set
    if (!channel.GetChannelTopic().empty())
        client.sendMessage("332 " + client.getNick() + " " + channelName + " :" + channel.GetChannelTopic() + "\r\n");
    // Ssnd NAMES list
    std::string names = channel.getNamesList();
    client.sendMessage("353 " + client.getNick() + " = " + channelName + " :" + names + "\r\n");
    client.sendMessage("366 " + client.getNick() + " " + channelName + " :End of /NAMES list.\r\n");
}



IrcServer::~IrcServer() {
    delete fT;
}