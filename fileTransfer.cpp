#include "fileTransfer.hpp"
#include "ircCore.hpp"
#include <fstream>
#include <sstream>

fileTransfer::FileTransferSession::FileTransferSession(int sender, int receiver, const std::string& fname)
: senderId(sender), receiverId(receiver), fileName(fname), totalSize(0)
{
}

fileTransfer::fileTransfer()
{
    validCommands.push_back("/send");
    validCommands.push_back("/accept");
    validCommands.push_back("/decline");
}

bool fileTransfer::isFileTransferCmd(const std::string &cmd)
{
    for(size_t i = 0; i < validCommands.size(); i++)
    {
        if(cmd == validCommands[i])
            return true;
    }
    return false;
}

std::vector<char> fileTransfer::readBinaryFile(const std::string &fileName)
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

std::string fileTransfer::buildTransferRequest(const std::string &sender, const std::string &fileName, size_t size)
{
    std::ostringstream oss;
    oss << "FILE_TRANSFER from " << sender << ": " << fileName << " (" << size << " bytes)\n" << "Use /accept or /decline\n";
    return oss.str();
}

std::string fileTransfer::buildFileStartHeader(const std::string &fileName, size_t size)
{
    std::ostringstream oss;
    validCommands.push_back("/recieve");
    oss << "FILE_START " << fileName << " " << size  << "\n";
    return oss.str();
}

void fileTransfer::handelfileTransferCmd(IrcServer &irc ,IrcClient &client,const cmd &command)
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
};

void fileTransfer::startTransfer(FileTransferSession &session)
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

void fileTransfer::handelSend(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args)
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

void fileTransfer::handelAccept(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args)
{
    (void)irc;
    (void)args;
    int clientFd = client.getClient();
    for(size_t i = 0 ; i < pendingTransfers.size(); i++)
    {
        if(pendingTransfers[i].receiverId == clientFd)
        {
            FileTransferSession session = pendingTransfers[i];
            std::string response = "file transfer starting...\n";
            send(client.getClient(), response.c_str(), response.size(), 0);
            send(session.senderId, "Sending file...\n", 16, 0);
            startTransfer(session);
            pendingTransfers.erase(pendingTransfers.begin() + i);
            return;
        }
    }
    std::string response = "Error : no pending file transfer\n";
    send(clientFd, response.c_str(), response.size(), 0);
}

void fileTransfer::handelDecline(IrcServer &irc, IrcClient &client, const std::vector<std::string> &args)
{
    (void)irc;
    (void)args;
    int  clientFd = client.getClient();
    for(size_t i = 0; i < pendingTransfers.size(); i++)
    {
        if(pendingTransfers[i].receiverId == clientFd)
        {
            FileTransferSession session = pendingTransfers[i];
            std::string response = "File transfer declined\n";
            send(clientFd, response.c_str(), response.size(), 0);
            std::string senderMsg = "File transfer declined by recipient\n";
            send(session.senderId, senderMsg.c_str(), senderMsg.size(), 0);
            pendingTransfers.erase(pendingTransfers.begin() + i);
            return;
        }
    }
    std::string response = "Error : no pending file transfer\n";
    send(clientFd, response.c_str(), response.size(), 0);
}


///////////////////////////////////////////////////////////////////////
// test method
#include "ircCore.hpp"
#include <fstream>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

void fileTransfer::testSendFile(IrcServer &irc, const std::string &filename)
{
    const std::string sendPath = "/home/ytabia/Desktop/";
    const std::string recvPath = "/home/ytabia/";

    IrcClient* sender = irc.findClientByNick("TestUser4");
    IrcClient* receiver = irc.findClientByNick("TestUser5");

    if (!sender || !receiver) {
        std::cout << "Test clients not found\n";
        return;
    }

    int port = 9000;

    // receiver thread
    std::thread recvThread([filename, recvPath, port]() {
        int listenSock = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        bind(listenSock, (sockaddr*)&addr, sizeof(addr));
        listen(listenSock, 1);

        int clientSock = accept(listenSock, nullptr, nullptr);

        std::ofstream out(recvPath + "received_" + filename, std::ios::binary);
        char buffer[4096];
        int bytes;
        while ((bytes = recv(clientSock, buffer, sizeof(buffer), 0)) > 0)
            out.write(buffer, bytes);

        close(clientSock);
        close(listenSock);
        std::cout << "✅ File saved to " << recvPath << "received_" << filename << "\n";
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // sender
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    connect(sock, (sockaddr*)&addr, sizeof(addr));

    std::ifstream file(sendPath + filename, std::ios::binary);
    if (!file) {
        std::cerr << "❌ Could not open file: " << sendPath + filename << "\n";
        return;
    }

    char buf[4096];
    while (file.read(buf, sizeof(buf)))
        send(sock, buf, file.gcount(), 0);
    send(sock, buf, file.gcount(), 0);

    close(sock);
    std::cout << "✅ File sent by " << sender->getNick() << " from " << sendPath << "\n";

    recvThread.join();
}

////////////////////////////////////////////////////////////////////////////////////