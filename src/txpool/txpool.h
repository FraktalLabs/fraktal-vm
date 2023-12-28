#pragma once

#include <map>
#include <queue>
#include <mutex>
#include <memory>
#include <string>

#include "transaction.h"

class TxPool {
public:
    TxPool(const std::string& snapshotFile);

    void addTx(std::shared_ptr<Transaction> tx);
    std::shared_ptr<Transaction> getTx(const uint256& txHash);
    std::shared_ptr<Transaction> popTx();

    void snapshot(const std::string& snapshotFile);

private:
    std::map<uint256, std::shared_ptr<Transaction>> txMap;

    std::queue<uint256> txQueue;
    std::mutex txQueueMutex;
};
