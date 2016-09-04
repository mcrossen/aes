#include <iostream>
#include <sstream>
#include <vector>
#include "state.h"
#include "logger.h"

#define BLOCK_LENGTH 128

int main(int argc, char** argv) {
  if (argc >= 1) {
    std::string arg1 = std::string(argv[1]);
    if (arg1 == "help" || arg1 == "--help") {
      std::cout << "aes [encrypt|decrypt] [text] [key]" << std::endl;
      return 0;
    }
  }

  std::vector<std::string> args;
  logger::verbose = false;
  for (signed int index = 1; index < argc; index++) {
    if (std::string(argv[index]) == "-v") {
      logger::verbose = true;
    } else {
      args.push_back(std::string(argv[index]));
    }
  }

  if (args.size() != 3) {
    std::cout << "wrong number of arguments" << std::endl;
  } else {

    // format the operation to be either "e" for encrypt or "d" for decrypt
    if (args[0].substr(0, 1) == "-") {
      args[0] = args[0].substr(1, args[0].length() - 1);
    }
    args[0] = args[0].substr(0, 1);

    std::string plaintext = std::string(args[1]);
    std::string key = std::string(args[2]);

    if (args[0] == "e") {
      std::stringstream cypher_text;
      for (unsigned int index = 0; index < plaintext.length(); index += BLOCK_LENGTH/4)
      {
        state crypt_state(plaintext.substr(0, BLOCK_LENGTH / 4));
        cypher_text << crypt_state.encrypt(key);
      }
      std::cout << cypher_text.str() << std::endl;
    } else if (args[0] == "d") {

    } else {
      std::cout << "invalid operation, must be either 'encrypt' or 'decrypt'" << std::endl;
    }

    return 0;
  }
  return 1;
}
