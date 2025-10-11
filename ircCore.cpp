#include "ircCore.hpp"
#include "fileTransfer.hpp"

IrcServer::IrcServer() {
    fT = new fileTransfer();
    testMode = false;
}

IrcServer::~IrcServer() {
    delete fT;
}