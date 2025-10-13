#include "bot.hpp"
#include <ctime>
#include <sys/socket.h>
#include <unistd.h>

#include "ircCore.hpp"

// std::string Bot::swapPronouns(const std::string &input) const
// {
//     std::istringstream iss(input);
//     std::ostringstream oss;
//     std::string word;

//     while (iss >> word)
//     {
//         if (word == "you" && word.length() == 3) word = "I";
//         else if (word == "I" && word.length() == 1) word = "you";
//         else if (word == "your" && word.length() == 4) word = "my";
//         else if (word == "my" && word.length() == 2) word = "your";
//         oss << word << " ";
//     }

//     std::string result = oss.str();
//     if (!result.empty())
//         result.pop_back(); // remove trailing space
//     return result;
// }

Bot::Bot() : botName("BOT") 
{
    std::srand(std::time(0));
    pattern p1;
    p1.keywords.push_back("hello");
    p1.keywords.push_back("hey");
    p1.keywords.push_back("hi");
    p1.keywords.push_back("whats up");
    p1.keywords.push_back("yo");
    p1.keywords.push_back("hello world");
    p1.keywords.push_back("hello bot");
    p1.response.push_back("Hey how can I help you today ?");
    p1.response.push_back("Hi there tell me how you feel");
    p1.response.push_back("Hello this is IRC bot take your free to talk with me");
    patterns.push_back(p1);

    pattern p2;
    p2.response.push_back("sad");
    p2.keywords.push_back("unhappy");
    p2.response.push_back("Why are you feeling sad ?");
    p2.response.push_back("Do you often feel unhappy");
    patterns.push_back(p2);

    pattern p3;
    p3.keywords.push_back("happy");
    p1.keywords.push_back("good");
    p3.keywords.push_back("joy");
    p1.keywords.push_back("cool");
    p3.response.push_back("What makes you happy ?");
    p3.response.push_back("Tell me more about your joy");
    patterns.push_back(p3);

    pattern p4;
    p4.keywords.push_back("angry");
    p4.keywords.push_back("mad");
    p4.keywords.push_back("Why are you angry ?");
    p4.keywords.push_back("Does anger often trouble you ?");
    patterns.push_back(p4);

    pattern p5;
    p5.keywords.push_back("love");
    p5.keywords.push_back("like");
    p5.response.push_back("Love is a strong feeling. Who do you love?");
    p5.response.push_back("That’s sweet What makes you feel this way?");
    p5.response.push_back("Do you find love easy to express?");
    patterns.push_back(p5);

    pattern p6;
    p6.keywords.push_back("life");
    p6.keywords.push_back("living");
    p6.response.push_back("Life can be complex What part of life are you thinking about?");
    p6.response.push_back("Do you enjoy how your life is going right now?");
    patterns.push_back(p6);

    pattern p7;
    p7.keywords.push_back("work");
    p7.keywords.push_back("school");
    p7.keywords.push_back("study");
    p7.response.push_back("Does work cause you stress?");
    p7.response.push_back("How do you feel about school?");
    p7.response.push_back("Do you enjoy what you’re doing?");
    patterns.push_back(p7);

    pattern p8;
    p8.keywords.push_back("sleep");
    p8.keywords.push_back("tired");
    p8.keywords.push_back("exhausted");
    p8.response.push_back("You sound tired. Do you get enough rest?");
    p8.response.push_back("Sleep is important. How have you been sleeping?");
    patterns.push_back(p8);

    pattern p9;
    p9.keywords.push_back("i feel");
    p9.keywords.push_back("i am");
    p9.response.push_back("Why do you feel that way?");
    p9.response.push_back("How long have you felt like that?");
    p9.response.push_back("Can you tell me more about it?");
    patterns.push_back(p9);

    pattern p10;
    p10.keywords.push_back("friend");
    p10.keywords.push_back("friends");
    p10.response.push_back("Tell me about your friend");
    p10.response.push_back("Are your friends supportive?");
    p10.response.push_back("Who do you trust most among your friends?");
    patterns.push_back(p10);

    pattern p11;
    p11.keywords.push_back("advice");
    p11.keywords.push_back("help");
    p11.keywords.push_back("what should I do");
    p11.response.push_back("Can you tell me more so I can give better advice?");
    p11.response.push_back("What do you feel is the most important issue right now?");
    p11.response.push_back("Let's think about this together. What's your goal?");
    patterns.push_back(p11);

    pattern p12;
    p12.keywords.push_back("life");
    p12.keywords.push_back("future");
    p12.keywords.push_back("decisions");
    p12.response.push_back("What are your main goals in life?");
    p12.response.push_back("Do you often worry about the future?");
    p12.response.push_back("Sometimes taking small steps helps in big decisions. What step can you take?");
    patterns.push_back(p12);
    
}

bool containWholeWord(const std::string &text , const std::string &word)
{
    bool startOk = false;
    bool endOk = false;

    size_t pos = text.find(word);
    while(pos != std::string::npos)
    {
        startOk = (pos == 0 || !std::isalnum(text[pos - 1]));
        endOk = (pos + word.size() == text.size() || !std::isalnum(text[pos + word.size()]));

        if(startOk && endOk)
            return true;
        pos = text.find(word,pos + 1);
    }
    return false;
}

void Bot::handelBotCommnads(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args)
{
    (void)irc;
    std::string response;

    if (args.empty())
    {
        response = botName + " : Please type something for me to respond. try BOT help\n";
        send(client.getClient(), response.c_str(), response.size(), 0);
        return;
    }
    std::string userInput;
    for(size_t i = 0; i < args.size(); i++)
    {
        userInput += args[i];
        if(i != args.size() - 1)
            userInput += " ";
    }
    // std::string botCommand = args[0];
    for(size_t i = 0; i < patterns.size(); ++i)
    {
        for(size_t j = 0; j < patterns[i].keywords.size(); ++j)
        {
            if(containWholeWord(userInput, patterns[i].keywords[j]))
            {
                size_t r = std::rand() % patterns[i].response.size();
                response = botName + " : " + patterns[i].response[r] + "\n";
                send(client.getClient(), response.c_str(), response.size(), 0);
                return;
            }
        }
    }
    // if (botCommand == "help")
    // {
    //     response = botName + " : Available commands : help, hello, echo, time\n";
    // }
    // else if (botCommand == "hello")
    // {
    //     response = botName + " : Hey this is IRC bot, can I help you!\n";
    // }
    // else if (botCommand == "echo" && args.size() > 1)
    // {
    //     response = botName + " : ";
    //     for (size_t i = 1; i < args.size(); ++i)
    //     {
    //         response += args[i] + " ";
    //     }
    //     response += '\n';
    // }
    // else if (botCommand == "time")
    // {
    //     response = botName + " : Current time : " + getCurrentTime() + "\n";
    // }
    // else
    // {
    //     response = botName + " : Unknown command, try BOT help\n";
    // }
    
    pattern p1;
    p1.response.push_back("Interesting just tell me more");
    p1.response.push_back("emmm i see continue please");
    p1.response.push_back("i got you just tell me more");
    // response = botName + " Interesting just tell me more\n";
    for(size_t i = 0; i < p1.response.size(); ++i)
    {
        size_t r = std::rand() % p1.response.size();
        response = botName + " " + p1.response[r] + "\n";
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
