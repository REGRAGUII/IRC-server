#include "bot.hpp"
#include <ctime>
#include <sys/socket.h>
#include <unistd.h>

#include "ircCore.hpp"

Bot::Bot() : botName("BOT") {}

void Bot::handelBotCommnads(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args)
{
    (void)irc;
    std::string response;

    if (args.empty())
    {
        response = botName + " : Please specify a command, try BOT help\n";
        send(client.getClient(), response.c_str(), response.size(), 0);
        return;
    }

    std::string botCommand = args[0];
    if (botCommand == "help")
    {
        response = botName + " : Available commands : help, hello, echo, time\n";
    }
    else if (botCommand == "hello")
    {
        response = botName + " : Hey this is IRC bot, can I help you!\n";
    }
    else if (botCommand == "echo" && args.size() > 1)
    {
        response = botName + " : ";
        for (size_t i = 1; i < args.size(); ++i)
        {
            response += args[i] + " ";
        }
        response += '\n';
    }
    else if (botCommand == "time")
    {
        response = botName + " : Current time : " + getCurrentTime() + "\n";
    }
    else
    {
        response = botName + " : Unknown command, try BOT help\n";
    }

    send(client.getClient(), response.c_str(), response.size(), 0);
}

std::string Bot::getCurrentTime()
{
    time_t now = time(0);
    char *timeStr = ctime(&now);
    std::string result(timeStr);
    if (!result.empty() && result[result.size() -1] == '\n')
        result.erase(result.size() -1);
    return result;
}
