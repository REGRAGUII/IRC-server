#include "ircCore.hpp"
#include "fileTransfer.hpp"

IrcServer::IrcServer() {
    fT = new fileTransfer();
}

IrcServer::~IrcServer() {
    delete fT;
}