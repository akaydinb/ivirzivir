#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"fatread.h"

// Data start offset in vdi file, usually 0x200000. False in case not vdi file
unsigned int vdi_offset;

void calcvalues(BootSector *bootblock, unsigned int *values)    {
    // Calculate fat_start sector, root directory start and data start for accurate cluster number calculation
    values[0] = bootblock->CommonPart.reserved_sector_count; // + bootblock->CommonPart.hidden_sector_count;
    values[1] = values[0] + bootblock->CommonPart.table_count * 
        ((bootblock->CommonPart.table_size_16 == 0) ? bootblock->UnionPart.FAT32_BootSector.table_size_32 :
                                                      bootblock->CommonPart.table_size_16);
    values[2] = values[1] + bootblock->CommonPart.root_entry_count / (bootblock->CommonPart.bytes_per_sector >> 5);
}

FATType fattest(BootSector *bootblock, unsigned int *total_clusters)        {
    // determine FAT version among FAT12, FAT16 and FAT32
    unsigned int total_sectors;
    unsigned int bootsectvalues[3];     // fat_start, root_dir_start, data_start

    calcvalues(bootblock, bootsectvalues);

    //printf("Fat Start = %X [%X], Root DIR Start = %X [%X], Data Start = %X [%X]\n", 
    //      bootsectvalues[0] * 512, bootsectvalues[0],
    //      bootsectvalues[1] * 512, bootsectvalues[1],
    //      bootsectvalues[2] * 512, bootsectvalues[2]);
    
    total_sectors = (bootblock->CommonPart.total_sectors_16 == 0)  ? 
        bootblock->CommonPart.total_sectors_32 : bootblock->CommonPart.total_sectors_16;
    //*total_clusters = total_sectors / bootblock->CommonPart.sectors_per_cluster;  <-- Rough calculation, don't use this.
    *total_clusters = (total_sectors - bootsectvalues[2] + 1) / bootblock->CommonPart.sectors_per_cluster - 1;

    // printf("total sectors = %d\ntotal clusters = %d\n", total_sectors, *total_clusters);

    if(bootblock->CommonPart.table_size_16 == 0)   
        return FAT32;
    
    if(*total_clusters < 4085)
        return FAT12;
    else if(*total_clusters < 65525)
        return FAT16;
    else    // ?!
        return FAT32;
}


void readFAT32(FILE *imagefile, unsigned int clusters, unsigned int FATsectors) {
    printf("FAT32 isn't implemented yet!.\n");
    return;
}

void readFAT16(FILE *imagefile, unsigned int clusters, unsigned short FATsectors) {
    unsigned char *FAT;
    unsigned short fat_entry;
    unsigned int fatsize = FATsectors << 9;     // correct unless sector size is 512
    
    FAT = (char *)malloc(sizeof(char) * fatsize);
    fread(FAT, sizeof(char), fatsize, imagefile);       // read whole FAT

    for(int k = 0; k < clusters; k++)    {
        fat_entry = *(unsigned short *)&FAT[k << 1];
        printf("%d -> %d [%X] \n", k, fat_entry, fat_entry);
    }

    return;
}

void readFAT12(FILE *imagefile, unsigned int clusters, unsigned short FATsectors) {
    /* This function is the first version. Don't use this because it is neglected and
     * doesn't function properly. Use optimized version readFAT12A instead. 
     *
     *  unsigned char *FAT;
     *  unsigned short fat_entry;
     *  int cur_cluster;
     *  unsigned int fatsize = FATsectors << 9;     // correct unless sector size is 512
     *
     *  FAT = (char *)malloc(sizeof(char) * fatsize);
     *  fread(FAT, sizeof(char), fatsize, imagefile);
     *
     *  for(int k = 0; k < clusters; k++)    {
     *      cur_cluster = (k >> 1) + k;
     *      if(k & 1)       {
     *          fat_entry = (FAT[cur_cluster] >> 4) | (FAT[cur_cluster + 1] << 4);
     *      }
     *      else    {
     *          fat_entry = FAT[cur_cluster] | ((FAT[cur_cluster + 1] & 0x0F) << 8);
     *      }
     *      printf("%d -> %d [%X] \n", k, fat_entry, fat_entry);
     *  }
     */
    return;
}

// more optimized:
void readFAT12A(FILE *imagefile, unsigned int clusters, unsigned short FATsectors) {
    unsigned char *FAT;
    unsigned short fat_entry;
    int cur_cluster;
    unsigned int fatsize = FATsectors << 9;     // correct unless sector size is 512
    
    FAT = (char *)malloc(sizeof(char) * fatsize);    
    fread(FAT, sizeof(char), fatsize, imagefile);

    for(int k = 0; k < clusters; k++)    {
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
    
    // TODO: extended partition destegi 
    if( isValidString( &sector[3], 8 ) && (strncmp(&sector[3], "NTFS    ", 8) == 0))        return NTFS;
    else if( isValidString( &sector[3], 8 ) && isValidString( &sector[0x2B], 11 ) && isValidString( &sector[0x36], 8 ))      return FATBS;
    // TODO: eger hicbir partition bootable degilse undefined olarak taniniyor.
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
                fprintf(stderr, "This disk contains a GPT partition table. \n");
                return 0;
            }

            printf("Part[%d]: Bootable flag = %02X, PartitionID = %02X, StartSect = %d\n", 
                   i + 1, sector->entry[i].bootableFlag, sector->entry[i].PartitionID, sector->entry[i].StartingLBA);
            fflush(stdout);
            existpartition[i] = 1;
            partitions++;
        }
        else
            existpartition[i] = 0;
    }
    
    if(partitions == 0) {
        fprintf(stderr, "This disk doesn't contain any partition.\n");
        return 0;
    }
    else if (partitions > 1)    {
        printf("\nSelect partition: ");
        fflush(stdout);

        do  {
            do  {
                c = getc(stdin);
            } while( (c < 0x31) || (c > 0x34));
            c = c - 0x31;
        } while( existpartition[c] != 1 );
        
        if(vdi_offset)
            return vdi_offset + (sector->entry[c].StartingLBA << 9);
        else
            return (sector->entry[c].StartingLBA << 9);
    }
    else    {
        for(int i = 0; i < 4; i++)  {
            if(existpartition[i])       {
                if(vdi_offset)
                    return vdi_offset + (sector->entry[c].StartingLBA << 9);
                else
                    return (sector->entry[c].StartingLBA << 9);
            }
        }
    }
//  sector->entry[i].bootableFlag
//  sector->entry[i].StartingHead
//  sector->entry[i].PartitionID
//  sector->entry[i].EndingHead
//  sector->entry[i].StartingLBA
//  sector->entry[i].TotalSectors
}

void processFATBS(FILE *imagefile, BootSector *BS)       {
    FATType     F;
    unsigned int num_of_cluster;

    F = fattest(BS, &num_of_cluster);    
    // printf("%d number of cluster\n", num_of_cluster);
    // printf("%d reserved sectors \n", BS->CommonPart.reserved_sector_count);
    
    if(BS->CommonPart.reserved_sector_count > 1)
        fseek(imagefile, (BS->CommonPart.reserved_sector_count - 1) << 9, SEEK_CUR);
        // Yukarida yine 9'a kaydirmayla sektör büyüklüğü 512 byte kabul ediliyor.
    
    if(F == FAT12)
        readFAT12A(imagefile, num_of_cluster, BS->CommonPart.table_size_16);
    else if(F == FAT16)
        readFAT16(imagefile, num_of_cluster, BS->CommonPart.table_size_16);
    else 
        readFAT32(imagefile, num_of_cluster, 0);        // Not implemented yet 


}

int main(int argc, char *argv[])        {

    FILE          *imagefile;
    unsigned char *firstSector;
    MBR_Type      *MBR_Sector;
    long          boot_sect_offset;
    BootSector    *BS;
    
    if( (imagefile = fopen(argv[1], "r")) == NULL)  {
        fprintf(stderr, "Image file not found.\n");
        return 1;
    }
    
    firstSector = getFirstSector(imagefile);    // unsigned char * (512 byte in size)
    
    switch(determineSector(firstSector))        {
        case MBR: 
            boot_sect_offset = processMBR(MBR_Sector = (MBR_Type *)firstSector);
            if(!boot_sect_offset)   return 2; // temp == 0 ise ya GPT disktir veya partition yoktur.
            
            BS = (BootSector *)malloc(sizeof(BootSector));
            if(fseek(imagefile, boot_sect_offset, SEEK_SET))    {
                fprintf(stderr, "fseek error.\n");
                if(!vdi_offset)
                    fprintf(stderr, "This could be possibly caused by a thin vdi file.\n");
            }
            fread(BS, sizeof(BootSector), 1, imagefile);
            
            if(determineSector((unsigned char *)BS) == FATBS)    {
                printf("Processing FAT type boot sector...\n");
                processFATBS(imagefile, BS);
            }
            else        {
                fprintf(stderr, "This boot sector has an unsupported format.\n");
                return 3;
            }
            break;
        case FATBS:
            printf("Fat partition found.\n");
            BS = (BootSector *)firstSector;
            processFATBS(imagefile, BS);
            break;
        case NTFS:
            fprintf(stderr, "NTFS partition found. NTFS partitions are not supported.\n"); break;
        case XFS:
            fprintf(stderr, "XFS partition found. XFS partitions are not supported.\n"); break;
        default:
            fprintf(stderr, "Partition format not recognized. Possibly Linux EXT or PV.\n"); break;
    }
    
    
    fclose(imagefile);
    free(firstSector);
    return 0;
}
