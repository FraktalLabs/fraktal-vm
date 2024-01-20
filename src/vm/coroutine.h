#pragma once

#include <proc-evm/proc-evm.h>

class ContractCoroutine {
public:
    // TODO: move stack?
    ContractCoroutine(): pc(0), stack() {}
    ContractCoroutine(uint64_t pc, Stack stack): pc(pc), stack(stack) {}

    uint64_t getPc() const { return pc; }
    Stack getStack() const { return stack; }
private:
    uint64_t pc;
    Stack stack;
};
