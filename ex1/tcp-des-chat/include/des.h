#ifndef DES_H
#define DES_H

#include <bitset>
#include <iostream>
#include <string>
#include <math.h>
#include "const.h"
using namespace std;
class des{
    private:
        bitset<64> key; 
        bitset<48> subKey[16];
    private:
        bitset<64> charToBitset(string s);
        string bitSetToChar(bitset<64> src); 
        void generateKeys(); 
        bitset<28> leftShift(bitset<28> k, int shift);
        bitset<32> f(bitset<32> R, bitset<48> k);
        bitset<64> encode_64(bitset<64> mes); 
        bitset<64> decode_64(bitset<64> res);
    
    public:
        string encode(string mes); //加密
        string decode(string res); //解密
        bool setKey(string key); //设置密钥
};
#endif