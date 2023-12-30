#pragma once

#include "txpool.h"

class TxPoolServer {
public:
  TxPoolServer(std::shared_ptr<TxPool> txpool, std::string host, int port, const std::string& txpoolSnapshotFile)
    : txpool(txpool), host(host), port(port), txpoolSnapshotFile(txpoolSnapshotFile) {}
  void run();
  void stop();
private:
  std::shared_ptr<TxPool> txpool;
  std::string host;
  int port;
  std::string txpoolSnapshotFile;
  bool stopServer;
};
