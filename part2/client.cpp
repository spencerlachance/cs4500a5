//lang::CwC

#include "network.h"

int main(int argc, char** argv) {
    Sys sys;
    sys.exit_if_not(argc == 3, "Incorrect number of command line arguments.");
    sys.exit_if_not(strcmp(argv[1], "-ip") == 0, "Incorrectly formatted command line arguments.");

    Client* client = new Client(argv[2]);
    client->register_with_server();
    delete client;
}