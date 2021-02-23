#include <string.h>
#include <stdlib.h>
#include "R_STDLIB.C"
#include "R_RANDOM.C"
#include "NN.C"
#include "RSA.C"
#include "DIGIT.C"
#include "MD5C.C"
#include "PRIME.C"
#include "R_KEYGEN.C"
#include "DESC.C"
#include "MD2C.C"
#include "R_ENCODE.C"
#include "R_ENHANC.C"
#include <stdio.h>

void shows (char *msg, unsigned char *output, unsigned int len)
{  printf ("%s: ", msg);
   for (unsigned int i=0; i<len; i++)
	    printf("%x", output[i]);
   printf("\n");
}

void seed_randomStruct (unsigned char *seed, R_RANDOM_STRUCT *randomStruct)
{
    unsigned int bytesNeeded = 256;  //�ṹ���������ӳ���

    R_RandomInit (randomStruct);	
    while (bytesNeeded > 0)
    { 
       R_RandomUpdate (randomStruct, seed, 
                                       strlen((char *)seed));
	   R_GetRandomBytesNeeded (&bytesNeeded, randomStruct);
	}
}

void create_RSAkey (R_RSA_PUBLIC_KEY *publicKey, R_RSA_PRIVATE_KEY *privateKey,  unsigned int modul_bits,
     int useFermat4, R_RANDOM_STRUCT *randomStruct)
{
   R_RSA_PROTO_KEY   protoKey;
   int flag;

   protoKey.bits = modul_bits;           //�趨ģ������
   protoKey.useFermat4 = useFermat4;      //�趨e

   flag = R_GeneratePEMKeys 
         (publicKey, privateKey, &protoKey, randomStruct);   // ����RSA��Կ
    if (RE_MODULUS_LEN == flag)
    {  printf ("modulus length invalid\n");  exit(0); }
    else if (RE_NEED_RANDOM == flag)
    {  printf ("randomStruct is not seeded\n");  exit(0); }
}



int main(int argc, char* argv[])
{  
   R_RANDOM_STRUCT randomStruct;
   R_RSA_PUBLIC_KEY publicKey;
   R_RSA_PRIVATE_KEY privateKey;
   R_SIGNATURE_CTX Scontext, Vcontext;

   unsigned char seed[] = "asdfsafsafs2341131231";
   unsigned char signature[MAX_ENCRYPTED_KEY_LEN];
   unsigned int  signatureLen;

   FILE *fp;
   unsigned char line[1000];
   
   // ���������ṹ�壬������RSA��Կ
   seed_randomStruct (seed, &randomStruct);
   create_RSAkey 
       (&publicKey, &privateKey,1024, 1, &randomStruct);

	// ���ļ�file1.txt����ǩ��
   if(NULL ==(fp= fopen("file1.txt", "r" )))
   {   printf("open file1 error\n"); return 0; }

   if (RE_DIGEST_ALGORITHM == R_SignInit (&Scontext, DA_MD5))
   {  printf ("digestAlgorithm is invalid\n");   return 0;  }

   while(fgets((char *)line, 1000, fp))
      R_SignUpdate (&Scontext, line, strlen((char *)line));

   fclose (fp);

   if (RE_PRIVATE_KEY == R_SignFinal 
                              (&Scontext, signature, &signatureLen,  &privateKey))
   {
      printf ("privateKey cannot encrypt message digest\n");
      return 0;
   }

   shows("signature", signature, signatureLen);

	// У��ǩ��
   if(NULL ==(fp= fopen("file2.txt", "r" )))
   {  printf("open file2 error\n");  return 0; }

   if (RE_DIGEST_ALGORITHM == R_VerifyInit (&Vcontext, DA_MD5))
   {  printf ("digestAlgorithm is invalid\n"); return 0;}

   while(fgets((char *)line, 1000, fp))
      R_VerifyUpdate 
           (&Vcontext, line, strlen((char *)line));

   fclose (fp);

   int ret = R_VerifyFinal 
                    (&Vcontext, signature, signatureLen, &publicKey);

   printf ("verify result: ");
   switch (ret)
   {  
      case 0: 
               printf("success\n"); break;
      case RE_SIGNATURE: 
              printf("signature is incorrect\n");  break;
	  case RE_LEN: 
              printf("signatureLen out of range\n"); break;
      case RE_PUBLIC_KEY: 
              printf("publicKey cannot decrypt signature\n"); break;
   }
   R_RandomFinal (&randomStruct);
   return 0; 
}
