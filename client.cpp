#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to handle receiving messages from the server
void receiveMessages(int sock) {
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = read(sock, buffer, sizeof(buffer));
        if (bytesRead > 0) {
            std::cout << "\nMessage from server: " << buffer << std::endl;
        }
    }
}

//Function to send commands to server
void sendCommand(int sock) {
    char buffer[BUFFER_SIZE];
    std::string command;

    while (true) {
        std::cout << "Enter Command (REGISTER, JOIN, SEND, EXIT): ";
        std::getline(std::cin, command);

        if (command == "EXIT") {
            std::cout << "Exiting..." << std::endl;
            close(sock);
            break;
        }

        send(sock, command.c_str(), command.length(), 0);

        if (command == "REGISTER") {
            // Register flow
            std::string username, password;

            std::cout << "Enter username: ";
            std::getline(std::cin, username);
            send(sock, username.c_str(), username.length(), 0); //Sends username entered

            std::cout << "Enter password: ";
            std::getline(std::cin, password);
            send(sock, password.c_str(), password.length(), 0); //Sends password entered
        } else if (command == "JOIN") {
            // Join flow
            std::string username, password, roomName;

            std::cout << "Enter username: ";
            std::getline(std::cin, username);
            send(sock, username.c_str(), username.length(), 0);

            std::cout << "Enter password: ";
            std::getline(std::cin, password);
            send(sock, password.c_str(), password.length(), 0);

            std::cout << "Enter chat room name: ";
            std::getline(std::cin, roomName);
            send(sock, roomName.c_str(), roomName.length(), 0);
        } else if (command == "SEND") {
            // Send flow
            std::string message;

            std::cout << "Enter message: ";
            std::getline(std::cin, message);
            send(sock, message.c_str(), message.length(), 0);
        }


        // No need to wait for a response here as we handle receiving in a separate thread

        //If the application wasn't a real-time, we shoudl have to extract 
        //the response sent from the server here, which is not the correct way to build the project.

        //So a separate thread should be created for each client to just handle the incoming messages thus divides the job of outgoing and incoming.
        //NOTE : Shoudl know (Multi - threading) to make this possible. Refer line no.168 & 169.
    }
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    //Create a socket for client
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address / Address not supported" << std::endl;
        return -1;
    }

    //Connects the client socket to server using the server address specified.
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return -1;
    }

    // Create a separate thread to listen for incoming messages from the server.
    // using this thread makes the application real-time. Each client instance keeps listening for the incoming messages.
    // so regardless of command entered by the current client, messages from other clients are received.

    std::thread receiveThread(receiveMessages, sock);
    receiveThread.detach();  // Detach the thread to allow it to run independently

    // Send commands to sever
    sendCommand(sock);

    return 0;
}








