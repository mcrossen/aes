#pragma once
#include <vector>

#define BLOCK_LENGTH 128

class state {
public:
  state(std::string block) {
    rows = std::vector<std::vector<unsigned int> >(4, std::vector<unsigned int>(BLOCK_LENGTH / 32, 0));
  }

  std::string encrypt(std::string key){
    return "";
  }
private:
  std::vector<std::vector<unsigned int> > rows;
};
