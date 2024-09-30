#ifndef CHAT_APP_H
#define CHAT_APP_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <ctime>
#include <sys/socket.h>
#include <unistd.h>


//This file contains the CORE LOGIC built for the Chat-Application.
//Has four classes 1.User 2.Message 3.ChatRoom 4.ChatSystem


//Base User Class
class User {
    int socket_fd;  // Socket file descriptor for communication
protected:
    std::string username;
    std::string status;
    std::string password;
    std::vector<std::string> messageHistory;

public:
    User(const std::string &uname, const std::string &pwd, int fd)
        : username(uname), password(pwd), status("online"), socket_fd(fd) {}

    virtual void sendMessage(const std::string &message) {
        std::string formattedMessage = username + ": " + message + "\n";
        send(socket_fd, formattedMessage.c_str(), formattedMessage.length(), 0);
        messageHistory.push_back("Sent: " + message);
    }

    virtual void receiveMessage(const std::string &message) {
        send(socket_fd, message.c_str(), message.length(), 0);
        messageHistory.push_back("Received: " + message);
    }

    std::string getUserName() const {
        return username;
    }

    int getSocketFD() const {
        return socket_fd;
    }

    bool verifyPassword(const std::string &pwd) const{
        return pwd==password;
    }

    virtual ~User() {
        close(socket_fd);  // Close the socket when user is destructed
    }
};


//RegisteredUser class
class RegisteredUser : public User {
public:
    RegisteredUser(const std::string &uname, const std::string &pwd, int fd)
        : User(uname, pwd, fd) {}
};


//Admin class
class Admin : public User {
public:
    Admin(const std::string &uname, const std::string &pwd, int fd)
        : User(uname, pwd, fd) {}

    void removeUser(class ChatRoom &room, User *user);
};


//Message class
class Message {
    std::string content;
    std::string timestamp;
    User &sender;

public:
    Message(const std::string &msgContent, const std::string &msgTime, User &msgSender)
        : content(msgContent), timestamp(msgTime), sender(msgSender) {}

    std::string getFormattedMessage() const {
        return sender.getUserName() + "[" + timestamp + "]: " + content;
    }
};



//ChatRoom class
class ChatRoom {
    std::string roomName;
    std::vector<User *> userList;

public:
    ChatRoom(const std::string &name) : roomName(name) {}

    void addUser(User *user) {
        userList.push_back(user);
        std::string message = user->getUserName() + " has joined the room: " + roomName + "\n";
        for (User *u : userList) {
            u->receiveMessage(message);
        }
    }

    void broadcastMessage(const Message &message, User *sender) {
        for (User *user : userList) {
            if (user != sender) {
                user->receiveMessage(message.getFormattedMessage());
            }
        }
    }
};


//ChatSystem class
class ChatSystem {
    

public:
    std::map<std::string, User *> users;
    std::map<std::string, ChatRoom *> chatRooms;
    
    void registerUser(const std::string &username, const std::string &password, int socket_fd, bool isAdmin = false) {
        if (isAdmin) {
            users[username] = new Admin(username, password, socket_fd);
        } else {
            users[username] = new RegisteredUser(username, password, socket_fd);
        }
    }

    void createChatRoom(const std::string &roomname) {
        chatRooms[roomname] = new ChatRoom(roomname);
    }

    void addUserToRoom(const std::string &username, const std::string &roomname) {
        if (users.find(username) != users.end() && chatRooms.find(roomname) != chatRooms.end()) {
            chatRooms[roomname]->addUser(users[username]);
        }
    }

    void broadcastMessageToRoom(const std::string &roomname, const std::string &message, const std::string &senderName) {
        if (chatRooms.find(roomname) != chatRooms.end() && users.find(senderName) != users.end()) {
            std::string timestamp = getCurrentTime();
            Message msg(message, timestamp, *users[senderName]);
            chatRooms[roomname]->broadcastMessage(msg, users[senderName]);
        }
    }

    bool chatRoomExists(const std::string &roomname){
        return chatRooms.find(roomname)!=chatRooms.end();
    }

    ~ChatSystem() {
        for (auto &[name, user] : users) {
            delete user;
        }
        for (auto &[name, room] : chatRooms) {
            delete room;
        }
    }

private:
    std::string getCurrentTime() {
        time_t now = time(0);
        char *dt = ctime(&now);
        std::string timeString(dt);
        return timeString.substr(0, timeString.size() - 1);
    }
};

#endif //CHAT_APP_H




