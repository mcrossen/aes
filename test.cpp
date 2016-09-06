#include <iostream>
#include "state.h"
#include "logger.h"

// compare an expected result against the actual result. display the status (successful|failed) of the test.
void single_test(std::string test_name, std::string expected_result, std::string actual_result) {
  logger log;
  std::cout << "testing " << test_name;
  if (expected_result == actual_result) {
    std::cout << " successful" << std::endl;
    log.clear_buffer();
  } else {
    std::cout << " failed" << std::endl;
    // if the test failed, print the debug log from the crypt
    log.dump_buffer(true);
    throw;
  }
}

// this is a simple script to test the encrypt/decrypt process.
int main() {
  logger::suppress_output = true;
  logger::verbose = true;
  state crypt_state128("00112233445566778899aabbccddeeff");
  state crypt_state192("00112233445566778899aabbccddeeff");
  state crypt_state256("00112233445566778899aabbccddeeff");
  single_test("128-bit key encryption", "69c4e0d86a7b0430d8cdb78070b4c55a", crypt_state128.encrypt("000102030405060708090a0b0c0d0e0f"));
  single_test("192-bit key encryption", "dda97ca4864cdfe06eaf70a0ec0d7191", crypt_state192.encrypt("000102030405060708090a0b0c0d0e0f1011121314151617"));
  single_test("256-bit key encryption", "8ea2b7ca516745bfeafc49904b496089", crypt_state256.encrypt("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"));
  single_test("128-bit key decryption", "00112233445566778899aabbccddeeff", crypt_state128.decrypt("000102030405060708090a0b0c0d0e0f"));
  single_test("192-bit key decryption", "00112233445566778899aabbccddeeff", crypt_state192.decrypt("000102030405060708090a0b0c0d0e0f1011121314151617"));
  single_test("256-bit key decryption", "00112233445566778899aabbccddeeff", crypt_state256.decrypt("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"));
}
