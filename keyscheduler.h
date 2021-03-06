#pragma once
#include <vector>
#include "hexhelpers.h"
#include "logger.h"
#define WORD_LENGTH 32
#define UPPER_BITS_MASK 0xf0
#define LOWER_BITS_MASK 0x0f
#define NO_ROUND_SPECIFIED 0xffff

class keyScheduler {
  public:
    keyScheduler(std::string key) {
      unsigned int total_keys;
      unsigned int prev_word_offset;
      // determine how many keys to create
      if (key.size()*4 == 128) {
        prev_word_offset = 4;
        total_keys = 11;
      } else if (key.size()*4 == 192) {
        prev_word_offset = 6;
        total_keys = 13;
      } else if (key.size()*4 == 256) {
        prev_word_offset = 8;
        total_keys = 15;
      } else {
        throw;
      }

      // the key scheduler stores all the keys in a very long vector of vectors.
      // every 4 columns is a new key.
      // there are 4 rows.
      columns = std::vector<std::vector<uint8_t> >(total_keys * 4, std::vector<uint8_t>(4, 0));
      unsigned int key_index = 0;
      for (unsigned int column = 0; column < key.size()*4/WORD_LENGTH; column++) {
        for (unsigned int row = 0; row < 4; row++) {
          columns[column][row] = hex_to_int(key.substr(key_index, 2));
          key_index += 2;
        }
      }

      // start the key expansion
      next_rcon = 1;
      for (unsigned int column = key.size()*4/WORD_LENGTH; column < columns.size(); column+=prev_word_offset) {
        // rotate the word, substitute the bytes, and xor it with the previous word and the same word from the previous chunk
        columns[column] = rcon(columns[column - prev_word_offset], subBytes(rotWord(columns[column - 1])));
        for (unsigned int word_index = column + 1; word_index < column + 4 && word_index < columns.size(); word_index++) {
          basic_core_expand(word_index, prev_word_offset);
        }
        // only do the following if using a 256 bit key
        if (key.size()*4 == 256 && column+3 < columns.size()) {
          std::vector<uint8_t> new_column = subBytes(columns[column+3]);
          if (column+4 < columns.size()) {
            // very similar to basic_core_expand, except use the subBytes column just created
            for (unsigned int row = 0; row < columns[column+4].size(); row++) {
              columns[column + 4][row] = columns[column + 4 - prev_word_offset][row] ^ new_column[row];
            }
          }
          for (unsigned int word_index = column + 5; word_index < column + 8 && word_index < columns.size(); word_index++) {
            basic_core_expand(word_index, prev_word_offset);
          }
        // only do the following if using a 192 bit key
        } else if (key.size()*4 == 192) {
          for (unsigned int word_index = column + 4; word_index < column + 6 && word_index < columns.size(); word_index++) {
            basic_core_expand(word_index, prev_word_offset);
          }
        }
      }
      //logger log; log.debug(to_string()); // dump the entire key schedule to the display
    }

    void basic_core_expand(unsigned int word_index, unsigned int prev_word_offset) {
      // xor it with the previous word and the same word from the previous chunk
      for (unsigned int row = 0; row < columns[word_index].size(); row++) {
        columns[word_index][row] = columns[word_index - prev_word_offset][row] ^ columns[word_index-1][row];
      }
    }

    // xor a word with the rcon column
    std::vector<uint8_t> rcon(std::vector<uint8_t> prev_word, std::vector<uint8_t> word) {
      std::vector<uint8_t> to_return(word.size());
      for (unsigned int index = 0; index < word.size(); index++) {
        to_return[index] = prev_word[index] ^ word[index] ^ rcon_q[next_rcon][index];
      }
      next_rcon++;
      return to_return;
    }

    // rotate the word
    std::vector<uint8_t> rotWord(std::vector<uint8_t> word) {
      std::vector<uint8_t> to_return(word.size());
      for (unsigned int index = 1; index < word.size(); index++) {
        to_return[index - 1] = word[index];
      }
      to_return[word.size() - 1] = word[0];
      return to_return;
    }

    // lookup a byte from the sbox
    unsigned int subByte(uint8_t byte) {
      return sbox[(byte & UPPER_BITS_MASK) >> 4][byte & LOWER_BITS_MASK];
    }

    // substitute all the bytes in a word from the sbox
    std::vector<uint8_t> subBytes(std::vector<uint8_t> word) {
      std::vector<uint8_t> to_return(word.size());
      for (unsigned int row = 0; row < word.size(); row++) {
        to_return[row] = subByte(word[row]);
      }
      return to_return;
    }

    // get a key using the passed in index
    std::vector<std::vector<uint8_t> > get(unsigned int key_index, unsigned int round_index = NO_ROUND_SPECIFIED) {
      std::vector<std::vector<uint8_t> > to_return(4, vector<uint8_t>(4, 0));
      stringstream debug_string;
      // convert the coloumns of rows to rows of columns
      for (unsigned int column = key_index*4; column < key_index*4+4 && column < columns.size(); column++) {
        for (unsigned int row = 0; row < columns[column].size(); row++) {
          to_return[row][column - key_index*4] = columns[column][row];
          debug_string << byte_to_hex(to_return[row][column - key_index*4]);
        }
      }
      // an optional 'round_index' argument can be passed in. this is printed in the debug
      logger log;
      if (round_index == NO_ROUND_SPECIFIED) {
        log.debug(key_index, "scheduler", debug_string.str());
      } else {
        log.debug(round_index, "scheduler", debug_string.str());
      }
      return to_return;
    }

    // dump the entire keyscheduler
    std::string to_string() {
      stringstream to_return;
      bool first_line = true;
      for (unsigned int row = 0; row < 4; row++) {
        if (!first_line) {
          to_return << std::endl;
        } else {
          first_line = false;
        }
        for (unsigned int column = 0; column < columns.size(); column++) {
          to_return << byte_to_hex(columns[column][row]) << " ";
        }
      }
      return to_return.str();
    }

  private:
    // the keyscheduler is kept as a long vector of columns of rows
    std::vector<std::vector<uint8_t> > columns;

    // same sbox as the state
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

    // keep track of which rcon is being used
    unsigned int next_rcon;

    // the rcon words
    const uint8_t rcon_q[52][4] = {
      { 0x00, 0x00, 0x00, 0x00 }, // Rcon[] is 1-based, 0x so the first entry is just a place holder
      { 0x01, 0x00, 0x00, 0x00 },
      { 0x02, 0x00, 0x00, 0x00 },
      { 0x04, 0x00, 0x00, 0x00 },
      { 0x08, 0x00, 0x00, 0x00 },
      { 0x10, 0x00, 0x00, 0x00 },
      { 0x20, 0x00, 0x00, 0x00 },
      { 0x40, 0x00, 0x00, 0x00 },
      { 0x80, 0x00, 0x00, 0x00 },
      { 0x1B, 0x00, 0x00, 0x00 },
      { 0x36, 0x00, 0x00, 0x00 },
      { 0x6C, 0x00, 0x00, 0x00 },
      { 0xD8, 0x00, 0x00, 0x00 },
      { 0xAB, 0x00, 0x00, 0x00 },
      { 0x4D, 0x00, 0x00, 0x00 },
      { 0x9A, 0x00, 0x00, 0x00 },
      { 0x2F, 0x00, 0x00, 0x00 },
      { 0x5E, 0x00, 0x00, 0x00 },
      { 0xBC, 0x00, 0x00, 0x00 },
      { 0x63, 0x00, 0x00, 0x00 },
      { 0xC6, 0x00, 0x00, 0x00 },
      { 0x97, 0x00, 0x00, 0x00 },
      { 0x35, 0x00, 0x00, 0x00 },
      { 0x6A, 0x00, 0x00, 0x00 },
      { 0xD4, 0x00, 0x00, 0x00 },
      { 0xB3, 0x00, 0x00, 0x00 },
      { 0x7D, 0x00, 0x00, 0x00 },
      { 0xFA, 0x00, 0x00, 0x00 },
      { 0xEF, 0x00, 0x00, 0x00 },
      { 0xC5, 0x00, 0x00, 0x00 },
      { 0x91, 0x00, 0x00, 0x00 },
      { 0x39, 0x00, 0x00, 0x00 },
      { 0x72, 0x00, 0x00, 0x00 },
      { 0xE4, 0x00, 0x00, 0x00 },
      { 0xD3, 0x00, 0x00, 0x00 },
      { 0xBD, 0x00, 0x00, 0x00 },
      { 0x61, 0x00, 0x00, 0x00 },
      { 0xC2, 0x00, 0x00, 0x00 },
      { 0x9F, 0x00, 0x00, 0x00 },
      { 0x25, 0x00, 0x00, 0x00 },
      { 0x4A, 0x00, 0x00, 0x00 },
      { 0x94, 0x00, 0x00, 0x00 },
      { 0x33, 0x00, 0x00, 0x00 },
      { 0x66, 0x00, 0x00, 0x00 },
      { 0xCC, 0x00, 0x00, 0x00 },
      { 0x83, 0x00, 0x00, 0x00 },
      { 0x1D, 0x00, 0x00, 0x00 },
      { 0x3A, 0x00, 0x00, 0x00 },
      { 0x74, 0x00, 0x00, 0x00 },
      { 0xE8, 0x00, 0x00, 0x00 },
      { 0xCB, 0x00, 0x00, 0x00 },
      { 0x8D, 0x00, 0x00, 0x00 }
    };
};
