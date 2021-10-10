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
    unsigned int fatsize = FATsectors << 9;     // correct if sector size is 512
    
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
     *  unsigned int fatsize = FATsectors << 9;     // correct if sector size is 512
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
    unsigned int fatsize = FATsectors << 9;     // correct if sector size is 512
    
    FAT = (char *)malloc(sizeof(char) * fatsize);    
    fread(FAT, sizeof(char), fatsize, imagefile);       // read whole FAT

    for(int k = 0; k < clusters; k++)    {
        cur_cluster = (k >> 1) + k;             // multiply by 1.5
        if(k & 1)       {
            // for odd numbered clusters just shift 4 bits
            fat_entry = *((unsigned short *) &FAT[cur_cluster]) >> 4;
        }
        else    {
            // for even numbered clusters take the last 12 bits into condieration
            fat_entry = *((unsigned short *) &FAT[cur_cluster]) & 0x0FFF;
        }
        printf("%d -> %d [%X] \n", k, fat_entry, fat_entry);
    }
}

unsigned char* getFirstSector(FILE *imagefile)  {
    int i = 0;
    unsigned char vdi_sign[] = "<<< Oracle VM VirtualBox Disk Image >>>";  // vdi file signature
    unsigned char *First512;
    
    First512 = (unsigned char *)malloc(sizeof(char) * 512);
    fread(First512, sizeof(char), 512, imagefile);      // read first 512 bytes
    
    while( vdi_sign[i] == First512[i] )         i++;
    
    if(i == strlen(vdi_sign))   {
        // if the file contains vdi file signature it is a vdi file. in that case
        // seek using the data start pointer at 0x158 and store data start offset
        fseek(imagefile, vdi_offset = *(unsigned int *)&First512[0x158], SEEK_SET);
        // Re-read first 512 bytes of the disk:
        fread(First512, sizeof(char), 512, imagefile);
    }
    else        {
        // if the file isn't a vdi file, set vdi_offset to FALSE
        vdi_offset = FALSE;
    }
    
    return First512;
}    

unsigned char isValidString(unsigned char *string, int len)     {
    /* this function checks a given string with a given length
     * is valid, by checking its characters one by one using 
     * isValidChar macro defined in fatread.h. Chars are iterated
     * as long as they are valid and compared with string length
     * at the function exit. 
     */
    int i = 0;
    
    while( isValidChar(string[i] ))      i++;
    
    if(i >= len)
        return TRUE;
    else
        return FALSE;
}

SectorType determineSector(unsigned char *sector)       {
// determine Sector type given by *sector pointer
// enum SectorType { MBR, FATBS, NTFS, XFS, undefined };
    
    // TODO: extended partition support.
    if( isValidString( &sector[3], 8 ) && (strncmp(&sector[3], "NTFS    ", 8) == 0))        return NTFS;
    else if( isValidString( &sector[3], 8 ) && isValidString( &sector[0x2B], 11 ) && isValidString( &sector[0x36], 8 ))      return FATBS;
    // TODO: in case none of the partitions are bootable, the disk is recognized as "undefined".
    // although there can be only one active partition only, the OR and comparison below is very
    // efficient in code size and speed but only "enough" accurate.
    else if( (sector[0x1BE] | sector[0x1CE] | sector[0x1DE] | sector[0x1EE]) == 0x80 )       return MBR;
    else if( strncmp(sector, "XFSB", 4) == 0 ) return XFS;
    else            return undefined;
}

unsigned long processMBR(MBR_Type *sector)       {
    unsigned char existpartition[4];    // flag, its value can be either 0 or 1
    unsigned char partitions = 0;       // number of active partitions 
    char c;
    
    for(int i = 0; i < 4; i++)  {
        // only check PartitionID field. PartitionID 0 is only valid for linux fdisk.
        if(sector->entry[i].PartitionID != 0)   {
            if(sector->entry[i].PartitionID == 0xEE)    {
                fprintf(stderr, "This disk contains a GPT partition table. \n");
                return 0;
            }
            // print the list of active partitions:
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
        // if there are more than one partition on the disk, the user has to select 
        // the partition to be read
        printf("\nSelect partition: ");
        fflush(stdout);

        do  {
            do  {
                c = getc(stdin);
            } while( (c < 0x31) || (c > 0x34));         // input must be in [1,2,3,4]
            c = c - 0x31;       // ascii to num conversion with decrement of 1
        } while( existpartition[c] != 1 );      // stay in loop until a valid partition is selected
        
        if(vdi_offset)
            // if this is a vdi file, starting sector offset must be within the vdi file
            return vdi_offset + (sector->entry[c].StartingLBA << 9);
        else
            return (sector->entry[c].StartingLBA << 9);
    }
    else    {
        // if there is only one valid partition on the disk:
        for(int i = 0; i < 4; i++)  {
            if(existpartition[i])       {
                if(vdi_offset)
                    // if this is a vdi file, starting sector offset must be within the vdi file
                    return vdi_offset + (sector->entry[i].StartingLBA << 9);
                else
                    return (sector->entry[i].StartingLBA << 9);
            }
        }
    }
//  sector->entry[i].bootableFlag       \
//  sector->entry[i].StartingHead        \
//  sector->entry[i].PartitionID          \____\  Just for autocompletion
//  sector->entry[i].EndingHead           /    /  Nothing important : )
//  sector->entry[i].StartingLBA         /
//  sector->entry[i].TotalSectors       /
}

void processFATBS(FILE *imagefile, BootSector *BS)       {
    FATType     F;
    unsigned int num_of_cluster;

    // test boot sector for FAT type, return FAT type and number of clusters
    F = fattest(BS, &num_of_cluster);    
    // printf("%d number of cluster\n", num_of_cluster);
    // printf("%d reserved sectors \n", BS->CommonPart.reserved_sector_count);
    
    /* if reserved sectors are just one (which is the boot sector itself) 
     * there is no need for an additional fseek since FAT begins right after
     * the boot sector and we already sought there by reading whole boot sector
     * 
     * on the other hand if reserved sectors are greater than one (in case FAT32
     * e.g.) that number of sectors minus one has to be skipped in order to find 
     * the beginning of FAT:
     */
    if(BS->CommonPart.reserved_sector_count > 1)
        fseek(imagefile, (BS->CommonPart.reserved_sector_count - 1) << 9, SEEK_CUR);
        // The 9-bit shift operation above is only correct for 512 byte sized sectors
    
    if(F == FAT12)
        readFAT12A(imagefile, num_of_cluster, BS->CommonPart.table_size_16);
    else if(F == FAT16)
        readFAT16(imagefile, num_of_cluster, BS->CommonPart.table_size_16);
    else 
        readFAT32(imagefile, num_of_cluster, 0);        // Not implemented yet 


}

int main(int argc, char *argv[])        {

    FILE          *imagefile;
    unsigned char *firstSector;         // since the first sector is undetermined yet, it is just unsigned char
    MBR_Type      *MBR_Sector;
    long          boot_sect_offset;
    BootSector    *BS;
    
    if( (imagefile = fopen(argv[1], "r")) == NULL)  {
        // just file not found. no complex checks
        fprintf(stderr, "Image file not found.\n");
        return 1;
    }
    
    firstSector = getFirstSector(imagefile);    // unsigned char * (512 byte in size)
    
    switch(determineSector(firstSector))        {
        case MBR: 
            boot_sect_offset = processMBR(MBR_Sector = (MBR_Type *)firstSector);
            if(!boot_sect_offset)   return 2; // temp == 0 => either GPT disk or no partition.
            
            BS = (BootSector *)malloc(sizeof(BootSector));
            if(fseek(imagefile, boot_sect_offset, SEEK_SET))    {
                fprintf(stderr, "fseek error.\n");
                if(!vdi_offset)
                    fprintf(stderr, "This could be possibly caused by a thin vdi file.\n");
                /* as the error states, thin vdi files are not implemented. the
                 * code seeks as if the vdi is dense and checks the bytes. if 
                 * there is a seek error or fseeks lands to a different sector,
                 * this is most likely because vdi file is thin...  */
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
            // first sector is determined as boot sector. So pointer assignment:
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
