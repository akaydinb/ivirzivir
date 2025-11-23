
#include "LedControl.h"
#define dataPin 12
#define clkPin  11
#define chipSel 10

// initialize single LED display
LedControl lc=LedControl(dataPin, clkPin, chipSel, 1);

void setup() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 0);
  lc.clearDisplay(0);
}

void loop() {
  int p;
  // This array is defined with words to check
  // overflow. So, eight words for eight rows.
  short a[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  while(true) {
    ++a[0];

    for(p = 0; p < 8; p++)  {
      if(a[p] == 0x100) {
        // here overflow check. Add carry
        // to next row in case of overflow
        a[p] = 0; ++a[p+1];
      }

      lc.setRow(0, p, a[p]);
    }
  }
}
