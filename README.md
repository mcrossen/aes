# AES
The AES cypher written in C++ for CS 360 at BYU by Mark Crossen.

## Usage
To encrypt or decrypt a hexadecimal string, open a terminal and type:
```bash
aes <encrypt|decrypt> <string> <key>
```
where \<string\> is replaced with the hexadecimal string that you want to encrypt or decrypt, and \<key\> is the 128, 192, or 256 bit key to encrypt or decrypt it with.
Note: the hexadecimal string must be a multiple of 128 bits.

## Building
To get a runnable executable, clone the repo and make the project like so:
```bash
git clone https://github.com/mcrossen/aes.git
cd aes/
make all
```
If successful, this should've created an executable for you called "aes".
