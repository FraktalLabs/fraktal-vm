#pragma once

#include <intx/intx.hpp>
#include <proc-evm/proc-evm.h>

using uint256 = intx::uint256;

class Transaction {
public:
  Transaction(address from, address to, uint256 value, bytes input, bytes code)
    : from(from), to(to), value(value), input(input), code(code) {}
  Transaction(address from, address to, uint256 value, bytes input)
    : from(from), to(to), value(value), input(input) {}
  Transaction(address from, address to, uint256 value)
    : from(from), to(to), value(value) {}
  Transaction(address from, address to)
    : from(from), to(to) {}
  Transaction(address from)
    : from(from) {}
  Transaction() {}
  Transaction(const std::string& txString) { fromString(txString); }

  intx::uint256 getHash() const;  // TODO: should this return a different type?
  std::string toString() const;
  void fromString(const std::string&);

  address getFrom() const { return from; }
  address getTo() const { return to; }
  uint256 getValue() const { return value; }
  bytes getInput() const { return input; }
  bytes getCode() const { return code; }

  std::shared_ptr<TxContext> asTxContext() const; // TODO: Local declaration of TxContext
private:
  address from;
  address to;
  uint256 value;
  bytes input;
  bytes code;
  // TODO: uint256 gasLimit & other gas related things
  // TODO: uint256 nonce;
  // TODO: Signature
};
