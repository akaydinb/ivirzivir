#include<stdio.h>
#include<dos.h>
#include<math.h>
//#include<time.h>
//#include<stdlib.h>

// Some useful definitions for Turbo C.
#define false 1 == 0
#define true !false
#define SCROLLSTEPFINE    2
#define SCROLLSTEPCOARSE  8


unsigned short linecount = 0;	// TODO: Make this local

void justify_to_80(char *s)	{
    char tempbuff[81];          // 81 because \n also counts
    int each, extra, i, j, len, missing;
    int buffpointer = 0, sp1 = 0, spaces = 0;
    double weight;
    double oldweight = 0.0;

    len = strlen(s);
    if(len >= 81)   return;     // this should actually never happen

    // Count existing spaces in the line
    for(i = 0; i < len; i++)
        if(s[i] == ' ')	    spaces++;

    if(spaces == 0)     return; // cannot justify without spaces

    missing = 81 - len;         // how many characters to add
    each    = missing / spaces; // space to be added to each space.
    extra   = missing % spaces; // some extra spaces to distribute evenly

    for (i = 0; i < len; i++)	{
        // start copying first words to the temp buffer
        tempbuff[buffpointer++] = s[i];

        if (s[i] == ' ')	 {
            // add spaces if "missing > spaces"
            for (j = 0; j < each; j++)
                tempbuff[buffpointer++] = ' ';

            // now calculate weights for more spaces to add. 
            weight = sp1 * (double)(extra + 1) / spaces;

			if(fabs(weight - oldweight) >= 1.0)  {
				// weights show "add space" points if they overflow to integer
				oldweight = oldweight + 1.0;
				tempbuff[buffpointer++] = ' ';
			}
			sp1++;  // Space Counter
		}
	}
	// Terminate the string and return
	tempbuff[buffpointer-1] = '\0';
	strcpy(s, tempbuff);
}

void vga_set_base_addr(const unsigned short lineP)	{
	unsigned short baseaddr = (lineP << 4) + (lineP << 6);
	outportb(0x3D4, 0xC);
	outportb(0x3D5, baseaddr >> 8);
	outportb(0x3D4, 0xD);
	outportb(0x3D5, baseaddr & 0xFF);
	
}

void waitbl()		{
	/* Wait for full VGA retrace: execution stays in this loop until 
	 * CRT completes its tracing at the lower right of the screen. 
	 */
	while((inportb(0x3DA) & 8) != 8);
	while((inportb(0x3DA) & 8) == 8);
}


void vgaprint(char *line, unsigned short lineBegin)	{
	// send strings directly to VGA memory
	unsigned short far *VideoMemory = (unsigned short far *)MK_FP(0xB800, 0x0000);
	// https://stackoverflow.com/questions/47588486/cannot-write-to-screen-memory-in-c

	int i;
	int linelength = strlen(line);
	
	if(line[linelength - 1] == '\n')
		line[linelength - 1] = 0;
	
	for(i = 0; i < 80; i++)
		VideoMemory[lineBegin + i] = (i < linelength ? line[i] + 0x700 : 0x720);
		// 0x700 is to add color code. VideoMemory is array of "short"s (word)

	linecount++;
}

int main(int argc, char *argv[])    {
	// the lines are longer than 80 chars, as they contain '\n' too.
	char line1[90], line2[90], filename[256];
	FILE *hFile;
	int i, j, keyPressed;
	unsigned short vgapointer = 0;	// to keep where to send data in VGA memory
	unsigned short topLineNumber = 0;
	
	if(argc > 1)	{
		// only the first argument
		strncpy(filename, argv[1], strlen(argv[1]));
		filename[strlen(argv[1])] = '\0';
	}
	else if(argc == 1)	{
		// open a dummy file
		strncpy(filename, "LOREM2.TXT", 10);
		filename[10] = '\0';
	}

  	if((hFile = fopen(filename, "r")) == NULL)	{
		printf("The specified file not found.\n");
		return 1;
	}
  	fgets(line1, 90, hFile);        // read first line.

	while( !feof(hFile) )    {
		fgets(line2, 90, hFile);    // read second line
		if((strlen(line2) > 1) && !feof(hFile))
			// if next line is just one character, it is a paragraph break.
			// do not justify in this case. don't justify if EoF, too.
			justify_to_80(line1);
		//printf("%s", line1);
		vgaprint(line1, vgapointer);
		vgapointer = vgapointer + 80;
		strcpy(line1, line2);       // next line is current line now.
  	}

  	fclose(hFile);

	// printf("Lines: %d\n", linecount);
	
	while((keyPressed = bioskey(0x10)) != 0x011B)		{
		switch (keyPressed) {
		case 0x48E0:
			//printf("Up Arrow Pressed\n");
			if(topLineNumber > 0)	{	// if you're at top, can't go higher
				vga_set_base_addr(--topLineNumber);
				for(i = 15; i >= 0; i = i - SCROLLSTEPFINE)	{
					// smooth scroll
					waitbl();
					outportb(0x3D4, 8);
					outportb(0x3D5, i);
				}
				if(i)	{
					// in case there are some lines left
					waitbl();
					outportb(0x3D4, 8);
					outportb(0x3D5, 0);
				}
			}
			break;
		case 0x50E0: case 0x1C0D:    // Down Arrow or Enter
			//printf("Down Arrow Pressed\n");
			if(topLineNumber < linecount - 24)	{	// if you're at the bottom, can't go lower
													// but keep showing the last 24 lines on the screen
				for(i = 0; i < 15; i = i + SCROLLSTEPFINE)	{
					// smooth scroll
					waitbl();
					outportb(0x3D4, 8);
					outportb(0x3D5, i);
				}
				vga_set_base_addr(++topLineNumber);
				// reset row scan
				waitbl();
				outportb(0x3D4, 8);
				outportb(0x3D5, 0);
			}
			break;
		case 0x49E0:
			//printf("Pg Up Pressed\n");
			for(j = 0; j < 24; j++)	{	// Scroll up 24 lines
				if(topLineNumber == 0)	// stop if you reach to top
					break;
				//waitbl();
				vga_set_base_addr(--topLineNumber);
				for(i = 15; i >= 0; i = i - SCROLLSTEPCOARSE)	{
					//smooth scroll
					outportb(0x3D4, 8);
					outportb(0x3D5, i);
				}
				if(i)	{
					// in case there are some lines left
					waitbl();	// <- This is maybe not needed
					outportb(0x3D4, 8);
					outportb(0x3D5, 0);
				}
			}
			break;
		case 0x51E0: case 0x3920:	// PG DOWN or Space
			//printf("Pg Down Pressed\n");
			for(j = 0; j < 24; j++)	{	// Scroll down 24 lines
				if(topLineNumber >= linecount - 24)
					// did we reach to the bottom? 
					break;
				for(i = 0; i < 15; i = i + SCROLLSTEPCOARSE)	{
					//smooth scroll
					outportb(0x3D4, 8);
					outportb(0x3D5, i);
				}
				waitbl();
				vga_set_base_addr(++topLineNumber);
				waitbl();
				// reset row scan
				outportb(0x3D4, 8);
				outportb(0x3D5, 0);
			}
			break;
//		default:
//			printf("Pressed Key is : %X\n", keyPressed);
//			break;    // Switch statement
		}
	}

	return 0;
}



/* ********************************************************************  
void dumpmem()	{
	unsigned char far *VMByte = (unsigned char far *)MK_FP(0xB800, 0x0000);
	
	int i;

	for(i = 0; i < 8; i++)
		printf("%X ", VMByte[i]);

	printf("- ");
	for(i = 8; i < 16; i++)
		printf("%X ", VMByte[i]);

	printf(" ");

	for(i = 0; i < 16; i++)
		printf("%c", VMByte[i] < 32 ? ' ' : VMByte[i]);

}	
*/
