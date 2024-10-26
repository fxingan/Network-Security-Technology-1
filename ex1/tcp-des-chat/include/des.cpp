#include"des.h"
//设置DES的加密密钥
bool des::setKey(string key){
    //判断密钥长度是否为8
    if(sizeof(key) != 8)
        return false;
    else
        this->key = charToBitset(key);
    generateKeys(); 
    return true;
}
//将字符串转换为64位的bitset
bitset<64> des:: charToBitset(string s){
    bitset<64> bits;
	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
            bits[56 - 8 * i + j] = ((s[i]>>j) & 1);
        }
    }
	return bits;
 }
 //将64位的bitset转换回字符串
string des:: bitSetToChar(bitset<64> src){
    string res;
    for(int i=0; i<8; ++i){
        int value = 0;
        for(int j=0; j<8; ++j){
            value += src[56 - 8 * i + j] * pow(2,j);
        }
       res += char(value);
    }
    return res;
}
//对28位的bitset进行循环左移
bitset<28> des::leftShift(bitset<28> k, int shift){
    bitset<28> temp = k;
	for(int i = 27; i >= 0; i--){
		if(i-shift<0){
			k[i] = temp[i-shift+28];
            }
		else{
			k[i] = temp[i-shift];
        }
    }
	return k;
}
// 从主加密密钥生成16个用于DES加密的子密钥
void des::generateKeys(){
    bitset<56> new_res;
    for(int i = 0; i < 56; i++){
        new_res[55 - i] = key[64 - PC_1[i]];
    }
    bitset<28> left;
    bitset<28> right;
    for(int i = 0; i < 16; i++){
        for(int j = 0; j < 28; j++){
            left[27 - j] = new_res[55 - j];
        }
         for(int j = 0; j < 28; j++){
            right[j] = new_res[j];
        }
        left = leftShift(left,ShiftBits[i]);
        right = leftShift(right,ShiftBits[i]);
        for(int j = 0; j < 28; j++){
            new_res[55 - j] = left[27 - j]; 
        }
         for(int j = 0; j < 28; j++){
            new_res[j] = right[j]; 
        }
        bitset<48> real_key;
        for(int j = 0; j < 48; j++){
            real_key[j] = new_res[56 - PC_2[j]];
        }
        this->subKey[i] = real_key;
    }
}
//执行Feistel函数，包括扩展、与子密钥的异或、S盒替换和置换
bitset<32> des:: f(bitset<32> R, bitset<48> k){
    bitset<48> expand;
    for(int i = 0; i < 48; i++){
        expand[47 - i] = R[32 - Expand[i]];
    }
    expand = expand ^ k;
    bitset<32> res; //S盒选择压缩后的结果
    int h = 0;
    for(int i = 0; i < 48; i += 6){
        int row = expand[47 - i]*2 + expand[42 - i];
        int col = expand[47 - i - 1]*8 + expand[47 - i - 2]*4 + expand[47- i - 3]*2 + expand[47 - i - 4];
        int value = S_BOX[i / 6][row][col];
        bitset<4> temp = value;
        res[31 - h] = temp[3];
        res[31 - h - 1] = temp[2];
        res[31 - h - 2] = temp[1];
        res[31 - h - 3] = temp[0];
        h += 4;
    }
    bitset<32> real_res;
    for(int i = 0; i < 32; i++){
        real_res[31 - i] = res[32 - P[i]];
    }
    return real_res;
}
//加密一个64位的明文块
bitset<64> des:: encode_64(bitset<64> mes){
    bitset<64> first_ip_displace;
    for(int i = 0; i < 64;i++){
        first_ip_displace[63 - i] = mes[64 - IP[i]];
    }
    bitset<32> left;
    bitset<32> right;
    for(int i = 32; i < 64;i++){
        left[i-32] = first_ip_displace[i];
    }
    for(int i = 0; i < 32;i++){
        right[i] = first_ip_displace[i];
    }
    bitset<32> new_left;
    for(int i = 0; i < 16;i++){
        new_left = right;
        right = left ^ f(right,subKey[i]);
        left = new_left;
    }
    bitset<64> new_res;
    for(int i = 0;i < 32;i++){
        new_res[i] = left[i];
    }
    for(int i = 32;i < 64;i++){
        new_res[i] = right[i - 32];
    }
    bitset<64> res;
    for(int i = 0; i < 64; i++){
        res[63 - i] = new_res[64 - IP_1[i]];
    }
    return res;
}
//解密一个64位的明文块
bitset<64> des::decode_64(bitset<64> res)
{
    bitset<64> first_ip_displace;
    for(int i = 0; i < 64;i++){
        first_ip_displace[63 - i] = res[64 - IP[i]];
    }
    bitset<32> left;
    bitset<32> right;
    for(int i = 32; i < 64;i++){
        left[i-32] = first_ip_displace[i];
    }
    for(int i = 0; i < 32;i++){
        right[i] = first_ip_displace[i];
    }
    bitset<32> old_left,new_left;
    for(int i = 0; i < 16;i++){
        new_left = right;
        right = left ^ f(right,subKey[15 - i]);
        left = new_left;
    }
    bitset<64> new_res;
    for(int i = 0;i < 32;i++){
        new_res[i] = left[i];
    }
    for(int i = 32;i < 64;i++){
        new_res[i] = right[i - 32];
    }
    bitset<64> mes;
    for(int i = 0; i < 64; i++){
        mes[63 - i] = new_res[64 - IP_1[i]];
    }
    return mes;
}
//对字符串进行DES加密
string des::encode(string mes){
    int size = mes.size(); 
    int round = size / 8;
    int i = 0;
    int h = 0;
    string res; 
    while(i < round) {
        string s;
        for(int j = 0; j < 8; j++){
            s += mes[i * 8 + j];
        }
        bitset<64> temp = charToBitset(s);
        bitset<64> cypher = encode_64(temp);
        res += bitSetToChar(cypher);
        h += 8;
        i++;
    }
    if(h < size){
        string temp1;
        for(int i = h; i < size; i++)
        {
            temp1 += mes[i];
        }
        bitset<64> temp2 = charToBitset(temp1);
        bitset<64> cypher = encode_64(temp2);
        res += bitSetToChar(cypher);
    }
    return res;
}
// 对字符串进行DES解密
string des::decode(string res){
    int size = res.size(); 
    int round = size / 8;
    int i = 0;
    int h = 0;
    string mes; 
    while(i < round) {
        string s;
        for(int j = 0; j < 8; j++){
            s += res[i * 8 + j];
        }
        bitset<64> temp = charToBitset(s);
        bitset<64> plain = decode_64(temp);
        mes += bitSetToChar(plain);
        h += 8;
        i++;
    }
    if(h < size){
        string temp1;
        for(int i = h; i < size; i++){
            temp1 += res[i];
        }
        bitset<64> temp2 = charToBitset(temp1);
        bitset<64> plain = decode_64(temp2);
        mes += bitSetToChar(plain);
    }
    return mes;
}