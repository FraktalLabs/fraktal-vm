#pragma once

#include <queue>
#include <memory>

#include "../txpool/txpool.h"
#include "evm_thread_routine.h"

#include <evm-state-db/state-db.h>

class EVMThread {
public:
  EVMThread(std::shared_ptr<State> state, int threadQueueMaxSize, std::shared_ptr<TxPool> txPool)
    : threadQueueMaxSize(threadQueueMaxSize), state(state), txPool(txPool) {}

  void run();
  void stop();
private:
  int threadQueueMaxSize;
  std::queue<EVMThreadRoutine> threadQueue;

  std::shared_ptr<State> state;
  std::shared_ptr<TxPool> txPool;
};
