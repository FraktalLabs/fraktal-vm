#include "cmds.h"

#include <string>
#include <iostream>
#include <memory>
#include <thread>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <functional>

#include "../txpool/txpool_server.h"
#include "../execution/evm_thread.h"

#include <evm-state-db/state-db.h>

struct RunFraktalVMData {
  std::string stateSnapshotFile;
  std::shared_ptr<State> state;

  std::string txPoolSnapshotFile;
  std::shared_ptr<TxPool> txPool;

  int threadPoolMaxThreads;
  int threadQueueMaxSize;

  std::string serverAddress;
  int serverPort;
};

std::unique_ptr<RunFraktalVMData> parseFraktalVMCmdlineArgs(int argc, char* argv[]) {
  // Cmdline Args
  std::string stateSnapshotFile;
  std::string txPoolSnapshotFile;
  int threadPoolMaxThreads;
  int threadQueueMaxSize;
  std::string serverAddress;
  int serverPort = 0;

  const std::string helpText = "Usage: " + std::string(argv[0]) + " run [options]\n"
                               "  Run Fraktal VM server exposing state access\n\n"
                               "Options:\n"
                               "  --stateSnapshot <file>       File to load state snapshot from (required)\n"
                               "  --txPoolSnapshot <file>      File to load tx pool snapshot from (required)\n"
                               "  --threadPoolMaxThreads <n>   Maximum number of threads in thread pool (default: 4)\n"
                               "  --threadQueueMaxSize <n>     Maximum number of threads in thread pool (default: 1024)\n"
                               "  --serverAddress <address>    Address to bind server to (default: localhost)\n"
                               "  --serverPort <port>          Port to bind server to (default: 8545)\n";

  if(argc < 2) {
    std::cerr << helpText << std::endl;
    exit(1);
  }

  for(int i = 2; i < argc; i++) {
    std::string arg = argv[i];
    if(arg == "--help") {
      std::cerr << helpText << std::endl;
      exit(1);
    } else if(arg == "--stateSnapshot") {
      if(i + 1 >= argc) {
        std::cerr << "Missing argument for --stateSnapshot" << std::endl;
        exit(1);
      }
      stateSnapshotFile = argv[++i];
    } else if(arg == "--txPoolSnapshot") {
      if(i + 1 >= argc) {
        std::cerr << "Missing argument for --txPoolSnapshot" << std::endl;
        exit(1);
      }
      txPoolSnapshotFile = argv[++i];
    } else if(arg == "--threadPoolMaxThreads") {
      if(i + 1 >= argc) {
        std::cerr << "Missing argument for --threadPoolMaxThreads" << std::endl;
        exit(1);
      }
      threadPoolMaxThreads = std::stoi(argv[++i]);
    } else if(arg == "--threadQueueMaxSize") {
      if(i + 1 >= argc) {
        std::cerr << "Missing argument for --threadQueueMaxSize" << std::endl;
        exit(1);
      }
      threadQueueMaxSize = std::stoi(argv[++i]);
    } else if(arg == "--serverAddress") {
      if(i + 1 >= argc) {
        std::cerr << "Missing argument for --serverAddress" << std::endl;
        exit(1);
      }
      serverAddress = argv[++i];
    } else if(arg == "--serverPort") {
      if(i + 1 >= argc) {
        std::cerr << "Missing argument for --serverPort" << std::endl;
        exit(1);
      }
      serverPort = std::stoi(argv[++i]);
    } else {
      std::cerr << "Unknown argument: " << arg << std::endl;
      exit(1);
    }
  }

  // Check required args
  if(stateSnapshotFile.empty()) {
    std::cerr << "Missing required argument --stateSnapshot" << std::endl;
    exit(1);
  }
  
  if(txPoolSnapshotFile.empty()) {
    std::cerr << "Missing required argument --txPoolSnapshot" << std::endl;
    exit(1);
  }


  // Set Defaults
  if(threadPoolMaxThreads == 0) {
    threadPoolMaxThreads = 4;
  }

  if(threadQueueMaxSize == 0) {
    threadQueueMaxSize = 1024;
  }

  if(serverAddress.empty()) {
    serverAddress = "localhost";
  }

  if(serverPort == 0) {
    serverPort = 8545;
  }

  auto state = std::make_shared<State>(stateSnapshotFile);
  auto txPool = std::make_shared<TxPool>(txPoolSnapshotFile);

  return std::make_unique<RunFraktalVMData>(RunFraktalVMData{
        stateSnapshotFile, state, txPoolSnapshotFile, txPool,
        threadPoolMaxThreads, threadQueueMaxSize, serverAddress, serverPort
      });
}

std::function<void(int)> shutdown_handler;
void signal_handler(int signal) {
  shutdown_handler(signal);
}

void runFraktalVM(const RunFraktalVMData& data) {
  std::cout << "Running Fraktal VM server on " << data.serverAddress << ":" << data.serverPort << std::endl;

  // Create EVM Thread Pool
  std::vector<std::shared_ptr<EVMThread>> threadPool(data.threadPoolMaxThreads);
  // TODO: std::array<std::shared_ptr<EVMThread>, data.threadPoolMaxThreads> threadPool;
  for(int i = 0; i < data.threadPoolMaxThreads; i++) {
    threadPool[i] = std::make_shared<EVMThread>(data.state, data.threadQueueMaxSize, data.txPool);
  }

  // Start TX Pool Server
  std::cout << "Starting TX Pool Server" << std::endl;
  TxPoolServer txPoolServer(data.txPool, data.serverAddress, data.serverPort, data.txPoolSnapshotFile);
  std::thread txPoolServerThread([&]() {
    txPoolServer.run();
  });

  // Start EVM Threads
  std::cout << "Starting EVM Threads" << std::endl;
  for(int i = 0; i < data.threadPoolMaxThreads; i++) {
    threadPool[i]->run();
  }

  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = signal_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  shutdown_handler = [&](int signal) {
    std::cout << "Received signal " << signal << std::endl;

    // Stop EVM Threads
    std::cout << "Stopping EVM Threads" << std::endl;
    for(int i = 0; i < threadPool.size(); i++) {
      threadPool[i]->stop();
    }

    // Stop TX Pool Server
    std::cout << "Stopping TX Pool Server" << std::endl;
    txPoolServer.stop();
    txPoolServerThread.join();

    std::cout << "Fraktal VM server stopped" << std::endl;

    // Save State Snapshot & TX Pool Snapshot
    std::cout << "Saving State Snapshot" << std::endl;
    data.state->snapshot(data.stateSnapshotFile);
    std::cout << "Saving TX Pool Snapshot" << std::endl;
    data.txPool->snapshot(data.txPoolSnapshotFile);

    exit(0);
  };

  sigaction(SIGINT, &sigIntHandler, NULL);

  std::cout << "Press Ctrl-C to exit" << std::endl;
  pause();

  while(true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  // Stop EVM Threads
  std::cout << "Stopping EVM Threads" << std::endl;
  for(int i = 0; i < data.threadPoolMaxThreads; i++) {
    threadPool[i]->stop();
  }

  // Stop TX Pool Server
  std::cout << "Stopping TX Pool Server" << std::endl;
  txPoolServerThread.join();
  data.txPool->snapshot(data.txPoolSnapshotFile);

  std::cout << "Fraktal VM server stopped" << std::endl;

  // Save State Snapshot & TX Pool Snapshot
  std::cout << "Saving State Snapshot" << std::endl;
  data.state->snapshot(data.stateSnapshotFile);
  std::cout << "Saving TX Pool Snapshot" << std::endl;
  data.txPool->snapshot(data.txPoolSnapshotFile);

  std::cout << "State Snapshot saved to " << data.stateSnapshotFile << std::endl;
  std::cout << "TX Pool Snapshot saved to " << data.txPoolSnapshotFile << std::endl;

  std::cout << "Fraktal VM server stopped" << std::endl;
}

int runFraktalVMCmdline(int argc, char* argv[])  {
  auto data = parseFraktalVMCmdlineArgs(argc, argv);
  runFraktalVM(*data);
  return 0;
}
