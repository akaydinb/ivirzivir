
// source: https://wiki.osdev.org/FAT

#define TRUE ( 1 == 1 )
#define FALSE ( 1 == 0 )

// mkfs.fat writes in small letters:
//                                 (Space)          '-'            '.'            '_'                   [0-9]                           [A-Z]                          [a-z]
#define isValidChar(C)          ((C == 0x20) || (C == 0x2D) || (C == 0x2E) || (C == 0x5F) || ((C >= 0x30) && (C <= 0x39)) || ((C >= 0x41) && (C <= 0x5A)) || ((C >=0x61) && (C <= 0x7A)))


enum SectorType { MBR, FATBS, NTFS, XFS, undefined };
enum FATType { FAT12, FAT16, FAT32 };

struct FATBPB           {       // Common for both FAT12 & FAT16 as well as FAT32
        unsigned char           bootjmp[3];                     // offset 0x0000
        unsigned char           oem_name[8];                    // offset 0x0003
        unsigned short          bytes_per_sector;               // offset 0x000B
        unsigned char           sectors_per_cluster;            // offset 0x000D
        unsigned short          reserved_sector_count;          // offset 0x000E
        unsigned char           table_count;                    // offset 0x0010
        unsigned short          root_entry_count;               // offset 0x0011
        unsigned short          total_sectors_16;               // offset 0x0013
        unsigned char           media_type;                     // offset 0x0015
        unsigned short          table_size_16;                  // offset 0x0016
        unsigned short          sectors_per_track;              // offset 0x0018
        unsigned short          head_side_count;                // offset 0x001A
        unsigned int            hidden_sector_count;            // offset 0x001C
        unsigned int            total_sectors_32;               // offset 0x0020
}__attribute__((packed));

struct FAT16BPB         {       // Only valid for FAT12 and FAT16 but not FAT32
        unsigned char           bios_drive_num;                 // offset 0x0024
        unsigned char           reserved1;                      // offset 0x0025
        unsigned char           boot_signature;                 // offset 0x0026
        unsigned int            volume_id;                      // offset 0x0027
        unsigned char           volume_label[11];               // offset 0x002B
        unsigned char           fat_type_label[8];              // offset 0x0036
        unsigned char           boot_sector_code[448];
        unsigned char           boot_sector_signature[2];       // offset 0x01FE
}__attribute__((packed));


struct FAT32BPB         {       // Only valid for FAT32
        unsigned int            table_size_32;                  // offset 0x0024
        unsigned short          extended_flags;                 // offset 0x0028
        unsigned short          fat_version;                    // offset 0x002A
        unsigned int            root_cluster;                   // offset 0x002C
        unsigned short          fat_info;                       // offset 0x0030
        unsigned short          backup_BS_sector;               // offset 0x0032
        unsigned char           reserved_0[12];                 // offset 0x0034
        unsigned char           drive_number;                   // offset 0x0040
        unsigned char           reserved_1;                     // offset 0x0041
        unsigned char           boot_signature;                 // offset 0x0042
        unsigned int            volume_id;                      // offset 0x0043
        unsigned char           volume_label[11];               // offset 0x0047
        unsigned char           fat_type_label[8];              // offset 0x0052
        unsigned char           boot_sector_code[420];
        unsigned char           boot_sector_signature[2];       // offset 0x01FE
}__attribute__((packed));

struct PartitionTableEntry      {
        unsigned char           bootableFlag;
        unsigned char           StartingHead;
        unsigned short          StartingSectCyl;
        unsigned char           PartitionID;
        unsigned char           EndingHead;
        unsigned short          EndingSectCyl;
        unsigned int            StartingLBA;
        unsigned int            TotalSectors;
}__attribute__((packed));

typedef struct FATBPB           FATBPB;
typedef struct FAT16BPB         FAT16BPB;
typedef struct FAT32BPB         FAT32BPB;
typedef enum   FATType          FATType;
typedef enum   SectorType       SectorType;
typedef struct PartitionTableEntry PartitionTableEntry;

typedef union RestBPB   {
    // RestBPB is whole sector except common part i.e. common for all FATs
    FAT16BPB    FAT16_BootSector;
    FAT32BPB    FAT32_BootSector;
} RestBPB;

typedef struct BootSector       {
    /* since FAT version cannot be known beforehand, common part is simple 
     * struct but the rest is union struct, so after determining FAT version,
     * separate fields can be accessed through subfields in the union. 
     */
    FATBPB      CommonPart;
    RestBPB     UnionPart;
} BootSector;

typedef struct MBR_Type      {  // struct for MBR 
        unsigned char           mbr_code[446];
        PartitionTableEntry     entry[4];
        unsigned char           mbr_signature;
} MBR_Type;
