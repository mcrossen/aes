#pragma once
#include <vector>
#include "hexhelpers.h"

class keyScheduler {
  public:
    keyScheduler(std::string key) {
      rows = std::vector<std::vector<unsigned int> >(4, std::vector<unsigned int>(4, 0));
      unsigned int key_index = 0;
      for (unsigned int column = 0; column < 4; column++) {
        for (unsigned int row = 0; row < 4; row++) {
          rows[row][column] = hex_to_int(key.substr(key_index, 2));
          key_index += 2;
        }
      }
    }

    std::vector<std::vector<unsigned int> > next() {
      return rows;
    }

    std::string to_string() {
      stringstream output;
      for (unsigned int column = 0; column < 4; column++) {
        for (unsigned int row = 0; row < 4; row++) {
          output << int_to_hex(rows[row][column]);
        }
      }
      return output.str();
    }
    
  private:
    std::vector<std::vector<unsigned int> > rows;
};
