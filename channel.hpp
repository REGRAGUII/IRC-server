#include <map>
#include <set>
#include <string>
#include <iostream>
#include "init.hpp"

// class Channel{
//     private:
//         std::string _ChannelName;
//         std::string _topic;
//         std::set<int> _members;
//         std::set<int> _operators;
//         std::set<int> _invites; 

//     public:

//         Channel(): _ChannelName("Default"), _topic("None"){};
//         Channel(std::string name){_ChannelName = name;};
//         const std::string GetChannelName() const {return this->_ChannelName;};
//         const std::string GetChannelTame() const {return this->_topic;}
//         void SetChannelTopic(const std::string NewTopic){ _topic = NewTopic;};
//         ~Channel();

//         void addMember(int fd) {_members.insert(fd);};
//         void addOperators(int fd) {_operators.insert(fd);};
//         void addInvites(int fd) {_invites.insert(fd);};

// };
