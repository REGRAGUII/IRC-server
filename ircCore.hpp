#ifndef IRC_CORE_HPP
#define IRC_CORE_HPP


#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <stdexcept>
#include <poll.h>
#include <sstream>
#include <map>
#include "bot.hpp"
#include "channel.hpp"

class fileTransfer;
struct cmd{

    std::string prefix;
    std::string c;
    std::vector<std::string> args;
};


class SocketData {
    private:
        int socket_fd;
    public:
        SocketData() : socket_fd(-1){}
        int getsocket_fd()const{return socket_fd;}
        void setsocket_fd(int socket){socket_fd = socket;}

};

class ConnectionData {
    private :
        int prt;
        std::string password;
    public:
        ConnectionData() : prt(0){}
        std::string getpassword()const{return password;}
        int getport()const {return (prt);}

        void setpassword(std::string pwd){password = pwd;}
        void setport(int poort){prt = poort;}


};

class IrcClient {
private:
    int client_fd;
    std::string _buffer;
    std::string _nick;
    std::string _username;
    std::string _realname;
    bool _passAccepted;
    bool _registered;
    std::string _host;

public:
    IrcClient() : client_fd(-1), _passAccepted(false), _registered(false), _host("localhost"){}
    IrcClient(int fd) :client_fd(fd), _passAccepted(false), _registered(false){}
    const std::string& getHost() const {return _host;}
    int getClient() const { return client_fd; }
    void Buffering(const std::string& add){ _buffer += add;}
    void sendMessage(const std::string& msg){
        send(client_fd, msg.c_str(), msg.size(), 0);
    }
    bool ExtractLine(std::string& line) {
    std::string::size_type pos = _buffer.find("\r\n");
    if (pos == std::string::npos) {
        pos = _buffer.find("\n");  // fallback for netcat/manual testing
        if (pos == std::string::npos)
            return false;
    }
    line = _buffer.substr(0, pos);
    _buffer.erase(0, (pos + (_buffer[pos] == '\r' ? 2 : 1)));
    return true;
}

    void setNick(const std::string& n) { _nick = n; }
    void setUsername(const std::string& u) { _username = u; }
    void setRealname(const std::string& r) { _realname = r; }
    void setPassAccepted(bool v) { _passAccepted = v; }
    void setRegistered(bool v) { _registered = v; }

    const std::string getNick() const {return _nick;}
    const std::string getUsername() const {return _username;}
    const std::string getRealname() const {return _realname;}
    bool hasPass() const { return _passAccepted; }
    bool hasNick() const { return !_nick.empty();}
    bool hasUser() const { return !_username.empty();}

    bool isRegistered() const { return _registered; }
    void tryAuthenticate(){
        if(!_registered && (hasNick() && hasUser() && hasPass())){
            // sendMessage("Welcome to IRC Server ");
            // std::cout << "Welcome to IRC Server " << getUsername() << "\n";
            _registered = true;};
        }

};


class IrcServer {
    private:
        ConnectionData  con_d;
        SocketData      sock_d;
        std::map <int, IrcClient> clients;
        std::map<std::string, Channel> _channels;
        Bot bot;
        fileTransfer *fT;
    public:
            // ******      Connection Data      ******
        IrcServer();
        Bot& getBot() {return bot;}
        void broadcastToChannel(const Channel& channel, const std::string& msg, IrcClient* exclude);
        void handleJoin(IrcClient& client, const std::vector<std::string>& args);
        fileTransfer& getFileTransfer() {return *fT;}
        std::string getpassword()const{return con_d.getpassword();}
        int getport()const {return con_d.getport();}
        void setpassword(std::string pwd) {con_d.setpassword(pwd);}
        void setport(int poort) {con_d.setport(poort);}
        ConnectionData& getConnectionData(){ return con_d; }
            // ******        Socket Data        ******
        int getsocket_fd()const {return (sock_d.getsocket_fd());}
        SocketData& getSocketData() {return sock_d;}
        void setsocket(int socket){sock_d.setsocket_fd(socket);}
            // ******        Clients Data       ******
        void add_client(int client_fd)
        {
            clients[client_fd] = IrcClient(client_fd);
            std::cout << "Client (fd = " << client_fd << ") ";
            std::cout << "Client number " << clients.size() - 1 << " is connected\n";
        }

         IrcClient* getClient(int id) {
        std::map<int, IrcClient>::iterator it = clients.find(id);
        return it == clients.end() ? 0 : &it->second;
        }

        IrcClient *findClientByNick(const std::string nickName)
        {
            std::map<int, IrcClient>::iterator it;
            for(it = clients.begin(); it != clients.end(); ++it)
            {
                if(it->second.getNick() == nickName)
                    return &it->second;
            }
            return 0;
        }

        void remove_client(int client_fd) {
            std::map<int, IrcClient>::iterator it = clients.find(client_fd);
            if (it != clients.end()) clients.erase(it);
        }

        bool isNickTaken(std::string nick) const {
            std::map<int, IrcClient>::const_iterator it = clients.begin();
            for(; it != clients.end(); it++)
            {
                if (it->second.getNick() == nick) return true;
            }
            return false;
        }
        void sendToClient(IrcClient& client, const std::string& raw)
        {
            const int fd = client.getClient();
            if (fd < 0) return;
            ssize_t n = ::send(fd, raw.data(), raw.size(), 0);
            if (n < 0) {throw std::runtime_error("send() failed");}
        }

        //channel
        Channel* findChannel(const std::string& cname);
        Channel& getOrCreateChannel(const std::string& cname);
        Channel* GetChannel(const std::string& cname);
        std::map<std::string, Channel>& channels() { return _channels; };

        // commandes handler
        void handlePass(IrcClient& client, const std::vector<std::string>& args)
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
            std::cout << "argument :" << args[0] << "\n" << "server :" << getpassword() << "\n";
            if (args[0] == getpassword())
                client.setPassAccepted(true);
            else
                sendToClient(client, ":ircserv 464 * :Password incorrect\r\n");
            client.tryAuthenticate();
        }

        void handleNick(IrcClient& client, const std::vector<std::string>& args)
        {
            if (args.empty())
            {
                sendToClient(client, ":ircserv 431 * :No nickname given\r\n");
                return;
            }
            std::string nick = args[0];
            // std::cout << "nick : " << nick << "\n" << "argument: " << args[0] << "\n";
            if (isNickTaken(nick))
            {
                sendToClient(client, ":ircserv 433 * " + nick + " :Nickname is already in use\r\n");
                return;
            }
            client.setNick(nick);
            client.tryAuthenticate();
        }

        void handleUser(IrcClient& client, const std::vector<std::string>& args)
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

        void handlePrivmsg(IrcClient& client, const std::vector<std::string>& args)
        {
            if(args.size() < 2)
            {
                std::string err = "Usage: PRIVMSG <recipient> <message>\n";
                send(client.getClient(), err.c_str(), err.size(), 0);
                return;
            }

            std::string targetNick = args[0];
            std::string message = args[1];

            IrcClient *targetClient = findClientByNick(targetNick);
            if(!targetClient)
            {
                std::string err = "Error: User not found\n";
                send(client.getClient(), err.c_str(), err.size(), 0);
                return;
            }
            std::ostringstream oss;
            oss << ":" << client.getNick() << " PRIVMSG " << targetNick << " :" << message << "\r\n";
            send(targetClient->getClient(), oss.str().c_str(), oss.str().size(), 0);
        }

        //modes
        // void handelKick(IrcClient& client, const std::vector<std::string>& args)
        // {

        // }
        //  void handelInvite(IrcClient& client, const std::vector<std::string>& args)
        // {

        // }
         void handelTopic(IrcClient& client, const std::vector<std::string>& args)
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
            if(channel->isTopicRestrected(&client) && !channel->isOperator(&client))
            {
                client.sendMessage(" 482 " + client.getNick()  + " " + channelName + " : you are not channel operator\r\n");
                return;
            }
            // set the topic
            channel->SetChannelTopic(newTopic);
            // broadcast topic change to channel
            std::string host = client.getHost().empty() ? "localhost" : client.getHost();
            std::string topicMsg = ":" + client.getNick() + "!" + client.getUsername() + "@" + host + "TOPIC " + channelName + " :" + newTopic + "\r\n";
            broadcastToChannel(*channel, topicMsg, NULL);
        }

        void handleMode_i(Channel* channel, bool adding, std::string& appliedModes)
        {
            channel->setInviteOnly(adding);
            appliedModes += adding ? "+i" : "-i";
        }
        void handleMode_t(Channel* channel, bool adding, std::string& appliedModes)
        {
            channel->setTopicRestrected(adding);
            appliedModes += adding ? "+t" : "-t";
        }
        bool handleMode_k(Channel* channel, IrcClient& client, bool adding, std::string& appliedModes, std::string& modeParams, const std::vector<std::string>& args, size_t& argIndex)
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

        bool handleMode_o(Channel* channel, IrcClient& client, bool adding, std::string& appliedModes, std::string& modeParams, const std::vector<std::string>& args, size_t& argIndex)
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
        bool handleMode_l(Channel* channel, IrcClient& client, bool adding, std::string& appliedModes, std::string& modeParams, const std::vector<std::string>& args, size_t& argIndex)
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

        void handleModes(IrcClient& client, const std::vector<std::string>& args)
        {
            // MODE #channel +/-mode [param]
            if (args.size() < 2)
            {
                client.sendMessage("461 " + client.getNick() + " MODE :not enough parameters\r\n");
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

            for(size_t i = 0; i < modeString.length(); i++)
            {
                char mode = modeString[i];
                if(mode == '+')
                    adding = true;
                else if(mode == '-')
                    adding = false;
                else if(mode == 'i') // mode +i/-i invite only
                    handleMode_i(channel, adding, appliedModes);
                else if(mode == 't') // mode +t/-t topic restricted
                    handleMode_t(channel, adding, appliedModes);
                else if(mode == 'k') // mode +k/-k channel password
                {
                    if (!handleMode_k(channel, client, adding, appliedModes, modeParams, args, argIndex))
                    continue;
                }
                else if(mode == 'o') // mode +o/-o operator privilege
                {
                    if (!handleMode_o(channel, client, adding, appliedModes, modeParams, args, argIndex))
                    continue;
                }
                else if(mode == 'l') // mode +l/-l User limit
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

        ~IrcServer() ;

};

#endif