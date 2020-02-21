//lang::CwC

// Much of the code in this file was interpreted from Beej's Guide to Socking Programming

#pragma once

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "object.h"
#include "helper.h"

#define PORT "8080"
// The fixed number of clients that this network supports 
#define BACKLOG 5
#define SERVER_IP "127.0.0.1"
// The size of the string buffer used to send messages
#define BUF_SIZE 1024

/**
 * Wrapper abstract class for a C socket.
 * Contains fields that all subclassses could theoretically use.
 * 
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 */
class Socket : public Object {
    public:
        char* ip_;
        // This Server's socket file descriptor and address info
        int fd_;
        struct addrinfo* info_;
        // The file descriptor and address info of the socket connecting to this Server
        int their_fd_;
        struct sockaddr_storage their_info_;
        // A string buffer used to send messages
        char* buffer_;
        // An array of IP strings
        char** client_ips_;
};

/**
 * Class representing a server. Child of the Socket class.
 * 
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 */
class Server : public Socket {
    public:

        /**
         * Constructor for Server object.
         * 
         * @param ip The IP address of this Server. The Server takes control of the char*.
         */
        Server(char* ip) {
            buffer_ = new char[BUF_SIZE];
            client_ips_ = new char*[BACKLOG];
            ip_ = ip;
            // Fill in the addrinfo struct, configuring the server's options, address, and port
            struct addrinfo hints;
            memset(&hints, 0, sizeof hints);
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            exit_if_not(getaddrinfo(ip, PORT, &hints, &info_) == 0, "Call to getaddrinfo() failed");
            // Create the socket
            exit_if_not((fd_ = socket(info_->ai_family, info_->ai_socktype, info_->ai_protocol)) >= 0, "Call to socket() failed");
            // Bind the Server's IP and port to the socket
            exit_if_not(bind(fd_, info_->ai_addr, info_->ai_addrlen) >= 0, "Call to bind() failed");
            freeaddrinfo(info_);
        }

        /**
         * Destructor
         * Closes the Server's socket to the Clients and deletes all fields.
         */
        ~Server() {
            close(fd_);
            delete ip_;
            delete buffer_;
            delete client_ips_;
        }

        /**
         * Listens for incoming connections from new clients.
         * Clients send IPs to server who then forwards them to its other clients.
         */
        void register_clients() {
            fd_set master;      // master file descriptor list
            fd_set read_fds;    // temporary fd list used by select()
            int fdmax;          // the maxmimum fd value in the master list
            socklen_t addrlen;  // length of client's addrinfo struct
            int nbytes;         // number of bytes read by recv()
            int ip_arr_idx = 0; // The index that we're at while iterating through the array of client ip strings
            // Clear the two fd lists
            FD_ZERO(&master);
            FD_ZERO(&read_fds);
            // Start listening
            exit_if_not(listen(fd_, BACKLOG) == 0, "Call to listen() failed");
            // Add this Server's fd to the master list and use it to initialize the max fd value
            FD_SET(fd_, &master);
            fdmax = fd_;
            // Main loop
            for (;;) {
                read_fds = master; // Copy the master list
                // Select the existing socket connections and iterate through them
                exit_if_not(select(fdmax + 1, &read_fds, NULL, NULL, NULL) >= 0, "Call to select() failed");
                for (int i = 0; i <= fdmax; i++) {
                    if (FD_ISSET(i, &read_fds)) {
                        // Found a connection
                        if (i == fd_) {
                            // The Server is receiving a new connection
                            addrlen = sizeof(their_info_);
                            // Accept it and add its fd to the master list
                            exit_if_not((their_fd_ = accept(fd_, (struct sockaddr*)&their_info_, &addrlen)), "Call to accept() failed");
                            FD_SET(their_fd_, &master);
                            // Update the max fd value
                            if (their_fd_ > fdmax) {
                                fdmax = their_fd_;
                            }
                            printf("Server %s: New connection on socket %d\n", ip_, their_fd_);
                        } else {
                            // The Server is receving an message from a client
                            if ((nbytes = recv(i, buffer_, BUF_SIZE, 0)) <= 0) {
                                // Connection to the client was closed or there was an error
                                if (nbytes == 0) {
                                    printf("Server %s: Socket %d hung up\n", ip_, i);
                                } else {
                                    exit_if_not(false, "Call to recv() failed");
                                }
                                close(i);
                                // Remove the bad client from the master list
                                FD_CLR(i, &master);
                            } else {
                                printf("Server %s: Received IP \"%s\" from new client at socket %d\n", ip_, buffer_, i);
                                // Add the new IP to the array maintained by the Server
                                if (ip_arr_idx >= BACKLOG) {
                                    // If the array is full, wrap around and replace older entries
                                    ip_arr_idx = 0;
                                }
                                client_ips_[ip_arr_idx] = buffer_;
                                // Forward the message to all other sockets that aren't this server's or the one that
                                // originally sent the message
                                for (int j = 0; j <= fdmax; j++) {
                                    if (FD_ISSET(j, &master)) {
                                        if (j != fd_ && j != i) {
                                            printf("Server %s: Sending new client IP \"%s\" to existing client at socket %d\n", 
                                                ip_, buffer_, j);
                                            exit_if_not(send(j, buffer_, nbytes, 0) > 0, "Call to send() failed");
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
};

/**
 * Class representing a client. Child of the Socket class.
 * 
 * @author Spencer LaChance <lachance.s@husky.neu.edu>
 * @author David Mberingabo <mberingabo.d@husky.neu.edu>
 */
class Client : public Socket {
    public:
        struct addrinfo* servinfo_;
        int servfd_;

        /**
         * Constructor for Client object.
         * 
         * @param ip The IP address of this Client. The Client takes control of the char*.
         */
        Client(char* ip) {
            buffer_ = new char[BUF_SIZE];
            client_ips_ = new char*[BACKLOG];
            ip_ = ip;
            // Fill in 2 addrinfo structs, one for this Client and one for the Server
            struct addrinfo hints;
            memset(&hints, 0, sizeof hints);
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            exit_if_not(getaddrinfo(SERVER_IP, PORT, &hints, &servinfo_) == 0, "Call to getaddrinfo() failed");
            // Create the socket to the Server
            exit_if_not((servfd_ = socket(servinfo_->ai_family, servinfo_->ai_socktype, servinfo_->ai_protocol)) >= 0,
                "Call to socket() failed");
            // Connect to the Server
            printf("Client %s: Connecting to server\n", ip_);
            exit_if_not(connect(servfd_, servinfo_->ai_addr, servinfo_->ai_addrlen) >= 0, "Call to connect() failed");
            freeaddrinfo(servinfo_);
        }

        /**
         * Destructor
         * Closes the Client's socket to the Server and deletes all fields.
         */
        ~Client() {
            close(servfd_);
            delete ip_;
            delete buffer_;
            delete client_ips_;
        }

        void register_with_server() {
            // Send IP to server
            printf("Client %s: Sending my IP \"%s\" to the server for registration.\n", ip_, ip_);
            exit_if_not(send(servfd_, ip_, strlen(ip_), 0) > 0, "Registering IP with server failed");
            // Receive new client IPs from server
            char *client_ip;
            int nbytes;
            while (true) {
                for (int i = 0; i < BACKLOG - 1; i++) {
                    if ((nbytes = recv(servfd_, buffer_, BUF_SIZE, 0)) <= 0) {
                        if (nbytes == 0) {
                            printf("Client %s: Server hung up\n", ip_);
                            return;
                        } else {
                            exit_if_not(false, "Call to recv() failed");
                        }
                        close(servfd_);
                    } else {
                        printf("Client %s: Received new client IP \"%s\" from server\n", ip_, buffer_);
                        strncpy(client_ip, buffer_, BUF_SIZE);
                        client_ips_[i] = client_ip;
                    }
                }
            }
        }
};