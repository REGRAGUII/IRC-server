#include "fileTransfer.hpp"
#include "ircCore.hpp"
#include <fstream>
#include <sstream>
#include <netdb.h>
#include <arpa/inet.h>


fileTransfer::fileTransfer() {}
void fileTransfer::handelSend(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args)
{
    if (args.size() < 2)
    {
        std::string response = "Usage: /send <recipient> <filename>\n";
        send(client.getClient(), response.c_str(), response.size(), 0);
        return;
    }

    std::string recipientNick = args[0];
    std::string fileName = args[1];

    IrcClient *recipient = irc.findClientByNick(recipientNick);
    if (!recipient)
    {
        std::string response = "Error: User not found\n";
        send(client.getClient(), response.c_str(), response.size(), 0);
        return;
    }

    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    gethostname(hostbuffer, sizeof(hostbuffer));
    host_entry = gethostbyname(hostbuffer);
    IPbuffer = inet_ntoa(*((struct in_addr *)host_entry->h_addr_list[0]));
    std::string localIP = IPbuffer;

    unsigned long ipInt = ntohl(inet_addr(localIP.c_str()));

    int port = 5000 + (rand() % 1000);

    std::ostringstream oss;
    oss << "PRIVMSG " 
        << recipientNick 
        << " :"
        << oss.put('\x01')
        << "DCC SEND " 
        << fileName
        << " " << ipInt 
        << " " << port 
        << " 0\x01\r\n";

    send(recipient->getClient(), oss.str().c_str(), oss.str().size(), 0);
    std::string msgForSender = "DCC file transfer request sent to " + recipientNick + "\n";
    send(client.getClient(), msgForSender.c_str(), msgForSender.size(), 0);
}
