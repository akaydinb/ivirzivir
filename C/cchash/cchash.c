
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<openssl/md5.h>
#include<openssl/sha.h>


void calc_luhn(char *num)  {
    int i, tmp;
    int alt = 1, toplam = 0;

    for(i = 14; i >= 0; i--)   {
        tmp = num[i] - 48;
        if(alt)    {
            tmp = tmp << 1;
            if(tmp > 9)   tmp = tmp - 9;
        }
        toplam = toplam + tmp;
        alt = !alt;
    }
    toplam = (toplam * 9) % 10;
    num[15] = toplam + 48;
    num[16] = 0;
}


char *str2md5(const char *str) {
    int n, length = 16;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char*)malloc(33);

    MD5_Init(&c);

    while (length > 0) {
        if (length > 512) {
            MD5_Update(&c, str, 512);
        } else {
            MD5_Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    MD5_Final(digest, &c);

    for (n = 0; n < 16; ++n) {
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
    }

    return out;
}


char *str2sha512(const char *str)    {
    int n;
    unsigned char *binhash;
    char *out = (char*)malloc(256);

    binhash = (char *)malloc(256 * sizeof(char));
    SHA512(str, 16, binhash);

    for (n = 0; n < 64; ++n) 
        sprintf(&out[n*2], "%02x", (unsigned long)binhash[n]);

    free(binhash);
    return out;
}

char *str2sha256(const char *str)    {
    int n;
    unsigned char *binhash;
    char *out = (char*)malloc(128);

    binhash = (char *)malloc(128 * sizeof(char));
    SHA256(str, 16, binhash);

    for (n = 0; n < 32; ++n)
        sprintf(&out[n*2], "%02x", (unsigned long)binhash[n]);

    free(binhash);
    return out;
}




int main(int argc, char *argv[])   {
    int i, k1;
    FILE *hDosya;
    char numara[17], *hash ;

    hDosya = fopen("cchash.txt", "w");
    hash = (char *)malloc(256 * sizeof(char));

    //for(k1 = 0; k1 < 10000000; k1++)	{
    for(k1 = 0; k1 < 10; k1++)  {
        sprintf(numara, "45436002%07d", k1);
        calc_luhn(numara);
//        hash = str2md5(numara);
//        hash = str2sha512(numara);
        hash = str2sha256(numara);
        fprintf(hDosya, "%s    %s\n", numara, hash);
        free(hash); 
    }

    fclose(hDosya);
    return 0;

}
