#ifndef init_hpp
#define init_hpp

#include <iostream>
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

public:
    IrcClient() : client_fd(-1), _passAccepted(false), _registered(false) {}
    IrcClient(int fd) :client_fd(fd), _passAccepted(false), _registered(false){}
    int getClient() const { return client_fd; }
    void Buffering(const std::string& add){ _buffer += add;}
    void sendMessage(const std::string& msg){
        send(client_fd, msg.c_str(), msg.size(), 0);
    }
    bool ExtractLine(std::string& line) {
        std::string::size_type pos = _buffer.find("\r\n");
        if (pos == std::string::npos)
            return false;
        line = _buffer.substr(0, pos + 2);
        _buffer.erase(0, pos + 2);
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
    bool isRegistered() const { return _registered; }

};

class IrcServer {
    private:
        ConnectionData  con_d;
        SocketData      sock_d;
        std::map <int, IrcClient> clients;
    public:
            // ******      Connection Data      ******

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

        void remove_client(int client_fd) {
            std::map<int, IrcClient>::iterator it = clients.find(client_fd);
            if (it != clients.end()) clients.erase(it);
        }


        ~IrcServer() {
        // for (size_t i = 0; i < clients.size(); ++i)
            // delete clients[i];
        // clients.clear();
        }
    };

class Bot
{
    private :
        std::string botName;
        // IrcServer *serverRef;
    public :
        Bot() : botName("BOT") {}
        void handelBotCommnads(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args)
        {
            std::string response;

            if(args.empty())
            {
                response = botName + " : Please specify a commnade, try BOT help\n";
                send(client.getClient(), response.c_str(), response.size(), 0);
                return;
            }
            std::string botCommand = args[0];
            if(botCommand == "help")
            {
                response = botName + " : Available commands : help, hello, echo, time\n";
                send(client.getClient(), response.c_str(), response.size(), 0);
            }
            else if(botCommand == "hello")
            {
                response = botName + " : Hey this is IRC bot, can i help you!\n";
            }
            else if(botCommand == "echo" && args.size() > 1)
            {
                std::string msg = botName + " : ";
                for(size_t i = 1; i < args.size(); i++)
                    msg += args[i] + " ";
                msg += '\n';
                send(client.getClient(), response.c_str(), response.size(),0);
            }
            else if(botCommand == "time")
            {
                response = botName + " : Current time : " + getCurrentTime();
                send(client.getClient(), response.c_str(), response.size(), 0);
            }
            else
            {
                response = botName + " : Unknown command, try BOT help\n";
                send(client.getClient(), response.c_str(), response.size(), 0);
            }

        }
        std::string getCurrentTime()
        {
            time_t now = time(0);
            char *timeStr = ctime(&now);
            std::string rslt(timeStr);
            if(!rslt.empty() && rslt[rslt.length()-1] == '\n')
                rslt.erase(rslt.length()-1);
            return rslt;
        }
};

class fileTransfer
{
    private :
        int senderFd;
        int receiverFd;
        std::vector<std::string> validCommands;
    public :
        fileTransfer()
        {
            senderFd = -1;
            receiverFd = -1;
            validCommands.push_back("/send");
            validCommands.push_back("/accept");
            validCommands.push_back("/decline");
            validCommands.push_back("/recieve");
        }
        bool isFileTransferCmd(const std::string &cmd) 
        {
            for(size_t i = 0; i < validCommands.size(); i++)
            {
                if(cmd == validCommands[i])
                    return true;
            }
            return false;
        }
        void handelfileTransferCmd(IrcServer &irc ,IrcClient &client, cmd command)
        {
            if(command.c == "/send")
            {
                handelSend(irc, client, command.args);
            }
            else if(command.c == "/accept")
            {
                handelAccept(irc, client, command.args);
            }
            else if(command.c == "/decline")
            {
                handelDecline(irc, client, command.args);
            }
            else if(command.c == "/receive")
            {
                handelRecieve(irc, client, command.args);
            }
        };
        // void startTransfer();
        // void sendChunk();
        // bool isComplete();
    private :
        void handelSend(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args);
        void handelAccept(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args);
        void handelDecline(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args);
        void handelRecieve(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args);
};

void ft_init(IrcServer& irc, char **argv);
void bind_and_listen_accept(IrcServer& irc);
int accept_new_client(IrcServer& irc);
void run_server_loop(IrcServer& irc);

void HandleCommand(IrcClient& client, const cmd& command, IrcServer& irc, Bot &bot);
cmd ft_parse(const std::string& msg);



#endif


