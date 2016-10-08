#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/aes.h>
#define AES_BITS 128
#define MSG_LEN 128

/*=============================
AES128 encryption using ecb 
Return number of byte encrypted.
===============================
*/
int aes_encrypt(char* in, char* key, char* out)//, int olen)可能会设置buf长度
{
   int i;
   char tempData[16];

    if(!in || !key || !out) return 0;
    unsigned char iv[AES_BLOCK_SIZE];//加密的初始化向量
    for(i=0; i<AES_BLOCK_SIZE; ++i)//iv一般设置为全0,可以设置其他，但是加密解密要一样就行
    	iv[i]=0;
    AES_KEY aes;
    if(AES_set_encrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
	printf("error in set encryption key\n");
        return 0;
    }
    int len=strlen(in);//这里的长度是char*in的长度，但是如果in中间包含'\0'字符的话

    printf("aes_encrypt = in len %d\n", len);
    //那么就只会加密前面'\0'前面的一段，所以，这个len可以作为参数传进来，记录in的长度

    //至于解密也是一个道理，光以'\0'来判断字符串长度，确有不妥，后面都是一个道理。
    //AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, len, &aes, iv, AES_ENCRYPT);

    //for ecb, we need to encrypt block by block independently.
    for (i = 0; i < (len/16 + 1) ; i++) {
       memset(tempData, 0, 16);
       AES_ecb_encrypt((unsigned char*)(in + 16*i), (unsigned char*)tempData, &aes, AES_ENCRYPT);
       memcpy( (unsigned char *)(out + 16*i),  tempData, 16);	
    }
	
  //  return 1;
    return 16*i;
}


int aes_decrypt(char* in, char* key, char* out, int delen)
{
   int i;
   char tempData[16];

    if(!in || !key || !out) return 0;
    unsigned char iv[AES_BLOCK_SIZE];//加密的初始化向量
   //iv一般设置为全0,可以设置其他，但是加密解密要一样就行
    for(i=0; i<AES_BLOCK_SIZE; ++i) {
    	iv[i]=0;
    }
    AES_KEY aes;
    if(AES_set_decrypt_key((unsigned char*)key, 128, &aes) < 0)
    {
	printf("error in set decryption key\n");
        return 0;
    }
    int len=delen;
   // AES_cbc_encrypt((unsigned char*)in, (unsigned char*)out, len, &aes, iv, AES_DECRYPT);
  //  AES_ecb_encrypt((unsigned char*)in, (unsigned char*)out, &aes, AES_DECRYPT);
  //  return 1;

   //for ecb, we need to dencrypt block by block independently.
    for (i = 0; i < (len/16 + 1) ; i++) {
       memset(tempData, 0, 16);
       AES_ecb_encrypt((unsigned char*)(in + 16*i), (unsigned char*)tempData, &aes, AES_DECRYPT);
       memcpy( (unsigned char *)(out + 16*i),  tempData, 16);	
    }

    return 1;

}



