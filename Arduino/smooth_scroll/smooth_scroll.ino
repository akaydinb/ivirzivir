
#include "LedControl.h"
//#define _DEBUG
#define dataPin 12
#define clkPin  11
#define chipSel 10

const byte lenKayan = 19;  // This has to be equal to the length of "dizi" array!

// initialize four LED Matrix Displays
LedControl lc = LedControl(dataPin, clkPin, chipSel, 4);

unsigned long delaytime = 100;

// Sample long string "desoxyribonukleinsäure"
// byte dizi[lenKayan] = { 4, 5, 16, 14, 19, 20, 15, 9, 2, 14, 13, 18, 10, 11, 5, 9, 13, 16, 21, 18, 15, 5, 0 }; // length: 23

// Sample long string "konsumgesellschaft"
byte dizi[lenKayan] = { 10, 14, 13, 16, 18, 12, 7, 5, 16, 5, 11, 11, 16, 3, 8, 1, 6, 17, 0 }; // length: 19

byte pointer = 0;

// This is a sample char table. Doesn't contain all chars for 
// optimization, but feel free to add according to your needs.
unsigned char table[][8] = {
 { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },     // 12:    (space)    
 { 0x00, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00 },    // HEX: 61    ASC: 'a'  1
 { 0xe0, 0x60, 0x60, 0x7c, 0x66, 0x66, 0xdc, 0x00 },    // HEX: 62    ASC: 'b'  2
 { 0x00, 0x00, 0x78, 0xcc, 0xc0, 0xcc, 0x78, 0x00 },    // HEX: 63    ASC: 'c'  3
 { 0x1c, 0x0c, 0x0c, 0x7c, 0xcc, 0xcc, 0x76, 0x00 },    // HEX: 64    ASC: 'd'  4
 { 0x00, 0x00, 0x78, 0xcc, 0xfc, 0xc0, 0x78, 0x00 },    // HEX: 65    ASC: 'e'  5
 { 0x38, 0x6c, 0x60, 0xf0, 0x60, 0x60, 0xf0, 0x00 },    // HEX: 66    ASC: 'f'  6
 { 0x00, 0x00, 0x76, 0xcc, 0xcc, 0x7c, 0x0c, 0xf8 },    // HEX: 67    ASC: 'g'  7
 { 0xe0, 0x60, 0x6c, 0x76, 0x66, 0x66, 0xe6, 0x00 },    // HEX: 68    ASC: 'h'  8
 { 0x30, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00 },    // HEX: 69    ASC: 'i'  9
 { 0xe0, 0x60, 0x66, 0x6c, 0x78, 0x6c, 0xe6, 0x00 },    // HEX: 6B    ASC: 'k'  10
 { 0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00 },    // HEX: 6C    ASC: 'l'  11
 { 0x00, 0x00, 0xcc, 0xfe, 0xfe, 0xd6, 0xc6, 0x00 },    // HEX: 6D    ASC: 'm'  12
 { 0x00, 0x00, 0xf8, 0xcc, 0xcc, 0xcc, 0xcc, 0x00 },    // HEX: 6E    ASC: 'n'  13
 { 0x00, 0x00, 0x78, 0xcc, 0xcc, 0xcc, 0x78, 0x00 },    // HEX: 6F    ASC: 'o'  14
 { 0x00, 0x00, 0xdc, 0x76, 0x66, 0x60, 0xf0, 0x00 },    // HEX: 72    ASC: 'r'  15
 { 0x00, 0x00, 0x7c, 0xc0, 0x78, 0x0c, 0xf8, 0x00 },    // HEX: 73    ASC: 's'  16
 { 0x10, 0x30, 0x7c, 0x30, 0x30, 0x34, 0x18, 0x00 },    // HEX: 74    ASC: 't'  17
 { 0x00, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00 },    // HEX: 75    ASC: 'u'  18
 { 0x00, 0x00, 0xc6, 0x6c, 0x38, 0x6c, 0xc6, 0x00 },    // HEX: 78    ASC: 'x'  19
 { 0x00, 0x00, 0xcc, 0xcc, 0xcc, 0x7c, 0x0c, 0xf8 },    // HEX: 79    ASC: 'y'  20
 { 0xcc, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x7e, 0x00 },    // HEX: 84    ASC: 'ä'  21
 { 0x00, 0xcc, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00 }     // HEX: 81    ASC: 'ü'  22
// { 0x00, 0xcc, 0x00, 0x78, 0xcc, 0xcc, 0x78, 0x00 },    // HEX: 94    ASC: 'ö'
};

byte kayanyazi[lenKayan][8];
byte carry_new, carry_old;
byte g = 0;


void setup() {
  #ifdef _DEBUG
    Serial.begin(9600); // open the serial port at 9600 bps:
  #endif
  for(int i = 0; i < 4; i++)  {
    lc.shutdown(i, false);
    lc.setIntensity(i, 0);
    lc.clearDisplay(i);
  }
  
  for(int i = 0; i < lenKayan; i++) {
    // table is a lookup table for chars. kayanyazi 
    // is generated based on sequence numbers in dizi
    kayanyazi[i][0] = table[dizi[i]][0];
    kayanyazi[i][1] = table[dizi[i]][1];
    kayanyazi[i][2] = table[dizi[i]][2];
    kayanyazi[i][3] = table[dizi[i]][3];
    kayanyazi[i][4] = table[dizi[i]][4];
    kayanyazi[i][5] = table[dizi[i]][5];
    kayanyazi[i][6] = table[dizi[i]][6];
    kayanyazi[i][7] = table[dizi[i]][7];
  }
}

void loop() {
  for(int j = 0; j < 8; j++)  {
    lc.setRow(3, j, kayanyazi[0][j]);
    lc.setRow(2, j, kayanyazi[1][j]);
    lc.setRow(1, j, kayanyazi[2][j]);
    lc.setRow(0, j, kayanyazi[3][j]);
  }

  carry_old = 0;
  // left shift. Determine here if there 
  // will be carry after shifting left from MSB
  for(int i = 0; i < 8; i++)  {
    carry_old |= (kayanyazi[0][i] & 0x80) >> i;
  }

  #ifdef _DEBUG
    // just in case, you know
    Serial.print("carry_old = "); 
    Serial.println(carry_old);
    delay(10000);
  #endif

  for(int i = lenKayan - 1; i >= 0; i--)   {
    carry_new = 0;
    for(int j = 0; j < 8; j++)  {
      carry_new |= (kayanyazi[i][j] & 0x80) >> j;
      // Shift left and add carry_old from previous display rows 
      kayanyazi[i][j] = (kayanyazi[i][j] << 1) + ((carry_old >> (7 - j)) & 1) ;
    }

    #ifdef _DEBUG
      Serial.print("carry old: ");
      Serial.println(carry_old);
      Serial.println(kayanyazi[i][0]);
    #endif

    carry_old = carry_new;
  }
  // Here is just some acceleration for fun.
  if( !(g++ % 40) && delaytime)
    delaytime--;

  #ifdef _DEBUG
    Serial.print("delaytime: ");
    Serial.println(delaytime); 
  #endif
  delay(delaytime);

}
