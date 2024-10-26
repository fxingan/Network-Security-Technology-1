#include "include/des.h"
#include <iostream>
#include <string>
#include "sys/socket.h"
#include "unistd.h"
#include "arpa/inet.h"
using namespace std;

const string key = "12345678";
const int bufferSize = 1024;

bool SendMessage(int sockfd, const string &data, des &des) {
    string encrypted_data = des.encode(data);
    size_t size = encrypted_data.size();
    size_t total = 0;
    while (total < size) {
        size_t wr = write(sockfd, encrypted_data.data() + total, size - total);
        if (wr < 0) {
            perror("write error");
            return false;
        }
        total += wr;
    }
    return true;
}

bool ReceiveMessage(int sockfd, string &data, des &des) {
    char buffer[bufferSize];
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_read = read(sockfd, buffer, sizeof(buffer));
    if (bytes_read < 0) {
        perror("read error");
        return false;
    }
    string received_data(buffer, bytes_read);
    data = des.decode(received_data);
    return true;
}

void BeginChat(int sockfd, des &des) {
    while (true) {
        string reply;
        getline(cin, reply);       
        if (!SendMessage(sockfd, reply, des)) {
            cout << "Error in sending data." << endl;
            exit(1);
        }
        string server_response;
        if (!ReceiveMessage(sockfd, server_response, des)) {
            cout << "Error in receiving data." << endl;
            exit(1);
        }
        cout << "Server: " << server_response << endl;
        if (reply == "-exit") {
            cout << "Exiting chat." << endl;
            exit(0);
        }
    }
}

int main(int argc, const char *argv[]) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <ip> <port>" << endl;
        return 1;
    }
    des des;
    des.setKey(key);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket error");
        return 1;
    }
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &serveraddr.sin_addr.s_addr);

    if (connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("connect error");
        return 1;
    }
    cout << "Connected to server!" << endl;
    string command;
    while (true) {
        cin >> command;

        if (command == "-chat") {
            BeginChat(sockfd, des);
        } else if (command == "-exit") {
            SendMessage(sockfd, command, des);
            cout << "Exiting program." << endl;
            break;
        } else {
            cout << "Invalid command." << endl;
        }
    }
    close(sockfd);
    return 0;
}