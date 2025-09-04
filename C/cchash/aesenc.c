
#include <stdio.h> 
#include <openssl/aes.h>   

static const unsigned char key[] = {
  0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
  0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f   };

//static const unsigned char key[] = "Qazxsw123";

void main()    {
    int k1 = 0;
    unsigned char text[] = "virident";
    unsigned char out[1024]; 
    unsigned char decout[1024];

    AES_KEY wctx;

    AES_set_encrypt_key(key, 128, &wctx);
    AES_encrypt(text, out, &wctx);  

    printf("Encrypted data = ");
    while( out[k1] != 0 )  {
        //printf("Encrypted data = %s\n", out);
        printf("%X", out[k1] >> 4);
        printf("%X ", out[k1++] % 16);
    }
    printf("\n");
    printf("Encrypted data = %s\n", out);

    AES_set_decrypt_key(key, 128, &wctx);
    AES_decrypt(out, decout, &wctx);
    printf("Decrypted data = %s\n", decout);


}


