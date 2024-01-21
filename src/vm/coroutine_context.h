#pragma once

#include <queue>

#include "coroutine.h"
#include "channel.h"

class ContractChannel;

class FraktalVMContext : public CallContext {
public:
  FraktalVMContext(const CallContext &context) : CallContext(context) {}

  void addCoroutine(std::shared_ptr<ContractCoroutine> coroutine);
  void prependCoroutine(std::shared_ptr<ContractCoroutine> coroutine);
  std::shared_ptr<ContractCoroutine> getCoroutine();
  bool hasCoroutine();
  bool nextCoroutine();

  uint64_t addChannel(std::shared_ptr<ContractChannel> channel);
  std::shared_ptr<ContractChannel> getChannel(uint64_t id);

  virtual bytes run() override;

private:
  //TODO: Use queue & don't return to caller?
  std::deque<std::shared_ptr<ContractCoroutine>> coroutines;
  std::vector<std::shared_ptr<ContractChannel>> channels;
};
