#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define PORT 11000
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE] = {0};
    const char *hello = "1000,2000,3000,4000,5000,6000;";

    // 创建 socket 文件描述符
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 设置 socket 地址和端口
    address.sin_family = AF_INET;
    // address.sin_addr.s_addr = inet_addr("192.168.255.10");
    address.sin_port = htons(PORT);

    // 绑定 socket 到指定的 IP 地址和端口
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 监听端口
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on 192.168.255.10:" << PORT << std::endl;

    std::cout << "Waiting for client connection..."<< std::endl;
    socklen_t addrlen = sizeof(address);
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    // 接受客户端连接
    for (int i = 0; i < 100; i++)
    {

        // 读取客户端消息
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        std::cout << "Message from client: " << buffer << std::endl;

        // 发送消息给客户端
        send(new_socket, hello, strlen(hello), 0);
        std::cout << "Hello message sent" << std::endl;


    }
        // 关闭 socket
        close(new_socket);
        close(server_fd);
        return 0;
    }
