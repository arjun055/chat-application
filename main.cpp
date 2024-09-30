#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<map>
#include<ctime>

//user class
class User{
protected:
    std::string username;
    std::string status;
    std::string password;
    std::vector<std::string> messageHistory;

public:
    User(const std::string &uname,const std::string &pwd):username(uname),password(pwd),status("online"){}

    virtual void sendMessage(const std::string &message){
        std::cout<<username<<" sends message: "<<message<<std::endl;
        messageHistory.push_back("Sent: "+message);
    }

    virtual void receiveMessage(const std::string &message){
        std::cout<<username<<" received: "<<message<<std::endl;
        messageHistory.push_back("Received: "+message);
    }

    virtual ~User(){
        std::cout<<"User "<<username<<" is destructed"<<std::endl;
    }

    void setStatus(const std::string &newStatus){
        status=newStatus;
    }

    std::string getStatus() const{
        return status;
    }

    std::string getUserName() const{
        return username;
    }

    bool verifyPassword(const std::string &pwd) const{
        return pwd==password;
    }

    virtual std::string getRole() const{
        return "User";
    }

    void printMessageHistory() const{
        std::cout<<username<<"'s Message History:\n";
        for(const std::string &msg:messageHistory){
            std::cout<< msg <<std::endl;
        }

    }

    virtual bool hasAdminPrivileges() const{
        return false;
    }

};


//Derived RegisteredUser class
class RegisteredUser : public User{
public:
    RegisteredUser(const std::string &uname,const std::string &pwd):User(uname,pwd){}

    void sendMessage(const std::string &message) override{
        std::cout<<" [Registered User] "<<username<<" sends: "<<message<<std::endl;
        messageHistory.push_back("Sent: "+message);//log the message
    }

    std::string getRole() const override{
        return "RegisteredUser";
    }
};


//Deived Admin class
class Admin : public User{
public:
    Admin(const std::string &uname, const std::string &pwd):User(uname,pwd){}

    void sendMessage(const std::string &message) override{
        std::cout<<"[Admin] "<<username<<" sends (with Authority) "<<message<<std::endl;
        messageHistory.push_back("Sent: "+message);//log the messaage
    }

    void removeUser(class ChatRoom &room, User *user);

    std::string getRole() const override{
        return "Admin";
    }

    bool hasAdminPrivileges() const override{
        return true;
    }
};

//message class
class Message{
    std::string content;
    std::string timestamp;
    User &sender;

public:
    Message(const std::string &msgContent,const std::string &msgTime,User &msgSender)
    :content(msgContent),timestamp(msgTime),sender(msgSender){}

    friend void displayMessageInfo(const Message &msg);

    std::string getFormmatedMessage() const{
        return sender.getUserName()+"["+timestamp+"]"+content;
    }
};

void displayMessageInfo(const Message &msg){
    std::cout<<"Message sent by: "<<msg.sender.getUserName()<<std::endl;
    std::cout<<"Message content: "<<msg.content<<std::endl;
}


//chatRoom class
class ChatRoom{
    std::string roomName;
    std::vector<User *> userList;

public:
    ChatRoom(const std::string &name):roomName(name){}

    void addUser(User *user){
        userList.push_back(user);
        std::cout<<user->getUserName()<<" has joined the chat room: "<<roomName<<std::endl;
    }

    void removeUser(User *user){
        userList.erase(std::remove(userList.begin(),userList.end(),user),userList.end());
        std::cout<<user->getUserName()<<" has left the chat room: "<<roomName<<std::endl;
    }

    void broadcastMessage(const Message &message,User *sender){
        for(User *user : userList){
            if(user!=sender){
                user->receiveMessage(message.getFormmatedMessage());
            }
            
        }
    }

    void sendPrivateMessage(User *sender,User *receiver,const std::string &content){
        std::string privateMessage = "(Private)"+sender->getUserName()+" : "+content;
        sender->sendMessage(privateMessage);
        receiver->receiveMessage(privateMessage); 
    }

    std::string getRoomName() const{
        return roomName;
    }

    int getUserCount() const{
        return userList.size();
    }

    std::vector<User *> getUserList() const{
        return userList;
    }

};



//chatSystem class to handle multiple chat rooms
class ChatSystem{
    std::map<std::string, User *> users;
    std::map<std::string, ChatRoom *> chatRooms;
    User *loggedInUser=nullptr;

public:
    ~ChatSystem(){
        for(auto &[uname,user]:users){
            delete user;
        }

        for(auto &[rname,room]:chatRooms){
            delete room;
        }
    }

    void registerUser(const std::string &username, const std::string &password, bool isAdmin=false){
        if(users.find(username)==users.end()){
            if(isAdmin){
                users[username]= new Admin(username,password);
            }else{
                users[username] = new RegisteredUser(username,password);
            }
            std::cout << username << " has been registered successfully." << std::endl;
        }else{
            std::cout << "Error: Username already exists!" << std::endl;
        }
    }

    bool loginUser(const std::string &username, const std::string &password){
        if(users.find(username)!=users.end()){
            if(users[username]->verifyPassword(password)){
                loggedInUser=users[username];
                std::cout << username << " logged in successfully." << std::endl;
                return true;
            }else{
                std::cout<<"Error: Incorrect password!"<<std::endl;
            }
        }else{
            std::cout<<"Error: User not found!"<<std::endl;
        }
        return false;
    }

    void logoutUser(){
        if(loggedInUser){
            std::cout << loggedInUser->getUserName() << " has logged out." << std::endl;
            loggedInUser = nullptr;
        }else{
            std::cout << "Error: No user is currently logged in!" << std::endl;
        }
    }

    void createChatRoom(const std::string &roomname){
        if(chatRooms.find(roomname)==chatRooms.end()){
            chatRooms[roomname]=new ChatRoom(roomname);
            std::cout<<"Chat Room "<<roomname<<" has been created."<<std::endl;
        }else{
            std::cout<<"Chat Room "<<roomname<<" already exists."<<std::endl;
        }
    }

    void joinChatRoom(const std::string &roomname){
        if(loggedInUser){
            if(chatRooms.find(roomname)!=chatRooms.end()){
                chatRooms[roomname]->addUser(loggedInUser);
            }else{
                std::cout<<"Chat Room "<<roomname<<" doesn't exist"<<std::endl;
            }
        }else{
            std::cout << "Error: No user is logged in!" << std::endl;
        } 
    }

    void sendGroupMessage(const std::string &roomname, const std::string &message){
        if(loggedInUser){
            if(chatRooms.find(roomname)!=chatRooms.end()){
                std::string timestamp = getCurrentTime();
                Message msg(message,timestamp,*loggedInUser);
                chatRooms[roomname]->broadcastMessage(msg,loggedInUser);
            }
            else{
                std::cout << "Error: Chat room " << roomname << " doesn't exist!" << std::endl;
            }
        }else{
            std::cout << "Error: No user is logged in!" << std::endl;
        }
    }


    void removeUserFromChatRoom(const std::string &roomname,const std::string &username){
        if(loggedInUser && loggedInUser->hasAdminPrivileges()){
            if(chatRooms.find(roomname)!=chatRooms.end() && users.find(username)!=users.end()){
                chatRooms[roomname]->removeUser(users[username]);
            }else{
                std::cout << "Error: Invalid room or user." << std::endl;
            }
        }else{
            std::cout << "Error: Only admins can remove users!" << std::endl;
        }
    }



    void leaveChatRoom(const std::string &roomname,User *user){
        if(chatRooms.find(roomname)!=chatRooms.end()){
            chatRooms[roomname]->removeUser(user);
        }else{
            std::cout<<"Chat Room "<<roomname<<" doesn't exist"<<std::endl;
        }
    }


    void listChatRooms() const{
        std::cout<<"Available Chat Rooms: \n";
        for(auto &[name,room]:chatRooms){
            std::cout<<"- "<<name<<" ("<<room->getUserCount()<<" users)\n";
        }
    }

private:
    std::string getCurrentTime(){
    time_t now = time(0);
    char* dt = ctime(&now);
    std::string timeString(dt);
    return timeString.substr(0,timeString.size()-1);
}

};

void Admin::removeUser(ChatRoom &room,User *user){
    room.removeUser(user);
    std::cout<<"Admin "<<username<<" removed "<<user->getUserName()<<" from the chat room."<<std::endl;
}



int main(){
    
    ChatSystem system;

    system.registerUser("Alice","alice123");
    system.registerUser("Bob","bob456");
    system.registerUser("Charlie","adminpass",true);

    if(system.loginUser("Charlie","adminpass")){

        system.createChatRoom("General");
        system.createChatRoom("Sports");

        system.listChatRooms();

        system.loginUser("Alice","alice123");
        system.joinChatRoom("General");

        system.loginUser("Bob","bob456");
        system.joinChatRoom("General");

        system.loginUser("Charlie", "adminpass");
        system.removeUserFromChatRoom("General", "Bob");

        system.loginUser("Alice", "alice123");
        system.sendGroupMessage("General", "Hello everyone!");

    }

    



    

    // std::string inputMessage;

    // while(room.getUserCount()>0){
    //     std::cout<<"\nType a message (or type 'exit' to leave the chat)";
    //     std::getline(std::cin,inputMessage);

    //     if(inputMessage=="exit"){
    //         room.removeUser(&user1);
    //         break;
    //     }

    //     Message msg(inputMessage,getCurrentTime(),user1);

    //     room.broadcastMessage(msg);

    //     displayMessageInfo(msg);
    // }

    

    // admin.removeUser(room,&user2);

    // Message msg1("Hello, everyone!", getCurrentTime() , user1);
    // room.broadcastMessage(msg1);

    // room.sendPrivateMessage(&admin,&user1,"This is a private message to alice");

    // Message msg2("I am the Admin here!", getCurrentTime() , admin);
    // room.broadcastMessage(msg2);

    // admin.printMessageHistory();

    
    return 0;
}





