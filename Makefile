Compile=g++ -Wall -g -std=c++11
Source=*.cpp
Output=aes
TestOutput=aes-test

.PHONY: all $(Output) clean
.PHONY: test $(TestOutput) clean

all:
	$(Compile) $(Source) -o $(Output)
test:
	$(Compile) $(Source) -o $(TestOutput) && valgrind --leak-check=full --show-leak-kinds=all ./$(TestOutput) -v decrypt 8ea2b7ca516745bfeafc49904b496089 000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f && rm $(TestOutput)
