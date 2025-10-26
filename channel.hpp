#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include<algorithm>

class IrcClient;
class ircServer;

class Channel{
    private:
        std::string _ChannelName;
        std::string _topic;
        std::string _key; //  channel password for +k
        int _userLimit;  // max users for +l

        std::vector<IrcClient*> _members;
        std::vector<IrcClient*> _operators;
        std::vector<IrcClient*> _inviteUsers;

        // channels mode
        bool inviteOnly;        // +i mode
        bool topicRestricted;   // +t mode (only ops can change topic)
        bool hasPassword;       // +k mode
        bool hasUserLimit;      // +l mode

    public:
        // getter
        Channel();
        Channel(std::string name);
        const std::string GetChannelName() const;
        const std::string GetChannelTopic() const;
        const std::string GetChannelKey() const;
        const std::vector<IrcClient*>GetMembers() const;
        int GetUserLimit() const;
        std::string getNamesList() const;
        // setters
        void setTopic(const std::string& newTopic);
        void SetChannelTopic(const std::string NewTopic);
        void setKey(const std::string password);
        void setUserLimit(int limit);
        // mode setters
        void setInviteOnly(bool value);
        void setTopicRestrected(bool value);
        // op manag
        bool isOperator(IrcClient* client) const;
        bool isMember(IrcClient* client) const;
        void addOperator(IrcClient* client);
        void removeOperator(IrcClient* client);
        // memmber manag
        void addMember(IrcClient* client);
        void removeMember(IrcClient* client);
        // invite manag
        bool isInvited(IrcClient* client);
        void addInvite(IrcClient* client);
        void removeInvite(IrcClient* client);
        ~Channel(){};

};

#endif