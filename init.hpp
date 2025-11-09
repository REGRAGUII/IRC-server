#ifndef INIT_HPP
#define INIT_HPP

#include "ircCore.hpp"
#include "bot.hpp"
#include "fileTransfer.hpp"


void ft_init(IrcServer& irc, char **argv);
void bind_and_listen_accept(IrcServer& irc);
int accept_new_client(IrcServer& irc);
void run_server_loop(IrcServer& irc);
void handlePrivmsg(IrcServer& server, IrcClient& client, const std::vector<std::string>& args);

void HandleCommand(IrcClient& client, const cmd& command, IrcServer& irc, Bot &bot, fileTransfer &fT);

cmd ft_parse(const std::string& msg);

#endif


