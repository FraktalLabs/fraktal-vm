#pragma once

#include <queue>
#include <memory>
#include <thread>

#include "../txpool/txpool.h"
#include "evm_thread_routine.h"

#include <fraktal-state-db/state-db.h>

class EVMThread {
public:
  EVMThread(std::shared_ptr<FraktalState> state, int threadQueueMaxSize, std::shared_ptr<TxPool> txPool)
    : threadQueueMaxSize(threadQueueMaxSize), state(state), txPool(txPool) {}

  void runInternal();
  void run();
  void stop();
private:
  int threadQueueMaxSize;
  std::queue<std::shared_ptr<Transaction>> threadQueue;
  // std::queue<EVMThreadRoutine> threadQueue;
  std::thread evmThread;
  bool running = false;

  std::shared_ptr<FraktalState> state;
  std::shared_ptr<TxPool> txPool;
};
