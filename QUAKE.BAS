'==========================================
' EARTHQUAKE by William Yu (07-23-94)
' Creates an Earthquake effect
' Works on any graphics mode
' Here's a short demo, modify as you wish
'==========================================

DECLARE SUB EarthQuake ()
DIM CRAFT(500)
CLS
SCREEN 12
OUT &H3D4, 9: OUT &H3D5, 1
DO
  RANDOMIZE TIMER
  Clock = Clock + 1
  X = INT(RND * 640) + 1
  Y = INT(RND * 280) + 1
  Z = INT(RND * 15) + 1
  PSET (X, Y), Z
LOOP UNTIL Clock = 150
LOCATE 3, 25: COLOR 14: PRINT "JOURNEY TO THE UNKNOWN DIMENSION"
CIRCLE (320, 120), 20, 13, , , 11 / 22
PAINT (320, 120), 13
CIRCLE (315, 122), 2, 6, , , 1 / 3: PAINT (315, 122), 6
CIRCLE (323, 124), 2, 5, , , 1 / 3: PAINT (323, 124), 5
CIRCLE (320, 117), 2, 6, , , 1 / 3: PAINT (320, 117), 6
SLEEP 1
LINE (610, 12)-(630, 4), 10
LINE (610, 12)-(634, 8), 10
LINE (634, 8)-(630, 4), 10
PAINT (630, 6), 10: PSET (617, 10), 10: PSET (616, 10), 10
CIRCLE (628, 7), 2, 9, , , 1 / 3: PAINT (628, 7), 9
LINE (632, 6)-(634, 6), 12: LINE (630, 4)-(632, 4), 12
LINE (634, 8)-(636, 8), 12
GET (639, 2)-(609, 13), CRAFT
X = 609: Y = 2
MOVECRAFT:
X = X - 3
Y = Y + 1.1
PUT (X, Y), CRAFT, PSET
IF X = 333 THEN GOTO CRASH
GOTO MOVECRAFT
CRASH:
PSET (330, 115), 10: PSET (330, 115), 12: PSET (325, 116), 15
PSET (330, 114), 15: PSET (329, 113), 12: PSET (326, 115), 10
PSET (331, 115), 10: PSET (331, 115), 12: PSET (325, 113), 10
PSET (332, 114), 15: PSET (327, 113), 10: PSET (329, 114), 10
PSET (330, 113), 10: PSET (328, 112), 10
LINE (333, 114)-(380, 110), 12
LINE (333, 114)-(380, 105), 12
LINE (333, 114)-(380, 100), 12
LINE (333, 114)-(360, 100), 12
LINE (333, 114)-(345, 102), 12
EarthQuake
LINE (333, 114)-(380, 110), 14
LINE (333, 114)-(380, 105), 14
LINE (333, 114)-(380, 100), 14
LINE (333, 114)-(360, 100), 14
LINE (333, 114)-(345, 102), 14
EarthQuake
PUT (333, 100), CRAFT, AND
LINE (333, 114)-(380, 110), 12
LINE (333, 114)-(380, 105), 12
LINE (333, 114)-(380, 100), 12
LINE (333, 114)-(360, 100), 12
LINE (333, 114)-(345, 102), 12
EarthQuake
LINE (333, 114)-(380, 110), 0
LINE (333, 114)-(380, 105), 0
LINE (333, 114)-(380, 100), 0
LINE (333, 114)-(360, 100), 0
LINE (333, 114)-(345, 102), 0
LINE (341, 105)-(355, 110), 0, BF
EarthQuake
OUT &H3D4, 8: OUT &H3D5, 0
SLEEP 1
LOCATE 3, 25: PRINT SPACE$(32)
LOCATE 3, 29: COLOR 12: PRINT "WHAT THE HELL WAS THAT!!!"
SLEEP 1
LOCATE 3, 29: COLOR 14: PRINT "HELP! HELP! AHHH...I'M..."
SLEEP 2
LOCATE 3, 29: COLOR 12: PRINT "       NO RESPONSE       "
SLEEP 1
LOCATE 3, 32: PRINT "WHAT HAPPENED TO HIM?"
SLEEP 2

SUB EarthQuake

Delay = 5500       ' Increase this or decrease for earthquake delay

FOR X = 1 TO Delay
  OUT &H3D4, 8: OUT &H3D5, X
NEXT X
END SUB