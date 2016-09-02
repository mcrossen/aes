#pragma once

using namespace std;

unsigned int hex_char_to_int(char c) {
  switch(toupper(c))
  {
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

unsigned int hex_to_int(string hex) {
  unsigned int cumulative = 0;
  for (unsigned int index = 0; index < hex.length(); index++) {
    cumulative += 16^index * hex_char_to_int(hex[hex.length() - index - 1]);
  }
  return cumulative;
}

unsigned int hex_to_int(char* hex) {
  return hex_to_int(string(hex));
}
