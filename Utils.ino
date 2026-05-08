
// Various fonctinality grouped here

// DisplayLat displays the given latitude value
// DisplayLon displays the given longitude value
// SetInterface reset bool values used for interface state
// SetInputBuf sets the input buffer to a given value
// InitInputBuf sets the input buffer to all zero
// DisplayDist displays the given ditance value
// DisplayTime displays the current time
// DisplayDate displays the current date
// ShowInt prints a value on a specific Row starting at Position, it does not erase value before the offset but erase the rest of the row after
//   overloaded to take a long instead of a int
// ShowText prints a cstring on a specific Row, it expect padding before for aligment, and erase the rest of the row if shorter than 7
// LatLonAbsolute converts the given long value to it's absolute and return the signs it was before conversion (bool)
// AddLatLonSign takes the absolute lat or lon value, it's N/S or E/W indicator and return the propper +/- lat or lon
// ConvertStringToNum returns a long that is the given char value that been multiplied by a given float factor
// SavePosition copies the current position to the selected waypoint
// ClearPosition erase the values from the selected waypoint
// ReadEEPROM reads the EEPROM and set the appropriate NavData when valid
// ReadDetailEEPROM
// UpdateEEPROM writes to EEPROM when a NavData is valid and different
// UpdateDetailEEPROM
// UpdateSplitLong writes a 32bits long to four 8bits bytes to consecutive EEPROM addrs
// ReadMergeLong reads four 8bits bytes from consecutive EEPROM addrs and return a single 32bits long
// UpdateSplitInt writes a 16bits int to two 8bits bytes to consecutive EEPROM addrs
// ReadMergeInt reads two 8bits bytes from consecutive EEPROM addrs and return a single 16bits int
// ResetParam sets to default all the NavData values and some internal state values as well as overwiting everyting in the EEPROM
// SetTask sets a task at position Slot to be executed every Delay miliseconds
// HandleTask runs and update tasks according to their timers
// RunTask runs all tasks that are due according to their timers
// CheckTask checks the next task to schedule

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Display the given latitude value
void DisplayLat(long Lat, bool NInd, bool Precision) {
  NInd ?  ToggleBox(Box_N)   : ToggleBox(Box_S);
  NInd ?  ToggleStar(Star_N) : ToggleStar(Star_S);

  Precision ? ShowText(LCDRowHi, "  00000") : ShowText(LCDRowHi, "   0000");

  if(Lat > 999999)
    ShowInt(LCDRowHi, &Lat, Precision ? 1 : 2);
  else if(Lat > 99999)
    ShowInt(LCDRowHi, &Lat, Precision ? 2 : 3);
  else if(Lat > 9999) 
    ShowInt(LCDRowHi, &Lat, Precision ? 3 : 4);
  else if(Lat > 999)
    ShowInt(LCDRowHi, &Lat, Precision ? 4 : 5);
  else if(Lat > 99)
    ShowInt(LCDRowHi, &Lat, Precision ? 5 : 6);
  else if(Precision)
    ShowInt(LCDRowHi, &Lat, 6);

  ToggleInd(LCDDecimalDot, !Precision);
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Display the given longitude value
void DisplayLon(long Lon, bool EInd, bool Precision) {
  EInd ?  ToggleBox(Box_E)   : ToggleBox(Box_W);
  EInd ?  ToggleStar(Star_E) : ToggleStar(Star_W);      

  Precision ? ShowText(LCDRowHi, "  00000") : ShowText(LCDRowHi, "   0000");

  if(Lon > 9999999)
    ShowInt(LCDRowHi, &Lon, Precision ? 0 : 1);
  else if(Lon > 999999)
    ShowInt(LCDRowHi, &Lon, Precision ? 1 : 2);
  else if(Lon > 99999)
    ShowInt(LCDRowHi, &Lon, Precision ? 2 : 3);
  else if(Lon > 9999)
    ShowInt(LCDRowHi, &Lon, Precision ? 3 : 4);
  else if(Lon > 999)
    ShowInt(LCDRowHi, &Lon, Precision ? 4 : 5);
  else if(Lon > 99)
    ShowInt(LCDRowHi, &Lon, Precision ? 5 : 6);
  else if(Precision)
    ShowInt(LCDRowHi, &Lon, 6);

  ToggleInd(LCDDecimalDot, !Precision);
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Reset bool values used for interface state
void SetInterface(void *) {
  MainState.ShowDist      = MainState.CurrentMode != M_HEADING;  // Dist not used in mode Heading
  MainState.ShowLat       = MainState.CurrentMode == M_HEADING;
  MainState.ShowLon       = false;
  MainState.ShowPrecision = false;
  MainState.ShowSpeed     = false;
  MainState.ShowAltitude  = false;
  MainState.ShowScale     = false;
  MainState.ShowDate      = false;
  MainState.Resetting     = 0;
  MainState.Recalibrate   = 0;
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Set the input buffer to a given value
void SetInputBuf(long Val, bool IndLon) {
  char IntBuffer[10];
  byte j = 0, len;
  long mask = 1000000;

  if(Val == 0)
    InitInputBuf(NULL);
  else {
    ltoa(Val, IntBuffer, 10);
    len = strlen(IntBuffer);

    if( IndLon )
      mask *= 10;

    for(byte i=0; i<8; i++) {
      if(i < MainState.InputPos || i > MainState.InputMax)
        MainState.InputBuffer[i] = ' ';
      else {
        if(Val < mask) {
          mask /= 10;
          MainState.InputBuffer[i] = '0';
        } else {
          if(j < len)
            MainState.InputBuffer[i] = IntBuffer[j];
          //else
          //  MainState.InputBuffer[i] = '0';
          j++;
        }
      }
    }
    MainState.InputBuffer[8] = 0x00;
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Set the input buffer to all zero
void InitInputBuf(void *) {

  for(byte i=0; i<8; i++) {
    if(i < MainState.InputPos || i > MainState.InputMax)
      MainState.InputBuffer[i] = ' ';
    else
      MainState.InputBuffer[i] = '0';
  }
  MainState.InputBuffer[8] = 0x00;
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Display the given ditance value
void DisplayDist(long Dist) {
  ShowText(LCDRowHi, "   0000");

  if(Dist < 10)
    ShowInt(LCDRowHi, &Dist, 6);
  else if(Dist < 100)
    ShowInt(LCDRowHi, &Dist, 5);
  else if(Dist < 1000)
    ShowInt(LCDRowHi, &Dist, 4);
  else if(Dist < 10000)
    ShowInt(LCDRowHi, &Dist, 3);
  else if(Dist < 100000)
    ShowInt(LCDRowHi, &Dist, 2);
  else if(Dist < 1000000)
    ShowInt(LCDRowHi, &Dist, 1);
  else
    ShowInt(LCDRowHi, &Dist, 0);

  ToggleInd(LCDDecimalDot, true);
  ToggleInd(LCDIndKmHi,    true);
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Display the current time
void DisplayTime(void *) {
  ShowText(LCDRowLow, MaskBlank_Msg);

  ToggleNumberVal(LCDRowLow, 2, NavValue.Time[0]);
  ToggleNumberVal(LCDRowLow, 3, NavValue.Time[1]);
  ToggleNumberVal(LCDRowLow, 4, NavValue.Time[2]);
  ToggleNumberVal(LCDRowLow, 5, NavValue.Time[3]);

  ToggleInd(LCDColonDotHi,  true);
  ToggleInd(LCDColonDotLow, true);
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Display the current date
void DisplayDate(void *) {
  switch(NavValue.Date[0]) {
    case  '0':  ToggleBox(Box_0);  ToggleStar(Star_0);  break;
    case  '1':  ToggleBox(Box_1);  ToggleStar(Star_1);  break;
    case  '2':  ToggleBox(Box_2);  ToggleStar(Star_2);  break;
    case  '3':  ToggleBox(Box_3);  ToggleStar(Star_3);  break;
    case  '4':  ToggleBox(Box_4);  ToggleStar(Star_4);  break;
    case  '5':  ToggleBox(Box_5);  ToggleStar(Star_5);  break;
    case  '6':  ToggleBox(Box_6);  ToggleStar(Star_6);  break;
    case  '7':  ToggleBox(Box_7);  ToggleStar(Star_7);  break;
    case  '8':  ToggleBox(Box_8);  ToggleStar(Star_8);  break;
    case  '9':  ToggleBox(Box_9);  ToggleStar(Star_9);  break;
  }
  ToggleNumberVal(LCDRowHi, 0, NavValue.Date[1]);
  ToggleNumberVal(LCDRowHi, 1, '-');
  ToggleNumberVal(LCDRowHi, 2, NavValue.Date[2]);
  ToggleNumberVal(LCDRowHi, 3, NavValue.Date[3]);
  ToggleNumberVal(LCDRowHi, 4, '-');
  ToggleNumberVal(LCDRowHi, 5, NavValue.Date[4]);
  ToggleNumberVal(LCDRowHi, 6, NavValue.Date[5]);
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Print a value on a specific Row starting at Position
//  does not erase value before the offset but erase the rest of the row after
void ShowInt(byte Row, int Val, byte Offset) {
  char IntBuffer[15];
  byte Index = 0;
  bool Ended = false;

  itoa(Val, IntBuffer, 10);

  for(byte i=0; i<7; i++) {
    if( i >= Offset ) {
      if(Ended)
        ToggleNumberVal(Row, i, ' '); 
      else {
        Ended = IntBuffer[Index] == 0x00;
        ToggleNumberVal(Row, i, IntBuffer[Index]);
        Index++;
      }
    }
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Print a value on a specific Row starting at Position
//  does not erase value before the offset but erase the rest of the row after
//  overloaded to take a long instead of a int
void ShowInt(byte Row, long *Val, byte Offset) {
  char IntBuffer[15];
  byte Index = 0;
  bool Ended = false;

  ltoa(*Val, IntBuffer, 10);

  for(byte i=0; i<7; i++) {
    if( i >= Offset ) {
      if(Ended)
        ToggleNumberVal(Row, i, ' '); 
      else {
        Ended = IntBuffer[Index] == 0x00;
        ToggleNumberVal(Row, i, IntBuffer[Index]);
        Index++;
      }
    }
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Print a cstring on a specific Row,
//   expect padding before for aligment, and erase the rest of the row if shorter than 7
void ShowText(byte Row, char Text[]) {
  bool Ended = false;

  for(byte i=0; i<7; i++) {
    if(Ended)
     ToggleNumberVal(Row, i, ' '); 
    else {
      Ended = Text[i] == 0x00;
      ToggleNumberVal(Row, i, Text[i]);
    }
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Convert the given long value to it's absolute and return the signs it was before conversion (bool)
bool LatLonAbsolute(long *Val) {
  long Out;
  if( *Val < 0 ) {
    Out = abs(*Val);
    *Val = Out;
    return false;
  } else
    return true;
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Take the absolute lat or lon value, it's N/S or E/W indicator and return the propper +/- lat or lon
long AddLatLonSign( long Val, bool *Ind, bool LatMode) {
  if (LatMode)
    return Ind ? Val    : Val*-1;
  else
    return Ind ? Val*-1 : Val;
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Return a long that is the given char value that been multiplied by a given float factor
long ConvertStringToNum(char *Val, float *Factor) {
  float ConvVal;

  ConvVal = atof(Val);
  ConvVal *= *Factor;
  return round(ConvVal);
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Copie the current position to the selected waypoint
void SavePosition(void *) {
  NavValue.Position->Lat  = NavValue.Current.Lat;
  NavValue.Position->Lon  = NavValue.Current.Lon;
  NavValue.Position->NInd = NavValue.Current.NInd;
  NavValue.Position->EInd = NavValue.Current.EInd;
  NavValue.Position->X    = NavValue.Current.X;
  NavValue.Position->Y    = NavValue.Current.Y;
  MainState.WaypointsSet[MainState.CurrentWaypoint] = true;
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Erase the values from the selected waypoint
void ClearPosition(void *) {
  NavValue.Position->Lat  = 0;
  NavValue.Position->Lon  = 0;
  NavValue.Position->NInd = true;
  NavValue.Position->EInd = false;
  NavValue.Position->X    = 0;
  NavValue.Position->Y    = 0;
  MainState.WaypointsSet[MainState.CurrentWaypoint] = false;
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Reads the EEPROM and set the appropriate NavData when valid
void ReadEEPROM(void *) {
  byte Offset;

  NavValue.HourOffset   = EEPROM.read(EEPROM_HourOffset); 
  NavValue.MinuteOffset = EEPROM.read(EEPROM_MinuteOffset); 

  ReadDetailEEPROM(&NavValue.RefPointA,   &MainState.RefPointA,   EEPROM_RefPointA);
  ReadDetailEEPROM(&NavValue.RefPointB,   &MainState.RefPointB,   EEPROM_RefPointB);
  ReadDetailEEPROM(&NavValue.Destination, &MainState.Destination, EEPROM_Destination);

  for(byte i=0; i<10; i++) {
    Offset = EEPROM_Waypoints + (i * EEPROM_WaypointSize);

    // 0-3 Lat, 4-7 Lon, 8 WaypointSet
    if( EEPROM.read(Offset + EEPROM_WaypointSetPos) == EEPROM_True ) {
      NavValue.Waypoints[i].Lat  = ReadMergeLong(Offset + EEPROM_LatPos);
      NavValue.Waypoints[i].NInd = LatLonAbsolute(&NavValue.Waypoints[i].Lat);

      NavValue.Waypoints[i].Lon  = ReadMergeLong(Offset + EEPROM_LonPos);
      NavValue.Waypoints[i].EInd = !LatLonAbsolute(&NavValue.Waypoints[i].Lon);

      MainState.WaypointsSet[i] = true;
    }
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

//
void ReadDetailEEPROM(NavData *Data, ElemStruct *State, byte Addr) {

  // 0-3 Lat, 4-7 Lon, 8-9 X, 10-11 Y, 12 LatLonVal, 13 XYVal 
  if( EEPROM.read(Addr + EEPROM_LatLonValPos) == EEPROM_True ) {
    Data->Lat  = ReadMergeLong(Addr + EEPROM_LatPos);
    Data->NInd = LatLonAbsolute(&Data->Lat);

    Data->Lon  = ReadMergeLong(Addr + EEPROM_LonPos);
    Data->EInd = !LatLonAbsolute(&Data->Lon);

    State->LatSet = true;
    State->LonSet = true;
  }

  if( Addr != EEPROM_Destination && EEPROM.read(Addr + EEPROM_XYValPos) == EEPROM_True ) {
    Data->X = ReadMergeInt(Addr + EEPROM_XPos);
    Data->Y = ReadMergeInt(Addr + EEPROM_YPos);

    State->XYSet  = true;
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Writes to EEPROM when a NavData is valid and different
void UpdateEEPROM(void *) {
  byte Offset;
  long ValLong;

  EEPROM.update(EEPROM_HourOffset,   NavValue.HourOffset); 
  EEPROM.update(EEPROM_MinuteOffset, NavValue.MinuteOffset); 

  UpdateDetailEEPROM(&NavValue.RefPointA,   &MainState.RefPointA,   EEPROM_RefPointA);
  UpdateDetailEEPROM(&NavValue.RefPointB,   &MainState.RefPointB,   EEPROM_RefPointB);
  UpdateDetailEEPROM(&NavValue.Destination, &MainState.Destination, EEPROM_Destination);

  for(byte i=0; i<10; i++) {
    Offset = EEPROM_Waypoints + (i * EEPROM_WaypointSize);

    // 0-3 Lat, 4-7 Lon, 8 WaypointSet
    if( MainState.WaypointsSet[i] ) {
      ValLong = AddLatLonSign( NavValue.Waypoints[i].Lat, &NavValue.Waypoints[i].NInd, true);
      UpdateSplitLong(ValLong, Offset + EEPROM_LatPos);

      ValLong = AddLatLonSign( NavValue.Waypoints[i].Lon, &NavValue.Waypoints[i].EInd, false);
      UpdateSplitLong(ValLong, Offset + EEPROM_LonPos);

      EEPROM.update(Offset + EEPROM_WaypointSetPos, EEPROM_True);
    } else {
      EEPROM.update(Offset + EEPROM_WaypointSetPos, EEPROM_False);
    }
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

//
void UpdateDetailEEPROM(NavData *Data, ElemStruct *State, byte Addr) {
  long ValLong;

  // 0-3 Lat, 4-7 Lon, 8-9 X, 10-11 Y, 12 LatLonVal, 13 XYVal
  if( State->LatSet && State->LonSet) {
    ValLong = AddLatLonSign( Data->Lat, &Data->NInd, true);
    UpdateSplitLong(ValLong, Addr + EEPROM_LatPos);

    ValLong = AddLatLonSign( Data->Lon, &Data->EInd, false);
    UpdateSplitLong(ValLong, Addr + EEPROM_LonPos);

    EEPROM.update(Addr + EEPROM_LatLonValPos, EEPROM_True);
  } else {
    EEPROM.update(Addr + EEPROM_LatLonValPos, EEPROM_False);
  }
  if( Addr != EEPROM_Destination && State->XYSet ) {
    UpdateSplitInt(Data->X, Addr + EEPROM_XPos);
    UpdateSplitInt(Data->Y, Addr + EEPROM_YPos);

    EEPROM.update(Addr + EEPROM_XYValPos, EEPROM_True);
  } else {
    EEPROM.update(Addr + EEPROM_XYValPos, EEPROM_False);
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Write a 32bits long to four 8bits bytes to consecutive EEPROM addrs
void UpdateSplitLong(long Val, byte ID) {
  EEPROM.update(ID,   (Val & 0xff000000) >> 24); 
  EEPROM.update(ID+1, (Val & 0x00ff0000) >> 16); 
  EEPROM.update(ID+2, (Val & 0x0000ff00) >>  8); 
  EEPROM.update(ID+3, (Val & 0x000000ff)      ); 
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Read four 8bits bytes from consecutive EEPROM addrs and return a single 32bits long
long ReadMergeLong(byte ID) {
  long   Val  = (long)EEPROM.read(ID)   << 24;
         Val += (long)EEPROM.read(ID+1) << 16;
         Val += (long)EEPROM.read(ID+2) << 8 ;
  return Val +  (long)EEPROM.read(ID+3)      ;
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Write a 16bits int to two 8bits bytes to consecutive EEPROM addrs
void UpdateSplitInt(int Val, byte ID) {
  EEPROM.update(ID  , (Val & 0xff00) >>  8); 
  EEPROM.update(ID+1, (Val & 0x00ff)      ); 
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Read two 8bits bytes from consecutive EEPROM addrs and return a single 16bits int
int ReadMergeInt(byte ID) {
  int    Val = EEPROM.read(ID)   << 8;
  return Val + EEPROM.read(ID+1)     ;
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Set to default all the NavData values and some internal state values as well as overwiting everyting in the EEPROM 
void ResetParam(void *) {
  ElemStruct *TargetElem;
  NavData    *TargetNav;
  byte i;

  ShowText(LCDRowHi, Loading_Msg);
  RefreshLCD(NULL);

  MainState.CursorValid = false;
  NavValue.HourOffset   = 0;
  NavValue.MinuteOffset = 0;
 
  for(i=0; i<5; i++) {
    switch(i) { 
      case  0:  TargetElem = &MainState.RefPointA;
                TargetNav  = &NavValue.RefPointA;
                break;
      case  1:  TargetElem = &MainState.RefPointB;
                TargetNav  = &NavValue.RefPointB;
                break;
      case  2:  TargetElem = &MainState.Destination;
                TargetNav  = &NavValue.Destination;
                break;
      case  3:  TargetElem = &MainState.Pointer;
                TargetNav  = &NavValue.Pointer;
                break;
      case  4:  TargetNav  = NavValue.Position;
                break;
    }

    TargetElem->LatSet = false;
    TargetElem->LonSet = false;
    TargetElem->XYSet  = false;  
    TargetNav->Lat   = 0;
    TargetNav->Lon   = 0;
    TargetNav->Dist  = 0;
    TargetNav->NInd  = true;
    TargetNav->EInd  = false;
    TargetNav->X     = 0;
    TargetNav->Y     = 0;
    TargetNav->Angle = 0;
  }

  for(i=0; i<=EEPROM_NbMaxAddrUsed; i++)
    EEPROM.update(i, 0);
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

//Heavily stripped down version of https://www.arduino.cc/reference/en/libraries/arduino-timer/

// Sets a task at position Slot to be executed every Delay miliseconds
inline void SetTask(byte Slot, unsigned int Delay, Task_Handler Handler) {
  struct Task *Curr = &TaskList[Slot];

  Curr->Handler = Handler;
  Curr->LastRun = millis();
  Curr->Delay   = Delay;
}

// Run and check tasks according to their timers
unsigned int HandleTask() {
  RunTask();
  return CheckTask();
}

// Run all tasks that are due according to their timers
void RunTask() {
  for (struct Task *Curr = TaskList; Curr < TaskEnd; ++Curr){
    //if (Curr->Handler) {
      const unsigned int Time = millis(); //ulong for longer than ~65 seconds

      if (Time - Curr->LastRun >= Curr->Delay) {
        Curr->Handler();
        Curr->LastRun = Time; // += Curr->Delay; // TBD if "no drift" solution is usefull
      }
    //}
  }
}

// Check the next task to schedule
unsigned int CheckTask() {
  unsigned int Wait = 0xFFFF;
  const unsigned int Start = millis(); //ulong for longer than ~65 seconds

  for (const struct Task *Curr = TaskList; Curr < TaskEnd; ++Curr){
    //if (Curr->Handler) {
      const unsigned int Delta = Start - Curr->LastRun;

      if (Delta >= Curr->Delay)
        return 0;
	
      const unsigned int Remaining = Curr->Delay - Delta;
      if (Remaining < Wait)
        Wait = Remaining;
    //}
  }

  return Wait;
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

//Heavily stripped down version of https://www.arduino.cc/reference/en/libraries/keypad/

// Returns a single key only
char GetCurrKey() {
  if (GetKeysPressed() && KeysPressed[0].StateChanged && (KeysPressed[0].State==PRESSED))
    return KeysPressed[0].Val;

  return NO_KEY;
}

// Populate the key list
bool GetKeysPressed() {
  ScanKeys();
  return UpdateList();
}

// Hardware scan, KeyBitMap stores ALL the keys that are being pressed
void ScanKeys() {
  byte Mask = 0b00000001;
  
  for (byte c=0; c<COLUMN_NUM; c++) {
    // Begin column pulse output
    pinMode(PinColumn[c], OUTPUT);
    digitalWrite(PinColumn[c], LOW);

    for (byte r=0; r<ROW_NUM; r++) {  
      // keypress is active low
      if (digitalRead(PinRows[r]) == LOW)
          KeyBitMap[r] |= Mask;
      else
          KeyBitMap[r] &= ~Mask;
    }
  
    // Set pin to high impedance input, effectively ends column pulse
    digitalWrite(PinColumn[c], HIGH);
    pinMode(PinColumn[c], INPUT);
    Mask <<= 1;
  }
}

// Manage the list without rearranging the keys. Returns true if any keys on the list changed state
bool UpdateList() {
  // Delete any IDLE keys
  for (byte i=0; i<LIST_MAX; i++) {
    if (KeysPressed[i].State == IDLE) {
      KeysPressed[i].Val  = NO_KEY;
      KeysPressed[i].Code = NOT_FOUND;
      KeysPressed[i].StateChanged = false;
    }
  }

  // Add new keys to empty slots in the key list
  for (byte r=0; r<ROW_NUM; r++) {
    byte Mask = 0b00000001;
    for (byte c=0; c<COLUMN_NUM; c++) {
      bool IsPressed = KeyBitMap[r] & Mask;
      byte KeyCode = r * COLUMN_NUM + c;
      byte Idx = FindInList(KeyCode);
      // Key is already on the list so set its next state
      if (Idx != NOT_FOUND)
        NextKeyState(Idx, IsPressed);
      // Key is NOT on the list so add it
      else if (IsPressed) {
        for (byte i=0; i<LIST_MAX; i++) {
          // Find an empty slot or don't add key to list
          if (KeysPressed[i].Val == NO_KEY) {
            KeysPressed[i].Val   = ((char*)KeypadLayout)[KeyCode];
            KeysPressed[i].Code  = KeyCode;
            KeysPressed[i].State = IDLE;   // Keys NOT on the list have an initial state of IDLE
            NextKeyState (i, IsPressed);
            break;
          }
        }
      }
      Mask <<= 1;
    }
  }

  // Report if the user changed the state of any key.
  for (byte i=0; i<LIST_MAX; i++) {
    if (KeysPressed[i].StateChanged)
      return true;
  }
  return false;
}

// Search by code for a key in the list of active keys
// Returns 255 if not found or the index into the list of active keys
byte FindInList(byte Code) {
  for (byte i=0; i<LIST_MAX; i++) {
    if (KeysPressed[i].Code == Code)
      return i;
  }
  return NOT_FOUND;
}

// This function is a state machine but is also used for debouncing the keys
void NextKeyState(byte Idx, bool IsPressed) {
  KeysPressed[Idx].StateChanged = false;

  switch (KeysPressed[Idx].State) {
    case IDLE:
        if (IsPressed) {
          TransitionTo (Idx, PRESSED);
          // Get ready for next HOLD state
          KeyHoldTimer = millis();
          //KeysPressed[Idx].HoldTimer = millis();
        }   
      break;
    case PRESSED:  
        if (IsPressed) {
          // Waiting for a key HOLD...
          if ((millis()-KeyHoldTimer) > KeyHoldDelay) //KeysPressed[Idx].HoldTimer
            TransitionTo(Idx, HOLD);
        } else 
          // or for a key to be RELEASED
          TransitionTo (Idx, RELEASED);
      break;
    case HOLD:
        if (!IsPressed)
          TransitionTo(Idx, RELEASED);
      break;
    case RELEASED:
        TransitionTo(Idx, IDLE);
      break;
  }
}

void TransitionTo(byte Idx, KeyStates NextState) {
  KeysPressed[Idx].State        = NextState;
  KeysPressed[Idx].StateChanged = true;

  // Keypad event listener could be here
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####






