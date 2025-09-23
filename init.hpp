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

class SocketData {
    private:
        int socket_fd;
    public:
        int getsocket_fd()const{return socket_fd;}
        void setsocket_fd(int socket){socket_fd = socket;}

};

class ConnectionData {
    private :
        int prt;
        std::string password;
    public:
        std::string getpassword()const{return password;}
        int getport()const {return (prt);}

        void setpassword(std::string pwd){password = pwd;}
        void setport(int poort){prt = poort;}


};

class IrcClient {
private:
    int client_fd;
public:
    IrcClient(int fd){
        client_fd = fd;
    }
    int getClient() const { return client_fd; }
};

class IrcServer {
    private:
        ConnectionData  con_d;
        SocketData      sock_d;
        std::vector <IrcClient*> clients;
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
            IrcClient *c = new IrcClient(client_fd);
            clients.push_back(c);
            std::cout << "Client number " << clients.size() - 1 << " is connected\n";
        }





        ~IrcServer() {
        for (size_t i = 0; i < clients.size(); ++i)
            delete clients[i];
        clients.clear();
        }
    };


void ft_init(IrcServer& irc, char **argv);
void bind_and_listen_accept(IrcServer& irc);
int accept_new_client(IrcServer& irc);
void run_server_loop(IrcServer& irc);


#endif


