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
#include <sstream>
#include "init.hpp"
#include <fcntl.h>
#include <csignal>
#

class fileTransfer;
struct cmd
{

    std::string prefix;
    std::string c;
    std::vector<std::string> args;
};
class SocketData
{
private:
    int socket_fd;
public:
    SocketData() : socket_fd(-1){}
    int getsocket_fd()const{return socket_fd;}
    void setsocket_fd(int socket){socket_fd = socket;}

};
class ConnectionData
{
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

class IrcClient
{
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
    IrcClient();
    IrcClient(int fd);
    const std::string& getHost() const;
    int getClient() const;
    void Buffering(const std::string& add);
    void sendMessage(const std::string& msg);
    void sendMessages(const std::string& msg, IrcClient*targetClient);
    bool ExtractLine(std::string& line);
    void setNick(const std::string& n);
    void setUsername(const std::string& u);
    void setRealname(const std::string& r);
    void setPassAccepted(bool v);
    void setRegistered(bool v);
    const std::string getNick() const;
    const std::string getUsername() const;
    const std::string getRealname() const;
    bool hasPass() const;
    bool hasNick() const;
    bool hasUser() const;
    bool isRegistered() const;
    void tryAuthenticate();

};


class IrcServer
{
private:
    ConnectionData  con_d;
    SocketData      sock_d;
    std::map <int, IrcClient> clients;
    std::map<std::string, Channel> _channels;
    Bot bot;
    fileTransfer *fT;
public:
    IrcServer();
    Bot& getBot();
    fileTransfer& getFileTransfer();
    std::string getpassword()const;
    int getport()const;

    void setpassword(std::string pwd);
    void setport(int poort);
    void broadcastToChannel(const Channel& channel, const std::string& msg, IrcClient* exclude);
    ConnectionData& getConnectionData();

    int getsocket_fd()const;
    SocketData& getSocketData();
    void setsocket(int socket);

    IrcClient* getClient(int id);
    void add_client(int client_fd);
    IrcClient* findClientByNick(const std::string nickName);
    void remove_client(int client_fd);
    bool isNickTaken(std::string nick) const;
    void sendToClient(IrcClient& client, const std::string& raw);

    Channel* findChannel(const std::string& cname);
    Channel& getOrCreateChannel(const std::string& cname, std::string pass, IrcClient &client);
    Channel* GetChannel(const std::string& cname);
    std::map<std::string, Channel>& channels();

    void handleUser(IrcClient& client, const std::vector<std::string>& args);
    void handlePass(IrcClient& client, const std::vector<std::string>& args);
    void handleNick(IrcClient& client, const std::vector<std::string>& args);
    void handlePrivmsg(IrcClient& client, const std::vector<std::string>& args);
    void handleJoin(IrcClient& client, const std::vector<std::string>& args);
    void handelKick(IrcClient& client, const std::vector<std::string>& args);
    void handelInvite(IrcClient& client, const std::vector<std::string>& args);
    void handelTopic(IrcClient& client, const std::vector<std::string>& args);

    void handleModes(IrcClient& client, const std::vector<std::string>& args);
    void handleMode_i(Channel* channel, bool adding, std::string& appliedModes);
    void handleMode_t(Channel* channel, bool adding, std::string& appliedModes);
    bool handleMode_k(Channel* channel, IrcClient& client, bool adding, std::string& appliedModes, std::string& modeParams, const std::vector<std::string>& args, size_t& argIndex);
    bool handleMode_o(Channel* channel, IrcClient& client, bool adding, std::string& appliedModes, std::string& modeParams, const std::vector<std::string>& args, size_t& argIndex);
    bool handleMode_l(Channel* channel, IrcClient& client, bool adding, std::string& appliedModes, std::string& modeParams, const std::vector<std::string>& args, size_t& argIndex);

    void removeClientFromAllChannels(IrcClient *client);
    ~IrcServer() ;
};

#endif