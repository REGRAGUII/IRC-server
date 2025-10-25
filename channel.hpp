#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include<algorithm>

class IrcClient;

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

        Channel(): _ChannelName("Default"), _topic("None"){};
        Channel(std::string name) : _ChannelName(name), _userLimit(0), inviteOnly(false), topicRestricted(true), hasPassword(false), hasUserLimit(false){}
        // getters
        const std::string GetChannelName() const {return this->_ChannelName;}
        const std::string GetChannelTopic() const {return this->_topic;}
        const std::string GetChannelKey() const {return this->_key;}
        const std::vector<IrcClient*>GetMembers() const { return this->_members;}
        int GetUserLimit() const {return this->_userLimit;}
        std::string getNamesList() const
        {
            std::string names;
            for(size_t i = 0; i < _members.size(); i++)
            {
                if(isOperator(_members[i]))
                    names += "@";
                names += _members[i]->getNick();
                if(i < _members.size() - 1)
                    names += " ";
            }
            return names;
        }
        // setters
        void setTopic(const std::string& newTopic) {_topic = newTopic;}
        void setKey(const std::string password)
        {
            _key = password;
            hasPassword = !password.empty();
        }
        void setUserLimit(int limit)
        {
            _userLimit = limit;
            hasUserLimit = (limit > 0);
        }
        // mode setters
        void setInviteOnly(bool value) { inviteOnly = value;}
        void setTopicRestrected(bool value){topicRestricted = value;}

        bool isOperator(IrcClient* client) const { return std::find(_operators.begin(), _operators.end(), client) !=_operators.end();}
        bool isMember(IrcClient* client) const {return std::find(_members.begin(), _members.end(), client) != _members.end();}
        // op manag
        void addOperator(IrcClient* client)
        {
            if(!isOperator(client))
            _operators.push_back(client);
        }
        void removeOperator(IrcClient* client)
        {
            std::vector<IrcClient*>::iterator it = std::find(_operators.begin(), _operators.end(), client);
            if(it != _operators.end())
                _operators.erase(it);
        }
        // memmber manag
        void addMember(IrcClient* client)
        {
            if (!isMember(client))
                _members.push_back(client);
            if(_members.size() == 1)
                addOperator(client);
        }
        void removeMember(IrcClient* client)
        {
            std::vector<IrcClient*>:: iterator it = std::find(_members.begin(), _members.end(), client);
            if(it != _members.end())
                _members.erase(it);
            removeOperator(client);
        }
        // invite manag
        bool isInvited(IrcClient* client) { return std::find(_inviteUsers.begin(), _inviteUsers.end(), client) != _inviteUsers.end();}
        void addInvite(IrcClient* client)
        {
            if(!isInvited)
                _inviteUsers.push_back(client);
        }

        void removeInvite(IrcClient* client)
        {
            std::vector<IrcClient*>::iterator it = std::find(_inviteUsers.begin(), _inviteUsers.end(), client);
            if(it != _inviteUsers.end())
                _inviteUsers.erase(it);
        }

        void SetChannelTopic(const std::string NewTopic){ _topic = NewTopic;};
        ~Channel(){};

};

#endif