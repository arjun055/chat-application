#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "chat_app.h" 

#define PORT 8080
#define BUFFER_SIZE 1024

// Handle client function with ChatSystem passed as a reference
void handleClient(int clientSocket, ChatSystem &chatSystem) {
    char buffer[BUFFER_SIZE];
    std::string response;
    std::string username;
    std::string chatRoomName;

    while (true) {
        memset(buffer, 0, sizeof(buffer));  // Clear buffer before receiving data
        int bytesRead = read(clientSocket, buffer, sizeof(buffer));

        if (bytesRead <= 0) {
            std::cout << "Client disconnected." << std::endl;
            close(clientSocket);
            break;
        }

        std::string command(buffer);
        command = command.substr(0, command.find("\n"));  // Strip out any newlines
        std::cout << "Server received command: " << command << std::endl;

        if (command == "REGISTER") {
            memset(buffer, 0, sizeof(buffer));  // Clear buffer for username
            read(clientSocket, buffer, sizeof(buffer));  // Receive username
            username = std::string(buffer).substr(0, std::string(buffer).find("\n"));

            memset(buffer, 0, sizeof(buffer));  // Clear buffer for password
            read(clientSocket, buffer, sizeof(buffer));  // Receive password
            std::string password = std::string(buffer).substr(0, std::string(buffer).find("\n"));

            chatSystem.registerUser(username, password, clientSocket);
            std::cout << "User " << username << " registered successfully." << std::endl;

            response = "REGISTERED\n";
            send(clientSocket, response.c_str(), response.size(), 0);

        } else if (command == "JOIN") {
            memset(buffer, 0, sizeof(buffer));  // Clear buffer for username
            read(clientSocket, buffer, sizeof(buffer));  // Receive username
            username = std::string(buffer).substr(0, std::string(buffer).find("\n"));

            memset(buffer, 0, sizeof(buffer));  // Clear buffer for password
            read(clientSocket, buffer, sizeof(buffer));  // Receive password
            std::string password = std::string(buffer).substr(0, std::string(buffer).find("\n"));

            // Verify the user's password
            if (chatSystem.users.find(username) != chatSystem.users.end() && 
                chatSystem.users[username]->verifyPassword(password)) {

                // ask for chatroom after verifying the password
                memset(buffer, 0, sizeof(buffer));  // Clear buffer for chat room name
                read(clientSocket, buffer, sizeof(buffer));  // Receive chat room name
                chatRoomName = std::string(buffer).substr(0, std::string(buffer).find("\n"));

                if (!chatSystem.chatRoomExists(chatRoomName)) {
                    chatSystem.createChatRoom(chatRoomName);  // Create the chat room if it doesn't exist
                    std::cout << "Chat room " << chatRoomName << " created." << std::endl;
                }

                chatSystem.addUserToRoom(username, chatRoomName);  // Add user to specified room,the method is defined in the chat_app.h
                std::cout << "User " << username << " joined room: " << chatRoomName << std::endl;

                response = "JOINED\n";
                send(clientSocket, response.c_str(), response.size(), 0);
            } else {
                response = "Invalid username or password!\n";
                send(clientSocket, response.c_str(), response.size(), 0);
            }

        } else if (command == "SEND") {
            memset(buffer, 0, sizeof(buffer));  // Clear buffer for message
            read(clientSocket, buffer, sizeof(buffer));  // Receive message
            std::string message = std::string(buffer).substr(0, std::string(buffer).find("\n"));
    
            std::cout << "Broadcasting message from " << username << ": " << message << std::endl;
    
            // Use ChatSystem to broadcast the message to the room, this method can be referred in the chat_app.h
            chatSystem.broadcastMessageToRoom(chatRoomName, message, username);
    
            response = "MESSAGE SENT\n";
            send(clientSocket, response.c_str(), response.size(), 0);
        }   

    }
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t clientAddrLen = sizeof(clientAddress);

    ChatSystem chatSystem;  //instance of ChatSystem class using which we can add user and create rooms.

    // create a socket for server
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt failed" << std::endl;
        close(serverSocket);
        return -1;
    }



    // Configure server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // Bind socket
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(serverSocket);
        return -1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        close(serverSocket);
        return -1;
    }

    std::cout << "Server is listening on port " << PORT << "..." << std::endl;

    // Create the default chatroom
    chatSystem.createChatRoom("default");


    // Accept clients and handle them in separate threads
    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddrLen);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        std::thread clientThread(handleClient, clientSocket, std::ref(chatSystem));
        clientThread.detach();  // Run client handling in a separate thread, using this it runs the thread independently.
    }

    return 0;
}


