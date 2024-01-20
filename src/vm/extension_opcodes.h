#pragma once

#include <proc-evm/proc-evm.h>

#include <iostream>

//TODO: Opcodes & Operations split

class ClogStackOperation : public Operation {
public:
  ExecStatus execute(CallContext&) override;
};

// TODO: Peek vs Pop && base 10 vs base 16
ExecStatus ClogStackOperation::execute(CallContext& context) {
  uint256 value = context.getStack()->pop();
  std::cout << "CLOG STACK: " << intx::to_string(value, 16) << std::endl;
  return CONTINUE;
}

void loadExtensionOpcodes() {
  extensionJumpTable[0xc0] = new ClogStackOperation();
  extensionOpcodeStrings[0xc0] = "CLOGSTACK";
};
