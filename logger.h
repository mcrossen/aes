#pragma once
#include <iostream>

class logger {
public:
  void debug(unsigned int round, std::string step, std::string key) {
    if (logger::verbose) {
      std::cout << "round[";
      if (round < 10) {
        std::cout << " ";
      }
      std::cout << round << "]." << step << "\t" << key << std::endl;
    }
  }

  void debug(std::string output) {
    if (logger::verbose) {
      std::cout << output << std::endl;
    }
  }

  static bool verbose;
};

bool logger::verbose;
