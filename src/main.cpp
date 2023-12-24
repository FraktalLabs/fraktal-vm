#include <iostream>

#include "cmds/cmds.h"

int main(int argc, char*argv[])
{
    const std::string helpMessage = "Usage: " + std::string(argv[0]) + " <subcommand> [options]\n"
                                    "Subcommands:\n"
                                    "  run       : Run the Fraktal VM service\n"
                                    "  help      : Print this help message\n";
    if (argc < 2) {
      std::cout << helpMessage << std::endl;
      return 0;
    }

    std::string subcommand = argv[1];
    if (subcommand == "run") {
      std::cout << "Running the Fraktal VM service" << std::endl;
      return runFraktalVMCmdline(argc, argv);
      // TODO: Run the service
    } else {
      std::cout << "Unknown subcommand: " << subcommand << std::endl << std::endl;
      std::cout << helpMessage << std::endl;
      return 1;
    }

    return 0;
}
