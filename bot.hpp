#include "init.hpp"

class Bot
{
    private :
        std::string botName;
        // IrcServer *serverRef;
    public :
        Bot() : botName("BOT") {}
        void handelBotCommnads(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args)
        {
            std::string response;

            if(args.empty())
            {
                response = botName + " : Please specify a commnade, try BOT help\n";
                send(client.getClient(), response.c_str(), response.size(), 0);
                return;
            }
            std::string botCommand = args[0];
            if(botCommand == "help")
            {
                response = botName + " : Available commands : help, hello, echo, time\n";
                send(client.getClient(), response.c_str(), response.size(), 0);
            }
            else if(botCommand == "hello")
            {
                response = botName + " : Hey this is IRC bot, can i help you!\n";
            }
            else if(botCommand == "echo" && args.size() > 1)
            {
                std::string msg = botName + " : ";
                for(size_t i = 1; i < args.size(); i++)
                    msg += args[i] + " ";
                msg += '\n';
                send(client.getClient(), response.c_str(), response.size(),0);
            }
            else if(botCommand == "time")
            {
                response = botName + " : Current time : " + getCurrentTime();
                send(client.getClient(), response.c_str(), response.size(), 0);
            }
            else
            {
                response = botName + " : Unknown command, try BOT help\n";
                send(client.getClient(), response.c_str(), response.size(), 0);
            }

        }
        std::string getCurrentTime()
        {
            time_t now = time(0);
            char *timeStr = ctime(&now);
            std::string rslt(timeStr);
            if(!rslt.empty() && rslt[rslt.length()-1] == '\n')
                rslt.erase(rslt.length()-1);
            return rslt;
        }
};