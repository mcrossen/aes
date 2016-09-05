#pragma once
#include <iostream>
#include <sstream>

class logger {
public:
  void debug(unsigned int round, std::string step, std::string key) {
    if (logger::verbose) {
      logger::buffer << "round[";
      if (round < 10) {
        logger::buffer << " ";
      }
      logger::buffer << round << "]." << step << "\t" << key << std::endl;
    }
    dump_buffer();
  }

  void debug(std::string output) {
    if (logger::verbose) {
      logger::buffer << output << std::endl;
    }
    dump_buffer();
  }

  void dump_buffer(bool force = false) {
    if (force || !suppress_output) {
      std::cout << logger::buffer.str();
      clear_buffer();
    }
  }

  void clear_buffer() {
    logger::buffer.str(std::string());
  }

  static bool verbose;
  static bool suppress_output;
private:
  static stringstream buffer;
};

bool logger::verbose;
bool logger::suppress_output;
stringstream logger::buffer;
