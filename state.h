#pragma once
#include <vector>
#include <sstream>
#include "hexhelpers.h"
#include "keyscheduler.h"
#include "logger.h"

#define BLOCK_LENGTH 128
#define UPPER_BITS_MASK 0xf0
#define LOWER_BITS_MASK 0x0f

class state {
  public:
    state(std::string block) {
      rows = std::vector<std::vector<uint8_t> >(4, std::vector<uint8_t>(4, 0));
      unsigned int block_index = 0;
      for (unsigned int column = 0; column < 4; column++) {
        for (unsigned int row = 0; row < 4; row++) {
          rows[row][column] = hex_to_int(block.substr(block_index, 2));
          block_index += 2;
        }
      }
    }

    std::string encrypt(std::string key) {
      logger log;
      log.debug(0, "input\t", to_string());

      keyScheduler keys(key);

      addRoundKey(keys.next());
      log.debug(0, "addRoundKey", to_string());
      unsigned int key_length = key.length() * 4;
      unsigned int total_rounds;
      if (key_length == 128) {
        total_rounds = 10;
      } else if (key_length == 192) {
        total_rounds = 12;
      } else if (key_length == 256) {
        total_rounds = 14;
      } else {
        throw;
      }

      // go through each round except the final round
      for (unsigned int round_index = 1; round_index < total_rounds; round_index++) {
        std::vector<std::vector<uint8_t> > key = keys.next();
        //log.debug(round_index, "scheduler", key)
        subBytes();
        log.debug(round_index, "subBytes", to_string());
        shiftRows();
        log.debug(round_index, "shiftRows", to_string());
        mixColumns();
        log.debug(round_index, "mixColumns", to_string());
        addRoundKey(key);
        log.debug(round_index, "addRoundKey", to_string());
      }
      // the final round doesn't include mixColumns step
      subBytes();
      log.debug(10, "subBytes", to_string());
      shiftRows();
      log.debug(10, "shiftRows", to_string());
      addRoundKey(keys.next());
      log.debug(10, "addRoundKey", to_string());
      return to_string();
    }

    void addRoundKey(std::vector<std::vector<uint8_t> > key) {
      for (unsigned int column = 0; column < 4; column++) {
        for (unsigned int row = 0; row < 4; row++) {
          rows[row][column] = rows[row][column] ^ key[row][column];
        }
      }
    }

    unsigned int subByte(uint8_t byte) {
      return sbox[(byte & UPPER_BITS_MASK) >> 4][byte & LOWER_BITS_MASK];
    }

    void subBytes() {
      for (unsigned int column = 0; column < 4; column++) {
        for (unsigned int row = 0; row < 4; row++) {
          rows[row][column] = subByte(rows[row][column]);
        }
      }
    }

    void shiftRows() {
      for (unsigned int row = 0; row < 4; row++) {
        vector<unsigned int> newrow(4);
        for (unsigned int column = 0; column < 4; column++) {
          newrow[(column + 4 - row) % 4] = rows[row][column];
        }
        for (unsigned int column = 0; column < 4; column ++) {
          rows[row][column] = newrow[column];
        }
      }
    }

    void mixColumns() {
      std::vector<std::vector<uint8_t> > new_state(4, std::vector<uint8_t>(4, 0));
      for (unsigned int column = 0; column < 4; column++) {
        for (unsigned int index = 0; index < 4; index++) {
          uint8_t cumulative = 0;
          for (unsigned int row = 0; row < 4; row ++) {
            uint8_t result = fixed_mat[index][row] * rows[row][column];
            if (result > 0xff) {
              cumulative = cumulative ^ (result ^ 0x1b);
            } else {
              cumulative = cumulative ^ result;
            }
          }
          new_state[index][column] = cumulative;
        }
      }
      rows = new_state;
    }

    std::string to_string() {
      stringstream output;
      for (unsigned int column = 0; column < 4; column++) {
        for (unsigned int row = 0; row < 4; row++) {
          output << byte_to_hex(rows[row][column]);
        }
      }
      return output.str();
    }
  private:
    std::vector<std::vector<uint8_t> > rows;
    const uint8_t sbox[16][16] = {
      { 0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76 } ,
      { 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0 } ,
      { 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15 } ,
      { 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75 } ,
      { 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84 } ,
      { 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf } ,
      { 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8 } ,
      { 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2 } ,
      { 0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73 } ,
      { 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb } ,
      { 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79 } ,
      { 0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08 } ,
      { 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a } ,
      { 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e } ,
      { 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf } ,
      { 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }
    };
    const uint8_t fixed_mat[4][4] = {
      {2, 3, 1, 1} ,
      {1, 2, 3, 1} ,
      {1, 1, 2, 3} ,
      {3, 1, 1, 2}
    };

};
