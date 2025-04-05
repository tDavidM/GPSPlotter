
// Provides many helpfull mask and named values to abstract some complexity

// heavily used by LCD_Values.ino and quite a bit by Interface.ino
// Define several EEPROM addr locations
// Define a few usefull long and float for convertion factors used in Navigation.ino
// Regroup many strings used for displaying messages

//Row ID on the LCD
const byte LCDRowHi       = 0;
const byte LCDRowLow      = 1;

//NEMA messages types
const byte MsgTypeGPGGA = 0;
const byte MsgTypeGPRMC = 1;
const byte MsgTypeGPVTG = 2;

const byte MaxWaitTimeForMsg = 10;

//Size of the XY grid in number of motor steps and other key control values
const unsigned int STEP_GridSizeX     = 1920;
const unsigned int STEP_GridSizeY     = 1920;
const unsigned int STEP_MicroSecDelay = 2500;
const unsigned int STEP_InitDelay     = 500;
const byte STEP_SizeCorse   = 128;
const byte STEP_SizeInter   = 12;
const byte STEP_SizeFine    = 1;
const byte STEP_MaxPerBatch = 15;
const byte STEP_Overshoot   = 20;

const long LatLonConvFactorL    = 100000;

const float LatLonConvFactorF   = 100000.0;
const float EarthDiamInM        = 12742000.0;
const float LatToMeter          = 1.119492;                 // 111194.92 m per degree
const float DegreMinToDec       = 1666.666666666666666666;  // 100000 / 60
const float DegreeToThirtyTwoTh = 0.088888888888888888;     // 32 / 360
const float KnotsToKmH          = 1.852;

const float StepToCM            = 0.01252604166;            // 1920 steps per 24,05cm  

// Supported chars  ->  0123456789
//   (only C and H      AbcCdEF
//    have both lower   ghHiJLnoPqrtuy
//    and upper case)   -_? 

// For S and Z use 5 and 2
// K M V W X aren't supported

const char Loading_Msg[8]  = "LoAding";
const char Corse_Msg[6]    = "Cor5E";
const char Inter_Msg[6]    = "intEr";
const char Fine_Msg[5]     = "FinE";
const char Lat_Msg[4]      = "LAt";
const char Lon_Msg[4]      = "Lon";
const char Offset_Msg[7]   = "oFF5Et";
const char Point_Msg[6]    = "Point";
const char Reset_Msg[8]    = "rE5Et ?";
const char Press1_Msg[8]   = "PrE55 1";
const char PressE_Msg[8]   = "PrE55 E";
const char TooLarge_Msg[7] = " LArgE";

const char MaskBlank_Msg[8] = "       ";

const byte EEPROM_ControlVal     = 0;
const byte EEPROM_MagicValue     = 42;
const byte EEPROM_True           = 1;
const byte EEPROM_False          = 0;
const byte EEPROM_HourOffset     = 2;
const byte EEPROM_MinuteOffset   = 3;
const byte EEPROM_RefPointA      = 10;   // 10-13 Lat, 14-17 Lon, 18-19 X, 20-21 Y, 22 LatLonVal, 23 XYVal 
const byte EEPROM_RefPointB      = 30;   // 30-33 Lat, 34-37 Lon, 38-39 X, 40-41 Y, 42 LatLonVal, 43 XYVal 
const byte EEPROM_Destination    = 50;   // 50-53 Lat, 54-57 Lon,                   62 LatLonVal
const byte EEPROM_Waypoints      = 100;  // 10 waypoints with each 10 bytes (two longs of 4 bytes each and two 1 byte indicators)
const byte EEPROM_LatPos         = 0;
const byte EEPROM_LonPos         = 4;
const byte EEPROM_XPos           = 8;
const byte EEPROM_YPos           = 10;
const byte EEPROM_LatLonValPos   = 12;
const byte EEPROM_XYValPos       = 13;
const byte EEPROM_WaypointSetPos = 8;
const byte EEPROM_WaypointSize   = 10;
const byte EEPROM_NbMaxAddrUsed = 200;



//Names for the IDs of the numerous indicators
const byte LCDIndSet      = 0 ;
const byte LCDIndDest     = 1 ;
const byte LCDIndQuest    = 2 ;
const byte LCDIndRange    = 3 ;
const byte LCDIndUpdate   = 4 ;
const byte LCDIndPosition = 5 ;
const byte LCDIndHeading  = 6 ;
const byte LCDIndGrid     = 7 ;
const byte LCDIndScale    = 8 ;
const byte LCDIndRefPoint = 9 ;
const byte LCDIndTest     = 10;
const byte LCDIndAlt      = 11;
const byte LCDIndNAlign   = 12;
const byte LCDIndZone     = 13;
const byte LCDIndBat      = 14;
const byte LCDIndKmHi     = 15;
const byte LCDIndFault    = 16;
const byte LCDIndMil      = 17;
const byte LCDIndKmLow    = 18;
const byte LCDLetterE     = 19;
const byte LCDPlusHi      = 20;
const byte LCDLetterN     = 21;
const byte LCDPlusLow     = 22;
const byte LCDDecimalDot  = 23;
const byte LCDColonDotHi  = 24;
const byte LCDColonDotLow = 25;

/*
//Unused
const byte BoxA   = 0 ;
const byte BoxB   = 1 ;
const byte BoxC   = 2 ;
const byte BoxD   = 3 ;
const byte BoxE   = 4 ;
const byte BoxF   = 5 ;
const byte BoxG   = 6 ;
const byte BoxH   = 7 ;
const byte CrossA = 8 ;
const byte CrossB = 9 ;
const byte CrossC = 10;
const byte CrossD = 11;
const byte CrossE = 12;
const byte CrossF = 13;
const byte CrossG = 14;
const byte CrossH = 15;
*/

//Names for the IDs of the 32 bearing/azimuth ring "spokes"
/*
const byte RingAngle00 = 0 ;  //NORTH
const byte RingAngle02 = 1 ;
const byte RingAngle04 = 2 ;
const byte RingAngle06 = 3 ;
const byte RingAngle08 = 4 ;
const byte RingAngle10 = 5 ;
const byte RingAngle12 = 6 ;
const byte RingAngle14 = 7 ;
const byte RingAngle16 = 8 ;  // EAST
const byte RingAngle18 = 9 ;
const byte RingAngle20 = 10;
const byte RingAngle22 = 11;
const byte RingAngle24 = 12;
const byte RingAngle26 = 13;
const byte RingAngle28 = 14;
const byte RingAngle30 = 15;
const byte RingAngle32 = 16;  // SOUTH
const byte RingAngle34 = 17;
const byte RingAngle36 = 18;
const byte RingAngle38 = 19;
const byte RingAngle40 = 20;
const byte RingAngle42 = 21;
const byte RingAngle44 = 22;
const byte RingAngle46 = 23;
const byte RingAngle48 = 24;  // WEST
const byte RingAngle50 = 25;
const byte RingAngle52 = 26;
const byte RingAngle54 = 27;
const byte RingAngle56 = 28;
const byte RingAngle58 = 29;
const byte RingAngle60 = 30;
const byte RingAngle62 = 31;
*/

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

//Masks to print a digit on a regular 7 segments display, 8th bit unused
const byte Num0 = 0b11111100;
const byte Num1 = 0b01100000;
const byte Num2 = 0b11011010;
const byte Num3 = 0b11110010;
const byte Num4 = 0b01100110;
const byte Num5 = 0b10110110;
const byte Num6 = 0b10111110;
const byte Num7 = 0b11100000;
const byte Num8 = 0b11111110;
const byte Num9 = 0b11110110;
const byte NumNull = 0b00000000;

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

//Masks to print a letter or a digit on the 16 segments square display, uses 2 bytes, 
const byte Box_N   = 0b01100110;
const byte Cross_N = 0b00010001;
//const byte Box_S   = 0b00001011;
//const byte Cross_S = 0b00001010;
const byte Box_S   = 0b10111011;
const byte Cross_S = 0b00100010;
//const byte Box_E   = 0b00001111;
//const byte Cross_E = 0b00000010;
const byte Box_E   = 0b10011111;
const byte Cross_E = 0b00100010;
const byte Box_W   = 0b01100110;
const byte Cross_W = 0b00010100;

const byte Box_Bar = 0b00011000;

const byte Cross_X = 0b01010101;
const byte Cross_Y = 0b01001001;
const byte Cross_K = 0b11011000;

const byte Box_0   = 0b11110000;
const byte Cross_0 = 0b10001000;
const byte Box_1   = 0b01100000;
const byte Cross_1 = 0b00000000;

const byte Box_2   = 0b11010000;
const byte Cross_2 = 0b00101000;

const byte Box_3   = 0b10010000;
const byte Cross_3 = 0b10101000;
const byte Box_4   = 0b01100000;
const byte Cross_4 = 0b10100000;
const byte Box_5   = 0b10110000;
const byte Cross_5 = 0b10100000;
const byte Box_6   = 0b10110000;
const byte Cross_6 = 0b10101000;
const byte Box_7   = 0b11100000;
const byte Cross_7 = 0b00000000;
const byte Box_8   = 0b11110000;
const byte Cross_8 = 0b10101000;
const byte Box_9   = 0b11110000;
const byte Cross_9 = 0b10100000;


//Positive and negative masks used to isolate each single bit from the 16 bits buffer banks
const unsigned int Bit1   = 0b1000000000000000;
const unsigned int Bit2   = 0b0100000000000000;
const unsigned int Bit3   = 0b0010000000000000;
const unsigned int Bit4   = 0b0001000000000000;
const unsigned int Bit5   = 0b0000100000000000;
const unsigned int Bit6   = 0b0000010000000000;
const unsigned int Bit7   = 0b0000001000000000;
const unsigned int Bit8   = 0b0000000100000000;
const unsigned int Bit9   = 0b0000000010000000;
const unsigned int Bit10  = 0b0000000001000000;
const unsigned int Bit11  = 0b0000000000100000;
const unsigned int Bit12  = 0b0000000000010000;
const unsigned int Bit13  = 0b0000000000001000;
const unsigned int Bit14  = 0b0000000000000100;
const unsigned int Bit15  = 0b0000000000000010;
const unsigned int Bit16  = 0b0000000000000001;

const unsigned int NBit1  = 0b0111111111111111;
const unsigned int NBit2  = 0b1011111111111111;
const unsigned int NBit3  = 0b1101111111111111;
const unsigned int NBit4  = 0b1110111111111111;
const unsigned int NBit5  = 0b1111011111111111;
const unsigned int NBit6  = 0b1111101111111111;
const unsigned int NBit7  = 0b1111110111111111;
const unsigned int NBit8  = 0b1111111011111111;
const unsigned int NBit9  = 0b1111111101111111;
const unsigned int NBit10 = 0b1111111110111111;
const unsigned int NBit11 = 0b1111111111011111;
const unsigned int NBit12 = 0b1111111111101111;
const unsigned int NBit13 = 0b1111111111110111;
const unsigned int NBit14 = 0b1111111111111011;
const unsigned int NBit15 = 0b1111111111111101;
const unsigned int NBit16 = 0b1111111111111110;
