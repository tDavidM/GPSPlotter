
// Provides many helpfull mask and named values to abstract some complexity

// heavily used by LCD_Values.ino and quite a bit by Interface.ino
// Define several EEPROM addr locations
// Define a few usefull long and float for convertion factors used in Navigation.ino
// Regroup many strings used for displaying messages

//DisplayModes
enum DisplayModes: byte { M_REFPOINTA, M_REFPOINTB, M_POSITION, M_HEADING, M_DESTINATION, M_MAP};
//(A)RefPointA, (B)RefPointB, (P)Position, (H)Heading, (D)Destination, (M)Map/Grid/Pointer

//Keypad Keys
enum KeypadKeys: char { KEY_0 = '0', KEY_1 = '1', KEY_2 = '2', KEY_3 = '3', KEY_4 = '4',
                           KEY_5 = '5', KEY_6 = '6', KEY_7 = '7', KEY_8 = '8', KEY_9 = '9',
                           KEY_CLEAR = 'C', KEY_ENTER = 'E', KEY_REFPOINT = 'R',
                           KEY_POSITION = 'P', KEY_HEADING = 'H', KEY_ILUM = 'i', // Ilum handled independently 
                           KEY_INPUT  = 'I',
                           KEY_SELECT = 'S',
                           KEY_UP     = 'A',  //Above (Up) (U was taken)
                           KEY_MAP    = 'M',
                           KEY_LEFT   = 'B',  //Back (Left) (L was taken and to fit with Right/Forward)
                           KEY_UPDATE = 'U',
                           KEY_RIGHT  = 'F',  //Forward (Right) (R was taken and to fit with Left/Back)
                           KEY_DEST   = 'D',
                           KEY_DOWN   = 'L',  //Lower (Down)  (D and U were taken and to fit with Up/Above)
                           KEY_PRECIS = 'Z',  //-_- (Precision)
                           KEY_BACK   = 'V',  //Sorry
                           KEY_NONE   = ' '};

enum KeyStates: byte { IDLE, PRESSED, HOLD, RELEASED };
constexpr char NO_KEY = '\0';
constexpr byte NOT_FOUND = 0xFF;

enum Quadrants: byte { Q_NONE, Q_NORTH, Q_EAST, Q_SOUTH, Q_WEST };

//Row ID on the LCD
constexpr byte LCDRowHi       = 0;
constexpr byte LCDRowLow      = 1;

//NEMA messages types
enum MsgTypes: byte { MSG_GPGGA, MSG_GPRMC, MSG_GPVTG};

constexpr byte MaxWaitTimeForMsg = 10;

//Size of the XY grid in number of motor steps and other key control values
constexpr unsigned int STEP_GridSizeX     = 1920;
constexpr unsigned int STEP_GridSizeY     = 1920;
constexpr unsigned int STEP_MicroSecDelay = 2500;
constexpr unsigned int STEP_InitDelay     = 500;
constexpr byte STEP_SizeCorse   = 128;
constexpr byte STEP_SizeInter   = 12;
constexpr byte STEP_SizeFine    = 1;
constexpr byte STEP_MaxPerBatch = 15;
constexpr byte STEP_Overshoot   = 20;
constexpr byte STEP_Cursor      = 75;

constexpr long LatLonConvFactorL     = 100000;

constexpr float LatLonConvFactorF    = 100000.0;
constexpr float FlatPlaneAproximDist = 100000.0;
constexpr float EarthDiamInM         = 12742000.0;
constexpr float LatToMeter           = 1.119492;                 // 111194.92 m per degree
constexpr float DegreMinToDec        = 1666.666666666666666666;  // 100000 / 60
constexpr float DegreeToThirtyTwoTh  = 0.088888888888888888;     // 32 / 360
constexpr float KnotsToKmH           = 1.852;
constexpr float StepToCM             = 0.01252604166;            // 1920 steps per 24,05cm  

constexpr unsigned int DestProximity = 500; // distance in m = Scale/500  =  ( Scale * 2[mm] ) / 1000[mm per m]

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

constexpr byte EEPROM_ControlVal     = 0;
constexpr byte EEPROM_MagicValue     = 42;
constexpr byte EEPROM_True           = 1;
constexpr byte EEPROM_False          = 0;
constexpr byte EEPROM_HourOffset     = 2;
constexpr byte EEPROM_MinuteOffset   = 3;
constexpr byte EEPROM_RefPointA      = 10;   // 10-13 Lat, 14-17 Lon, 18-19 X, 20-21 Y, 22 LatLonVal, 23 XYVal 
constexpr byte EEPROM_RefPointB      = 30;   // 30-33 Lat, 34-37 Lon, 38-39 X, 40-41 Y, 42 LatLonVal, 43 XYVal 
constexpr byte EEPROM_Destination    = 50;   // 50-53 Lat, 54-57 Lon,                   62 LatLonVal
constexpr byte EEPROM_Waypoints      = 100;  // 10 waypoints with each 10 bytes (two longs of 4 bytes each and two 1 byte indicators)
constexpr byte EEPROM_LatPos         = 0;
constexpr byte EEPROM_LonPos         = 4;
constexpr byte EEPROM_XPos           = 8;
constexpr byte EEPROM_YPos           = 10;
constexpr byte EEPROM_LatLonValPos   = 12;
constexpr byte EEPROM_XYValPos       = 13;
constexpr byte EEPROM_WaypointSetPos = 8;
constexpr byte EEPROM_WaypointSize   = 10;
constexpr byte EEPROM_NbMaxAddrUsed = 200;



//Names for the IDs of the numerous indicators
constexpr byte LCDIndSet      = 0 ;
constexpr byte LCDIndDest     = 1 ;
constexpr byte LCDIndQuest    = 2 ;
constexpr byte LCDIndRange    = 3 ;
constexpr byte LCDIndUpdate   = 4 ;
constexpr byte LCDIndPosition = 5 ;
constexpr byte LCDIndHeading  = 6 ;
constexpr byte LCDIndGrid     = 7 ;
constexpr byte LCDIndScale    = 8 ;
constexpr byte LCDIndRefPoint = 9 ;
constexpr byte LCDIndTest     = 10;
constexpr byte LCDIndAlt      = 11;
constexpr byte LCDIndNAlign   = 12;
constexpr byte LCDIndZone     = 13;
constexpr byte LCDIndBat      = 14;
constexpr byte LCDIndKmHi     = 15;
constexpr byte LCDIndFault    = 16;
constexpr byte LCDIndMil      = 17;
constexpr byte LCDIndKmLow    = 18;
constexpr byte LCDLetterE     = 19;
constexpr byte LCDPlusHi      = 20;
constexpr byte LCDLetterN     = 21;
constexpr byte LCDPlusLow     = 22;
constexpr byte LCDDecimalDot  = 23;
constexpr byte LCDColonDotHi  = 24;
constexpr byte LCDColonDotLow = 25;

/*
//Unused
constexpr byte BoxA   = 0 ;
constexpr byte BoxB   = 1 ;
constexpr byte BoxC   = 2 ;
constexpr byte BoxD   = 3 ;
constexpr byte BoxE   = 4 ;
constexpr byte BoxF   = 5 ;
constexpr byte BoxG   = 6 ;
constexpr byte BoxH   = 7 ;
constexpr byte StarA = 8 ;
constexpr byte StarB = 9 ;
constexpr byte StarC = 10;
constexpr byte StarD = 11;
constexpr byte StarE = 12;
constexpr byte StarF = 13;
constexpr byte StarG = 14;
constexpr byte StarH = 15;
*/

//Names for the IDs of the 32 bearing/azimuth ring "spokes"
/*
constexpr byte RingAngle00 = 0 ;  //NORTH
constexpr byte RingAngle02 = 1 ;
constexpr byte RingAngle04 = 2 ;
constexpr byte RingAngle06 = 3 ;
constexpr byte RingAngle08 = 4 ;
constexpr byte RingAngle10 = 5 ;
constexpr byte RingAngle12 = 6 ;
constexpr byte RingAngle14 = 7 ;
constexpr byte RingAngle16 = 8 ;  // EAST
constexpr byte RingAngle18 = 9 ;
constexpr byte RingAngle20 = 10;
constexpr byte RingAngle22 = 11;
constexpr byte RingAngle24 = 12;
constexpr byte RingAngle26 = 13;
constexpr byte RingAngle28 = 14;
constexpr byte RingAngle30 = 15;
constexpr byte RingAngle32 = 16;  // SOUTH
constexpr byte RingAngle34 = 17;
constexpr byte RingAngle36 = 18;
constexpr byte RingAngle38 = 19;
constexpr byte RingAngle40 = 20;
constexpr byte RingAngle42 = 21;
constexpr byte RingAngle44 = 22;
constexpr byte RingAngle46 = 23;
constexpr byte RingAngle48 = 24;  // WEST
constexpr byte RingAngle50 = 25;
constexpr byte RingAngle52 = 26;
constexpr byte RingAngle54 = 27;
constexpr byte RingAngle56 = 28;
constexpr byte RingAngle58 = 29;
constexpr byte RingAngle60 = 30;
constexpr byte RingAngle62 = 31;
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
constexpr byte Num0 = 0b11111100;
constexpr byte Num1 = 0b01100000;
constexpr byte Num2 = 0b11011010;
constexpr byte Num3 = 0b11110010;
constexpr byte Num4 = 0b01100110;
constexpr byte Num5 = 0b10110110;
constexpr byte Num6 = 0b10111110;
constexpr byte Num7 = 0b11100000;
constexpr byte Num8 = 0b11111110;
constexpr byte Num9 = 0b11110110;
constexpr byte NumNull = 0b00000000;

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

  "Box" forms the outer square (in uppercaps)
  "Star" consists of inner spokes (in lowercaps)
*/

//Masks to print a letter or a digit on the 16 segments square display, uses 2 bytes, 
constexpr byte Box_N   = 0b01100110;
constexpr byte Star_N = 0b00010001;
//constexpr byte Box_S   = 0b00001011;
//constexpr byte Star_S = 0b00001010;
constexpr byte Box_S   = 0b10111011;
constexpr byte Star_S = 0b00100010;
//constexpr byte Box_E   = 0b00001111;
//constexpr byte Star_E = 0b00000010;
constexpr byte Box_E   = 0b10011111;
constexpr byte Star_E = 0b00100010;
constexpr byte Box_W   = 0b01100110;
constexpr byte Star_W = 0b00010100;

constexpr byte Box_Bar = 0b00011000;

constexpr byte Star_X = 0b01010101;
constexpr byte Star_Y = 0b01001001;
constexpr byte Star_K = 0b11011000;

constexpr byte Box_0   = 0b11110000;
constexpr byte Star_0 = 0b10001000;
constexpr byte Box_1   = 0b01100000;
constexpr byte Star_1 = 0b00000000;

constexpr byte Box_2   = 0b11010000;
constexpr byte Star_2 = 0b00101000;

constexpr byte Box_3   = 0b10010000;
constexpr byte Star_3 = 0b10101000;
constexpr byte Box_4   = 0b01100000;
constexpr byte Star_4 = 0b10100000;
constexpr byte Box_5   = 0b10110000;
constexpr byte Star_5 = 0b10100000;
constexpr byte Box_6   = 0b10110000;
constexpr byte Star_6 = 0b10101000;
constexpr byte Box_7   = 0b11100000;
constexpr byte Star_7 = 0b00000000;
constexpr byte Box_8   = 0b11110000;
constexpr byte Star_8 = 0b10101000;
constexpr byte Box_9   = 0b11110000;
constexpr byte Star_9 = 0b10100000;


//Positive and negative masks used to isolate each single bit from the 16 bits buffer banks
constexpr unsigned int Bit1   = 0b1000000000000000;
constexpr unsigned int Bit2   = 0b0100000000000000;
constexpr unsigned int Bit3   = 0b0010000000000000;
constexpr unsigned int Bit4   = 0b0001000000000000;
constexpr unsigned int Bit5   = 0b0000100000000000;
constexpr unsigned int Bit6   = 0b0000010000000000;
constexpr unsigned int Bit7   = 0b0000001000000000;
constexpr unsigned int Bit8   = 0b0000000100000000;
constexpr unsigned int Bit9   = 0b0000000010000000;
constexpr unsigned int Bit10  = 0b0000000001000000;
constexpr unsigned int Bit11  = 0b0000000000100000;
constexpr unsigned int Bit12  = 0b0000000000010000;
constexpr unsigned int Bit13  = 0b0000000000001000;
constexpr unsigned int Bit14  = 0b0000000000000100;
constexpr unsigned int Bit15  = 0b0000000000000010;
constexpr unsigned int Bit16  = 0b0000000000000001;

constexpr unsigned int NBit1  = 0b0111111111111111;
constexpr unsigned int NBit2  = 0b1011111111111111;
constexpr unsigned int NBit3  = 0b1101111111111111;
constexpr unsigned int NBit4  = 0b1110111111111111;
constexpr unsigned int NBit5  = 0b1111011111111111;
constexpr unsigned int NBit6  = 0b1111101111111111;
constexpr unsigned int NBit7  = 0b1111110111111111;
constexpr unsigned int NBit8  = 0b1111111011111111;
constexpr unsigned int NBit9  = 0b1111111101111111;
constexpr unsigned int NBit10 = 0b1111111110111111;
constexpr unsigned int NBit11 = 0b1111111111011111;
constexpr unsigned int NBit12 = 0b1111111111101111;
constexpr unsigned int NBit13 = 0b1111111111110111;
constexpr unsigned int NBit14 = 0b1111111111111011;
constexpr unsigned int NBit15 = 0b1111111111111101;
constexpr unsigned int NBit16 = 0b1111111111111110;

constexpr byte SBit1 = 0b10000000;
constexpr byte SBit2 = 0b01000000;
constexpr byte SBit3 = 0b00100000;
constexpr byte SBit4 = 0b00010000;
constexpr byte SBit5 = 0b00001000;
constexpr byte SBit6 = 0b00000100;
constexpr byte SBit7 = 0b00000010;
constexpr byte SBit8 = 0b00000001;

