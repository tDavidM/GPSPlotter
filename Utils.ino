
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

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

void DisplayLat(long Lat, bool NInd, bool Precision) {
  NInd ?  ToggleBox(Box_N)     : ToggleBox(Box_S);
  NInd ?  ToggleCross(Cross_N) : ToggleCross(Cross_S);

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

void DisplayLon(long Lon, bool EInd, bool Precision) {
  EInd ?  ToggleBox(Box_E)     : ToggleBox(Box_W);
  EInd ?  ToggleCross(Cross_E) : ToggleCross(Cross_W);      

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

void SetInterface(void *) {
  MainState.ShowDist      = MainState.CurrentMode != 'H';  // Dist not used in mode H
  MainState.ShowLat       = MainState.CurrentMode == 'H';
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

void DisplayDate(void *) {
  switch(NavValue.Date[0]) {
    case  '0':  ToggleBox(Box_0);  ToggleCross(Cross_0);  break;
    case  '1':  ToggleBox(Box_1);  ToggleCross(Cross_1);  break;
    case  '2':  ToggleBox(Box_2);  ToggleCross(Cross_2);  break;
    case  '3':  ToggleBox(Box_3);  ToggleCross(Cross_3);  break;
    case  '4':  ToggleBox(Box_4);  ToggleCross(Cross_4);  break;
    case  '5':  ToggleBox(Box_5);  ToggleCross(Cross_5);  break;
    case  '6':  ToggleBox(Box_6);  ToggleCross(Cross_6);  break;
    case  '7':  ToggleBox(Box_7);  ToggleCross(Cross_7);  break;
    case  '8':  ToggleBox(Box_8);  ToggleCross(Cross_8);  break;
    case  '9':  ToggleBox(Box_9);  ToggleCross(Cross_9);  break;
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

// ShowInt does not erase value before the offset but erase the rest of the row after
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

// ShowInt does not erase value before the offset but erase the rest of the row after
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

// ShowText expect padding before for aligment, and erase the rest of the row if shorter than 7
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

long AddLatLonSign( long Val, bool *Ind, bool LatMode) {
  if (LatMode)
    return Ind ? Val    : Val*-1;
  else
    return Ind ? Val*-1 : Val;
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

long ConvertStringToNum(char *Val, float *Factor) {
  float ConvVal;

  ConvVal = atof(Val);
  ConvVal *= *Factor;
  return round(ConvVal);
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

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

void UpdateSplitLong(long Val, int ID) {
  EEPROM.update(ID,   (Val & 0xff000000) >> 24); 
  EEPROM.update(ID+1, (Val & 0x00ff0000) >> 16); 
  EEPROM.update(ID+2, (Val & 0x0000ff00) >>  8); 
  EEPROM.update(ID+3, (Val & 0x000000ff)      ); 
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

long ReadMergeLong(int ID) {
  long   Val  = (long)EEPROM.read(ID)   << 24;
         Val += (long)EEPROM.read(ID+1) << 16;
         Val += (long)EEPROM.read(ID+2) << 8 ;
  return Val +  (long)EEPROM.read(ID+3)      ;
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

void UpdateSplitInt(int Val, int ID) {
  EEPROM.update(ID  , (Val & 0xff00) >>  8); 
  EEPROM.update(ID+1, (Val & 0x00ff)      ); 
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

int ReadMergeInt(int ID) {
  int    Val = EEPROM.read(ID)   << 8;
  return Val + EEPROM.read(ID+1)     ;
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

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


