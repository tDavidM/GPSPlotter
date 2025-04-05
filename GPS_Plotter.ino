

//Sketch uses 26020 bytes (84%) of program storage space. Maximum is 30720 bytes.
//Global variables use 1651 bytes (80%) of dynamic memory, leaving 397 bytes for local variables. Maximum is 2048 bytes.

//ToDo/Ideas
//parse GPVTG messages ??? (maybe to heavy on processing, no real value in the additional data)
//Alter Date based on Hour/Minute offset ??? (maybe to heavy on code, those 30Kb are precious)
//Use 3 RefPoint to help with projection errors ????? (definitively too heavy on computations)



// Contains the high level fonctions called by timers

// UpdateDisplay runs every 100ms to process any new keypress and display the interface
// ReadKeypad runs every 25ms, scan the KeyPad matrix for any new keypress
// ReadSerial is called every single loop to process every incoming char from serial and put them in a buffer
// GetSerialGPS keeps NavData up to date by processing messages previously received from Serial (keept in a buffer)
//   when nothing is ready in the buffer, low priority computations are done using ComputeNewAngleDist
// ToggleCursor runs every 500ms, control blinking of the cursor (but also turn the steps motors off after a moments when done moving)


#include <Keypad.h>
#include <arduino-timer.h>
#include <EEPROM.h>
#include "Const.h"

//LCD segment display control pins
const int LoadPin    = 13; // Pulled high then low when done sending data to the LCD shift register (Not connected, seems to work without)
const int DataPin    = 3;  // Sends the current bit value for the LCD
const int ClockPin   = 2;  // Register the bit value on the falling edge
const int LCDBackPin = A0; // Alternatively pulled briefly high then back low to be later briefly low then back high,
                           //   keeps the LCD backplane more or less correctly energized, should be improved but it's good enough
 
//LEDs control pins
const int LedPin       = A1; // Controls the red LED next to the LCD
const int BackLightPin = 4;  // Controls the PNP transistor that turn On/Off the backligh (NOTE:pin high = backlight off)

//Stepper motors control pins
const int DirXPin  = A2;      // Controls the direction of the X axis stepper motor
const int StepXPin = A3;      // Move the X stepper motor by one full step every high/low cycle
const int DirYPin  = A4;      // Controls the direction of the Y axis stepper motor
const int StepYPin = A5;      // Move the Y stepper motor by one full step every high/low cycle
const int StepEnablePin = A7; // Enables the 2 stepper motors when moving and put them back to sleep a short while after done moving (see ToggleCursor)

// Timer used in setup()
auto timer = timer_create_default();

// Keypad vars
bool NewKeyPress = false; // Resetted by the fonction consuming the keypress
char KeyPressVal = ' ';
bool CurState    = false;  // Set by ToggleCursor() to control blinking of the char under the cursor
bool FastBlink   = false;

// Keypad size
const int ROW_NUM = 4;    //4 rows
const int COLUMN_NUM = 4; //4 columns

// Matrix of keys on the keypad, used to set the global variable char KeyPressVal by ReadKeypad()
char AvailableKeys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'R'},
  {'4','5','6', 'P'},
  {'7','8','9', 'H'},
  {'C','0','E', 'i'}
};

// Set the pinout for the keypad
byte pin_rows[ROW_NUM] = {5, 6, 7, 8};
byte pin_column[COLUMN_NUM] = {9, 10, 11, 12};
Keypad keypad = Keypad( makeKeymap(AvailableKeys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// One instance for each Modes
struct ElemStruct {
   bool LatSet = false;
   bool LonSet = false;
   bool XYSet  = false;
};

// Structure for storing state for the interface
struct StateStruct {
  // Interface mode
  char CurrentMode = 'H';  //(A)RefPointA, (B)RefPointB, (P)Position, (H)Heading, (D)Destination, (M)Map/Grid/Pointer

  // In each mode, control the sub-mode (displayed value)
  bool ShowDist = false, ShowLat = true, ShowLon = false; // Generic sub-modes
  bool ShowPrecision = false;                              // only for Lat/Lon
  bool ShowAltitude  = false, ShowSpeed = false, ShowDate = false, ShowScale = false; // Heading specific sub-modes

  // Flags indicating in each modes if the XY or Lat/Lon value have been set
  ElemStruct RefPointA, RefPointB, Destination, Pointer;

  // Flag to know if the current position (GPS fix) is valid and if the XY plotter can be used (RefPointA and RefPointB XY/Lat/Lon all set)
  bool CurrentValid = false, CursorValid = false;

  // Flags for Inputing, Editing, Updating modes
  bool Editing  = false, Updating = false;
  byte Inputing = 0, Resetting = 0, Recalibrate=0;

  // When the plotter is actively moving
  bool MovingCur = false;
  // If a waypoint was set for Position
  bool WaypointsSet[10]= {false, false, false, false, false, false, false, false, false, false };
  byte CurrentWaypoint=0; 

  // Used in Inputing, Editing, Updating modes
  char IndQuadrant = ' ';           // In Editing, the current N/S or E/W value
  char InputBuffer[10];             // In Inputing, keeps the full value displayed and modified by keypress
  byte InputPos = 0, InputMax = 8;  // In Inputing, manage the current cursor position and max cursor position
  int  CursorX, CursorY = 0;        // In Updating, keeps the current X or Y value
  bool IndDirX    = true;           // In Updating, if the X or Y value is displayed
  byte CursorStep = 75;             // In Updating, how many steps per arrow keypress
};

// Only one instance
StateStruct MainState;

// One instance for each Modes
struct NavData {
  long Lat=0, Lon=0;           // Latitude/Longitude in absolute values without decimal
                               //   where, for exemple, -45.123 is stored as 4512300 (conversion factor of x100000)
  long Dist=0;                // Distance in meters
  bool NInd=true, EInd=false;  // North/South and East/West indicator for Latitude/Longitude
  int  X=0, Y=0;               // Number of steps away from the top left corner of the plotter (always positive)
  byte Angle=255;              // Direction in "ThirtyTwoTh" of a degre (0=N, 8=E, 16=S, 24=W, 255=none) 
};

// Structure for storing navigation values
struct NavStruct {
  // For each mode, keep the specific navigation data (Lat/Lon, N/S or E/W inds, XY vals and direction/distance)
  NavData RefPointA, RefPointB, Destination, Pointer, Current;
  NavData *Position; // pointer to the current waypoint
  NavData Waypoints[10];

  // Generic navigation data (altitude, speed, map scale, date/time, time zonde offset and GPS fix)
  long Altitude=0, Scale=0; 
  unsigned int Speed=0;
  char Date[8]="000000", Time[6]="0000";
  byte HourOffset = 0, MinuteOffset = 0,  GPSFix=0;
  bool NearZone=false;
};

// Only one instance
NavStruct NavValue;

// Structure for storing stepper motor control values
struct StepMotorStruct {
  int CurrentX=0, CurrentY=0;  // Current number of steps away from the top/left corner of the plotter
                               //   requires calibration when turned on (always positive)
  int TargetX=0 , TargetY=0;   // Target number of steps away from the top/left corner of the plotter

  // Scaling factor used to take a given X/Y value and compute the coresponding Lat/Lon value
  float ScalingFactorX=0  , ScalingFactorY=0;
  // Scaling factor used to take a given Lat/Lon value and compute the coresponding X/Y value
  float ScalingFactorLat=0, ScalingFactorLon=0;

  bool RatioSet=false;
  bool Calibrating=false;
} ;

// Only one instance
StepMotorStruct StepMotorVal;

// Current BackLight state, used to drive the BackLightPin
bool BackLightState = false;

// Buffer used for serial GPS data recv
const int BufferSize = 90;  
char RecvBuff[BufferSize];   // Stores the data being activly received from Serial
char GPGGABuff[BufferSize];  // Stores the last GPGGA message received
char GPRMCBuff[BufferSize];  // Stores the last GPRMC message received
//char GPVTGBuff[BufferSize];  // Stores the last GPVTG message received
bool GPGGAAvailable = false; // If a valid NEMA GPGGA message was received
bool GPRMCAvailable = false; // If a valid NEMA GPRMC message was received
//bool GPVTGAvailable = false; // If a valid NEMA GPVTG message was received
byte LastMessageTTL=0;          // Used to detect that GPS messages are no longer received

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

void setup() {
  byte TestVal=0;

  // Initialize serial communication
  //Serial.begin(9600);
  Serial.begin(38400);

  pinMode(LoadPin,      OUTPUT);
  pinMode(DataPin,      OUTPUT);
  pinMode(ClockPin,     OUTPUT);
  pinMode(LCDBackPin,   OUTPUT);

  pinMode(LedPin,       OUTPUT);
  pinMode(BackLightPin, OUTPUT);

  pinMode(StepXPin, OUTPUT);
  pinMode(DirXPin,  OUTPUT);
  pinMode(StepYPin, OUTPUT);
  pinMode(DirYPin,  OUTPUT);
  pinMode(StepEnablePin, OUTPUT);

  // Because the pin is connected to a PNP transistor, high = backlight off
  digitalWrite(BackLightPin, HIGH);

  //Set the initial display to "Loading"
  ShowText(LCDRowHi, Loading_Msg);
  RefreshLCD(NULL);

  NavValue.Position = &NavValue.Waypoints[0]; // [MainState.CurrentWaypoint];

  // Read from EEPROM if it's valid
  TestVal = EEPROM.read(EEPROM_ControlVal);
  if( TestVal == EEPROM_MagicValue ) {
    ReadEEPROM(NULL);
    ComputeNewXY('P');
  }

  delay(500);

  // Force the plotter to the top/left corner for calibration
  CalibrateXY(NULL);
  //ShowText(LCDRowHi, "       ");

  HeadingRefresh(NULL);
  RefreshLCD(NULL);

  timer.every(25,  ReadKeypad);       // Reads keypress and update the stepper motor in small increments
  timer.every(100, UpdateDisplay);    // Takes the LCD bit buffer and pushs it to the display
  timer.every(125, GetSerialGPS);     // Keep NavData up to date by listening on Serial and computing angle, distance and scaling factor when idle
  timer.every(500, ToggleCursor);     // Controls blinking of the cursor, LED and make sure the display state is up to date
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

void loop() {

  ReadSerial(NULL);

  timer.tick();
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Runs every 100ms, display the interface
bool UpdateDisplay(void *) {
  //See bellow
  CurState ? digitalWrite(LCDBackPin, HIGH) : digitalWrite(LCDBackPin, LOW);

  if(NewKeyPress) {
    NewKeyPress = false;
    MainInterface(KeyPressVal);
  } 
  
  //Toggle the value at the current cursor position with _ every half second
  Blinking(NULL);
  //Force the inner ring on the dearing indicator on
  ToggleRing(true, false); 
  //push the buffer to the LCD display
  RefreshLCD(NULL);

  // Alternatively pulled briefly high then back low to be later briefly low then back high,
  //   keeps the LCD backplane more or less correctly energized, should be improved but it's good enough
  CurState ? digitalWrite(LCDBackPin, LOW) : digitalWrite(LCDBackPin, HIGH);

  return true; // to repeat the action - false to stop
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Runs every 25ms, scan the KeyPad matrix for any new keypress
bool ReadKeypad(void *) {
  
  // Register a keypres
  char KeyPress = keypad.getKey();
  if (KeyPress){

    if(KeyPress == 'i') {
      // Turning the backlight is done (this one is not "raised" )
      digitalWrite(BackLightPin, BackLightState);
      BackLightState = !BackLightState;
    } else {
      // any other keypress is raised for an other fonction to process it
      NewKeyPress = true; 
      KeyPressVal = KeyPress; // Resetted by the fonction consuming the keypress
    }
  }

  // Move the XY plotter by a few steps and hands back control to avoid locking everyting while moving
  MakeStep(NULL);

  return true; // to repeat the action - false to stop
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

bool ReadSerial(void *) {
  char Recv, MsgType[7];
  static byte NbByte = 0;
  byte MaxIter = 32;
  
  // If any bytes are available from Serial
  while( MaxIter != 0 && Serial.available() > 0 ) {
    Recv = Serial.read();

    // Add the new characters to the buffer until it's a line return
    if( Recv != '\n' ) {
      RecvBuff[NbByte] = Recv;
      NbByte++;

      // Prevent overflow
      if( NbByte >= BufferSize )
        NbByte = BufferSize - 1;
    } else {
      // terminate the string and pre-process the message to ignore invalid ones
      RecvBuff[NbByte] = '\0';
      NbByte = 0;
      strncpy(MsgType, RecvBuff, 6);
      MsgType[6] = '\0';

      // If the message is valid, copy it to the correct second buffer (e.g.  $GPGGA,,,,,,0,00,99.99,,,,,,*48  is not good)
      if( RecvBuff[7] != ',' ) {
        if( strcmp(MsgType, "$GPGGA") == 0  ||  strcmp(MsgType, "$GNGGA") == 0 ) {
          strncpy(GPGGABuff, RecvBuff, BufferSize-1);
          GPGGAAvailable = true;
        } else if( strcmp(MsgType, "$GPRMC") == 0  ||  strcmp(MsgType, "$GNRMC") == 0 ) {
          strncpy(GPRMCBuff, RecvBuff, BufferSize-1);
          GPRMCAvailable = true;
        } /*else if( strcmp(MsgType, "$GPVTG") == 0  ||  strcmp(MsgType, "$GNVTG") == 0 ) {
          strncpy(GPVTGBuff, RecvBuff, BufferSize-1);
          GPVTGAvailable = true;
        }*/
      }

    }
    MaxIter--;
  }

}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Keep NavData up to date by processing messages previously received from Serial
bool GetSerialGPS(void *) {

  // If any of the buffers are ready for processing
  if (GPGGAAvailable) {
    ProcessNMEAData(GPGGABuff, MsgTypeGPGGA);
    GPGGAAvailable = false;

  } else if(GPRMCAvailable) {
    ProcessNMEAData(GPRMCBuff, MsgTypeGPRMC);
    GPRMCAvailable = false;

  } //else if(GPVTGAvailable) {
    //ProcessNMEAData(GPVTGBuff, MsgTypeGPVTG);
    //GPVTGAvailable = false;
  //}
  else {
    // Otherwise do less time sensitive computations
    ComputeNewAngleDist(NULL);
  }

  return true; // to repeat the action - false to stop
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Runs every 500ms, control blinking of the cursor
bool ToggleCursor(void *) {
  // Control blinking of the char under the cursor
  CurState=!CurState;

  // Give the step motors some time before turning them off
  if( !MainState.MovingCur ) {
    digitalWrite(StepEnablePin, LOW);
    digitalWrite(DirXPin, LOW);
    digitalWrite(DirYPin, LOW);
    StepMotorVal.Calibrating = false;
  }
  
  // Detect that GPS messages are no longer received
  if(LastMessageTTL < MaxWaitTimeForMsg)
    LastMessageTTL++; 

  return true; // to repeat the action - false to stop
}

