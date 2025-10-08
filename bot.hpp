#ifndef BOT_HPP
#define BOT_HPP

#include <string>
#include <vector>


class IrcServer;
class IrcClient;

class Bot
{
private:
    std::string botName;

public:
    Bot();
    void handelBotCommnads(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args);
    std::string getCurrentTime();
};

#endif
