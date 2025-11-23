
//#define _DEBUG
#define dataPin 12
#define clkPin  11
#define chipSel 10

void setup()  {
  #ifdef _DEBUG
    Serial.begin(9600);
  #endif

  pinMode(dataPin, OUTPUT);
  pinMode(clkPin , OUTPUT);
  pinMode(chipSel, OUTPUT);

  digitalWrite(chipSel, HIGH);  // disable chip

/* Initializing Led Matrix:
 * 1. Power On ( Register 12 )
 * 2. Set Scan Limit to 7 for 8x8 Matrix Display ( Register: 11 )
 * 3. Set Decode Mode to 0. i.e. No 7-Seg Display ( Register: 9 )
 * Optional: Set LED intensity ( Register: 10 )
 * 
 * In case there are more than one 8x8 matrix displays cascaded, Register 00
 * tells the chip to shift out *next* command to the next chip. For example
 * if you want to do a display test on second display, you first have to 
 * send 00, then 15. First 00 will be striped out and 15 will be received 
 * by the next display. 
 */

  // PowerOn
  digitalWrite(chipSel, LOW);
  shiftOut(dataPin, clkPin, MSBFIRST, 12);  // Register
  shiftOut(dataPin, clkPin, MSBFIRST, 1);   // Data
  digitalWrite(chipSel, HIGH);

  // Set Scan Limit
  digitalWrite(chipSel, LOW);
  shiftOut(dataPin, clkPin, MSBFIRST, 11);  // Register
  shiftOut(dataPin, clkPin, MSBFIRST, 7);   // Data
  digitalWrite(chipSel, HIGH);

  // Set Decode Mode
  digitalWrite(chipSel, LOW);
  shiftOut(dataPin, clkPin, MSBFIRST, 9);   // Register
  shiftOut(dataPin, clkPin, MSBFIRST, 0);   // No decode
  digitalWrite(chipSel, HIGH);

  // Set intensity
  digitalWrite(chipSel, LOW);
  shiftOut(dataPin, clkPin, MSBFIRST, 10);  // Register
  shiftOut(dataPin, clkPin, MSBFIRST, 0);   // Set to Low
  digitalWrite(chipSel, HIGH);

  // Clear Display
  for(int i = 0; i <= 7; i++)   {
    digitalWrite(chipSel, LOW);
    shiftOut(dataPin, clkPin, MSBFIRST, i + 1);      // Register
    //shiftOut(dataPin, clkPin, MSBFIRST, (1 << i));   // Data
    shiftOut(dataPin, clkPin, MSBFIRST, 0);   // Data
    digitalWrite(chipSel, HIGH);
  }
}

void loop() {
  int p = 0;
  short a[8] = { 0, 0, 0, 0,    0, 0, 0, 0 };

  while(true)   {
    ++a[0];

    for(p = 0; p < 8; p++)  {
      if(a[p] == 0x100) {
        a[p] = 0; ++a[p+1];
      }     

      digitalWrite(chipSel, LOW);
      shiftOut(dataPin, clkPin, MSBFIRST, p + 1);      // Register
      shiftOut(dataPin, clkPin, MSBFIRST, a[p]);   // Data
      digitalWrite(chipSel, HIGH);
    }
  }
}