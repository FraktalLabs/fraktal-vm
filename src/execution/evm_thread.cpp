#include "evm_thread.h"

#include <iostream>
#include <thread>
#include <memory>

#include <evm-cpp-utils/types.h>

#include "../vm/coroutine_context.h"

void EVMThread::runInternal() {
  running = true;
  while(running) {
    // TODO: choosing when to pull tx or use routines
    if (threadQueueMaxSize > threadQueue.size()) {
      std::shared_ptr<Transaction> tx = txPool->popTx();
      if (tx == nullptr) {
        // Sleep for a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        continue;
      }
      threadQueue.emplace(tx);
    }

    if (threadQueue.size() > 0) {
      std::shared_ptr<Transaction> tx = threadQueue.front();
      threadQueue.pop();

      address contractAddress = tx->getTo(); // TODO: use routine address
      std::string contractAddressStr = addressToHex(contractAddress);

      std::shared_ptr<FraktalAccount> account = std::static_pointer_cast<FraktalAccount>(state->get(contractAddressStr));
      bytes contractCode = account->getCode();
      auto contract = std::make_shared<Contract>(contractCode, contractAddressStr);

      // TODO: Pass in blockContext from external
      std::shared_ptr<BlockContext> blockContext = std::make_shared<BlockContext>();

      // TODO: Use caller of routine
      // TODO: Inner calls wont be able to do this in current setup ( overwrite in jumptable )
      std::shared_ptr<FraktalVMContext> callContext =
        std::make_shared<FraktalVMContext>(CallContext(contract, tx->getValue(), tx->getInput(),
                                           tx->getFrom(), state, blockContext, tx->asTxContext()));

      intx::uint256 txHash = tx->getHash();
      std::cout << "Running tx: " << intx::to_string(txHash, 16) << "  on thread: " << std::this_thread::get_id() << std::endl;
      auto result = callContext->run();
      std::cout << "Tx: " << intx::to_string(txHash, 16) << "  on thread: " << std::this_thread::get_id() << " finished" << std::endl;

      // TODO: check result & remove tx from pool
      txPool->removeTx(txHash);
    }
  }
}

void EVMThread::run() {
  evmThread = std::thread(&EVMThread::runInternal, this);
}

void EVMThread::stop() {
  running = false;
  evmThread.join();
}
