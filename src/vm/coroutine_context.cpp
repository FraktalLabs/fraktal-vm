#include "coroutine_context.h"

void FraktalVMContext::addCoroutine(std::shared_ptr<ContractCoroutine> coroutine) {
  coroutines.push_back(coroutine);
}

void FraktalVMContext::prependCoroutine(std::shared_ptr<ContractCoroutine> coroutine) {
  coroutines.push_front(coroutine);
}

std::shared_ptr<ContractCoroutine> FraktalVMContext::getCoroutine() {
  if (coroutines.empty()) {
    return nullptr;
  }
  auto coroutine = coroutines.front();
  coroutines.pop_front();
  return coroutine;
}

bool FraktalVMContext::hasCoroutine() {
  return !coroutines.empty();
}

bool FraktalVMContext::nextCoroutine() {
  // Return true once done ( ie no more coroutines )

  auto coroutine = getCoroutine();
  if (coroutine == nullptr) {
    return true;
  }

  setPc(coroutine->getPc());
  setStack(std::make_shared<Stack>(coroutine->getStack()));

  return false;
}

uint64_t FraktalVMContext::addChannel(std::shared_ptr<ContractChannel> channel) {
  channels.push_back(channel);
  return channels.size() - 1;
}

std::shared_ptr<ContractChannel> FraktalVMContext::getChannel(uint64_t id) {
  if (id >= channels.size()) {
    return nullptr;
  }
  return channels[id];
}

bytes FraktalVMContext::run() {
  bytes ret;
  bool done = false;
  while(!done) {
    ret = CallContext::run();

    // TODO: Break on non-stoptoken error
    done = nextCoroutine();
  }

  return ret;
}
