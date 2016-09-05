Compile=g++ -Wall -g -std=c++11
Source=main.cpp
TestSource=test.cpp
Output=aes
TestOutput=aes-test

.PHONY: all $(Output) clean
.PHONY: test $(TestOutput) clean

all:
	$(Compile) $(Source) -o $(Output)
test:
	$(Compile) $(TestSource) -o $(TestOutput) && valgrind --leak-check=full ./$(TestOutput) && rm $(TestOutput)
