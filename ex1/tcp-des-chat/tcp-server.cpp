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

string ReceiveMessage(int sockfd, des &des) {
    char buffer[bufferSize];
    size_t bytes_read = read(sockfd, buffer, sizeof(buffer));
    if (bytes_read < 0) {
        perror("read error");
        exit(1);
    }
    string received_data(buffer, bytes_read);
    return des.decode(received_data);
}

void HandleClient(int client_socket, des &des) {
    cout << "Client connected." << endl;
    while (true) {
        string received_data = ReceiveMessage(client_socket, des);
        if (received_data == "-exit")
            break;
        else if (received_data == "-call")
            cout << "Incoming call! Please answer!" << endl;
        else
            cout << "Client: " << received_data << endl;
        
        string reply;
        getline(cin, reply);
        if (!SendMessage(client_socket, reply, des)) {
            cout << "Error in sending data." << endl;
            exit(1);
        }
    }
    close(client_socket);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <port>" << endl;
        return 1;
    }

    des des;
    des.setKey(key);

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket error");
        return 1;
    }

    struct sockaddr_in server_addr, client_addr;
    int port = stoi(argv[1]);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind error");
        return 1;
    }

    if (listen(server_socket, 10) < 0) {
        perror("listen error");
        return 1;
    }

    cout << "Listening on port ..." << port << endl;

    socklen_t client_addr_len = sizeof(client_addr);
    int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket < 0) {
        perror("accept error");
        return 1;
    }

    HandleClient(client_socket, des);

    close(server_socket);
    return 0;
}