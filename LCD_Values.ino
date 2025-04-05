
// The LCD segment display does not map cleanly to a nice simple sets of bits,
//   therefor those functions are used to abstract that mess.

// First, LCDBuffer stores the current state and is sent in that order 0b1234 0b5678 0b9...
// To interact with that buffer, the lowlevel bit maping is encoded in the numerous switch statement
// where a clean and well ordered value, indicator or mask, is translated into the strange, discontinuous bits
// Const.h provides many helpfull mask and named values to abstract some complexity

// RefreshLCD push the buffer to the LCD display
// ToggleRing and ClearRing control the left bearing/azimuth ring
// ToggleInd and ClearAllInd control all the numerous indicators on the right half that are not segments (include dots)
// ToggleBox, ToggleCross and ClearBoxCross control 16 segments square at the beginning of the top row

// The 14 lowlevel procedures ToggleNumber[Hi-Low][0-6] take a mask and set the coresponding 7 segments digit, never used directly
// ToggleNumberMask take a Row/Possition/Mask and call the appropriate lowlevel procedure, never used directly
// ToggleNumberVal take a Row/Position and signle char or int (0 to 9 only) value that is translated into the correct mask for calling the underlying procedures


//  Bits are feed starting from most significant to least, from index 0 to 11
unsigned int LCDBuffer[12] = { 0b0000000000000000, 0b0000000000000000,
                               0b0000000000000000, 0b0000000000000000,
                               0b0000000000000000, 0b0000000000000000,
                               0b0000000000000000, 0b0000000000000000,
                               0b0000000000000000, 0b0000000000000000,
                               0b0000000000000000, 0b0000000000000000 };

void RefreshLCD(void *) {
  unsigned int BitMask;

  for (int i=0; i<12; i++) {
    BitMask = Bit1; //0b1000000000000000;
    //Serial.print(LCDBuffer[i]);
    //Serial.print(" ");

    for (int j=0; j<16; j++) {
      digitalWrite(ClockPin, HIGH);
      digitalWrite(DataPin, (LCDBuffer[i] & BitMask) > 0 );
      digitalWrite(ClockPin, LOW);
      BitMask = BitMask >> 1 ;
    }
  }
  digitalWrite(DataPin, LOW);
  //Serial.println(" ");

  digitalWrite(LoadPin, HIGH);
  digitalWrite(LoadPin, LOW);
}

void ToggleRing(bool Center, bool Mils) {
    LCDBuffer[0]  = Center ? LCDBuffer[0]  | Bit9  :  LCDBuffer[0]  & NBit9;
    LCDBuffer[10] = Mils   ? LCDBuffer[10] | Bit8  :  LCDBuffer[10] & NBit8;
}

void ToggleRing(byte Angle, bool Val) {
  switch(Angle) {
    case  0:  LCDBuffer[10] = Val ? LCDBuffer[10] | Bit10  :  LCDBuffer[10] & NBit10;   break;  //NORTH
    case  1:  LCDBuffer[10] = Val ? LCDBuffer[10] | Bit1   :  LCDBuffer[10] & NBit1;    break;
    case  2:  LCDBuffer[10] = Val ? LCDBuffer[10] | Bit11  :  LCDBuffer[10] & NBit11;   break;
    case  3:  LCDBuffer[10] = Val ? LCDBuffer[10] | Bit12  :  LCDBuffer[10] & NBit12;   break;
    case  4:  LCDBuffer[10] = Val ? LCDBuffer[10] | Bit13  :  LCDBuffer[10] & NBit13;   break;
    case  5:  LCDBuffer[10] = Val ? LCDBuffer[10] | Bit14  :  LCDBuffer[10] & NBit14;   break;
    case  6:  LCDBuffer[10] = Val ? LCDBuffer[10] | Bit15  :  LCDBuffer[10] & NBit15;   break;
    case  7:  LCDBuffer[10] = Val ? LCDBuffer[10] | Bit16  :  LCDBuffer[10] & NBit16;   break;
    case  8:  LCDBuffer[11] = Val ? LCDBuffer[11] | Bit1   :  LCDBuffer[11] & NBit1;    break;  // EAST
    case  9:  LCDBuffer[0]  = Val ? LCDBuffer[0]  | Bit1   :  LCDBuffer[0]  & NBit1;    break;
    case 10:  LCDBuffer[0]  = Val ? LCDBuffer[0]  | Bit2   :  LCDBuffer[0]  & NBit2;    break;
    case 11:  LCDBuffer[0]  = Val ? LCDBuffer[0]  | Bit3   :  LCDBuffer[0]  & NBit3;    break;
    case 12:  LCDBuffer[0]  = Val ? LCDBuffer[0]  | Bit4   :  LCDBuffer[0]  & NBit4;    break;
    case 13:  LCDBuffer[0]  = Val ? LCDBuffer[0]  | Bit5   :  LCDBuffer[0]  & NBit5;    break;
    case 14:  LCDBuffer[0]  = Val ? LCDBuffer[0]  | Bit6   :  LCDBuffer[0]  & NBit6;    break;
    case 15:  LCDBuffer[1]  = Val ? LCDBuffer[1]  | Bit1   :  LCDBuffer[1]  & NBit1;    break;
    case 16:  LCDBuffer[0]  = Val ? LCDBuffer[0]  | Bit7   :  LCDBuffer[0]  & NBit7;    break;  // SOUTH
    case 17:  LCDBuffer[0]  = Val ? LCDBuffer[0]  | Bit8   :  LCDBuffer[0]  & NBit8;    break;
    case 18:  LCDBuffer[0]  = Val ? LCDBuffer[0]  | Bit10  :  LCDBuffer[0]  & NBit10;   break;
    case 19:  LCDBuffer[0]  = Val ? LCDBuffer[0]  | Bit11  :  LCDBuffer[0]  & NBit11;   break;
    case 20:  LCDBuffer[0]  = Val ? LCDBuffer[0]  | Bit16  :  LCDBuffer[0]  & NBit16;   break;
    case 21:  LCDBuffer[0]  = Val ? LCDBuffer[0]  | Bit12  :  LCDBuffer[0]  & NBit12;   break;
    case 22:  LCDBuffer[0]  = Val ? LCDBuffer[0]  | Bit15  :  LCDBuffer[0]  & NBit15;   break;
    case 23:  LCDBuffer[0]  = Val ? LCDBuffer[0]  | Bit13  :  LCDBuffer[0]  & NBit13;   break;
    case 24:  LCDBuffer[0]  = Val ? LCDBuffer[0]  | Bit14  :  LCDBuffer[0]  & NBit14;   break;  // WEST
    case 25:  LCDBuffer[10] = Val ? LCDBuffer[10] | Bit3   :  LCDBuffer[10] & NBit3;    break;
    case 26:  LCDBuffer[10] = Val ? LCDBuffer[10] | Bit4   :  LCDBuffer[10] & NBit4;    break;
    case 27:  LCDBuffer[10] = Val ? LCDBuffer[10] | Bit2   :  LCDBuffer[10] & NBit2;    break;
    case 28:  LCDBuffer[10] = Val ? LCDBuffer[10] | Bit5   :  LCDBuffer[10] & NBit5;    break;
    case 29:  LCDBuffer[10] = Val ? LCDBuffer[10] | Bit6   :  LCDBuffer[10] & NBit6;    break;
    case 30:  LCDBuffer[10] = Val ? LCDBuffer[10] | Bit7   :  LCDBuffer[10] & NBit7;    break;
    case 31:  LCDBuffer[10] = Val ? LCDBuffer[10] | Bit9   :  LCDBuffer[10] & NBit9;    break;
  }
}

void ClearRing(void *) {
  LCDBuffer[0]  = LCDBuffer[0]  & Bit9;
  LCDBuffer[1]  = LCDBuffer[1]  & NBit1;
  LCDBuffer[10] = LCDBuffer[10] & Bit8;
  LCDBuffer[11] = LCDBuffer[11] & NBit1;
}

void ToggleInd(byte Id, bool Val) {
  switch(Id) {
    case  0:  LCDBuffer[11] = Val ? LCDBuffer[11] | Bit8   :  LCDBuffer[11] & NBit8;    break;
    case  1:  LCDBuffer[11] = Val ? LCDBuffer[11] | Bit9   :  LCDBuffer[11] & NBit9;    break;
    case  2:  LCDBuffer[11] = Val ? LCDBuffer[11] | Bit2   :  LCDBuffer[11] & NBit2;    break;
    case  3:  LCDBuffer[11] = Val ? LCDBuffer[11] | Bit10  :  LCDBuffer[11] & NBit10;   break;
    case  4:  LCDBuffer[3]  = Val ? LCDBuffer[3]  | Bit2   :  LCDBuffer[3]  & NBit2;    break;
    case  5:  LCDBuffer[4]  = Val ? LCDBuffer[4]  | Bit2   :  LCDBuffer[4]  & NBit2;    break;
    case  6:  LCDBuffer[3]  = Val ? LCDBuffer[3]  | Bit1   :  LCDBuffer[3]  & NBit1;    break;
    case  7:  LCDBuffer[2]  = Val ? LCDBuffer[2]  | Bit13  :  LCDBuffer[2]  & NBit13;   break;
    case  8:  LCDBuffer[11] = Val ? LCDBuffer[11] | Bit11  :  LCDBuffer[11] & NBit11;   break;
    case  9:  LCDBuffer[2]  = Val ? LCDBuffer[2]  | Bit14  :  LCDBuffer[2]  & NBit14;   break;
    case 10:  LCDBuffer[7]  = Val ? LCDBuffer[7]  | Bit9   :  LCDBuffer[7]  & NBit9;    break;
    case 11:  LCDBuffer[2]  = Val ? LCDBuffer[2]  | Bit12  :  LCDBuffer[2]  & NBit12;   break;
    case 12:  LCDBuffer[5]  = Val ? LCDBuffer[5]  | Bit1   :  LCDBuffer[5]  & NBit1;    break;
    case 13:  LCDBuffer[4]  = Val ? LCDBuffer[4]  | Bit12  :  LCDBuffer[4]  & NBit12;   break;
    case 14:  LCDBuffer[11] = Val ? LCDBuffer[11] | Bit12  :  LCDBuffer[11] & NBit12;   break;
    case 15:  LCDBuffer[4]  = Val ? LCDBuffer[4]  | Bit3   :  LCDBuffer[4]  & NBit3;    break;
    case 16:  LCDBuffer[11] = Val ? LCDBuffer[11] | Bit13  :  LCDBuffer[11] & NBit13;   break;
    case 17:  LCDBuffer[4]  = Val ? LCDBuffer[4]  | Bit1   :  LCDBuffer[4]  & NBit1;    break;
    case 18:  LCDBuffer[4]  = Val ? LCDBuffer[4]  | Bit4   :  LCDBuffer[4]  & NBit4;    break;
    case 19:  LCDBuffer[9]  = Val ? LCDBuffer[9]  | Bit9   :  LCDBuffer[9]  & NBit9;    break;
    case 20:  LCDBuffer[8]  = Val ? LCDBuffer[8]  | Bit12  :  LCDBuffer[8]  & NBit12;   break;
    case 21:  LCDBuffer[2]  = Val ? LCDBuffer[2]  | Bit15  :  LCDBuffer[2]  & NBit15;   break;
    case 22:  LCDBuffer[2]  = Val ? LCDBuffer[2]  | Bit11  :  LCDBuffer[2]  & NBit11;   break;
    case 23:  LCDBuffer[6]  = Val ? LCDBuffer[6]  | Bit3   :  LCDBuffer[6]  & NBit3;    break;
    case 24:  LCDBuffer[5]  = Val ? LCDBuffer[5]  | Bit5   :  LCDBuffer[5]  & NBit5;    break;
    case 25:  LCDBuffer[4]  = Val ? LCDBuffer[4]  | Bit16  :  LCDBuffer[4]  & NBit16;   break;
  }
}

void ClearAllInd(void *) {
  LCDBuffer[2]  = LCDBuffer[2]  & 0b1111111111100001;
  LCDBuffer[3]  = LCDBuffer[3]  & 0b0011111111111111; 
  LCDBuffer[4]  = LCDBuffer[4]  & 0b0000111111101110;
  LCDBuffer[5]  = LCDBuffer[5]  & 0b0111011111111111; 
  LCDBuffer[6]  = LCDBuffer[6]  & NBit3; 
  LCDBuffer[7]  = LCDBuffer[7]  & NBit9; 
  LCDBuffer[8]  = LCDBuffer[8]  & NBit12;
  LCDBuffer[9]  = LCDBuffer[9]  & NBit9; 
  LCDBuffer[11] = LCDBuffer[11] & 0b1011111000000111;  
}

/*
    --H-- --A--
   | \   |   / |
   G  h  a  b  B
   |    \|/    |
    --g-- --c--
   |    /|\    |
   F  f  e  d  C
   | /   |   \ |
    --E-- --D--

  0bABCDEFGH

  "Box" is the outer square (in uppercaps)
  "Cross" is the inner star (in lowercaps)
*/

void ToggleBox(byte MaskBox) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<8; i++) {
    TempBuff = MaskBox & BitMask;
    switch(i) {
      case  0:  LCDBuffer[8]  = TempBuff != 0x00  ? LCDBuffer[8]  | Bit5   :  LCDBuffer[8]  & NBit5;   break;
      case  1:  LCDBuffer[8]  = TempBuff != 0x00  ? LCDBuffer[8]  | Bit6   :  LCDBuffer[8]  & NBit6;   break;
      case  2:  LCDBuffer[8]  = TempBuff != 0x00  ? LCDBuffer[8]  | Bit7   :  LCDBuffer[8]  & NBit7;   break;
      case  3:  LCDBuffer[11] = TempBuff != 0x00  ? LCDBuffer[11] | Bit7   :  LCDBuffer[11] & NBit7;   break;
      case  4:  LCDBuffer[8]  = TempBuff != 0x00  ? LCDBuffer[8]  | Bit1   :  LCDBuffer[8]  & NBit1;   break;
      case  5:  LCDBuffer[8]  = TempBuff != 0x00  ? LCDBuffer[8]  | Bit2   :  LCDBuffer[8]  & NBit2;   break;
      case  6:  LCDBuffer[8]  = TempBuff != 0x00  ? LCDBuffer[8]  | Bit3   :  LCDBuffer[8]  & NBit3;   break;
      case  7:  LCDBuffer[8]  = TempBuff != 0x00  ? LCDBuffer[8]  | Bit4   :  LCDBuffer[8]  & NBit4;   break;
    }
    BitMask = BitMask >> 1 ;
  }
}

void ToggleCross(byte MaskCross) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<8; i++) {
    TempBuff = MaskCross & BitMask;
    switch(i) {
      case  0:  LCDBuffer[9]  = TempBuff != 0x00  ? LCDBuffer[9]  | Bit16  :  LCDBuffer[9]   & NBit16;  break;
      case  1:  LCDBuffer[9]  = TempBuff != 0x00  ? LCDBuffer[9]  | Bit15  :  LCDBuffer[9]   & NBit15;  break;
      case  2:  LCDBuffer[9]  = TempBuff != 0x00  ? LCDBuffer[9]  | Bit14  :  LCDBuffer[9]   & NBit14;  break;
      case  3:  LCDBuffer[9]  = TempBuff != 0x00  ? LCDBuffer[9]  | Bit13  :  LCDBuffer[9]   & NBit13;  break;
      case  4:  LCDBuffer[11] = TempBuff != 0x00  ? LCDBuffer[11] | Bit6   :  LCDBuffer[11]  & NBit6;   break;
      case  5:  LCDBuffer[11] = TempBuff != 0x00  ? LCDBuffer[11] | Bit5   :  LCDBuffer[11]  & NBit5;   break;
      case  6:  LCDBuffer[11] = TempBuff != 0x00  ? LCDBuffer[11] | Bit4   :  LCDBuffer[11]  & NBit4;   break;
      case  7:  LCDBuffer[11] = TempBuff != 0x00  ? LCDBuffer[11] | Bit3   :  LCDBuffer[11]  & NBit3;   break;
    }
    BitMask = BitMask >> 1 ;
  }
}

void ClearBoxCross(void *) {
  LCDBuffer[8]  = LCDBuffer[8]  & 0b0000000111111111; 
  LCDBuffer[9]  = LCDBuffer[9]  & 0b1111111111110000;
  LCDBuffer[11] = LCDBuffer[11] & 0b1100000111111111;  
}

/*
  |--A--|
  F     B
  |     |
  ---G---
  |     |
  E     C
  |--D--|

  0bABCDEFG0
*/

//HiRow = 0 and LowRow = 1, Pos from Left to Right 0 to 6
void ToggleNumberVal(byte Row, byte Pos, char Val) {
  switch(Val) { 
    case  '0':  ToggleNumberMask(Row, Pos, Num0);  break;
    case  '1':  ToggleNumberMask(Row, Pos, Num1);  break;
    case  '2':  ToggleNumberMask(Row, Pos, Num2);  break;
    case  '3':  ToggleNumberMask(Row, Pos, Num3);  break;
    case  '4':  ToggleNumberMask(Row, Pos, Num4);  break;
    case  '5':  ToggleNumberMask(Row, Pos, Num5);  break;
    case  '6':  ToggleNumberMask(Row, Pos, Num6);  break;
    case  '7':  ToggleNumberMask(Row, Pos, Num7);  break;
    case  '8':  ToggleNumberMask(Row, Pos, Num8);  break;
    case  '9':  ToggleNumberMask(Row, Pos, Num9);  break;
    case  'A':  ToggleNumberMask(Row, Pos, 0b11101110);  break;
    case  'b':  ToggleNumberMask(Row, Pos, 0b00111110);  break;
    case  'c':  ToggleNumberMask(Row, Pos, 0b00011010);  break;
    case  'C':  ToggleNumberMask(Row, Pos, 0b10011100);  break;
    case  'd':  ToggleNumberMask(Row, Pos, 0b01111010);  break;
    case  'E':  ToggleNumberMask(Row, Pos, 0b10011110);  break;
    case  'F':  ToggleNumberMask(Row, Pos, 0b10001110);  break;
    case  'g':  ToggleNumberMask(Row, Pos, 0b10111100);  break;
    case  'h':  ToggleNumberMask(Row, Pos, 0b00101110);  break;
    case  'H':  ToggleNumberMask(Row, Pos, 0b01101110);  break;
    case  'i':  ToggleNumberMask(Row, Pos, 0b00100000);  break;
    case  'J':  ToggleNumberMask(Row, Pos, 0b01111000);  break;
    case  'L':  ToggleNumberMask(Row, Pos, 0b00011100);  break;
    case  'n':  ToggleNumberMask(Row, Pos, 0b00101010);  break;
    case  'o':  ToggleNumberMask(Row, Pos, 0b00111010);  break;
    case  'P':  ToggleNumberMask(Row, Pos, 0b11001110);  break;
    case  'q':  ToggleNumberMask(Row, Pos, 0b11100110);  break;
    case  'r':  ToggleNumberMask(Row, Pos, 0b00001010);  break;
    case  't':  ToggleNumberMask(Row, Pos, 0b00011110);  break;
    case  'u':  ToggleNumberMask(Row, Pos, 0b00111000);  break;
    case  'y':  ToggleNumberMask(Row, Pos, 0b01110110);  break;
    case  '-':  ToggleNumberMask(Row, Pos, 0b00000010);  break;
    case  '_':  ToggleNumberMask(Row, Pos, 0b00010000);  break;
    case  '?':  ToggleNumberMask(Row, Pos, 0b11001010);  break;
    default  :  ToggleNumberMask(Row, Pos, NumNull);  break;
  }
}

//HiRow = 0 and LowRow = 1, Pos from Left to Right 0 to 6
/*void ToggleNumberVal(byte Row, byte Pos, byte Val) {
  switch(Val) { 
    case  0:  ToggleNumberMask(Row, Pos, Num0);  break;
    case  1:  ToggleNumberMask(Row, Pos, Num1);  break;
    case  2:  ToggleNumberMask(Row, Pos, Num2);  break;
    case  3:  ToggleNumberMask(Row, Pos, Num3);  break;
    case  4:  ToggleNumberMask(Row, Pos, Num4);  break;
    case  5:  ToggleNumberMask(Row, Pos, Num5);  break;
    case  6:  ToggleNumberMask(Row, Pos, Num6);  break;
    case  7:  ToggleNumberMask(Row, Pos, Num7);  break;
    case  8:  ToggleNumberMask(Row, Pos, Num8);  break;
    case  9:  ToggleNumberMask(Row, Pos, Num9);  break;
    default:  ToggleNumberMask(Row, Pos, NumNull);  break;
  }
}*/

//HiRow = 0 and LowRow = 1, Pos from Left to Right 0 to 6
void ToggleNumberMask(int Row, int Pos, byte SegMask) {
  Pos = Pos + (Row*7);
  switch(Pos) { 
    case  0:  ToggleNumberHi0(SegMask);  break;
    case  1:  ToggleNumberHi1(SegMask);  break;
    case  2:  ToggleNumberHi2(SegMask);  break;
    case  3:  ToggleNumberHi3(SegMask);  break;
    case  4:  ToggleNumberHi4(SegMask);  break;
    case  5:  ToggleNumberHi5(SegMask);  break;
    case  6:  ToggleNumberHi6(SegMask);  break;

    case  7:  ToggleNumberLow0(SegMask);  break;
    case  8:  ToggleNumberLow1(SegMask);  break;
    case  9:  ToggleNumberLow2(SegMask);  break;
    case  10: ToggleNumberLow3(SegMask);  break;
    case  11: ToggleNumberLow4(SegMask);  break;
    case  12: ToggleNumberLow5(SegMask);  break;
    case  13: ToggleNumberLow6(SegMask);  break;
  }
}

void ToggleNumberHi0(byte SegMask) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<7; i++) {
    TempBuff = SegMask & BitMask;
    switch(i) {
      case  0:  LCDBuffer[9]  = TempBuff != 0x00  ? LCDBuffer[9]  | Bit11  :  LCDBuffer[9]   & NBit11;  break;
      case  1:  LCDBuffer[8]  = TempBuff != 0x00  ? LCDBuffer[8]  | Bit10  :  LCDBuffer[8]   & NBit10;  break;
      case  2:  LCDBuffer[8]  = TempBuff != 0x00  ? LCDBuffer[8]  | Bit11  :  LCDBuffer[8]   & NBit11;  break;
      case  3:  LCDBuffer[8]  = TempBuff != 0x00  ? LCDBuffer[8]  | Bit8   :  LCDBuffer[8]   & NBit8;   break;
      case  4:  LCDBuffer[9]  = TempBuff != 0x00  ? LCDBuffer[9]  | Bit12  :  LCDBuffer[9]   & NBit12;  break;
      case  5:  LCDBuffer[8]  = TempBuff != 0x00  ? LCDBuffer[8]  | Bit9   :  LCDBuffer[8]   & NBit9;   break;
      case  6:  LCDBuffer[9]  = TempBuff != 0x00  ? LCDBuffer[9]  | Bit10  :  LCDBuffer[9]   & NBit10;  break;
    }
    BitMask = BitMask >> 1 ;
  }
}

void ToggleNumberHi1(byte SegMask) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<7; i++) {
    TempBuff = SegMask & BitMask;
    switch(i) {
      case  0:  LCDBuffer[8]  = TempBuff != 0x00  ? LCDBuffer[8]  | Bit14  :  LCDBuffer[8]   & NBit14;  break;
      case  1:  LCDBuffer[9]  = TempBuff != 0x00  ? LCDBuffer[9]  | Bit6   :  LCDBuffer[9]   & NBit6;   break;
      case  2:  LCDBuffer[9]  = TempBuff != 0x00  ? LCDBuffer[9]  | Bit5   :  LCDBuffer[9]   & NBit5;   break;
      case  3:  LCDBuffer[9]  = TempBuff != 0x00  ? LCDBuffer[9]  | Bit8   :  LCDBuffer[9]   & NBit8;   break;
      case  4:  LCDBuffer[8]  = TempBuff != 0x00  ? LCDBuffer[8]  | Bit13  :  LCDBuffer[8]   & NBit13;  break;
      case  5:  LCDBuffer[9]  = TempBuff != 0x00  ? LCDBuffer[9]  | Bit7   :  LCDBuffer[9]   & NBit7;   break;
      case  6:  LCDBuffer[8]  = TempBuff != 0x00  ? LCDBuffer[8]  | Bit15  :  LCDBuffer[8]   & NBit15;  break;
    }
    BitMask = BitMask >> 1 ;
  }
}

void ToggleNumberHi2(byte SegMask) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<7; i++) {
    TempBuff = SegMask & BitMask;
    switch(i) {
      case  0:  LCDBuffer[9]  = TempBuff != 0x00  ? LCDBuffer[9]  | Bit3   :  LCDBuffer[9]   & NBit3;   break;
      case  1:  LCDBuffer[9]  = TempBuff != 0x00  ? LCDBuffer[9]  | Bit2   :  LCDBuffer[9]   & NBit2;   break;
      case  2:  LCDBuffer[7]  = TempBuff != 0x00  ? LCDBuffer[7]  | Bit15  :  LCDBuffer[7]   & NBit15;  break;
      case  3:  LCDBuffer[8]  = TempBuff != 0x00  ? LCDBuffer[8]  | Bit16  :  LCDBuffer[8]   & NBit16;  break;
      case  4:  LCDBuffer[9]  = TempBuff != 0x00  ? LCDBuffer[9]  | Bit4   :  LCDBuffer[9]   & NBit4;   break;
      case  5:  LCDBuffer[9]  = TempBuff != 0x00  ? LCDBuffer[9]  | Bit1   :  LCDBuffer[9]   & NBit1;   break;
      case  6:  LCDBuffer[7]  = TempBuff != 0x00  ? LCDBuffer[7]  | Bit16  :  LCDBuffer[7]   & NBit16;  break;
    }
    BitMask = BitMask >> 1 ;
  }
}

void ToggleNumberHi3(byte SegMask) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<7; i++) {
    TempBuff = SegMask & BitMask;
    switch(i) {
      case  0:  LCDBuffer[7]  = TempBuff != 0x00  ? LCDBuffer[7]  | Bit11  :  LCDBuffer[7]   & NBit11;  break;
      case  1:  LCDBuffer[6]  = TempBuff != 0x00  ? LCDBuffer[6]  | Bit1   :  LCDBuffer[6]   & NBit1;   break;
      case  2:  LCDBuffer[6]  = TempBuff != 0x00  ? LCDBuffer[6]  | Bit2   :  LCDBuffer[6]   & NBit2;   break;
      case  3:  LCDBuffer[7]  = TempBuff != 0x00  ? LCDBuffer[7]  | Bit14  :  LCDBuffer[7]   & NBit14;  break;
      case  4:  LCDBuffer[7]  = TempBuff != 0x00  ? LCDBuffer[7]  | Bit12  :  LCDBuffer[7]   & NBit12;  break;
      case  5:  LCDBuffer[7]  = TempBuff != 0x00  ? LCDBuffer[7]  | Bit13  :  LCDBuffer[7]   & NBit13;  break;
      case  6:  LCDBuffer[7]  = TempBuff != 0x00  ? LCDBuffer[7]  | Bit10  :  LCDBuffer[7]   & NBit10;  break;
    }
    BitMask = BitMask >> 1 ;
  }
}

void ToggleNumberHi4(byte SegMask) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<7; i++) {
    TempBuff = SegMask & BitMask;
    switch(i) {
      case  0:  LCDBuffer[6]  = TempBuff != 0x00  ? LCDBuffer[6]  | Bit5   :  LCDBuffer[6]   & NBit5;   break;
      case  1:  LCDBuffer[7]  = TempBuff != 0x00  ? LCDBuffer[7]  | Bit6   :  LCDBuffer[7]   & NBit6;   break;
      case  2:  LCDBuffer[7]  = TempBuff != 0x00  ? LCDBuffer[7]  | Bit5   :  LCDBuffer[7]   & NBit5;   break;
      case  3:  LCDBuffer[7]  = TempBuff != 0x00  ? LCDBuffer[7]  | Bit8   :  LCDBuffer[7]   & NBit8;   break;
      case  4:  LCDBuffer[6]  = TempBuff != 0x00  ? LCDBuffer[6]  | Bit4   :  LCDBuffer[6]   & NBit4;   break;
      case  5:  LCDBuffer[7]  = TempBuff != 0x00  ? LCDBuffer[7]  | Bit7   :  LCDBuffer[7]   & NBit7;   break;
      case  6:  LCDBuffer[6]  = TempBuff != 0x00  ? LCDBuffer[6]  | Bit6   :  LCDBuffer[6]   & NBit6;   break;
    }
    BitMask = BitMask >> 1 ;
  }
}

void ToggleNumberHi5(byte SegMask) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<7; i++) {
    TempBuff = SegMask & BitMask;
    switch(i) {
      case  0:  LCDBuffer[7]  = TempBuff != 0x00  ? LCDBuffer[7]  | Bit3   :  LCDBuffer[7]   & NBit3;   break;
      case  1:  LCDBuffer[6]  = TempBuff != 0x00  ? LCDBuffer[6]  | Bit9   :  LCDBuffer[6]   & NBit9;   break;
      case  2:  LCDBuffer[6]  = TempBuff != 0x00  ? LCDBuffer[6]  | Bit10   :  LCDBuffer[6]   & NBit10;   break;
      case  3:  LCDBuffer[6]  = TempBuff != 0x00  ? LCDBuffer[6]  | Bit7   :  LCDBuffer[6]   & NBit7;   break;
      case  4:  LCDBuffer[7]  = TempBuff != 0x00  ? LCDBuffer[7]  | Bit4   :  LCDBuffer[7]   & NBit4;   break;
      case  5:  LCDBuffer[6]  = TempBuff != 0x00  ? LCDBuffer[6]  | Bit8   :  LCDBuffer[6]   & NBit8;   break;
      case  6:  LCDBuffer[7]  = TempBuff != 0x00  ? LCDBuffer[7]  | Bit2   :  LCDBuffer[7]   & NBit2;   break;
    }
    BitMask = BitMask >> 1 ;
  }
}

void ToggleNumberHi6(byte SegMask) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<7; i++) {
    TempBuff = SegMask & BitMask;
    switch(i) {
      case  0:  LCDBuffer[6]  = TempBuff != 0x00  ? LCDBuffer[6]  | Bit12   :  LCDBuffer[6]   & NBit12;   break;
      case  1:  LCDBuffer[6]  = TempBuff != 0x00  ? LCDBuffer[6]  | Bit15   :  LCDBuffer[6]   & NBit15;   break;
      case  2:  LCDBuffer[6]  = TempBuff != 0x00  ? LCDBuffer[6]  | Bit14   :  LCDBuffer[6]   & NBit14;   break;
      case  3:  LCDBuffer[7]  = TempBuff != 0x00  ? LCDBuffer[7]  | Bit1    :  LCDBuffer[7]   & NBit1;    break;
      case  4:  LCDBuffer[6]  = TempBuff != 0x00  ? LCDBuffer[6]  | Bit11   :  LCDBuffer[6]   & NBit11;   break;
      case  5:  LCDBuffer[6]  = TempBuff != 0x00  ? LCDBuffer[6]  | Bit16   :  LCDBuffer[6]   & NBit16;   break;
      case  6:  LCDBuffer[6]  = TempBuff != 0x00  ? LCDBuffer[6]  | Bit13   :  LCDBuffer[6]   & NBit13;   break;
    }
    BitMask = BitMask >> 1 ;
  }
}

void ToggleNumberLow0(byte SegMask) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<7; i++) {
    TempBuff = SegMask & BitMask;
    switch(i) {
      case  0:  LCDBuffer[2]  = TempBuff != 0x00  ? LCDBuffer[2]  | Bit16  :  LCDBuffer[2]   & NBit16;  break;
      case  1:  LCDBuffer[3]  = TempBuff != 0x00  ? LCDBuffer[3]  | Bit5   :  LCDBuffer[3]   & NBit5;   break;
      case  2:  LCDBuffer[3]  = TempBuff != 0x00  ? LCDBuffer[3]  | Bit4   :  LCDBuffer[3]   & NBit4;   break;
      case  3:  LCDBuffer[2]  = TempBuff != 0x00  ? LCDBuffer[2]  | Bit9   :  LCDBuffer[2]   & NBit9;   break;
      case  4:  LCDBuffer[3]  = TempBuff != 0x00  ? LCDBuffer[3]  | Bit3   :  LCDBuffer[3]   & NBit3;   break;
      case  5:  LCDBuffer[2]  = TempBuff != 0x00  ? LCDBuffer[2]  | Bit10  :  LCDBuffer[2]   & NBit10;  break;
      case  6:  LCDBuffer[2]  = TempBuff != 0x00  ? LCDBuffer[2]  | Bit8   :  LCDBuffer[2]   & NBit8;   break;
    }
    BitMask = BitMask >> 1 ;
  }
}

void ToggleNumberLow1(byte SegMask) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<7; i++) {
    TempBuff = SegMask & BitMask;
    switch(i) {
      case  0:  LCDBuffer[2]  = TempBuff != 0x00  ? LCDBuffer[2]  | Bit7   :  LCDBuffer[2]   & NBit7;   break;
      case  1:  LCDBuffer[2]  = TempBuff != 0x00  ? LCDBuffer[2]  | Bit4   :  LCDBuffer[2]   & NBit4;   break;
      case  2:  LCDBuffer[2]  = TempBuff != 0x00  ? LCDBuffer[2]  | Bit5   :  LCDBuffer[2]   & NBit5;   break;
      case  3:  LCDBuffer[3]  = TempBuff != 0x00  ? LCDBuffer[3]  | Bit7   :  LCDBuffer[3]   & NBit7;   break;
      case  4:  LCDBuffer[2]  = TempBuff != 0x00  ? LCDBuffer[2]  | Bit6   :  LCDBuffer[2]   & NBit6;   break;
      case  5:  LCDBuffer[3]  = TempBuff != 0x00  ? LCDBuffer[3]  | Bit6   :  LCDBuffer[3]   & NBit6;   break;
      case  6:  LCDBuffer[3]  = TempBuff != 0x00  ? LCDBuffer[3]  | Bit8   :  LCDBuffer[3]   & NBit8;   break;
    }
    BitMask = BitMask >> 1 ;
  }
}

void ToggleNumberLow2(byte SegMask) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<7; i++) {
    TempBuff = SegMask & BitMask;
    switch(i) {
      case  0:  LCDBuffer[3]  = TempBuff != 0x00  ? LCDBuffer[3]  | Bit9   :  LCDBuffer[3]   & NBit9;   break;
      case  1:  LCDBuffer[3]  = TempBuff != 0x00  ? LCDBuffer[3]  | Bit12  :  LCDBuffer[3]   & NBit12;  break;
      case  2:  LCDBuffer[3]  = TempBuff != 0x00  ? LCDBuffer[3]  | Bit11  :  LCDBuffer[3]   & NBit11;  break;
      case  3:  LCDBuffer[2]  = TempBuff != 0x00  ? LCDBuffer[2]  | Bit2   :  LCDBuffer[2]   & NBit2;   break;
      case  4:  LCDBuffer[3]  = TempBuff != 0x00  ? LCDBuffer[3]  | Bit10  :  LCDBuffer[3]   & NBit10;  break;
      case  5:  LCDBuffer[2]  = TempBuff != 0x00  ? LCDBuffer[2]  | Bit3   :  LCDBuffer[2]   & NBit3;   break;
      case  6:  LCDBuffer[2]  = TempBuff != 0x00  ? LCDBuffer[2]  | Bit1   :  LCDBuffer[2]   & NBit1;   break;
    }
    BitMask = BitMask >> 1 ;
  }
}

void ToggleNumberLow3(byte SegMask) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<7; i++) {
    TempBuff = SegMask & BitMask;
    switch(i) {
      case  0:  LCDBuffer[3]  = TempBuff != 0x00  ? LCDBuffer[3]  | Bit13  :  LCDBuffer[3]   & NBit13;  break;
      case  1:  LCDBuffer[5]  = TempBuff != 0x00  ? LCDBuffer[5]  | Bit4   :  LCDBuffer[5]   & NBit4;   break;
      case  2:  LCDBuffer[5]  = TempBuff != 0x00  ? LCDBuffer[5]  | Bit3   :  LCDBuffer[5]   & NBit3;   break;
      case  3:  LCDBuffer[3]  = TempBuff != 0x00  ? LCDBuffer[3]  | Bit16  :  LCDBuffer[3]   & NBit16;  break;
      case  4:  LCDBuffer[3]  = TempBuff != 0x00  ? LCDBuffer[3]  | Bit15  :  LCDBuffer[3]   & NBit15;  break;
      case  5:  LCDBuffer[3]  = TempBuff != 0x00  ? LCDBuffer[3]  | Bit14  :  LCDBuffer[3]   & NBit14;  break;
      case  6:  LCDBuffer[5]  = TempBuff != 0x00  ? LCDBuffer[5]  | Bit2   :  LCDBuffer[5]   & NBit2;   break;
    }
    BitMask = BitMask >> 1 ;
  }
}

void ToggleNumberLow4(byte SegMask) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<7; i++) {
    TempBuff = SegMask & BitMask;
    switch(i) {
      case  0:  LCDBuffer[5]  = TempBuff != 0x00  ? LCDBuffer[5]  | Bit6   :  LCDBuffer[5]   & NBit6;   break;
      case  1:  LCDBuffer[5]  = TempBuff != 0x00  ? LCDBuffer[5]  | Bit9   :  LCDBuffer[5]   & NBit9;   break;
      case  2:  LCDBuffer[5]  = TempBuff != 0x00  ? LCDBuffer[5]  | Bit8   :  LCDBuffer[5]   & NBit8;   break;
      case  3:  LCDBuffer[4]  = TempBuff != 0x00  ? LCDBuffer[4]  | Bit14  :  LCDBuffer[4]   & NBit14;  break;
      case  4:  LCDBuffer[5]  = TempBuff != 0x00  ? LCDBuffer[5]  | Bit7   :  LCDBuffer[5]   & NBit7;   break;
      case  5:  LCDBuffer[4]  = TempBuff != 0x00  ? LCDBuffer[4]  | Bit15  :  LCDBuffer[4]   & NBit15;  break;
      case  6:  LCDBuffer[4]  = TempBuff != 0x00  ? LCDBuffer[4]  | Bit13  :  LCDBuffer[4]   & NBit13;  break;
    }
    BitMask = BitMask >> 1 ;
  }
}

void ToggleNumberLow5(byte SegMask) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<7; i++) {
    TempBuff = SegMask & BitMask;
    switch(i) {
      case  0:  LCDBuffer[5]  = TempBuff != 0x00  ? LCDBuffer[5]  | Bit10  :  LCDBuffer[5]   & NBit10;  break;
      case  1:  LCDBuffer[5]  = TempBuff != 0x00  ? LCDBuffer[5]  | Bit13  :  LCDBuffer[5]   & NBit13;  break;
      case  2:  LCDBuffer[5]  = TempBuff != 0x00  ? LCDBuffer[5]  | Bit12  :  LCDBuffer[5]   & NBit12;  break;
      case  3:  LCDBuffer[4]  = TempBuff != 0x00  ? LCDBuffer[4]  | Bit10  :  LCDBuffer[4]   & NBit10;  break;
      case  4:  LCDBuffer[5]  = TempBuff != 0x00  ? LCDBuffer[5]  | Bit11  :  LCDBuffer[5]   & NBit11;  break;
      case  5:  LCDBuffer[4]  = TempBuff != 0x00  ? LCDBuffer[4]  | Bit11  :  LCDBuffer[4]   & NBit11;  break;
      case  6:  LCDBuffer[4]  = TempBuff != 0x00  ? LCDBuffer[4]  | Bit9   :  LCDBuffer[4]   & NBit9;   break;
    }
    BitMask = BitMask >> 1 ;
  }
}

void ToggleNumberLow6(byte SegMask) {
  byte TempBuff, BitMask = 0b10000000;

  for (int i=0; i<7; i++) {
    TempBuff = SegMask & BitMask;
    switch(i) {
      case  0:  LCDBuffer[4]  = TempBuff != 0x00  ? LCDBuffer[4]  | Bit8   :  LCDBuffer[4]   & NBit8;   break;
      case  1:  LCDBuffer[4]  = TempBuff != 0x00  ? LCDBuffer[4]  | Bit5   :  LCDBuffer[4]   & NBit5;   break;
      case  2:  LCDBuffer[4]  = TempBuff != 0x00  ? LCDBuffer[4]  | Bit6   :  LCDBuffer[4]   & NBit6;   break;
      case  3:  LCDBuffer[5]  = TempBuff != 0x00  ? LCDBuffer[5]  | Bit15  :  LCDBuffer[5]   & NBit15;  break;
      case  4:  LCDBuffer[4]  = TempBuff != 0x00  ? LCDBuffer[4]  | Bit7   :  LCDBuffer[4]   & NBit7;   break;
      case  5:  LCDBuffer[5]  = TempBuff != 0x00  ? LCDBuffer[5]  | Bit14  :  LCDBuffer[5]   & NBit14;  break;
      case  6:  LCDBuffer[5]  = TempBuff != 0x00  ? LCDBuffer[5]  | Bit16  :  LCDBuffer[5]   & NBit16;  break;
    }
    BitMask = BitMask >> 1 ;
  }
}
