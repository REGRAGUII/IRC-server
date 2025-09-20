#ifndef init_hpp
#define init_hpp

#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>


class SocketData {
    private:
        int socket_fd;
    public:
        int getsocket_fd(){return socket_fd;}
        void setsocket_fd(int socket){socket_fd = socket;}

};

class ConnectionData {
    private :
        int prt;
        std::string password;
    public:
        std::string getpassword(){return password;}
        int getport(){return (prt);}

        void setpassword(std::string pwd){password = pwd;}
        void setport(int poort){prt = poort;}


};


class Ircclient {
    private:
        ConnectionData  con_d;
        SocketData      sock_d;   
    public:
            // ******      Connection Data      ****** 

        std::string getpassword() {return con_d.getpassword();}
        int getport() {return con_d.getport();}
        void setpassword(std::string pwd) {con_d.setpassword(pwd);}
        void setport(int poort) {con_d.setport(poort);}
        ConnectionData& getConnectionData() { return con_d; }
            // ******        Socket Data        ****** 
        int getsocket_fd(){return (sock_d.getsocket_fd());}
        SocketData& getSocketData() {return sock_d;}
        void setsocket(int socket){sock_d.setsocket_fd(socket);}

    };


void ft_init(Ircclient& irc, char **argv);
void bind_and_listen(Ircclient& irc);


#endif


