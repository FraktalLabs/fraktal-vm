#include "txpool_server.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <chrono>
#include <sys/types.h>

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <functional>

void TxPoolServer::run() {
  int server_fd, new_socket;
  long valread;
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  int flags = fcntl(server_fd, F_GETFL, 0);
  fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

  // TODO: use host provided
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  memset(address.sin_zero, '\0', sizeof address.sin_zero);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 10) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  std::cout << "Waiting for connection..." << std::endl;
  stopServer = false;
  while (stopServer == false) {
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&addrlen)) < 0) {
      if (errno == EWOULDBLOCK || errno == EAGAIN) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        continue;
      } else {
        close(server_fd);
        close(new_socket);
        perror("accept");
        exit(EXIT_FAILURE);
      }
    }
    std::cout << "Connection accepted" << std::endl;

    char buffer[1024] = {0};
    valread = read(new_socket, buffer, 1024);

    // Parse buffer as HTTP request
    buffer[valread] = '\0';
    std::string request(buffer);
    // Get data from request
    std::string data = request.substr(request.find("\r\n\r\n") + 4);
    std::cout << "Data: " << data << std::endl;

    // Add tx to pool
    txpool->addTx(std::make_shared<Transaction>(data));

    std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
    std::string html = "<html><body><h1>Hello, World!</h1></body></html>";
    response += html;

    send(new_socket, response.c_str(), response.size(), 0);
    std::cout << "Response sent" << std::endl;
    close(new_socket);

    std::cout << "Waiting for connection..." << std::endl;
  }
  close(server_fd);
}

void TxPoolServer::stop() {
  stopServer = true;
}
