#ifndef init_hpp
#define init_hpp

#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

class Data {
    private :
        int prt;
        std::string password;
    public:
        std::string getpassword(){return password;};
        int getport(){return (prt);};

        void setpassword(std::string pwd){password = pwd;};
        void setport(int poort){prt = poort;};


};


class Pt {
    private:

    public:
    };


#endif

