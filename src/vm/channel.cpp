#include "channel.h"

#include "coroutine_context.h"

ExecStatus ContractChannel::send(CallContext& context, uint256 value) {
  if(receiveQueue.empty()) {
    if(buffer.size() < capacity) {
      buffer.push(value);
      return CONTINUE;
    } else {
      if(sendQueue.size() >= CHANNEL_QUEUE_CAPACITY) {
        std::cout << "Channel send queue is full" << std::endl;
        return STOPEXEC;
      }
      
      std::shared_ptr<ContractCoroutine> newCoroutine =
          std::make_shared<ContractCoroutine>(context.getPc() + 1, *context.getStack());
      sendQueue.push(newCoroutine);
      sendBuffer.push(value);
      return STOPEXEC;
    }
  } else {
    std::shared_ptr<ContractCoroutine> newCoroutine =
        std::make_shared<ContractCoroutine>(context.getPc() + 1, *context.getStack());
    static_cast<FraktalVMContext&>(context).addCoroutine(newCoroutine);

    auto receiveCoroutine = receiveQueue.front();
    receiveQueue.pop();
    receiveCoroutine->pushStack(value);
    static_cast<FraktalVMContext&>(context).prependCoroutine(receiveCoroutine);
    return STOPEXEC;
  }

  return CONTINUE;
}

ExecStatus ContractChannel::receive(CallContext& context) {
  if(buffer.empty()) {
    if(receiveQueue.size() >= CHANNEL_QUEUE_CAPACITY) {
      std::cout << "Channel receive queue is full" << std::endl;
      return STOPEXEC;
    }

    std::shared_ptr<ContractCoroutine> newCoroutine =
        std::make_shared<ContractCoroutine>(context.getPc() + 1, *context.getStack());
    receiveQueue.push(newCoroutine);
    return STOPEXEC;
  } else {
    context.getStack()->push(buffer.front());
    buffer.pop();
    if(!sendQueue.empty()) {
      auto sendCoroutine = sendQueue.front();
      sendQueue.pop();
      buffer.push(sendBuffer.front());
      sendBuffer.pop();
      static_cast<FraktalVMContext&>(context).addCoroutine(sendCoroutine);
    }
    return CONTINUE;
  }

  return CONTINUE;
}
