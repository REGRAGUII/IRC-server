#include "ircCore.hpp"
#include "fileTransfer.hpp"


IrcServer::IrcServer() {
    fT = new fileTransfer();
}

Channel* IrcServer::findChannel(const std::string& cname) {
    std::map<std::string, Channel>::iterator it = _channels.find(cname);
    return (it == _channels.end()) ? 0 : &it->second;
}
Channel& IrcServer::getOrCreateChannel(const std::string& cname) {
    std::map<std::string, Channel>::iterator it = _channels.find(cname);
    if (it != _channels.end()) return it->second;
    Channel ch(cname);
    _channels[cname] = ch;
    return _channels[cname];
}


IrcServer::~IrcServer() {
    delete fT;
}