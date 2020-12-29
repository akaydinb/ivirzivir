#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"fatread.h"

unsigned int vdi_offset;

FATType fattest(BootSector *bootblock)        {
    unsigned int total_sectors, total_clusters;
        
    if(bootblock->CommonPart.table_size_16 == 0)   
        return FAT32;
    
    total_sectors = (bootblock->CommonPart.total_sectors_16 == 0)  ? 
        bootblock->CommonPart.total_sectors_32 : bootblock->CommonPart.total_sectors_16;
    total_clusters = total_sectors / bootblock->CommonPart.sectors_per_cluster;
    
    if(total_clusters < 4085)
        return FAT12;
    else if(total_clusters < 65525)
        return FAT16;
    else    // ?!
        return FAT32;
}

void calcvalues(BootSector *bootblock, unsigned int *values)    {
    
    values[0] = bootblock->CommonPart.reserved_sector_count + bootblock->CommonPart.hidden_sector_count;
    values[1] = values[0] + bootblock->CommonPart.table_count * 
        ((bootblock->CommonPart.table_size_16 == 0) ? bootblock->UnionPart.FAT32_BootSector.table_size_32 :
                                                      bootblock->CommonPart.table_size_16);
    values[2] = values[1] + bootblock->CommonPart.root_entry_count / (bootblock->CommonPart.bytes_per_sector >> 5);

}

void readFAT32(FILE *imagefile, unsigned int *values) {
    return;
}

void readFAT16(FILE *imagefile, unsigned int *values) {
    return;
}

void readFAT12(FILE *imagefile, unsigned int *values) {
    unsigned char *FAT;
    int fat_entry;
    int cur_cluster = 0;
        
    // << 8, (256 ile carpma) 512 bytes_per_sector / 2 FAT_count sonucu 
    // bu degerler de boot sektörden alinsa daha iyi olurdu. 
    int fatsize = (values[1] - values[0]) << 8; 
    
    FAT = (char *)malloc(sizeof(char) * fatsize);
    
    // burada aslinda 512 yerine boot sektördeki bytes_per_sector degeri kullanilmaliydi.
    fseek(imagefile, values[0] * 512, SEEK_SET);
    fread(FAT, sizeof(char), fatsize, imagefile);

    for(int k = 0; cur_cluster < fatsize; k++)    {
        // cur_cluster disketteki cluster sayisindan fazla olabilir cunku FAT sektörü 
        // sonuna kadar okunuyor ama sondaki girdilerin cluster karsiligi olmayabilir.
        cur_cluster = (k >> 1) + k;
        if(k & 1)       {
            fat_entry = (FAT[cur_cluster] >> 4) | (FAT[cur_cluster + 1] << 4);
        }
        else    {
            fat_entry = FAT[cur_cluster] | ((FAT[cur_cluster + 1] & 0x0F) << 8);
        }
        printf("%d -> %d [%X] \n", k, fat_entry, fat_entry);
    }
}

// daha optimize:
void readFAT12A(FILE *imagefile, unsigned int *values) {
    unsigned char *FAT;
    unsigned short fat_entry;
    int cur_cluster = 0;
        
    // << 8, (256 ile carpma) 512 bytes_per_sector / 2 FAT_count sonucu 
    // bu degerler de boot sektörden alinsa daha iyi olurdu. 
    int fatsize = (values[1] - values[0]) << 8; 
    
    FAT = (char *)malloc(sizeof(char) * fatsize);
    
    // burada aslinda 512 yerine boot sektördeki bytes_per_sector degeri kullanilmaliydi.
    fseek(imagefile, values[0] * 512, SEEK_SET);
    fread(FAT, sizeof(char), fatsize, imagefile);

    for(int k = 0; cur_cluster < fatsize; k++)    {
        // cur_cluster disketteki cluster sayisindan fazla olabilir cunku FAT sektörü 
        // sonuna kadar okunuyor ama sondaki girdilerin cluster karsiligi olmayabilir.
        cur_cluster = (k >> 1) + k;
        if(k & 1)       {
            fat_entry = *((unsigned short *) &FAT[cur_cluster]) >> 4;
        }
        else    {
            fat_entry = *((unsigned short *) &FAT[cur_cluster]) & 0x0FFF;
        }
        printf("%d -> %d [%X] \n", k, fat_entry, fat_entry);
    }
}

unsigned char* getFirstSector(FILE *imagefile)  {
    int i = 0;
    unsigned char vdi_sign[] = "<<< Oracle VM VirtualBox Disk Image >>>";
    unsigned char *First512;
    
    First512 = (unsigned char *)malloc(sizeof(char) * 512);
    fread(First512, sizeof(char), 512, imagefile);
    
    while( vdi_sign[i] == First512[i] )         i++;
    
    if(i == strlen(vdi_sign))   {
        fseek(imagefile, vdi_offset = *(unsigned int *)&First512[0x158], SEEK_SET);
        fread(First512, sizeof(char), 512, imagefile);
    }
    else        {
        vdi_offset = FALSE;
    }
    
    return First512;
}    
    

unsigned char isValidString(unsigned char *string, int len)     {
    int i = 0;
    
    while( isValidChar(string[i] ))      i++;
    
    if(i >= len)
        return TRUE;
    else
        return FALSE;
}


SectorType determineSector(unsigned char *sector)       {
// enum SectorType { MBR, FATBS, NTFS, XFS, undefined };
    
    if( isValidString( &sector[3], 8 ) && (strncmp(&sector[3], "NTFS    ", 8) == 0))        return NTFS;
    else if( isValidString( &sector[3], 8 ) && isValidString( &sector[0x2B], 11 ) && isValidString( &sector[0x36], 8 ))      return FATBS;
    else if( (sector[0x1BE] | sector[0x1CE] | sector[0x1DE] | sector[0x1EE]) == 0x80 )       return MBR;
    else if( strncmp(sector, "XFSB", 4) == 0 ) return XFS;
    else            return undefined;
}

unsigned long processMBR(MBR_Type *sector)       {
    unsigned char existpartition[4];
    unsigned char partitions = 0;
    char c;
    
    for(int i = 0; i < 4; i++)  {
        if(sector->entry[i].PartitionID != 0)   {
            if(sector->entry[i].PartitionID == 0xEE)    {
                printf("This disk contains a GPT partition table. \n");
                return 0;
            }

            printf("Part[%d]: Bootable flag = %02X, PartitionID = %02X, StartSect = %d\n", 
                   i + 1, sector->entry[i].bootableFlag, sector->entry[i].PartitionID, sector->entry[i].StartingLBA);
            existpartition[i] = 1;
            partitions++;
        }
        else
            existpartition[i] = 0;
    }
    
    if(partitions == 0) {
        printf("This disk doesn't contain any partition.\n");
        return 0;
    }
    else if (partitions > 1)    {
        printf("\nSelect partition: ");

        do  {
            do  {
                c = getc(stdin);
            } while( (c < 0x31) || (c > 0x34));
            c = c - 0x31;
        } while( existpartition[c] != 1 );
        
        // TODO: return sektör numarası değil dosya offseti döndürmeli
        return sector->entry[c].StartingLBA;
    }
    else    {
        for(int i = 0; i < 4; i++)  {
            if(existpartition[i])
                // TODO: return sektör numarası değil dosya offseti döndürmeli
                return sector->entry[i].StartingLBA;
        }
    }
//  sector->entry[i].bootableFlag
//  sector->entry[i].StartingHead
//  sector->entry[i].PartitionID
//  sector->entry[i].EndingHead
//  sector->entry[i].StartingLBA
//  sector->entry[i].TotalSectors
    
}

int main(int argc, char *argv[])        {

    FILE          *imagefile;
    unsigned char *firstSector;
    MBR_Type      *MBR_Sector;
    
    long temp;
    
    if( (imagefile = fopen(argv[1], "r")) == NULL)  {
        printf("Image file not found.\n");
        return 1;
    }
    
    firstSector = getFirstSector(imagefile);
    
    switch(determineSector(firstSector))        {
        case MBR: 
            // calculate and seek boot sector
            printf("MBR\n"); 
            temp = processMBR(MBR_Sector = (MBR_Type *)firstSector);
            // TODO: temp == 0 ise hatali birseyler vardir
            printf("temp = %d\n", (int)temp);
            break;
        case FATBS:
            // fattest and calculate values
            printf("FATBS\n"); break;
        case NTFS:
            printf("NTFS partition found. NTFS partitions are not supported.\n"); break;
        case XFS:
            printf("XFS partition found. XFS partitions are not supported.\n"); break;
        default:
            printf("Partition format not recognized. Possibly Linux EXT or PV.\n"); break;
    }
    
    
    
    return 0;
}






// int main(int argc, char *argv[])        {
//     
//     BootSector  A;
//     FATType     F;
//     unsigned int bootsectvalues[3];     // fat_start, root_dir_start, data_start 
// 
//     fread(&A, sizeof(BootSector), 1, imagefile);
//     F = fattest(&A);
//     calcvalues(&A, bootsectvalues);
//     
//     // printf("Fat Start = %X, Root DIR Start = %X, Data Start = %X\n", bootsectvalues[0] * 512, bootsectvalues[1] * 512, bootsectvalues[2] * 512);
//     if(F == FAT12)
//         readFAT12A(imagefile, bootsectvalues);
//     //else if(F == FAT16)
//     //    readFAT16(image, bootsectvalues);
//     //else 
//     //    readFAT32(image, bootsectvalues);
//     
//     return 0;
// }
