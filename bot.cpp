#include "bot.hpp"
#include <ctime>
#include <sys/socket.h>
#include <unistd.h>

#include "ircCore.hpp"

Bot::Bot() : botName("BOT") 
{
    std::srand(std::time(0));
    pattern p1;
    p1.keywords.push_back("hello");
    p1.keywords.push_back("hey");
    p1.keywords.push_back("hi");
    p1.keywords.push_back("whats up");
    p1.keywords.push_back("hello world");
    p1.keywords.push_back("hello bot");
    p1.response.push_back("Hey how can i help you today ?");
    p1.response.push_back("Hi there tell me how you feel");
    p1.response.push_back("Hello this is IRC bot take your free to talk with me");
    patterns.push_back(p1);

    pattern p2;
    p2.keywords.push_back("sad");
    p2.keywords.push_back("unhappy");
    p2.response.push_back("Why are you feeling sad ?");
    p2.response.push_back("Do you often feel unhappy");
    patterns.push_back(p2);

    pattern p3;
    p3.keywords.push_back("happy");
    p3.keywords.push_back("good");
    p3.keywords.push_back("joy");
    p3.keywords.push_back("cool");
    p3.response.push_back("What makes you happy ?");
    p3.response.push_back("Tell me more about your joy");
    patterns.push_back(p3);

    pattern p4;
    p4.keywords.push_back("angry");
    p4.keywords.push_back("mad");
    p4.response.push_back("Why are you angry ?");
    p4.response.push_back("Does anger often trouble you ?");
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
    p11.response.push_back("Can you tell me more so i can give better advice");
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

    
    pattern p13;
    p13.keywords.push_back("how are you");
    p13.keywords.push_back("how do you feel");
    p13.response.push_back("I'm just a bot, but I'm feeling great! How about you?");
    p13.response.push_back("Doing well! What about you?");
    p13.response.push_back("I’m functioning perfectly today! How are you feeling?");
    patterns.push_back(p13);
    
    pattern p14;
    p14.keywords.push_back("thank");
    p14.keywords.push_back("thanks");
    p14.keywords.push_back("appreciate");
    p14.response.push_back("You're welcome!");
    p14.response.push_back("Happy to help anytime!");
    p14.response.push_back("No problem, glad I could assist!");
    patterns.push_back(p14);
    
    pattern p15;
    p15.keywords.push_back("good bot");
    p15.keywords.push_back("nice");
    p15.keywords.push_back("smart");
    p15.response.push_back("Thank you! I try my best.");
    p15.response.push_back("Aww, that’s kind of you to say!");
    p15.response.push_back("I’m just doing my job");
    patterns.push_back(p15);
    
    pattern p16;
    p16.keywords.push_back("what are you");
    p16.keywords.push_back("who are you");
    p16.response.push_back("I’m an IRC bot made to chat and keep you company.");
    p16.response.push_back("Just a friendly bot here to talk with you!");
    p16.response.push_back("I’m BOT — your little AI companion in this chat.");
    patterns.push_back(p16);
    
    pattern p17;
    p17.keywords.push_back("what can you do");
    p17.keywords.push_back("your abilities");
    p17.response.push_back("I can chat, listen, and maybe make your day a little better!");
    p17.response.push_back("Mostly I talk — but I learn from every message!");
    p17.response.push_back("For now I’m all ears, tell me something interesting!");
    patterns.push_back(p17);
    
    pattern p18;
    p18.keywords.push_back("bored");
    p18.keywords.push_back("boring");
    p18.response.push_back("Maybe you just need a change of pace — what do you enjoy doing?");
    p18.response.push_back("I can chat to keep you company! What’s been boring you?");
    p18.response.push_back("What do you usually do for fun?");
    patterns.push_back(p18);
    
    pattern p19;
    p19.keywords.push_back("tired");
    p19.keywords.push_back("sleepy");
    p19.response.push_back("You should get some rest soon.");
    p19.response.push_back("Maybe a nap will help. Have you been sleeping well?");
    p19.response.push_back("You sound exhausted — want to talk about what’s tiring you?");
    patterns.push_back(p19);
    
    pattern p20;
    p20.keywords.push_back("i think");
    p20.response.push_back("Why do you think that?");
    p20.response.push_back("What makes you believe that?");
    p20.response.push_back("Is that something you’ve thought about often?");
    patterns.push_back(p20);
    
    pattern p21;
    p21.keywords.push_back("because");
    p21.response.push_back("That’s an interesting reason. Can you tell me more?");
    p21.response.push_back("Do you always feel that’s the reason?");
    p21.response.push_back("Sometimes reasons are deeper than they seem — what do you think?");
    patterns.push_back(p21);
    
    pattern p22;
    p22.keywords.push_back("i don’t know");
    p22.response.push_back("That’s okay, sometimes not knowing is part of learning.");
    p22.response.push_back("What do you wish you knew?");
    p22.response.push_back("Maybe talking about it will help you figure it out.");
    patterns.push_back(p22);
    
    pattern p23;
    p23.keywords.push_back("joke");
    p23.keywords.push_back("funny");
    p23.response.push_back("I only know one joke: Why did the computer go to therapy? It had a hard drive!");
    p23.response.push_back("I’m trying to be funny, but I might need an update!");
    p23.response.push_back("Haha! Do you like jokes?");
    patterns.push_back(p23);
    
    pattern p24;
    p24.keywords.push_back("bye");
    p24.keywords.push_back("goodbye");
    p24.keywords.push_back("exit");
    p24.keywords.push_back("adios");
    p24.response.push_back("Goodbye. It was nice talking with yo");
    p24.response.push_back("Goodbye. I hope this conversation has been helpful");
    p24.response.push_back("Until next time. Take care of yourself");
    p24.response.push_back("It's time to say goodbye for now. Perhaps we can talk soon");
    patterns.push_back(p24);

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

void Bot::handelBot(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args)
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
    pattern p1;
    p1.response.push_back("Interesting just tell me more");
    p1.response.push_back("emmm i see continue please");
    p1.response.push_back("i got you just tell me more");

    size_t r = std::rand() % p1.response.size();
    response = botName + " " + p1.response[r] + "\n";

    send(client.getClient(), response.c_str(), response.size(), 0);
}

