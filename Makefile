Compile=g++ -Wall -g -std=c++11
Source=*.cpp
Output=aes
TestOutput=aes-test

.PHONY: all $(Output) clean
.PHONY: test $(TestOutput) clean

all:
	$(Compile) $(Source) -o $(Output)
test:
	$(Compile) $(Source) -o $(TestOutput) && valgrind --leak-check=full --show-leak-kinds=all ./$(TestOutput) -v encrypt 00112233445566778899aabbccddeeff 000102030405060708090a0b0c0d0e0f && rm $(TestOutput)
