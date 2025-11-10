#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include<algorithm>

class IrcClient;
class Channel
{
private:
    std::string _ChannelName;
    std::string _topic;
    std::string _key;
    int _userLimit;
    std::vector<IrcClient*> _members;
    std::vector<IrcClient*> _operators;
    std::vector<IrcClient*> _inviteUsers;

    bool inviteOnly;
    bool topicRestricted;
    bool hasPassword;
    bool hasUserLimit;
public:
    Channel();
    Channel(std::string name);

    const std::string GetChannelName() const ;
    const std::string GetChannelTopic() const ;
    const std::string GetChannelKey() const ;
    const std::vector<IrcClient*>GetMembers() const ;
    int GetUserLimit() const ;
    std::string getNamesList() const ;

    void setTopic(const std::string& newTopic) ;
    void setKey(const std::string password);
    void setUserLimit(int limit);

    void setInviteOnly(bool value) ;
    void setTopicRestrected(bool value);
    bool isOperator(IrcClient* client) const ;
    bool isMember(IrcClient* client) const ;
    bool isInviteOnly() const;
    bool isTopicRestrected() const;
 
    void addOperator(IrcClient* client);
    void removeOperator(IrcClient* client);

    void addMember(IrcClient* client);
    void removeMember(IrcClient* client);

    bool isInvited(IrcClient* client) ;
    void addInvite(IrcClient* client);
    void removeInvite(IrcClient* client);
    void SetChannelTopic(const std::string NewTopic);
    ~Channel();
};

#endif