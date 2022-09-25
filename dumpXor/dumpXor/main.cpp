
#include <stdio.h>



int main(int argc, char* argv[]) {


    int keylen, index = 0;
    char* source, * dest, fBuffer[1], tBuffer[20], ckey;

    FILE* fSource, * fDest;

    source = argv[1]; // 原文件
    dest = argv[2];   // 目的文件

    char* key = (char*)"thisisgood";

    // 获取key长度
    keylen = sizeof(key);

    fSource = fopen(source, "rb");
    fDest = fopen(dest, "wb");

    while (!feof(fSource)) {

        fread(fBuffer, 1, 1, fSource);    // 读取1字节

        if (!feof(fSource)) {
            ckey = key[index % keylen];     // 循环获取key
            *fBuffer = *fBuffer ^ ckey;   // xor encrypt
            fwrite(fBuffer, 1, 1, fDest); // 写入文件
            index++;
        }

    }

    fclose(fSource);
    fclose(fDest);

}