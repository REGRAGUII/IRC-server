#include "ircCore.hpp"
#include "fileTransfer.hpp"


IrcClient::IrcClient() : client_fd(-1), _passAccepted(false), _registered(false), _host("localhost"){}
IrcClient::IrcClient(int fd) : client_fd(fd), _passAccepted(false), _registered(false){}
const std::string& IrcClient::getHost() const {return _host;}
int IrcClient::getClient() const {return client_fd; }
void IrcClient::Buffering(const std::string& add) { _buffer += add;}
void IrcClient::sendMessages(const std::string& msg, IrcClient*targetClient)
{

    send(targetClient->getClient(), msg.c_str(), msg.size(), 0);
}
void IrcClient::sendMessage(const std::string& msg){
    send(client_fd, msg.c_str(), msg.size(), 0);
    // (void)msg;
}
bool IrcClient::ExtractLine(std::string& line)
{
    std::string::size_type pos = _buffer.find("\r\n");
    if (pos == std::string::npos)
    {
        pos = _buffer.find("\n");
        if (pos == std::string::npos)
            return false;
    }
    line = _buffer.substr(0, pos);
    _buffer.erase(0, (pos + (_buffer[pos] == '\r' ? 2 : 1)));
    return true;
}
void IrcClient::setNick(const std::string& n) { _nick = n; }
void IrcClient::setUsername(const std::string& u) { _username = u; }
void IrcClient::setRealname(const std::string& r) { _realname = r; }
void IrcClient::setPassAccepted(bool v) { _passAccepted = v; }
void IrcClient::setRegistered(bool v) { _registered = v; }

const std::string IrcClient::getNick() const {return _nick;}
const std::string IrcClient::getUsername() const {return _username;}
const std::string IrcClient::getRealname() const {return _realname;}

bool IrcClient::hasPass() const { return _passAccepted; }
bool IrcClient::hasNick() const { return !_nick.empty();}
bool IrcClient::hasUser() const { return !_username.empty();}
bool IrcClient::isRegistered() const { return _registered; }

void IrcClient::tryAuthenticate()
{
    if(!_registered && (hasNick() && hasUser() && hasPass()))
        _registered = true;
}



IrcServer::IrcServer() { fT = new fileTransfer();}
Bot& IrcServer::getBot() { {return bot;}}
fileTransfer& IrcServer::getFileTransfer() {return *fT;}
std::string IrcServer::getpassword()const {return con_d.getpassword();}
int IrcServer::getport()const {return con_d.getport();}
ConnectionData& IrcServer::getConnectionData() {return con_d;}
int IrcServer::getsocket_fd()const {return (sock_d.getsocket_fd());}
SocketData& IrcServer::getSocketData() {return sock_d;}

void IrcServer::setpassword(std::string pwd) {con_d.setpassword(pwd);}
void IrcServer::setport(int poort) {con_d.setport(poort);}
void IrcServer::setsocket(int socket) {sock_d.setsocket_fd(socket);}


void IrcServer::broadcastToChannel(const Channel& channel, const std::string& msg, IrcClient* sender)
{
    std::vector<IrcClient *> j = channel.GetMembers();
    for (size_t i = 0; i < j.size(); i++)
    {
        IrcClient* member = j[i];
        if (member != sender)
            member->sendMessages(msg, member);
    }
}

void IrcServer::add_client(int client_fd)
{
    clients[client_fd] = IrcClient(client_fd);
    std::cout << "Client (fd = " << client_fd << ") ";
    std::cout << "Client number " << clients.size() - 1 << " is connected\n";
}
IrcClient* IrcServer::getClient(int id)
{
    std::map<int, IrcClient>::iterator it = clients.find(id);
    return it == clients.end() ? 0 : &it->second;
}
IrcClient* IrcServer::findClientByNick(const std::string nickName)
{
    std::map<int, IrcClient>::iterator it;
    for(it = clients.begin(); it != clients.end(); ++it)
    {
        if(it->second.getNick() == nickName)
        return &it->second;
    }
    return 0;
}
void IrcServer::remove_client(int client_fd)
{
    std::map<int, IrcClient>::iterator it = clients.find(client_fd);
    if (it != clients.end()) 
    {
        if (client_fd >= 0) {
            ::shutdown(client_fd, SHUT_RDWR);
            ::close(client_fd);
        }
        clients.erase(it);
    }
}
bool IrcServer::isNickTaken(std::string nick) const
{
    std::map<int, IrcClient>::const_iterator it = clients.begin();
    for(; it != clients.end(); it++)
    {
        if (it->second.getNick() == nick) return true;
    }
    return false;
}
void IrcServer::sendToClient(IrcClient& client, const std::string& raw)
{
    const int fd = client.getClient();
    if (fd < 0) return;
    ssize_t n = ::send(fd, raw.data(), raw.size(), 0);
    if (n < 0) {throw std::runtime_error("send() failed");}
}

Channel* IrcServer::findChannel(const std::string& cname)
{
    std::map<std::string, Channel>::iterator it = _channels.find(cname);
    return (it == _channels.end()) ? 0 : &it->second;
}
Channel& IrcServer::getOrCreateChannel(const std::string& cname, std::string pass, IrcClient &client)
{
    std::map<std::string, Channel>::iterator it = _channels.find(cname);
    if (it != _channels.end())
    {
        return it->second;
    }

    _channels.insert(std::make_pair(cname, Channel(cname)));
    _channels.find(cname)->second.setKey(pass);
    _channels.find(cname)->second.addOperator(&client);
    return _channels.find(cname)->second;
}
Channel* IrcServer::GetChannel(const std::string& cname)
{
    std::map<std::string, Channel>::iterator it = _channels.find(cname);
    if (it != _channels.end())
        return &it->second;
    return NULL;
}
std::map<std::string, Channel>& IrcServer::channels() { return _channels; };

void IrcServer::handleUser(IrcClient& client, const std::vector<std::string>& args)
{
    if (client.hasUser())
    {
        sendToClient(client, ":ircserv 462 " + client.getNick() +
        " :You may not reregister\r\n");
        return;
    }
    if (args.size() < 4)
    {
        sendToClient(client, ":ircserv 461 " + client.getNick() +
        " USER :Not enough parameters\r\n");
        return;
    }
    std::string username = args[0];
    std::string realname = args[3];
    client.setUsername(username);
    client.setRealname(realname);
    client.tryAuthenticate();
}
void IrcServer::handlePass(IrcClient& client, const std::vector<std::string>& args)
{
    if (client.isRegistered())
    {
        sendToClient(client, ":ircserv 462 " + client.getNick() +
            " :You may not reregister\r\n");
        return;
    }
    if (args[0].empty())
    {
        sendToClient(client, ":ircserv 461 * PASS :Not enough parameters\r\n");
        return;
    }
    if (args[0] == getpassword())
        client.setPassAccepted(true);
    else
        sendToClient(client, ":ircserv 464 * :Password incorrect\r\n");
    client.tryAuthenticate();
}
void IrcServer::handleNick(IrcClient& client, const std::vector<std::string>& args)
{
    if (args.empty())
    {
        sendToClient(client, ":ircserv 431 * :No nickname given\r\n");
        return;
    }
    std::string nick = args[0];
    if (isNickTaken(nick))
    {
        sendToClient(client, ":ircserv 433 * " + nick + " :Nickname is already in use\r\n");
        return;
    }
    client.setNick(nick);
    client.tryAuthenticate();
}

void IrcServer::handlePrivmsg(IrcClient& client, const std::vector<std::string>& args)
{
    if(args.size() < 2)
    {
        std::string err = "Usage: PRIVMSG <recipient> <message>\n";
        send(client.getClient(), err.c_str(), err.size(), 0);
        return;
    }
    std::string targetNick = args[0];
    std::string message;
    for(std::vector<std::string>::const_iterator it = args.begin(); it < args.end(); ++it)
        message += *it + " ";
    if(targetNick[0] == '#')
    {
        Channel *ch =  GetChannel(targetNick);
        std::vector<IrcClient *> j = ch->GetMembers();
        bool joined = false;
        for (size_t i = 0; i < j.size(); i++) {
            if (j[i]->getClient() == client.getClient())
            {
                joined = true;
                break;
            }
        }
        if (!joined)
            return;
        std::string allArgs;
        for(std::vector<std::string>::const_iterator it = args.begin(); it < args.end(); it++)
        {
            allArgs += *it + " ";
        }
        std::string msg = ":" + client.getNick() + "!" + client.getUsername() + "@1337.ma PRIVMSG " + targetNick + " :"+ allArgs + "\r\n";
        broadcastToChannel(*GetChannel(targetNick), msg, &client);
        return;
    }
    IrcClient *targetClient = findClientByNick(targetNick);
    if(!targetClient || client.getClient() < 0)
    {
        std::string err = "Error: User not found\n";
        send(client.getClient(), err.c_str(), err.size(), 0);
        return;
    }
    std::ostringstream oss;
    oss << ":" << client.getNick() << " PRIVMSG " << targetNick << " :" << message << "\r\n";
    send(targetClient->getClient(), oss.str().c_str(), oss.str().size(), 0);
}

void IrcServer::handelTopic(IrcClient& client, const std::vector<std::string>& args)
{
    if(args.empty())
    {
        client.sendMessage(" 461 " + client.getNick() + " TOPIC : not enogh parameter\r\n");
        return;
    }
    const std::string channelName = args[0];
    // check if channel exist
    Channel *channel = GetChannel(channelName);
    if(!channel)
    {
        client.sendMessage(" 403 " + client.getNick()  + " " + channelName + " : no such channel\r\n");
        return;
    }
    // check if client is in the channel
    if(!channel->isMember(&client))
    {
        client.sendMessage(" 442 " + client.getNick()  + " " + channelName + " : you are not in this channel\r\n");
        return;
    }
    // if there is no topic show cuurent topic
    if(args.size() ==  1)
    {
        if(channel->GetChannelTopic().empty())
            client.sendMessage(" 331 " + client.getNick()  + " " + channelName + " : not topic is set\r\n");
            else
            client.sendMessage(" 331 " + client.getNick()  + " " + channelName + " :" + channel->GetChannelTopic() + "\r\n");
            return;
    }
    // setting new topic
    std::string newTopic = args[1];
    // check if topic restrected to operators
    if(channel->isTopicRestrected() && !channel->isOperator(&client))
    {
        client.sendMessage(" 482 " + client.getNick()  + " " + channelName + " : you are not channel operator\r\n");
        return;
    }
    // set the topic
    channel->SetChannelTopic(newTopic);
    // broadcast topic change to channel
    std::string host = client.getHost().empty() ? "localhost" : client.getHost();
    std::string topicMsg = ":" + client.getNick() + "!" + client.getUsername() + "@" + host + " TOPIC " + channelName + " :" + newTopic + "\r\n";
    broadcastToChannel(*channel, topicMsg, NULL);
}
void IrcServer::handleJoin(IrcClient& client, const std::vector<std::string>& args)
{
    if (args.empty()) {
        client.sendMessage("461 " + client.getNick() + " JOIN :Not enough parameters\r\n");
        return;
    }
    if (args[0].empty() ||  args[0][0] != '#')
    {
        client.sendMessage("461 " + client.getNick() + " JOIN :Not a valid channel name \r\n");
        return;
    }
    std::string channelName = args[0];
    std::string password = (args.size() > 1) ? args[1] : "";

    // get or create channel
    Channel& channel = getOrCreateChannel(channelName, password, client);

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
    // :Nick!user@host JOIN :#channel
    std::string joinMsg = ":" + client.getNick() + "!" + client.getUsername() + "@1337.ma JOIN " + args[0] + "\r\n";
    // send JOIN to the joining client
    client.sendMessage(joinMsg);
    // send topic if set
    if (!channel.GetChannelTopic().empty())
    client.sendMessage("332 " + client.getNick() + " " + channelName + " :" + channel.GetChannelTopic() + "\r\n");
    // Ssnd NAMES list
    std::vector<IrcClient *> j = channel.GetMembers();
    std::string names = channel.getNamesList();
    for (size_t i = 0; i < j.size(); i++) {
        j[i]->sendMessage(":irc.1337.ma 353 " + j[i]->getNick() + " = " + channelName + " :" + names + "\r\n");
        j[i]->sendMessage(":irc.1337.ma 366 " + j[i]->getNick() + " " + channelName + " :End of /NAMES list.\r\n");
    }
}

void IrcServer::handelInvite(IrcClient& client, const std::vector<std::string>& args)
{
    // INVITE nickname #channel
    if(args.size() < 2)
    {
        client.sendMessage("461 " + client.getNick() + " INVITE :not enough parameters\r\n");
        return;
    }
    std::string targetNick = args[0];
    const std::string channelName = args[1];

    Channel* channel = GetChannel(channelName);
    if(!channel)
    {
        client.sendMessage("403 " + client.getNick() + " " + channelName + " :no such channel\r\n");
        return;
    }
    if(!channel->isMember(&client))
    {
        client.sendMessage("442 " + client.getNick() + " " + channelName + " :you are not on that channel\r\n");
        return;
    }
    // check if client is operator (required for invite-only channels)
    if(channel->isInviteOnly() && !channel->isOperator(&client))
    {
        client.sendMessage("482 " + client.getNick() + " " + channelName + " :you are not channel operator\r\n");
        return;
    }
    IrcClient *target = findClientByNick(targetNick);
    if(!target)
    {
        client.sendMessage("401 " + client.getNick() + " " + targetNick + " :no such nick\r\n");
        return;
    }
    if(channel->isMember(target))
    {
        client.sendMessage("443 " + client.getNick() + " " + targetNick + " " + channelName + " :is already on channel\r\n");
        return;
    }
    // add target to invite list
    channel->addInvite(target);
    // send invite
    std::string host = client.getHost().empty() ? "localhost" : client.getHost();
    std::string inviteMsg = ":" + client.getNick() + "!" + client.getUsername() + "@" + host + " INVITE " + targetNick + " :" + channelName + "\r\n";
    target->sendMessage(inviteMsg);
    // confirm
    client.sendMessage("341 " + client.getNick() + " " + targetNick + " " + channelName + "\r\n");

}

void IrcServer::handelKick(IrcClient& client, const std::vector<std::string>& args)
{
    // KICK #channel nickname [reason]
    if(args.size() < 2)
    {
        client.sendMessage("461 " + client.getNick() + " KICK :not enough parameters\r\n");
        return;
    }
     std::string targetNick = args[2];
    const std::string channelName = args[1];
    std::string reason = (args.size() > 2) ? args[2] : client.getNick();

    Channel* channel = GetChannel(channelName);
    if(!channel)
    {
        client.sendMessage("403 " + client.getNick() + " " + channelName + " :no such channel\r\n");
        return;
    }
    if(!channel->isMember(&client))
    {
        client.sendMessage("442 " + client.getNick() + " " + channelName + " :you are not on that channel\r\n");
        return;
    }
    if(!channel->isOperator(&client))
    {
        client.sendMessage("482 " + client.getNick() + " " + channelName + " :you are not channel operator\r\n");
        return;
    }
    // find target
    IrcClient* target = findClientByNick(args[2]);
    if(!target)
    {
            client.sendMessage("401 " + client.getNick() + " " + targetNick + " :no such nick\r\n");
            return;
    }
    if (!channel->isMember(target))
    {
        client.sendMessage("441 " + client.getNick() + " " + targetNick + " " + channelName + " :they are not on that channel\r\n");
        return;
    }
    std::string host =  "1337.ma";
    std::string kickMsg = ":" + client.getNick() + "!" + client.getUsername() + "@" + host + " KICK " + channelName + " " + targetNick  + "\r\n";
    broadcastToChannel(*channel, kickMsg, NULL);
    channel->removeMember(target);

}
void IrcServer::handleModes(IrcClient& client, const std::vector<std::string>& args)
{
    // MODE #channel +/-mode [param]
    if (args.size() < 2)
    {
        client.sendMessage("461 " + client.getNick() + " MODE :Not enough parameters\r\n");
        return;
    }
    std::string channelName = args[0];
    std::string modeString = args[1];
    // check if channel exists
    Channel* channel = GetChannel(channelName);
    if (!channel)
    {
        client.sendMessage("403 " + client.getNick() + " " + channelName + " :no such channel\r\n");
        return;
    }
     // check if client in the channel
    if (!channel->isMember(&client))
    {
        client.sendMessage("442 " + client.getNick() + " " + channelName + " :you are not on that channel\r\n");
        return;
    }
    // check if client is operator
    if (!channel->isOperator(&client))
    {
        client.sendMessage("482 " + client.getNick() + " " + channelName + " :you are not channel operator\r\n");
        return;
    }
     // parse modes
    bool adding = true;
    std::string appliedModes;
    std::string modeParams;
    size_t argIndex = 2;
    std::cout <<  "mode string -> " << modeString << std::endl;
    for(size_t i = 0; i < modeString.length(); i++)
    {
        char mode = modeString[i];
        if(mode == '+')
            adding = true;
        else if(mode == '-')
            adding = false;
        else if(mode == 'i')
            handleMode_i(channel, adding, appliedModes);
        else if(mode == 't')
            handleMode_t(channel, adding, appliedModes);
        else if(mode == 'k')
        {
            if (!handleMode_k(channel, client, adding, appliedModes, modeParams, args, argIndex))
            continue;
        }
        else if(mode == 'o')
        {
            if (!handleMode_o(channel, client, adding, appliedModes, modeParams, args, argIndex))
            continue;
        }
        else if(mode == 'l')
        {
            if (!handleMode_l(channel, client, adding, appliedModes, modeParams, args, argIndex))
            continue;
        }
        else
        {
            std::string unknownMode(1, mode);
            client.sendMessage("472 " + client.getNick() + " " + unknownMode + " :is unknown mode char to me\r\n");
        }
    }
    // broadcast mode change to channel if any modes were applied
    if (!appliedModes.empty())
    {
        std::string host = client.getHost().empty() ? "localhost" : client.getHost();
        std::string modeMsg = ":" + client.getNick() + "!" + client.getUsername() + "@" + host + " MODE " + channelName + " " + appliedModes + modeParams + "\r\n";
        broadcastToChannel(*channel, modeMsg, NULL);
    }
}
void IrcServer::handleMode_i(Channel* channel, bool adding, std::string& appliedModes)
{
    channel->setInviteOnly(adding);
    appliedModes += adding ? "+i" : "-i";
}
void IrcServer::handleMode_t(Channel* channel, bool adding, std::string& appliedModes)
{
    appliedModes += adding ? "+t" : "-t";
    channel->setTopicRestrected(adding);
    std::cout << "adding is " << adding << std::endl;
}
bool IrcServer::handleMode_k(Channel* channel, IrcClient& client, bool adding, std::string& appliedModes, std::string& modeParams, const std::vector<std::string>& args, size_t& argIndex)
{
    if (adding)
    {
        if (argIndex < args.size())
        {
            channel->setKey(args[argIndex]);
            appliedModes += "+k";
            modeParams += " " + args[argIndex];
            argIndex++;
            return true;
        }
        else
        {
            client.sendMessage("461 " + client.getNick() + " MODE +k :not enough parameters\r\n");
            return false;
        }
    }
    else
    {
        channel->setKey("");
        appliedModes += "-k";
        return true;
    }
}
bool IrcServer::handleMode_o(Channel* channel, IrcClient& client, bool adding, std::string& appliedModes, std::string& modeParams, const std::vector<std::string>& args, size_t& argIndex)
{
    if (argIndex < args.size())
    {
        std::string targetNick = args[argIndex];
        IrcClient* target = findClientByNick(targetNick);
        if (!target)
        {
            client.sendMessage("401 " + client.getNick() + " " + targetNick + " :no such nick\r\n");
            argIndex++;
            return false;
        }
        if (!channel->isMember(target))
        {
            client.sendMessage("441 " + client.getNick() + " " + targetNick + " " + channel->GetChannelName() + " :they aren't on that channel\r\n");
            argIndex++;
            return false;
        }
        if (adding)
        {
            channel->addOperator(target);
            appliedModes += "+o";
        }
        else
        {
            channel->removeOperator(target);
            appliedModes += "-o";
        }
        modeParams += " " + targetNick;
        argIndex++;
        return true;
    }
    else
    {
        client.sendMessage("461 " + client.getNick() + " MODE +o :not enough parameters\r\n");
        return false;
    }
}
bool IrcServer::handleMode_l(Channel* channel, IrcClient& client, bool adding, std::string& appliedModes, std::string& modeParams, const std::vector<std::string>& args, size_t& argIndex)
{
    if (adding)
    {
        if (argIndex < args.size())
        {
            int limit = atoi(args[argIndex].c_str());
            if (limit > 0)
            {
                channel->setUserLimit(limit);
                appliedModes += "+l";
                modeParams += " " + args[argIndex];
                argIndex++;
                return true;
            }
            else
            {
                client.sendMessage("461 " + client.getNick() + " MODE +l :Invalid limit\r\n");
                argIndex++;
                return false;
            }
        }
        else
        {
            client.sendMessage("461 " + client.getNick() + " MODE +l :Not enough parameters\r\n");
            return false;
        }
    }
    else
    {
        channel->setUserLimit(0);
        appliedModes += "-l";
        return true;
    }
}
void IrcServer::removeClientFromAllChannels(IrcClient *client)
{
    std::map<std::string, Channel>::iterator it;
    for(it = _channels.begin(); it != _channels.end(); ++it)
    {
        Channel& channel = it->second;
        if(channel.isMember(client))
        {
            std::string msg = ":" + client->getNick() + " QUIT :Client disconnected\r\n";
            broadcastToChannel(channel, msg, NULL);
            channel.removeMember(client);
        }
    }
}
IrcServer::~IrcServer() { delete fT; }

void IrcServer::handleQuit(IrcClient& client, const std::vector<std::string>& args)
{
    std::string reason = "Client has quit";
    if (!args.empty() && !args[0].empty())
        reason = args[0];

    std::string quitMsg = ":" + client.getNick() + " QUIT :" + reason + "\r\n";

    std::vector<Channel*> joined;
    std::map<std::string, Channel>::iterator it;
    for (it = _channels.begin(); it != _channels.end(); ++it)
    {
        Channel &ch = it->second;
        if (ch.isMember(&client))
            joined.push_back(&ch);
    }

    for (size_t i = 0; i < joined.size(); ++i)
    {
        Channel *ch = joined[i];
        if (ch)
        {
            broadcastToChannel(*ch, quitMsg, &client);
            ch->removeMember(&client);
        }
    }

    int fd = client.getClient();
    remove_client(fd);
}
