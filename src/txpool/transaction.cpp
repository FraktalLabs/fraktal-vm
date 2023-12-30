#include "transaction.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <vector>

#include <ethash/keccak.h>
#include <proc-evm/proc-evm.h>

intx::uint256 Transaction::getHash() const {
  // Hash tx fields
  std::string txStr = toString();
  uint8_t* txData = (uint8_t*)txStr.c_str();
  intx::uint256 txHash = intx::be::load<intx::uint256>(ethash_keccak256(txData, txStr.size()));
  return txHash;
}

std::string Transaction::toString() const {
  std::string result;
  result += addressToHex(from);
  result += ",";
  result += addressToHex(to);
  result += ",";
  result += intx::to_string(value);
  result += ",";
  result += bytecodeToHex(input);
  result += ",";
  result += bytecodeToHex(code);
  result += ",";
  return result;
}

void Transaction::fromString(const std::string& txStr) {
  std::stringstream ss(txStr);
  std::string item;
  std::vector<std::string> tokens;
  while (std::getline(ss, item, ',')) {
    tokens.push_back(item);
  }
  if (tokens.size() != 5) {
    throw std::runtime_error("Invalid transaction string");
  }
  from = parseAddress(tokens[0]);
  to = parseAddress(tokens[1]);
  value = intx::from_string<intx::uint256>(tokens[2]);
  input = parseBytecode(tokens[3]);
  code = parseBytecode(tokens[4]);
}
