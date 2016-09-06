#pragma once
#include <sstream>
#include <math.h>
#define UPPER_BITS_MASK 0xf0
#define LOWER_BITS_MASK 0x0f

using namespace std;

// convert a single hex digit to an integer
unsigned int hex_char_to_int(char c) {
  switch(toupper(c)) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'A': return 10;
    case 'B': return 11;
    case 'C': return 12;
    case 'D': return 13;
    case 'E': return 14;
    case 'F': return 15;
    default: throw;
  }
}

// convert a long hex string to a number
unsigned int hex_to_int(string hex) {
  unsigned int cumulative = 0;
  for (unsigned int index = 0; index < hex.length(); index++) {
    cumulative += pow(16, hex.length() - index - 1) * hex_char_to_int(hex[index]);
  }
  return cumulative;
}

// convert a char* of hex digits to a number
unsigned int hex_to_int(char* hex) {
  return hex_to_int(string(hex));
}

// convert half a byte to a string of hex digits
std::string half_byte_to_hex(uint8_t num) {
  switch(num) {
    case 0: return "0";
    case 1: return "1";
    case 2: return "2";
    case 3: return "3";
    case 4: return "4";
    case 5: return "5";
    case 6: return "6";
    case 7: return "7";
    case 8: return "8";
    case 9: return "9";
    case 10: return "a";
    case 11: return "b";
    case 12: return "c";
    case 13: return "d";
    case 14: return "e";
    case 15: return "f";
    default: throw;
  }
}

// split a byte into two parts and convert to a string of hex digits
std::string byte_to_hex(uint8_t num) {
  return half_byte_to_hex(num >> 4) + half_byte_to_hex(num & LOWER_BITS_MASK);
}
