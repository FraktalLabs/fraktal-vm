#include "txpool.h"

#include <fstream>

TxPool::TxPool(const std::string& snapshotFile) {
  std::ifstream snapshot(snapshotFile);
  if (snapshot.is_open()) {
    std::string line;
    while (getline(snapshot, line)) {
      std::shared_ptr<Transaction> tx = std::make_shared<Transaction>(line);
      txMap[tx->getHash()] = tx;
      txQueue.push(tx->getHash());
    }
    snapshot.close();
  }
}

void TxPool::addTx(std::shared_ptr<Transaction> tx) {
  txMap[tx->getHash()] = tx;
  txQueue.push(tx->getHash());
}

std::shared_ptr<Transaction> TxPool::getTx(const uint256& txHash) {
  return txMap[txHash];
}

std::shared_ptr<Transaction> TxPool::popTx() {
  txQueueMutex.lock();
  uint256 txHash = txQueue.front();
  txQueue.pop();
  txQueueMutex.unlock();
  return txMap[txHash];
}

void TxPool::snapshot(const std::string& snapshotFile) {
  std::ofstream snapshot(snapshotFile);
  for (auto& tx : txMap) {
    snapshot << tx.second->toString() << std::endl;
  }
}

// TODO: Remove tx from map after tx is consumed
