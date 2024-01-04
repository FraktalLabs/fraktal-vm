#include "txpool.h"

#include <fstream>
#include <iostream>

TxPool::TxPool(const std::string& snapshotFile) {
  std::cout << "Loading txpool from " << snapshotFile << std::endl;
  std::ifstream snapshot(snapshotFile);
  if (snapshot.is_open()) {
    std::string line;
    while (getline(snapshot, line)) {
      std::cout << "Loading tx: " << line << std::endl;
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

void TxPool::removeTx(const uint256& txHash) {
  txMap.erase(txHash);
}

std::shared_ptr<Transaction> TxPool::getTx(const uint256& txHash) {
  return txMap[txHash];
}

std::shared_ptr<Transaction> TxPool::popTx() {
  txQueueMutex.lock();
  if (txQueue.empty()) {
    txQueueMutex.unlock();
    return nullptr;
  }
  uint256 txHash = txQueue.front();
  txQueue.pop();
  txQueueMutex.unlock();
  return txMap[txHash];
}

void TxPool::snapshot(const std::string& snapshotFile) {
  std::cout << "Snapshotting txpool to " << snapshotFile << std::endl;
  std::ofstream snapshot(snapshotFile);
  for (auto& tx : txMap) {
    snapshot << tx.second->toString() << std::endl;
  }
}

// TODO: Remove tx from map after tx is consumed
