#ifndef FILETRANSFER_HPP
#define FILETRANSFER_HPP

#include <string>
#include <vector>
#include <sys/socket.h>
// #include "ircCore.hpp"

class IrcServer;  // ✅ forward declare
class IrcClient;  // ✅ forward declare
struct cmd;

class fileTransfer
{
    private :
        //// session management
        struct FileTransferSession
        {
            int senderId;
            int receiverId;
            std::string fileName;
            std::vector<char> fileData;
            size_t totalSize;
            FileTransferSession(int sender, int receiver, const std::string& fname);
        };
        std::vector<FileTransferSession> pendingTransfers;
        std::vector<std::string> validCommands;

        //// file operations
        std::vector<char> readBinaryFile(const std::string &fileName);

    public :
       fileTransfer();

       bool isFileTransferCmd(const std::string &cmd);

        // headers for transfer info
        std::string buildTransferRequest(const std::string &sender, const std::string &fileName, size_t size);
        std::string buildFileStartHeader(const std::string &fileName, size_t size);

        //// command handlers
        void handelfileTransferCmd(IrcServer &irc ,IrcClient &client,const cmd &command);
        void startTransfer(FileTransferSession &session);
        void handelSend(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args);
        void handelAccept(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args);
        void handelDecline(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args);

};

#endif
