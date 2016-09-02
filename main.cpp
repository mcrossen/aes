#include <iostream>
#include <sstream>
#include "state.h"

#define BLOCK_LENGTH 128

int main(int argc, char** argv) {
  if (argc >= 1) {
    std::string arg1 = std::string(argv[1]);
    if (arg1 == "help" || arg1 == "--help") {
      std::cout << "aes [encrypt|decrypt] [text] [key]" << std::endl;
      return 0;
    }
  }

  if (argc != 4) {
    std::cout << "wrong number of arguments" << std::endl;
  } else {
    // format the operation to be either "e" for encrypt or "d" for decrypt
    std::string arg1 = std::string(argv[1]);
    if (arg1.substr(0, 1) == "-") {
      arg1 = arg1.substr(1, arg1.length() - 1);
    }
    arg1 = arg1.substr(0, 1);

    std::string plaintext = std::string(argv[2]);
    std::string key = std::string(argv[3]);

    if (arg1 == "e") {
      std::stringstream cypher_text;
      for (unsigned int index = 0; index < plaintext.length(); index += BLOCK_LENGTH/4)
      {
        state crypt_state(plaintext.substr(0, BLOCK_LENGTH / 4 - 1));
        cypher_text << crypt_state.encrypt(key);
      }
      std::cout << cypher_text.str() << std::endl;
    } else if (arg1 == "d") {

    } else {
      std::cout << "invalid operation, must be either 'encrypt' or 'decrypt'" << std::endl;
    }

    return 0;
  }
  return 1;
}
