#include<stdio.h>
#include<dos.h>
#include<time.h>
#include<stdlib.h>

// Some useful definitions for Turbo C.
#define false 1 == 0
#define true !false

void ScrollUp(void);
void ScrollDown(void);
void waitlinefull(void);
void waitlinehalf(void);

// https://stackoverflow.com/questions/47588486/cannot-write-to-screen-memory-in-c
unsigned short far *VideoMemory = (unsigned short far *)MK_FP(0xB800, 0x0000);
unsigned short *DoubleBuff;

int main(int argc, char *argv[])    {
	int i;
	void (*fp)(void);	// function pointer

	DoubleBuff = (unsigned short *)malloc(sizeof(short) * 80 * 25);

	while(true)		{
		i = bioskey(0x10);
		if(i == 0x48E0)		{
			// if up key pressed
			fp = ScrollUp;
			break;
		}
		else if (i == 0x50E0)	{
			// if down key pressed
			fp = ScrollDown;
			break;
		}
	}

	for(i = 0; i < 25; i++)     {
		// call the function pointed at function pointer "fp"
		// 25 times. each function call scrolls just one line
		(fp)();
		//sleep(1);
	}

	getch();
	return 0;
}

void ScrollUp()   {
	/* In this function, the screen memory is copied to a buffer 
	 * in a scrolled fashion. After VGA scroll operations is done
	 * this buffer is copied back to the screen memory again. 
	 * This is some sort of Double Buffering implementation.
	 */
	int i;
	char oldval;

	for(i = 0; i < 80; )
		VideoMemory[2000 + i] = VideoMemory[i++];
		// copy top line to the (for now) invisible bottom line,
		// the line 26. this is required for a smooth scroll, as
		// this line will be visible while row scan is increased.

	for(i = 0; i < 2000; )
		DoubleBuff[(i + 1920) % 2000] = VideoMemory[i++];
		// the top line of the screen to the bottom line of the buffer
		// second line of the screen to top line of the buffer and so on. 
		// line (N+1) on the screen to the line N in the buffer

	outportb(0x3D4, 0x8);
	oldval = inportb(0x3D5);
	// get the initial value which is most likely 0

	for(i = 0; i < 15; i++)     {
		waitlinefull();
		outportb(0x3D5, oldval | i);
	}

	asm		{
		mov		cx, 2000
		mov		si, DoubleBuff
		mov		ax, 0xB800
		push	ax
		pop		es
		xor		di, di
		rep		movsw
	}	// copy DoubleBuff to the VGA memory as fast as you can

	// now the scrolled screen has been copied,
	// restore the initial value of row scan
	outportb(0x3D4, 0x8);
	outportb(0x3D5, oldval);

}

void ScrollDown()   {
	int i;
	char oldval;
	unsigned short *TempLine;

	TempLine = (unsigned short *)malloc(sizeof(short) * 80);
	// this is a buffer for just a single line of 80 chars. As 
	// VGA also contains color data for each character, the buffer 
	// consists of 80 words (not bytes) for the sake of speed.

	for(i = 0; i < 80; )
		TempLine[i] = VideoMemory[1920 + i++];
		// get the bottom line of the screen. 

	outportb(0x3D4, 0x8);
	oldval = inportb(0x3D5);
	outportb(0x3D5, oldval | 0x0f);
	// get the initial value of row scan and scrool the screen up

	asm		{
		/* Scroll screen down by copying each character to the lower row.
		 * Again: as fast as you can therefore with assembly. 
		 * CX initially contains 4000 for 80 * 25 words. SI points
		 * to the end of VGA Memory, DI added by 160 points to the 
		 * end of the invisible 26th line. As the pointers actually
		 * point to the next line, they needed to be decreased by 
		 * one word. Instead of that, I increased the counter by one
		 * so that, one more invisible char will be copied additionally
		 * but got the same result with one single instruction instead 
		 * of four. The copy operation will be backwards, therefore 
		 * the direction flag is set to UP and data is copied word by 
		 * word for the sake of speed. After that direction flag
		 * is reset back. 
		 */
		push	ds
		mov		cx, 4000
		mov		si, cx
		mov		di, cx
		add		di, 160
		shr		cx, 1
//		dec     si
//		dec     di
//		dec     si
//		dec     di
		inc     cx
		mov		ax, 0xB800
		push	ax
		pop		es
		push	ax
		pop		ds
		std
		rep		movsw
		cld
		pop		ds
	}	

	waitlinefull();		// Wait VGA Retrace

	for(i = 0; i < 80; )
		VideoMemory[i] = TempLine[i++];
		// the bottom line is copied to the top line of the screen

	outportb(0x3D4, 0x8);
	for(i = 14; i >= 0; i--)     {
		waitlinefull();
		outportb(0x3D5, oldval | i);
		// scroll screen down slowly with VGA row scan
	}

	free(TempLine);
}

void waitlinefull()		{
	/* Wait for full VGA retrace: execution stays in this loop until 
	 * CRT completes its tracing at the lower right of the screen. 
	 */
	while((inportb(0x3DA) & 8) != 8);
	while((inportb(0x3DA) & 8) == 8);
}

void waitlinehalf()		{
	/* Wait for half VGA retrace. Do not wait here if the CRT has already
	 * completed its tracing, but only wait if it is still tracing. Faster
	 * than the above variant but less reliable.
	 */
	while((inportb(0x3DA) & 8) == 8);
}
