#ifndef BOT_HPP
#define BOT_HPP

#include <string>
#include <vector>


class IrcServer;
class IrcClient;

class Bot
{
private:
    struct pattern
    {
        std::vector<std::string> keywords;
        std::vector<std::string> response;
    };
    std::string botName;
    std::vector<pattern> patterns;
    std::string swapPronouns(const std::string &input) const;

public:

    Bot();
    void handelBot(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args);
    std::string getCurrentTime();
};

#endif
