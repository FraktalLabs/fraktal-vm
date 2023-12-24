#include "cmds.h"

#include <string>
#include <iostream>
#include <memory>

struct RunFraktalVMData {
  std::string serverAddress;
  int serverPort;
};

std::unique_ptr<RunFraktalVMData> parseFraktalVMCmdlineArgs(int argc, char* argv[]) {
  // Cmdline Args
  std::string snapshotFile;
  std::string serverAddress;
  int serverPort = 0;

  const std::string helpText = "Usage: " + std::string(argv[0]) + " run [options]\n"
                               "  Run Fraktal VM server exposing state access\n\n"
                               "Options:\n"
                               "  --serverAddress <address>    Address to bind server to (default: localhost)\n"
                               "  --serverPort <port>          Port to bind server to (default: 8545)\n";

  if(argc < 2) {
    std::cerr << helpText << std::endl;
    exit(1);
  }

  for(int i = 2; i < argc; i++) {
    std::string arg = argv[i];
    if(arg == "--serverAddress") {
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

  // Set Defaults
  if(serverAddress.empty()) {
    serverAddress = "localhost";
  }

  if(serverPort == 0) {
    serverPort = 8545;
  }

  return std::make_unique<RunFraktalVMData>(RunFraktalVMData{serverAddress, serverPort});
}

void runFraktalVM(const RunFraktalVMData& data) {
  // TODO
  std::cout << "Running Fraktal VM server on " << data.serverAddress << ":" << data.serverPort << std::endl;
}

int runFraktalVMCmdline(int argc, char* argv[])  {
  auto data = parseFraktalVMCmdlineArgs(argc, argv);
  runFraktalVM(*data);
  return 0;
}
