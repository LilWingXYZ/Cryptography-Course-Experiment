//#include "stdafx.h"
#include <string.h>
#include <stdlib.h>
#include "R_STDLIB.C"
#include "MD5C.C"
#include "DESC.C"
#include "R_RANDOM.C"
#include <stdio.h>

//��ʮ��������ʽ��ʾoutput�е�����
void shows (char * msg, unsigned char *output, int len)
{  
   printf("%s: ", msg);
   for (int i=0; i<len; i++)
	   printf("%x", output[i]);
   printf("\n");
}

//���������ṹ��
void seed_randomStruct (unsigned char *seed, R_RANDOM_STRUCT *randomStruct)
{
    unsigned int bytesNeeded = 256;  //�ṹ���������ӳ���

    R_RandomInit (randomStruct);	
    while (bytesNeeded > 0)
    { 
       R_RandomUpdate (randomStruct, seed, 
                                       strlen((char *)seed));
	   R_GetRandomBytesNeeded (&bytesNeeded, 
                                                        randomStruct);
	}
}

//��input��mac�е����ݸ��Ƶ�plaintext��
void combines (unsigned char *plaintext, unsigned char *input, unsigned char mac[8], int inputlen)
{   
	for (int i=0; i< inputlen; i++)
       plaintext[i] = input[i];

	for (int j=0; j<8; j++)
	   plaintext[inputlen+j] = mac[j];
}

//���ָ��������е����ݲ�ֵ�output��mac
void decombines (unsigned char *plaintext, unsigned char *output, unsigned char mac[8], int outputlen)
{   
	for (int i=0; i< outputlen; i++)
       output[i] = plaintext[i];
	output[outputlen] = '\0';

	for (int j = 0; j < 8; j++)
	   mac [j] = plaintext[outputlen+j];
}

//����MAC
void create_MAC(unsigned char mac_output[8], unsigned char *input, 
		                int len, unsigned char key[8], unsigned char iv[8])
{
   unsigned char *output;
   DES_CBC_CTX  context;

   output = new unsigned char[len];

   DES_CBCInit(&context, key, iv, 1);
   DES_CBCUpdate(&context, output, input, len);

   for (int i=0; i<8;i++)
	   mac_output[i] = output[len-8+i];

   delete []output;
}

//�Ƚ��յ���MAC�Ͷ��յ���Ϣ������MAC
int compares(unsigned char received_mac[8], 
                      unsigned char new_mac[8])
{
	for(int i=0; i<8; i++)
	{
		if (received_mac[i] != new_mac[i])
		     return 0;
	}
	return 1;
}

#define TEXT_LEN  16  // ��Ϣ����
#define PLAINTEXT_LEN   TEXT_LEN + 8 // �����ĳ���(�������ַ���ĩβ��'\0')
int main(int argc, char* argv[])
{  
unsigned char seed[] = "12312ae12qweqweqweqe";
unsigned char key1[8], key2[8], iv1[8]={0}, iv2[8];
unsigned char input[TEXT_LEN+1] = "12345678abcdefgh"; // ���淢�͵���Ϣ
unsigned char mac[8];                                    // ����Է�����Ϣ������MAC
unsigned char plaintext[PLAINTEXT_LEN];   // ��������
unsigned char ciphertext[PLAINTEXT_LEN]; // ��������
unsigned char plaintext2[PLAINTEXT_LEN]; // ����ָ�������
unsigned char output[TEXT_LEN+1]; // ����ָ������Ϣ
unsigned char received_mac[8];        // ����ָ����MAC
unsigned char new_mac [8];              // ����Խ��յ�����Ϣ������MAC

R_RANDOM_STRUCT randomStruct;
DES_CBC_CTX  context;

seed_randomStruct (seed, &randomStruct);
//������Կ��IV, �ֱ����ڲ���MAC�ͼӽ���
R_GenerateBytes (key1, 8, &randomStruct);
R_GenerateBytes (key2, 8, &randomStruct);
R_GenerateBytes (iv2, 8, &randomStruct);
// ��ʾ������Ϣ
printf ("sent message: %s\n", input);

//�Է��͵���Ϣ����MAC
create_MAC(mac, input, TEXT_LEN, key1, iv1);

// ��ʾMAC
shows("sent MAC", mac, 8);

//�����Ϣ��MACΪ����
combines (plaintext, input, mac, TEXT_LEN);

//����
DES_CBCInit(&context, key2, iv2, 1);
DES_CBCUpdate(&context, ciphertext, plaintext,   
                               PLAINTEXT_LEN);

// ��ʾ����
shows("ciphertext", ciphertext, PLAINTEXT_LEN);

//ciphertext[10] = ciphertext[10] + 1; //�ı����ĵ�һ���ֽ�

//����
DES_CBCInit(&context, key2, iv2, 0);
DES_CBCUpdate(&context, plaintext2, ciphertext, 
                              PLAINTEXT_LEN);

//�������е����ݲ�ֵ�output��received_mac
decombines (plaintext2, output, received_mac, TEXT_LEN);

//��ʾ���ܺ����Ϣ��MAC
printf("reveived message: %s\n", output);
shows("received MAC", received_mac, 8);

//���յ�����Ϣ����MAC, ����ʾ
create_MAC(new_mac, output, TEXT_LEN, key1, iv1);
shows("MAC for received message", new_mac, 8);

//У��MAC
if (compares(received_mac, new_mac))
  printf ("received message OK!\n");
else
  printf ("received message ERROR!\n");

R_RandomFinal (&randomStruct);

return 0; 
}





 