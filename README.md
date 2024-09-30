# Chat Application using C++ (OOP + Client-Server Architecture)

## Description

This project is a **Chat Application** implemented using C++ that follows **Object-Oriented Programming (OOP)** principles for its core logic. It supports **real-time communication** using a **Client-Server architecture** and handles multiple clients using **multithreading**. 

The project consists of three main files:
- **chat_app.h**: Contains class definitions and the core logic.
- **server.cpp**: Implements the server-side logic, including handling multiple clients with threads.
- **client.cpp**: Implements the client-side logic for communicating with the server.
- **main.cpp**: Contains initial implementations and trial code for the chat application before moving the logic to `chat_app.h`.



## Features

- **OOP structure**: Modular and maintainable code structure using classes.
- **Real-time messaging**: Enables communication between multiple clients.
- **Threading**: Server uses threads to handle multiple clients concurrently.
- **Command-based interaction**: Users can join chat rooms, send messages, and exit the application.

## Setup & Installation

### Prerequisites

- **C++11** or later
- Knowledge of **socket programming** and **multithreading**
- A **Linux/Unix** environment is recommended for testing

### Installation Steps

1. **Clone the repository**:

    ```bash
    git clone https://github.com/arjun055/chat-application.git
    cd chat-application
    ```

2. **Compile the project**:

    If you have a Makefile:

    ```bash
    make
    ```

    Alternatively, compile the server and client manually:

    ```bash
    g++ -o server server.cpp -lpthread
    g++ -o client client.cpp
    ```

## How to Run

### 1. Start the Server

Run the server with a port number:

```bash
./server <port_number>
```


### 2. Run the Client(s)
To connect a client to the server, use:

```bash
./client <server_ip> <port_number>
```


### 3. Chat Commands
After connecting to server client can use following commands
1. REGISTER: Register a user on the server.
2. JOIN: Join a specified chat room.
3. SEND: Send messages to peers in the room.
4. EXIT: Exit the application.


Contributing
Contributions are welcome! Feel free to open an issue or submit a pull request if you want to improve the project.

