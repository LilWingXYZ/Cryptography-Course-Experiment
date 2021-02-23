//#include "stdafx.h"
#include <string.h>
#include "R_STDLIB.C"
#include "R_RANDOM.C"
#include "MD5C.C"
#include "DESC.C"
#include <stdio.h>

//��ʮ��������ʽ��ʾoutput�е�����
void shows (unsigned char *output, int len)
{  printf ("ciphertext: ");
   for (int i=0; i<len; i++)
	   printf("%x", output[i]);
   printf("\n");
}

//��output�е����ݸ��Ƶ�input��
void copys (unsigned char *output, unsigned char *input, int len)
{   for (int i=0; i< len; i++)
         input[i] = output[i];
}

//����salt
void create_salt (unsigned char *salt, int saltlen, unsigned char *seed)
{
	R_RANDOM_STRUCT randomStruct;  //������ṹ��
    unsigned int bytesNeeded = 256;          //�ṹ���������ӳ���

    R_RandomInit (&randomStruct);	
    while (bytesNeeded > 0)
    {   
       R_GetRandomBytesNeeded (&bytesNeeded, &randomStruct);
       R_RandomUpdate (&randomStruct, seed, strlen((char *)seed));
	}
    R_GenerateBytes (salt, saltlen, &randomStruct);
    R_RandomFinal (&randomStruct);
}
//��MD5��Ͽ����salt
void MixPwdSalt (unsigned char *password, 
                            unsigned char *salt, unsigned char result[16])
{
	MD5_CTX context; 

	MD5Init (&context);
    MD5Update (&context, password, 
                          strlen((char *)password));
    MD5Update (&context, salt, strlen((char *)salt));
    MD5Final (result, &context);
}
//��DES�ӽ���
void DES_deal (unsigned char *input, unsigned char *output, 
                          unsigned char *output2,  int len, 
                          unsigned char key[8], unsigned char *iv)
{
   DES_CBC_CTX  context;
   //��ʾ����
   printf ("plaintext: %s\n", input);
   //����
   DES_CBCInit(&context, key, iv, 1);
   DES_CBCUpdate(&context, output, input, len);
  //��ʾ����
   shows (output, len);
   //����
   DES_CBCInit (&context, key, iv, 0); 
   DES_CBCUpdate(&context, output2, output, len);
  //��ʾ���ܺ������
   printf("decrypted ciphertext: %s\n",output2);     
}
#define SALT_LEN   10    //������salt�ĳ���
#define BLOCK_LEN  16  //�����ĳ���
int main(int argc, char* argv[])
{  
    unsigned char seed[] = "3adqwe1212asd"; // ����

    unsigned char salt[SALT_LEN];                  // ���������salt

    unsigned char password[10];                      // ����

    unsigned char mixedresult[16];                   // �����Ϻ�Ľ��

    unsigned char key[8];                                  // ����DES����Կ

    unsigned char iv[8+1] = "abcdfgji";              // IV

    unsigned char input[BLOCK_LEN+1] = "12345678abcdefgh"; // ����

    unsigned char output[BLOCK_LEN]="", output2[BLOCK_LEN+1]=""; 
create_salt (salt, SALT_LEN, seed);      // ����salt
    printf ("please input your password:"); 
    scanf ("%s", password);                         // �������

    MixPwdSalt (password, salt, mixedresult);  // ���salt�Ϳ���
    copys (mixedresult, key, 8);

    DES_deal (input, output, output2, BLOCK_LEN, key, iv);   
    
    return 0; 
}
