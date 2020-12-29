#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<./cpiread.h>

// typedef struct FontFileHeader FontFileHeader;
// typedef struct FontInfoHeader FontInfoHeader;
// typedef struct CodePageEntryHeader CodePageEntryHeader;
// typedef struct CodePageInfoHeader  CodePageInfoHeader;
// typedef struct ScreenFontHeader ScreenFontHeader;

void printchars(FILE *fileHandle, char fontwidth, char fontheight, short numberofchars)         {    
    char t;
    
    for(int i = 0; i < numberofchars; i++)      {
        printf("\n\n========== Char %d ==========\n", i);
        for(int height = 0; height < fontheight; height++)      {
            fread(&t, sizeof(char), 1, fileHandle);
            for(int width = fontwidth; width > 0; width--)
                printf("%c", (t & (1 << width - 1)) ? 'X' : ' ');
            printf("\n");
        }
    }
}


int main(int argc, char *argv[])        {

    int numCP, numFont;
    int fontprint = 0;
    char filename[256];
    FILE *cpihandle;
    FontFileHeader ffheader;
    FontInfoHeader fiheader;
    CodePageEntryHeader *cpeheaders;
    CodePageInfoHeader  *cpiheaders;
    ScreenFontHeader **scfh;
    
    
    if((argc == 1) || (argc > 3))       {
        printf("This program extracts information from cpi files\nUsage: cpiread  [ -p ]  <cpifile>\n\n");
        return 2;
    }
    else if(argc == 2)
        strcpy(filename, argv[1]);
    else     {
        if( argv[1][0] == '-' && argv[1][1] == 'p' )    {
            strcpy(filename, argv[2]);
            fontprint = 1;
        }
        if( argv[2][0] == '-' && argv[2][1] == 'p' )    {
            strcpy(filename, argv[1]);
            fontprint = 1;
        }
    }
    
    if((cpihandle = fopen(filename, "r")) == NULL)       {
        printf("File not found!\n");
        return 1;
    }
    
    fread(&ffheader, sizeof(FontFileHeader), 1, cpihandle);
    fseek(cpihandle, ffheader.fih_offset, SEEK_SET);
    fread(&fiheader, sizeof(FontInfoHeader), 1, cpihandle);
    
    printf("File name is %s\nId = \"%s\"\n", filename, ffheader.id);
    printf("Number of CPs in this file = %X\n", numCP = fiheader.num_codepages);
    cpeheaders = (CodePageEntryHeader *)malloc(numCP * sizeof(CodePageEntryHeader));
    cpiheaders = (CodePageInfoHeader *)malloc(numCP * sizeof(CodePageInfoHeader));
    scfh = (ScreenFontHeader **)malloc(numCP * sizeof(ScreenFontHeader *));
    
    for(int i = 0; i < numCP; i++)      {
        fread(&cpeheaders[i], sizeof(CodePageEntryHeader), 1, cpihandle);
        printf("\n\n[%d] ===== %d =====\n", (unsigned)ftell(cpihandle), i);
        printf("Device type is: %s\n", (cpeheaders[i].device_type == 1 ? "screen" : "printer"));
        printf("Device name is: %.8s\n", cpeheaders[i].device_name);
        printf("Code page number: %hu\n", cpeheaders[i].codepage);
        
        fread(&cpiheaders[i], sizeof(CodePageInfoHeader), 1, cpihandle);
        printf("Code Page version: %d, Number of fonts: %d, Size: %d\n\n", 
               cpiheaders[i].version, numFont = cpiheaders[i].num_fonts, cpiheaders[i].size);
        
        scfh[i] = (ScreenFontHeader *)malloc(numFont * sizeof(ScreenFontHeader));
        
        for(int j = 0; j < numFont; j++)        {
            fread( &scfh[i][j], sizeof(ScreenFontHeader), 1, cpihandle );
            
            printf("[%d] ========== %d ==========\n", (unsigned)ftell(cpihandle), j);
            printf("Size: %d x %d\n", scfh[i][j].height, scfh[i][j].width);
            printf("Number of Chars: %d\n\n", scfh[i][j].num_chars);
            
            if(fontprint)
                printchars(cpihandle, scfh[i][j].width, scfh[i][j].height, scfh[i][j].num_chars);
            else
                // skip char block                 
                fseek(cpihandle, ((scfh[i][j].height * scfh[i][j].width) >> 3) * scfh[i][j].num_chars, SEEK_CUR);
        }
        
        // jump to next CodePageEntryHeader
        fseek(cpihandle, cpeheaders[i].next_cpeh_offset, SEEK_SET);
    }
    
    // Free memory
    for(int i = 0; i < numCP; i++)
        free(scfh[i]);
    
    free(scfh);
    free(cpiheaders);
    free(cpeheaders);
    
    fclose(cpihandle);
    
    return 0;
    
}