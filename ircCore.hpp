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
        void handleJoin(IrcClient& client, const std::string& channelName);
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
            // forward the message (for DCC or normal chat)
            std::ostringstream oss;
            oss << ":" << client.getNick() << " PRIVMSG " << targetNick << " :" << message << "\r\n";
            send(targetClient->getClient(), oss.str().c_str(), oss.str().size(), 0);
        }

        //modes
        void handelKick(IrcClient& client, const std::vector<std::string>& args)
        {

        }
         void handelInvite(IrcClient& client, const std::vector<std::string>& args)
        {

        }
         void handelTopic(IrcClient& client, const std::vector<std::string>& args)
        {

        }
         void handelMode(IrcClient& client, const std::vector<std::string>& args)
        {

        }

        ~IrcServer() ;

};

#endif