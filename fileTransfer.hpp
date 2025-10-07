#include "init.hpp"

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
            size_t bytesSent;
            bool isActive;
            FileTransferSession(int sender, int receiver, const std::string& fname): senderId(sender), receiverId(receiver), fileName(fname),totalSize(0), bytesSent(0), isActive(false) {}
        };
        std::vector<FileTransferSession> pendingTransfers;
        // std::vector<FileTransferSession> activeTransfers;
        std::vector<std::string> validCommands;
        //// file operations
        std::vector<char> readBinaryFile(const std::string &fileName)
        {
            std::ifstream file(fileName.c_str(), std::ios::binary);
            if(!file)
                return std::vector<char>();

            //// get file size
            file.seekg(0,std::ios::end);
            std::streampos fileSize = file.tellg();
            file.seekg(0, std::ios::beg);

            //// read vector info
            std::vector<char> buff(static_cast<size_t>(fileSize));
            file.read(&buff[0], fileSize);
            file.close();
            return buff;

        }

        void saveBinaryFile(const std::string fileName, const std::vector<char> &data) //////
        {
    
        }

    public :
        fileTransfer()
        {
            validCommands.push_back("/send");
            validCommands.push_back("/accept");
            validCommands.push_back("/decline");
            validCommands.push_back("/recieve");
        }

        bool isFileTransferCmd(const std::string &cmd) 
        {
            for(size_t i = 0; i < validCommands.size(); i++)
            {
                if(cmd == validCommands[i])
                    return true;
            }
            return false;
        }
        //// headers for transfer info
        std::string buildTransferRequest(const std::string &sender, const std::string &fileName, size_t size)
        {
            std::ostringstream oss;
            oss << "File transfer from " << sender << ": " << fileName << " (" << size << " bytes)\n";
            return oss.str();
        }
        std::string buildFileStartHeader(const std::string fileName, size_t size)
        {
            std::ostringstream oss;
            oss << "FILE_START " << fileName << size;
            return oss.str();
        }
        //// command handlers
        void handelfileTransferCmd(IrcServer &irc ,IrcClient &client, cmd &command)
        {
            if(command.c == "/send")
            {
                if(command.args.size() < 2)
                {
                    std::string respose = "Usage: /send <recipient> <filename>\n";
                    send(client.getClient(), respose.c_str(), respose.size(), 0);
                    return;
                }
                handelSend(irc, client, command.args);
            }
            else if(command.c == "/accept")
            {
                handelAccept(irc, client, command.args);
            }
            else if(command.c == "/decline")
            {
                handelDecline(irc, client, command.args);
            }
            else if(command.c == "/receive")
            {
                handelRecieve(irc, client, command.args);
            }
        };

        void startTransfer(FileTransferSession &session)
        {
            //// send file header
            std::string response = buildFileStartHeader(session.fileName, session.totalSize);
            send(session.receiverId, response.c_str(), response.size(), 0);
            //// send in chunks
            const size_t chunkSize = 4096;
            size_t bytesSent = 0;
            while(bytesSent < session.totalSize)
            {
                size_t remaining = session.totalSize - bytesSent;
                size_t chunkSz = (remaining < chunkSize) ? remaining : chunkSize;
                ssize_t sent = send(session.receiverId, &session.fileData[bytesSent], chunkSz, 0);
                // error handeling
                if(sent <= 0)
                {
                    std::string response = "Error: Transfer failed\n";
                    send(session.senderId, response.c_str(), response.size(), 0);
                    return;
                }
                bytesSent += static_cast<size_t>(sent);
            }
            //// send end marker
            std::string endMarker = "FILE_END\n";
            send(session.receiverId, endMarker.c_str(), endMarker.size(),0);
            std::string completeMsg = "File transfer complete\n";
            send(session.receiverId, completeMsg.c_str(), completeMsg.size(), 0);
            //// send msg to sender
            send(session.senderId, "File sent successfully\n", 23, 0);
        }

        void handelSend(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args)
        {
            if(args.size() < 2)
            {
                std::string respose = "Usage: /send <recipient> <filename>\n";
                send(client.getClient(), respose.c_str(), respose.size(), 0);
                return;
            }

            std::string recipeintNick = args[0];
            std::string fileName = args[1];
            //// read file
            std::vector<char> fileData = readBinaryFile(fileName);
            if(fileData.empty())
            {
                std::string response = "Error : cannot read file\n";
                send(client.getClient(), response.c_str(), response.size(), 0);
                return;
            }
            //// find recepient
            IrcClient *recepient = irc.findClientByNick(recipeintNick);
            if(!recepient)
            {
                std::string response = "Error : User not found\n";
                send(client.getClient(), response.c_str(), response.size(), 0);
                return;
            }
            //// start session
            FileTransferSession session(client.getClient(), recepient->getClient(), fileName);
            session.fileData = fileData;
            session.totalSize = fileData.size();
            pendingTransfers.push_back(session);

            int recieverFd = recepient->getClient();
            //// notify recepeint
            std::string response = buildTransferRequest(client.getNick(), fileName, fileData.size());
            send(recieverFd, response.c_str(), response.size(), 0);
            //// notify sender
            std::string msgForSender = "File transfer request sent\n";
            send(client.getClient(), msgForSender.c_str(), msgForSender.size(), 0);
        }

        void handelAccept(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args)
        {
            if(args.empty())
            {
                // acccept without id 
            }
            else
                std::string transferId = args[0];
                // accept with specific id
        }

        void handelDecline(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args)
        {
            if(args.empty())
                std::cout << "here\n";
            else
                std::string transferId = args[0];
            ////////////////////
        }

        void handelRecieve(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args)
        {
            std::string fileName = args.empty() ? "recieved file" : args[0];
            //////////////////

        }
};