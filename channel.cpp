#include "channel.hpp"
#include "ircCore.hpp"

Channel::Channel(): _ChannelName("Default"), _topic("None"){};
Channel::Channel(std::string name) : _ChannelName(name), _userLimit(0), inviteOnly(false), topicRestricted(true), hasPassword(false), hasUserLimit(false){}

const std::string Channel::GetChannelName() const {return this->_ChannelName;}
const std::string Channel::GetChannelTopic() const {return this->_topic;}
const std::string Channel::GetChannelKey() const {return this->_key;}
const std::vector<IrcClient*> Channel::GetMembers() const { return this->_members;}
int Channel::GetUserLimit() const {return this->_userLimit;}
std::string Channel::getNamesList() const
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

void Channel::setTopic(const std::string& newTopic) {_topic = newTopic;}
void Channel::setKey(const std::string password)
{
    _key = password;
    hasPassword = !password.empty();
}
void Channel::setUserLimit(int limit)
{
    _userLimit = limit;
    hasUserLimit = (limit > 0);
}

void Channel::setInviteOnly(bool value) { inviteOnly = value;}
void Channel::setTopicRestrected(bool value){topicRestricted = value;}
bool Channel::isOperator(IrcClient* client) const { return std::find(_operators.begin(), _operators.end(), client) !=_operators.end();}
bool Channel::isMember(IrcClient* client) const {return std::find(_members.begin(), _members.end(), client) != _members.end();}
bool Channel::isInviteOnly() const { return inviteOnly; }
bool Channel::isTopicRestrected() const {return topicRestricted;}


void Channel::addOperator(IrcClient* client)
{
    if(!isOperator(client))
    _operators.push_back(client);
}
void Channel::removeOperator(IrcClient* client)
{
    std::vector<IrcClient*>::iterator it = std::find(_operators.begin(), _operators.end(), client);
    if(it != _operators.end())
        _operators.erase(it);
}


void Channel::addMember(IrcClient* client)
{
    if (!isMember(client))
        _members.push_back(client);
    if(_members.size() == 1)
        addOperator(client);
}
void Channel::removeMember(IrcClient* client)
{
    std::vector<IrcClient*>:: iterator it = std::find(_members.begin(), _members.end(), client);
    if(it != _members.end())
        _members.erase(it);
    removeOperator(client);
    removeInvite(client);
}


bool Channel::isInvited(IrcClient* client) { return std::find(_inviteUsers.begin(), _inviteUsers.end(), client) != _inviteUsers.end();}
void Channel::addInvite(IrcClient* client)
{
    if(!isInvited(client))
        _inviteUsers.push_back(client);
}
void Channel::removeInvite(IrcClient* client)
{
    std::vector<IrcClient*>::iterator it = std::find(_inviteUsers.begin(), _inviteUsers.end(), client);
    if(it != _inviteUsers.end())
        _inviteUsers.erase(it);
}
void Channel::SetChannelTopic(const std::string NewTopic){ _topic = NewTopic;};
Channel::~Channel(){};