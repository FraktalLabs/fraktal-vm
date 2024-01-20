#pragma once

#include <queue>

#include "coroutine.h"

class FraktalVMContext : public CallContext {
public:
  FraktalVMContext(const CallContext &context) : CallContext(context) {}

  void addCoroutine(std::shared_ptr<ContractCoroutine> coroutine) {
    coroutines.push(coroutine);
  }

  std::shared_ptr<ContractCoroutine> getCoroutine() {
    if (coroutines.empty()) {
      return nullptr;
    }
    auto coroutine = coroutines.front();
    coroutines.pop();
    return coroutine;
  }

  bool hasCoroutine() {
    return !coroutines.empty();
  }

  bool nextCoroutine() {
    // Return true once done ( ie no more coroutines )

    auto coroutine = getCoroutine();
    if (coroutine == nullptr) {
      return true;
    }
    
    setPc(coroutine->getPc());
    setStack(std::make_shared<Stack>(coroutine->getStack()));

    return false;
  }

  virtual bytes run() override;

private:
  std::queue<std::shared_ptr<ContractCoroutine>> coroutines;
};
