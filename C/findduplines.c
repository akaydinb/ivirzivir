
/* .bash_history icerisindeki duplicate satirlari bulup bunlari siler.
 * olusturulan bashhistoryuniq dosyasini .bash_history'nin üzerine
 * yazmak kullaniciya birakilmistir. Orjinal dosyayi degistirmez.
 * Dikkat: Eğer timestamp'ler aktifse, bunlari dikkate almadan siler
 */

#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[])	{
    int i, j;
    int max_satiruz = 0, satsay = 0, satiruz = 0;
    char ch;
    char *bh_tablo, *birsatir;
    FILE *hBH, *hBHout;
    
    hBH    = fopen(".bash_history", "r");
    // .bash_history'deki satir sayisini ve en uzun satirin uzunlugunu bul
    while(EOF != (ch = getc(hBH)))	{
        ++satiruz;
        if(ch == '\n')	{
            ++satsay;
            if(max_satiruz < satiruz)	max_satiruz = satiruz;
            satiruz = 0;
        }
    }

    // en uzun satirin uzunlugunu 2nin bir sonraki en yakin ussune yuvarla
    max_satiruz--;
    max_satiruz |= max_satiruz >> 1;
    max_satiruz |= max_satiruz >> 2;
    max_satiruz |= max_satiruz >> 4;
    max_satiruz |= max_satiruz >> 8;
    max_satiruz |= max_satiruz >> 16;
    max_satiruz++;

    fseek(hBH, 0, SEEK_SET);
    //fprintf(stdout, "satir sayisi: %d\nen uzun satir: %d\n\n", satsay, max_satiruz);

    // butun bash_history bellege aktarilsin
    bh_tablo = (char *)malloc(sizeof(char) * max_satiruz * satsay);
    //birsatir = (char *)malloc(sizeof(char) * max_satiruz);

    for(i = 0; i < satsay; i++)	{
        fgets(&bh_tablo[i * max_satiruz], max_satiruz, hBH);
        //printf("%s", &bh_tablo[i * max_satiruz]);
    }

    // essiz kayitlar icin yeni bir bash_history ac
    hBHout = fopen("bashhistuniq", "w");

    for(i = 0; i < satsay - 1; i++)	{
        // ch, kaydin essizligine ait bir bayrak
        ch = 0;
        for(j = i + 1; j < satsay; j++)	{
            // ayni kaydin esi var mi karsilastirip eger varsa ch'yi bir yap
            if(!strcmp(&bh_tablo[i * max_satiruz], &bh_tablo[j * max_satiruz]))   ch = 1;
        }
        // eger ch hic bir olmamissa bu essiz bir kayittir. bunu dosyaya yaz
        if(!ch)	fprintf(hBHout, "%s", &bh_tablo[i * max_satiruz]);
    }

    fclose(hBH);
    fclose(hBHout);

    free(bh_tablo);

    return 0;
}
