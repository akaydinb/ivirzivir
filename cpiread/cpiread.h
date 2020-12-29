#include<stdint.h>

/* CPI file reader.  akaydinb -- 14.12.2020
 * With -p parameter, it extracts fonts as well. 
 * Without -p param, prints only fony info. 
 * 
 * Compile:     gcc cpiread.c -std=c99 -I . -o cpiread.x
 * Note: UPX compressed CPI files of FreeDOS (.cpx files) should be 
 * decompressed by upx utility first. 
 * 
 * Source: https://www.seasip.info/DOS/CPI/cpi.html
 * 
 */

struct  FontFileHeader  {
        char  id0;
        char  id[7];
        char  reserved[8];
        short pnum;
        char  ptyp;
        int32_t  fih_offset;
}__attribute__((packed));


struct  FontInfoHeader  {
        short num_codepages;
}__attribute__((packed));


struct  CodePageEntryHeader     {
        short cpeh_size;
        int32_t next_cpeh_offset;
        short device_type;
        char device_name[8];
        short codepage;
        char reserved[6];
        int32_t cpih_offset;
}__attribute__((packed));


struct  CodePageInfoHeader      {
        short version;
        short num_fonts;
        short size;
}__attribute__((packed));


struct  ScreenFontHeader        {
        char height;
        char width;
        char yaspect;
        char xaspect;
        short num_chars;
}__attribute__((packed));

typedef struct FontFileHeader FontFileHeader;
typedef struct FontInfoHeader FontInfoHeader;
typedef struct CodePageEntryHeader CodePageEntryHeader;
typedef struct CodePageInfoHeader  CodePageInfoHeader;
typedef struct ScreenFontHeader ScreenFontHeader;
