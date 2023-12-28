#pragma once

#include "txpool.h"

class TxPoolServer {
public:
  TxPoolServer(std::shared_ptr<TxPool> txpool, std::string host, int port)
    : txpool(txpool), host(host), port(port) {}
  void run();
private:
  std::shared_ptr<TxPool> txpool;
  std::string host;
  int port;
};
