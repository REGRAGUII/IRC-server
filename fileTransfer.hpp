#ifndef FILETRANSFER_HPP
#define FILETRANSFER_HPP

#include <string>
#include <vector>
#include <sys/socket.h>

class IrcServer;
class IrcClient;
struct cmd;
class fileTransfer
{
public :
    fileTransfer();
    void handelSend(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args);
};

#endif
