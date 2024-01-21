#pragma once

#include <queue>

#include "coroutine_context.h"

// TODO: variable capacity?
#define CHANNEL_QUEUE_CAPACITY 1024

class ContractChannel {
public:
  ContractChannel(uint64_t capacity):
    capacity(capacity) {}

  // TODO: Exec status or bool for continue or not?
  ExecStatus send(CallContext& context, uint256 value);
  ExecStatus receive(CallContext& context);

private:
  std::queue<uint256> buffer;
  uint64_t capacity;
  std::queue<std::shared_ptr<ContractCoroutine>> receiveQueue;

  std::queue<std::shared_ptr<ContractCoroutine>> sendQueue;
  std::queue<uint256> sendBuffer;
};
