#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
class IrcClient;

class Channel{
    private:
        std::string _ChannelName;
        std::string _topic;
        std::vector<IrcClient*> _members;
        std::vector<int> _operators;
        std::vector<int> _invites; 

    public:

        Channel(): _ChannelName("Default"), _topic("None"){};
        Channel(std::string name){_ChannelName = name;};
        const std::string GetChannelName() const {return this->_ChannelName;};
        const std::string GetChannelTame() const {return this->_topic;}
        void addMember(IrcClient* client);
        std::string getNamesList() const;
        const std::vector<IrcClient*>& getMembers() const
        {
            return _members;
        };
        void SetChannelTopic(const std::string NewTopic){ _topic = NewTopic;};
        ~Channel(){};

        // void addMember(int fd) {_members.push_back(fd);};
        void addOperators(int fd) {_operators.push_back(fd);};
        void addInvites(int fd) {_invites.push_back(fd);};

};

#endif