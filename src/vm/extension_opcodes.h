#pragma once

#include <iostream>

#include "coroutine_context.h"

#include <fraktal-state-db/state-db.h>

//TODO: Opcodes & Operations split

class ClogStackOperation : public Operation {
public:
  ExecStatus execute(CallContext&) override;
};

class ClogMemoryOperation : public Operation {
public:
  ExecStatus execute(CallContext&) override;
};

class ClogMemoryStringOperation : public Operation {
public:
  ExecStatus execute(CallContext&) override;
};

// TODO: Peek vs Pop && base 10 vs base 16
ExecStatus ClogStackOperation::execute(CallContext& context) {
  uint256 value = context.getStack()->pop();
  std::cout << "CLOG STACK: " << "0x" << intx::to_string(value, 16) << std::endl;
  return CONTINUE;
}

ExecStatus ClogMemoryOperation::execute(CallContext& context) {
  uint256 pos = context.getStack()->pop();
  uint256 value = context.getMemory()->load32(static_cast<uint64_t>(pos));
  std::cout << "CLOG MEMORY: " << intx::to_string(value, 16) << std::endl;
  return CONTINUE;
}

ExecStatus ClogMemoryStringOperation::execute(CallContext& context) {
  uint256 offset = context.getStack()->pop();
  uint64_t offset64 = static_cast<uint64_t>(offset);

  intx::uint256 length = context.getMemory()->load32(offset64);
  uint8_t* valuePtr = context.getMemory()->getPointer(offset64 + 32);
  std::string value = std::string(reinterpret_cast<char*>(valuePtr), static_cast<uint64_t>(length));
  std::cout << "CLOG MEMORY: " << value << std::endl;
  return CONTINUE;
}

class YieldOperation : public Operation {
public:
  ExecStatus execute(CallContext&) override;
};

class SpawnOperation : public Operation {
public:
  ExecStatus execute(CallContext&) override;
};

ExecStatus YieldOperation::execute(CallContext& context) {
  std::shared_ptr<ContractCoroutine> coroutine =
      std::make_shared<ContractCoroutine>(context.getPc() + 1, *context.getStack());

  static_cast<FraktalVMContext&>(context).addCoroutine(coroutine);

  return STOPEXEC;
}

ExecStatus SpawnOperation::execute(CallContext& context) {
  uint256 pos = context.getStack()->pop();
  // TODO: Check valid jump destination & ...
  
  std::shared_ptr<ContractCoroutine> coroutine =
      std::make_shared<ContractCoroutine>(static_cast<uint64_t>(pos), *context.getStack());

  static_cast<FraktalVMContext&>(context).addCoroutine(coroutine);

  return CONTINUE;
}

class ChannelCreateOperation : public Operation {
public:
  ExecStatus execute(CallContext&) override;
};

class ChannelSendOperation : public Operation {
public:
  ExecStatus execute(CallContext&) override;
};

class ChannelReceiveOperation : public Operation {
public:
  ExecStatus execute(CallContext&) override;
};

ExecStatus ChannelCreateOperation::execute(CallContext& context) {
  uint256& capacity = context.getStack()->peek();
  std::shared_ptr<ContractChannel> channel =
    std::make_shared<ContractChannel>(static_cast<uint64_t>(capacity));

  uint64_t channelIdx = static_cast<FraktalVMContext&>(context).addChannel(channel);
  capacity = channelIdx;

  return CONTINUE;
}

ExecStatus ChannelSendOperation::execute(CallContext& context) {
  uint256 channelIdx = context.getStack()->pop();
  uint256 value = context.getStack()->pop();
  auto channel = static_cast<FraktalVMContext&>(context).getChannel(static_cast<uint64_t>(channelIdx));
  // TODO: existence checks

  return channel->send(context, value);
}

ExecStatus ChannelReceiveOperation::execute(CallContext& context) {
  uint256 channelIdx = context.getStack()->pop();
  auto channel = static_cast<FraktalVMContext&>(context).getChannel(static_cast<uint64_t>(channelIdx));

  return channel->receive(context);
}

class MutexCreateOperation : public Operation {
public:
  ExecStatus execute(CallContext&) override;
};

class MutexLockOperation : public Operation {
public:
  ExecStatus execute(CallContext&) override;
};

class MutexUnlockOperation : public Operation {
public:
  ExecStatus execute(CallContext&) override;
};

ExecStatus MutexCreateOperation::execute(CallContext& context) {
  uint256 mutexIdx = context.getStack()->pop();

  // TODO: Store in storage slot w/ nonce as value?
  std::string contract = context.getContract()->getAddressString();
  std::shared_ptr<FraktalAccount> account = 
      std::static_pointer_cast<FraktalAccount>(context.getState()->get(contract));

  account->createMutex(mutexIdx);

  return CONTINUE;
}

ExecStatus MutexLockOperation::execute(CallContext& context) {
  uint256 mutexIdx = context.getStack()->pop();

  std::string contract = context.getContract()->getAddressString();
  std::shared_ptr<FraktalAccount> account = 
      std::static_pointer_cast<FraktalAccount>(context.getState()->get(contract));

  account->lockMutex(static_cast<uint64_t>(mutexIdx));

  return CONTINUE;
}

ExecStatus MutexUnlockOperation::execute(CallContext& context) {
  uint256 mutexIdx = context.getStack()->pop();

  std::string contract = context.getContract()->getAddressString();
  std::shared_ptr<FraktalAccount> account = 
      std::static_pointer_cast<FraktalAccount>(context.getState()->get(contract));

  account->unlockMutex(static_cast<uint64_t>(mutexIdx));

  return CONTINUE;
}

void loadExtensionOpcodes() {
  // Contract Coroutines + Channels : 0x00 - 0x0f
  extensionJumpTable[0x00] = new YieldOperation();
  extensionOpcodeStrings[0x00] = "YIELD";

  extensionJumpTable[0x01] = new SpawnOperation();
  extensionOpcodeStrings[0x01] = "SPAWN";

  extensionJumpTable[0x02] = new ChannelCreateOperation();
  extensionOpcodeStrings[0x02] = "CHANCREATE";

  extensionJumpTable[0x03] = new ChannelSendOperation();
  extensionOpcodeStrings[0x03] = "CHANSEND";

  extensionJumpTable[0x04] = new ChannelReceiveOperation();
  extensionOpcodeStrings[0x04] = "CHANRECV";

  // FraktalVM Coroutines + Channels : 0x10 - 0x1f
  
  // Mutex Operations : 0x20 - 0x2f
  extensionJumpTable[0x20] = new MutexCreateOperation();
  extensionOpcodeStrings[0x20] = "MUTEXCREATE";

  extensionJumpTable[0x21] = new MutexLockOperation();
  extensionOpcodeStrings[0x21] = "MUTEXLOCK";

  extensionJumpTable[0x22] = new MutexUnlockOperation();
  extensionOpcodeStrings[0x22] = "MUTEXUNLOCK";
  // TODO: MutexDestroy?

  // Clog Operations : 0xc0 - 0xc9
  extensionJumpTable[0xc0] = new ClogStackOperation();
  extensionOpcodeStrings[0xc0] = "CLOGSTACK";

  extensionJumpTable[0xc1] = new ClogMemoryOperation();
  extensionOpcodeStrings[0xc1] = "CLOGMEM";

  extensionJumpTable[0xc2] = new ClogMemoryStringOperation();
  extensionOpcodeStrings[0xc2] = "CLOGMEMSTR";
};
