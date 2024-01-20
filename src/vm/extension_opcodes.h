#pragma once

#include <proc-evm/proc-evm.h>

#include <iostream>

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
  uint32_t value = context.getMemory()->load32(static_cast<uint64_t>(pos));
  std::cout << "CLOG MEMORY: " << std::hex << value << std::endl;
  return CONTINUE;
}

ExecStatus ClogMemoryStringOperation::execute(CallContext& context) {
  uint256 offset = context.getStack()->pop();
  uint64_t offset64 = static_cast<uint64_t>(offset);

  uint32_t length = context.getMemory()->load32(offset64);
  uint8_t* valuePtr = context.getMemory()->getPointer(offset64 + 32);
  std::string value = std::string(reinterpret_cast<char*>(valuePtr), length);
  std::cout << "CLOG MEMORY: " << value << std::endl;
  return CONTINUE;
}

void loadExtensionOpcodes() {
  extensionJumpTable[0xc0] = new ClogStackOperation();
  extensionOpcodeStrings[0xc0] = "CLOGSTACK";

  extensionJumpTable[0xc1] = new ClogMemoryOperation();
  extensionOpcodeStrings[0xc1] = "CLOGMEMORY";

  extensionJumpTable[0xc2] = new ClogMemoryStringOperation();
  extensionOpcodeStrings[0xc2] = "CLOGMEMORYSTRING";
};
