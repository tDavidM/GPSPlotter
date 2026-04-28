
// All the logic for the interface is handled here

// Blinking toggles the value at the current cursor position with _ every half second
// MainInterface is the entry point for everything related to the interface and keypress processing
// GenericInterface handles the Map, Dest, RefPoint and Position modes keypress
// HeadingInterface handles the Heading mode keypress
// HeadingRefresh handles displaying when in Heading mode
// GenericRefresh handles displaying when in Map, Dist, RefPoint or Position modes
// Inputing handles keypress when a numerical value is being inputed regardless of the underlying mode (used for Lat/Lon/X/Y/Offset values)
// Editing handles keypress for setting the N/S or E/W indicator after a Lat or Lon value has been inputed (not used for X/Y/Offset values)
// SaveValue saves the inputed/edited lat or lon value and indicator to the appropriate NAvData after inputing and editing is done
// Updating handles keypress when the XY plotter position is changed 

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Toggle the value at the current cursor position with _ every half second
void Blinking(void *) { 
  // If Inputing, blink the SET segment and the cursor at the correct position
  if(MainState.Inputing > 0) {
    ToggleInd(LCDDecimalDot,  false);
    ToggleInd(LCDColonDotHi,  false);
    ToggleInd(LCDColonDotLow, false);
    ToggleInd(LCDIndRange,    false);
    ToggleInd(LCDIndKmHi,     false);

    ToggleInd(LCDIndSet, CurState);
    if(MainState.InputPos<7) {
      if(CurState) 
        ToggleNumberVal(LCDRowHi, MainState.InputPos, '_');
      else
        ToggleNumberVal(LCDRowHi, MainState.InputPos, MainState.InputBuffer[MainState.InputPos]);
    }
  // If Editing, blink the SET segment and the cursor in the box
  } else if(MainState.Editing) {
    ToggleInd(LCDIndSet, CurState);
    if(CurState) {
      ClearBoxStar(NULL);
      ToggleBox(Box_Bar);
    } else {
      if(MainState.IndQuadrant == Q_NORTH) {
        ToggleBox(Box_N);
        ToggleStar(Star_N);
      } else if(MainState.IndQuadrant == Q_SOUTH){
        ToggleBox(Box_S);
        ToggleStar(Star_S);
      } else if(MainState.IndQuadrant == Q_EAST){
        ToggleBox(Box_E); 
        ToggleStar(Star_E);
      } else if(MainState.IndQuadrant == Q_WEST){
        ToggleBox(Box_W);
        ToggleStar(Star_W);  
      }
    } 
  // If Updating, blink the UPDATE segment
  } else if (MainState.Updating) {
    //ClearAllInd(NULL);
    ToggleInd(LCDDecimalDot,  false);
    ToggleInd(LCDColonDotHi,  false);
    ToggleInd(LCDColonDotLow, false);
    ToggleInd(LCDIndRange,    false);
    ToggleInd(LCDIndKmHi,     false);

    ToggleInd(LCDPlusHi,    true);
    ToggleInd(LCDIndUpdate, CurState);
  // Otherwise, just refresh the display
  }/* else if(MainState.CurrentMode == M_HEADING)
    HeadingRefresh(NULL);
  else if(MainState.CurrentMode == M_MAP)
    GenericRefresh(&NavValue.Pointer, LCDIndGrid);
  else if(MainState.CurrentMode == M_DESTINATION)
    GenericRefresh(&NavValue.Destination, LCDIndDest);
  else if(MainState.CurrentMode == M_POSITION)
    GenericRefresh(NavValue.Position, LCDIndPosition);
  else if(MainState.CurrentMode == M_REFPOINTA)
    GenericRefresh(&NavValue.RefPointA, LCDIndRefPoint);
  else if(MainState.CurrentMode == M_REFPOINTB)
    GenericRefresh(&NavValue.RefPointB, LCDIndRefPoint);*/

  // If no reference point set (slow LED blink)
  if (!MainState.RefPointA.LatSet  || !MainState.RefPointA.LonSet ||
      !MainState.RefPointB.LatSet  || !MainState.RefPointB.LonSet ||
      !MainState.RefPointA.XYSet   || !MainState.RefPointB.XYSet  ) {
    ToggleInd(LCDIndQuest, CurState);
    MainState.CursorValid = false;
    digitalWrite(LedPin, CurState);
  } else {
    ToggleInd(LCDIndQuest, false);
    MainState.CursorValid = true;
    digitalWrite(LedPin, LOW);
  }

  // If the GPS fix is not valid or messages no longer received (solid LED)
  if( !MainState.CurrentValid || LastMessageTTL == 10)
    digitalWrite(LedPin, HIGH);

  if ( StepMotorVal.Calibrating ) {
    ToggleInd(LCDIndTest, CurState);
    //ToggleInd(LCDIndNAlign, CurState);
  }

  // If within ~2mm of the destination on the map (take scale into account) (fast LED blink)
  if( NavValue.NearZone ) {
    FastBlink = !FastBlink;
    digitalWrite(LedPin, FastBlink);
    ToggleInd(LCDIndZone, CurState);
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Entry point for everything related to the interface and keypress processing
void MainInterface(KeypadKeys* Key) {
  //Translate Key if not Inputing 
  if(MainState.Inputing == 0) {
    switch(*Key) { 
      case  KEY_1:  *Key=KEY_SELECT; break;
      case  KEY_2:  *Key=KEY_UP;     break;
      case  KEY_3:  *Key=KEY_MAP;    break;
      case  KEY_4:  *Key=KEY_LEFT;   break;
      case  KEY_5:  *Key=KEY_UPDATE; break;
      case  KEY_6:  *Key=KEY_RIGHT;  break;
      case  KEY_7:  *Key=KEY_DEST;   break;
      case  KEY_8:  *Key=KEY_DOWN;   break;
      case  KEY_9:  *Key=KEY_PRECIS; break;
      case  KEY_0:  *Key=KEY_INPUT;  break;
    }
  } else {
    //Inputing is when numbers are typed
    Inputing(Key);
  }

  if(MainState.Editing) {
    //Editing comes after editing, it's for sign or N/S - E/W ind
    Editing(Key);
  }

  if(MainState.Updating) {
    //When the arrow keys are used to change the cursor position
    Updating(Key);
  }
 
  if(*Key == KEY_HEADING) {
    MainState.CurrentMode = M_HEADING;
    SetInterface(NULL);
    HeadingRefresh(NULL);
  } else if(*Key == KEY_MAP || (*Key == KEY_CLEAR && MainState.CurrentMode == M_MAP)) {
    MainState.CurrentMode = M_MAP;
    SetInterface(NULL);
    GenericRefresh(&NavValue.Pointer, LCDIndGrid);
  } else if(*Key == KEY_DEST || (*Key == KEY_CLEAR && MainState.CurrentMode == M_DESTINATION)) {
    MainState.CurrentMode = M_DESTINATION;
    SetInterface(NULL);
    GenericRefresh(&NavValue.Destination, LCDIndDest);
  } else if(*Key == KEY_POSITION || (*Key == KEY_CLEAR && MainState.CurrentMode == M_POSITION)) {
    MainState.CurrentMode = M_POSITION;
    SetInterface(NULL);
    GenericRefresh(NavValue.Position, LCDIndPosition);
  } else if(*Key == KEY_REFPOINT || (*Key == KEY_CLEAR && (MainState.CurrentMode == M_REFPOINTA || MainState.CurrentMode == M_REFPOINTB))) {
    if( MainState.CurrentMode == M_REFPOINTA && *Key == KEY_REFPOINT )
      MainState.CurrentMode = M_REFPOINTB;
    else
      MainState.CurrentMode = M_REFPOINTA;
    SetInterface(NULL);  

    if( MainState.CurrentMode == M_REFPOINTA )
      GenericRefresh(&NavValue.RefPointA, LCDIndRefPoint);
    else
      GenericRefresh(&NavValue.RefPointB, LCDIndRefPoint);
  }

  if(MainState.CurrentMode == M_HEADING)
    HeadingInterface(Key);
  else if(MainState.CurrentMode == M_DESTINATION)
    GenericInterface(Key, &NavValue.Destination);
  else if(MainState.CurrentMode == M_MAP)
    GenericInterface(Key, &NavValue.Pointer);
  else if(MainState.CurrentMode == M_POSITION)
    GenericInterface(Key, NavValue.Position);
  else if(MainState.CurrentMode == M_REFPOINTA)
    GenericInterface(Key, &NavValue.RefPointA);
  else if(MainState.CurrentMode == M_REFPOINTB)
    GenericInterface(Key, &NavValue.RefPointB);
  else {
    ToggleInd(LCDIndFault, true);
    //MainState.CurrentMode = M_HEADING;
  }

};

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Handle the Map, Dest, RefPoint and Position modes keypress
void GenericInterface(KeypadKeys* Key, NavData *Data) {
  MainState.Resetting    = 0;
  MainState.Recalibrate = 0;

  if(*Key == KEY_SELECT) {
    //Switch to next state 
    if(MainState.ShowDist) {
      MainState.ShowDist = false;
      MainState.ShowLat  = true;
    } else if(MainState.ShowLat) {
      MainState.ShowLat  = false;
      MainState.ShowLon  = true;
      MainState.ShowPrecision = false;
    } else if(MainState.ShowLon) {
      MainState.ShowLon  = false;
      MainState.ShowDist = true;
      MainState.ShowPrecision = false;
    }

  } else if(*Key == KEY_ENTER) {
    if(MainState.CurrentMode != M_POSITION) {
      MainState.Updating = true;
      MainState.CursorStep = STEP_SizeCorse;
      MainState.CursorX = Data->X;
      MainState.CursorY = Data->Y;
      GoToXY(Data->X, Data->Y);
      MainState.IndDirX = true;

      ShowInt(LCDRowHi, MainState.CursorX, 1); 
      ShowText(LCDRowLow, Corse_Msg);
      ClearBoxStar(NULL);
      ToggleStar(Star_X);
    } else {
      SavePosition(NULL);
    }

  } else if(*Key == KEY_PRECIS && ( MainState.ShowLat || MainState.ShowLon ) ) {
    MainState.ShowPrecision = !MainState.ShowPrecision;

  } else if(*Key == KEY_UPDATE && MainState.CursorValid) {
    GoToXY(Data->X, Data->Y);

  } else if(*Key == KEY_CLEAR ) {
    if( MainState.MovingCur )
      GoToXY(StepMotorVal.CurrentX, StepMotorVal.CurrentY);
    else if( MainState.ShowPrecision )
      MainState.ShowPrecision = false;
    else if(MainState.CurrentMode == M_POSITION)
      ClearPosition(NULL);

  } else if(*Key == KEY_INPUT ) {
    if(MainState.CurrentMode != M_POSITION && ( MainState.ShowLat || MainState.ShowLon )) {
      MainState.Inputing = 1;
      MainState.Editing  = true;
      MainState.InputMax = 7;
      if(MainState.ShowLat) {
        //ShowText(LCDRowHi, " 000000");  
        MainState.InputPos = 1;
        /*if(MainState.CurrentMode == M_REFPOINTB && MainState.CurrentValid) {
          NavValue.Current.NInd ? MainState.IndQuadrant = Q_NORTH : MainState.IndQuadrant = Q_SOUTH;
          SetInputBuf(NavValue.Current.Lat, false);
          DisplayLat(NavValue.Current.Lat, NavValue.Current.NInd, true);
        } else {*/
          Data->NInd ? MainState.IndQuadrant = Q_NORTH : MainState.IndQuadrant = Q_SOUTH;
          SetInputBuf(Data->Lat, false);
          DisplayLat(Data->Lat, Data->NInd, true);
        //}
        ShowText(LCDRowLow, Lat_Msg);  
  
      } else {
        //ShowText(LCDRowHi, "0000000");
        MainState.InputPos = 0;
        /*if(MainState.CurrentMode == M_REFPOINTB && MainState.CurrentValid) {
          NavValue.Current.EInd ? MainState.IndQuadrant = Q_EAST : MainState.IndQuadrant = Q_WEST;
          SetInputBuf(NavValue.Current.Lon, true);
          DisplayLon(NavValue.Current.Lon, NavValue.Current.EInd, true);
        } else {*/
          Data->EInd ? MainState.IndQuadrant = Q_EAST : MainState.IndQuadrant = Q_WEST;
          SetInputBuf(Data->Lon, true);
          DisplayLon(Data->Lon, Data->EInd, true);
        //}
        ShowText(LCDRowLow, Lon_Msg);
      }
    } else if(MainState.CurrentMode == M_POSITION)  {
      MainState.Inputing = 1;
      MainState.Editing  = false;
      MainState.InputMax = 0;
      MainState.InputPos = 0;
      InitInputBuf(NULL);
      MainState.InputBuffer[0] = MainState.CurrentWaypoint + '0';
      ToggleNumberVal(LCDRowHi, 0, MainState.CurrentWaypoint);
      ShowText(LCDRowLow, Point_Msg);  

      ClearBoxStar(NULL);
    }
  } else if(*Key == KEY_BACK) { //Back from inputing
      MainState.InputPos = 0;
      MainState.CurrentWaypoint = MainState.InputBuffer[0] - '0';
      NavValue.Position = &NavValue.Waypoints[MainState.CurrentWaypoint];
      ComputeNewXY(M_POSITION);
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Handle the Heading mode keypress
void HeadingInterface(KeypadKeys* Key) {
  char LocalBuffer[3];
  byte HourOffsetDigit;

  if(*Key == KEY_SELECT) {
    //Switch to next state 
    if( MainState.Resetting > 0 ) {
      if( MainState.Resetting == 1 )
        MainState.Resetting = 2;
      else
        MainState.Resetting = 0;
    } else if(MainState.ShowLat) {
      MainState.ShowLat  = false;
      MainState.ShowLon  = true;
      MainState.ShowPrecision = false;
    } else if(MainState.ShowLon) {
      MainState.ShowLon       = false;
      MainState.ShowSpeed     = true;
      MainState.ShowPrecision = false;
    } else if (MainState.ShowSpeed) {
      MainState.ShowSpeed    = false;
      MainState.ShowAltitude = true;
    } else if (MainState.ShowAltitude) {
      MainState.ShowAltitude = false;
      MainState.ShowScale    = true;
    } else if (MainState.ShowScale) {
      MainState.ShowScale = false;
      MainState.ShowDate  = true;
    } else if (MainState.ShowDate) {
      MainState.ShowDate = false;
      MainState.ShowLat  = true;
    }
    MainState.Recalibrate = 0;

  } else if(*Key == KEY_PRECIS) {
    if( MainState.ShowLat || MainState.ShowLon ) {
      MainState.ShowPrecision = !MainState.ShowPrecision;
    } else if( MainState.Recalibrate == 4 ) {
      CalibrateXY(NULL);
      MainState.Recalibrate = 0;
    } else {
      MainState.Recalibrate++;
    }
  } else if(*Key == KEY_UPDATE && MainState.CursorValid) {
    GoToXY(NavValue.Current.X, NavValue.Current.Y);
    MainState.Resetting    = 0;
    MainState.Recalibrate = 0;

  } else if(*Key == KEY_INPUT /*&& MainState.ShowDate*/ ) {
    MainState.Resetting    = 0;
    MainState.Recalibrate = 0;
    MainState.Inputing = 1;
    MainState.Editing  = false;
    MainState.InputMax = 5;
    MainState.InputPos = 2;
    InitInputBuf(NULL);

    ShowText(LCDRowHi, "  0000"); // 
    HourOffsetDigit = NavValue.HourOffset;
    if( NavValue.HourOffset > 9 ) {
      HourOffsetDigit -= 10;
      ShowInt(LCDRowHi, NavValue.HourOffset,   2);
       if( NavValue.HourOffset > 19 ) {
          HourOffsetDigit -= 10;
          MainState.InputBuffer[2]='2';
        } else {
         MainState.InputBuffer[2]='1';
        }
    } else {
      ShowInt(LCDRowHi, NavValue.HourOffset,   3) ;
    }
    MainState.InputBuffer[3] = HourOffsetDigit + '0';
    NavValue.MinuteOffset>9 ? ShowInt(LCDRowHi, NavValue.MinuteOffset, 4) : ShowInt(LCDRowHi, NavValue.MinuteOffset, 5);
    
    ShowText(LCDRowLow, Offset_Msg);  
    ClearBoxStar(NULL);
  } else if(*Key == KEY_BACK) { //Back from inputing
    //MainState.InputBuffer[6] = 0x00;
    MainState.InputPos = 0;
    
    LocalBuffer[0] = MainState.InputBuffer[2];
    LocalBuffer[1] = MainState.InputBuffer[3];
    LocalBuffer[2] = 0x00;
    NavValue.HourOffset = atoi(LocalBuffer);

    LocalBuffer[0] = MainState.InputBuffer[4];
    LocalBuffer[1] = MainState.InputBuffer[5];
    NavValue.MinuteOffset = atoi(LocalBuffer);

  } else if(*Key == KEY_ENTER) {
    MainState.Recalibrate = 0;
    if( MainState.Resetting == 0 ){
      MainState.Resetting = 1;
    } else if( MainState.Resetting == 1 ){
      MainState.Resetting = 0;
    } else if( MainState.Resetting == 2 ) {
      ResetParam(NULL);
      MainState.Resetting = 0;
    } else{
      MainState.Resetting = 0;
    }
  } else if(*Key == KEY_CLEAR) {
    MainState.Resetting    = 0;
    MainState.Recalibrate = 0;
    if( StepMotorVal.Calibrating ) {
      StepMotorVal.CurrentX = 0;
      StepMotorVal.CurrentY = 0;
    }
    if( MainState.MovingCur )
      GoToXY(StepMotorVal.CurrentX, StepMotorVal.CurrentY);
  } 
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Handle displaying when in Heading mode
void HeadingRefresh(void *) {
  ClearBoxStar(NULL);
  ClearAllInd(NULL);
  ClearRing(NULL);

  if( MainState.Resetting > 0 ) {
    ShowText(LCDRowHi, Reset_Msg);
    if( MainState.Resetting == 1 )
      ShowText(LCDRowLow, Press1_Msg);
    else if( MainState.Resetting == 2 )
      ShowText(LCDRowLow, PressE_Msg);
  } else {
    DisplayTime(NULL);
    ToggleRing(NavValue.Current.Angle, true);
    ToggleInd(LCDIndHeading, true);

    if(MainState.ShowLat)
      DisplayLat(NavValue.Current.Lat, NavValue.Current.NInd, MainState.ShowPrecision);
    else if(MainState.ShowLon) 
      DisplayLon(NavValue.Current.Lon, NavValue.Current.EInd, MainState.ShowPrecision);
    else if(MainState.ShowAltitude) {
      ToggleInd(LCDIndAlt,  true); 
      DisplayDist(NavValue.Altitude);
    } else if(MainState.ShowSpeed) {
      ToggleStar(Star_K);
      ToggleNumberVal(LCDRowHi, 0, 'n');
      ToggleNumberVal(LCDRowHi, 1, 'h');
      ToggleNumberVal(LCDRowHi, 2, ' ');
      ShowInt(LCDRowHi, NavValue.Speed, 3);
    } else if(MainState.ShowDate)
      DisplayDate(NULL);
    else if(MainState.ShowScale) {
      ToggleInd(LCDIndScale,  true); 
      ShowText(LCDRowHi, MaskBlank_Msg);
      if( NavValue.Scale > 9999999 ) {
        ShowText(LCDRowHi, TooLarge_Msg);
      } else if ( NavValue.Scale > 999999 ) {
        ShowInt(LCDRowHi, &NavValue.Scale, 0);
      } else
        ShowInt(LCDRowHi, &NavValue.Scale, 1);
    }
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Handle displaying when in Map, Dist, RefPoint or Position modes
void GenericRefresh(NavData *Data, byte Ind) {
  ClearBoxStar(NULL);
  ClearAllInd(NULL);  
  ToggleInd(Ind, true);

  DisplayTime(NULL);
  if(MainState.CurrentMode == M_REFPOINTA)
    ToggleNumberVal(LCDRowLow, 0, '1'); // 'A'
  else if (MainState.CurrentMode == M_REFPOINTB)
    ToggleNumberVal(LCDRowLow, 0, '2'); // 'b'
  else if (MainState.CurrentMode == M_POSITION)
    ToggleNumberVal(LCDRowLow, 0,  MainState.CurrentWaypoint + '0'); // 0-9

  ClearRing(NULL);
  ToggleRing(Data->Angle, true);

  if(MainState.ShowDist) {
    DisplayDist(Data->Dist);
    ToggleInd(LCDIndRange, true);
  } else if(MainState.ShowLat)
    DisplayLat(Data->Lat, Data->NInd, MainState.ShowPrecision);
  else if(MainState.ShowLon) 
    DisplayLon(Data->Lon, Data->EInd, MainState.ShowPrecision);
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Handle keypress when a numerical value is being inputed regardless of the underlying mode (used for Lat/Lon/X/Y/Offset values)
void Inputing(KeypadKeys* Key) {
  if (*Key==KEY_ENTER) {
    MainState.Inputing--;
    if(MainState.InputPos < 7)
      ToggleNumberVal(LCDRowHi, MainState.InputPos, MainState.InputBuffer[MainState.InputPos]);
    *Key = KEY_BACK;
  //Pressing R, P or H while editing will cancel
  } else if( *Key==KEY_CLEAR || *Key == KEY_HEADING || *Key == KEY_POSITION || *Key == KEY_REFPOINT) {
    *Key==KEY_CLEAR ? MainState.Inputing-- : MainState.Inputing = 0;
    MainState.Editing  = false;
    MainState.InputPos = 0;
  } else if(MainState.InputPos <= MainState.InputMax) {
    MainState.InputBuffer[MainState.InputPos] = *Key;
    if(MainState.InputPos < 7) {
      ToggleNumberVal(LCDRowHi, MainState.InputPos, *Key); 
      if(MainState.InputPos < MainState.InputMax)
        MainState.InputPos++;
    }
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Handle keypress for setting the N/S or E/W indicator after a Lat or Lon value has been inputed (not used for X/Y/Offset values)
void Editing(KeypadKeys* Key) {
  if (*Key==KEY_ENTER) {
    SaveValue(NULL);
    MainState.Editing = false;
    MainState.InputPos = 0;
    *Key = KEY_NONE;
  //Pressing R, P or H while editing will cancel
  } else if( *Key==KEY_CLEAR || *Key == KEY_HEADING || *Key == KEY_MAP || *Key == KEY_DEST || *Key == KEY_POSITION || *Key == KEY_REFPOINT ) {
    MainState.Editing  = false;
    MainState.InputPos = 0;
  } else if(*Key==KEY_SELECT) {
    if(MainState.IndQuadrant == Q_NORTH) {
      MainState.IndQuadrant = Q_SOUTH;
      ToggleBox(Box_S);
      ToggleStar(Star_S);
    } else if(MainState.IndQuadrant == Q_SOUTH){
      MainState.IndQuadrant = Q_NORTH;
      ToggleBox(Box_N);
      ToggleStar(Star_N);
    } else if(MainState.IndQuadrant == Q_EAST){
      MainState.IndQuadrant = Q_WEST;
      ToggleBox(Box_W); 
      ToggleStar(Star_W);
    } else if(MainState.IndQuadrant == Q_WEST){
      MainState.IndQuadrant = Q_EAST;
      ToggleBox(Box_E);
      ToggleStar(Star_E);  
    }
    *Key = KEY_NONE;
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Save the inputed/edited lat or lon value and indicator to the appropriate NAvData after inputing and editing is done
void SaveValue(void *) {
  ElemStruct *TargetElem;
  NavData    *TargetNav;
  byte        TargetInd;

  MainState.InputBuffer[8] = 0x00;
  long NewVal = atol(MainState.InputBuffer);

  MainState.ShowLat ? NewVal=min(NewVal,9000000) : NewVal=min(NewVal,18000000);

  if(MainState.CurrentMode == M_DESTINATION) {
    TargetElem = &MainState.Destination;
    TargetNav  = &NavValue.Destination;
    TargetInd  = LCDIndDest;
  } else if(MainState.CurrentMode == M_MAP) {
    TargetElem = &MainState.Pointer;
    TargetNav  = &NavValue.Pointer;
    TargetInd  = LCDIndGrid;
  } else if(MainState.CurrentMode == M_REFPOINTA) {
    TargetElem = &MainState.RefPointA;
    TargetNav  = &NavValue.RefPointA;
    TargetInd  = LCDIndRefPoint;
  } else if(MainState.CurrentMode == M_REFPOINTB) {
    TargetElem = &MainState.RefPointB;
    TargetNav  = &NavValue.RefPointB;
    TargetInd  = LCDIndRefPoint;
  }

  if(MainState.ShowLat) {
    TargetNav->NInd    = MainState.IndQuadrant == Q_NORTH;
    TargetNav->Lat     = NewVal;
    TargetElem->LatSet = true;
    if(MainState.CurrentMode == M_DESTINATION || MainState.CurrentMode == M_MAP)
      TargetElem->XYSet = false;
  } else if(MainState.ShowLon) {
    TargetNav->EInd    = MainState.IndQuadrant == Q_EAST;
    TargetNav->Lon     = NewVal;
    TargetElem->LonSet = true;
    if(MainState.CurrentMode == M_DESTINATION || MainState.CurrentMode == M_MAP)
      TargetElem->XYSet = false;
  }

  if(MainState.CurrentMode == M_DESTINATION)
    ComputeNewXY(M_DESTINATION);
  else if(MainState.CurrentMode == M_MAP)
    ComputeNewXY(M_MAP);
  else if(MainState.CurrentMode == M_REFPOINTA || MainState.CurrentMode == M_REFPOINTB) {
    ComputeNewXY(M_DESTINATION);
    ComputeNewXY(M_MAP);
    ComputeNewXY(M_POSITION);
  }
  GenericRefresh(TargetNav, TargetInd);
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Handle keypress when the XY plotter position is changed 
void Updating(KeypadKeys* Key) {
  ToggleNumberVal(LCDRowHi, 0, ' '); 

  if(*Key == KEY_UP) {
    MainState.CursorY = max(MainState.CursorY-MainState.CursorStep, 0);    
    ShowInt(LCDRowHi, MainState.CursorY, 1);
    GoToXY(MainState.CursorX, MainState.CursorY);
    MainState.IndDirX = false;
    ToggleStar(Star_Y);

  } else if(*Key == KEY_DOWN) {
    MainState.CursorY = min(MainState.CursorY+MainState.CursorStep, STEP_GridSizeY);
    ShowInt(LCDRowHi, MainState.CursorY, 1);
    GoToXY(MainState.CursorX, MainState.CursorY);
    MainState.IndDirX = false;
    ToggleStar(Star_Y);

  } else if(*Key == KEY_LEFT) {
    MainState.CursorX = max(MainState.CursorX-MainState.CursorStep, 0);
    ShowInt(LCDRowHi, MainState.CursorX, 1);
    GoToXY(MainState.CursorX, MainState.CursorY);
    MainState.IndDirX = true;
    ToggleStar(Star_X);

  } else if(*Key == KEY_RIGHT) {
    MainState.CursorX = min(MainState.CursorX+MainState.CursorStep, STEP_GridSizeX);
    ShowInt(LCDRowHi, MainState.CursorX, 1);
    GoToXY(MainState.CursorX, MainState.CursorY);
    MainState.IndDirX = true;
    ToggleStar(Star_X);

  } else if(*Key == KEY_SELECT) {
    MainState.CursorStep == STEP_SizeCorse ? MainState.CursorStep = STEP_SizeInter  :
                                            MainState.CursorStep == STEP_SizeInter ? MainState.CursorStep = STEP_SizeFine :
                                                                                    MainState.CursorStep = STEP_SizeCorse;
    MainState.CursorStep == STEP_SizeCorse ? ShowText(LCDRowLow, Corse_Msg)                  :
                                            MainState.CursorStep == STEP_SizeInter ? ShowText(LCDRowLow, Inter_Msg):
                                                                                    ShowText(LCDRowLow, Fine_Msg);
    *Key = KEY_NONE;

  } else if (*Key == KEY_INPUT) {
    MainState.Inputing = 2;
    MainState.InputMax = 4;
    MainState.InputPos = 1;
    //SetInputBuf(MainState.IndDirX ? MainState.CursorX : MainState.CursorY , false);
    InitInputBuf(NULL);
    ShowText(LCDRowHi, " 0000  ");  
    *Key = KEY_NONE;

  } else if (*Key == KEY_BACK) { //Back from inputing
    MainState.InputBuffer[5] = 0x00;
    if (MainState.IndDirX) {
      MainState.CursorX = min(atoi(MainState.InputBuffer), STEP_GridSizeX);
      ShowInt(LCDRowHi, MainState.CursorX, 1);
    } else {
      MainState.CursorY = min(atoi(MainState.InputBuffer), STEP_GridSizeY);
      ShowInt(LCDRowHi, MainState.CursorY, 1);
    }
    GoToXY(MainState.CursorX, MainState.CursorY);
    MainState.InputPos = 0;
    MainState.Inputing = 0;
    *Key = KEY_NONE;

  } else if (*Key == KEY_CLEAR && MainState.Inputing == 1) { //Back from inputing
    MainState.IndDirX ? ShowInt(LCDRowHi, MainState.CursorX, 1) : ShowInt(LCDRowHi, MainState.CursorY, 1);
    MainState.InputPos = 0;
    MainState.Inputing = 0;
    *Key = KEY_NONE;

  } else if(*Key == KEY_ENTER) {
    if(MainState.CurrentMode == M_MAP) {
      NavValue.Pointer.X = MainState.CursorX;
      NavValue.Pointer.Y = MainState.CursorY;
      MainState.Pointer.XYSet  = true;
      MainState.Pointer.LatSet = false;
      MainState.Pointer.LonSet = false;
      ComputeNewGrid(M_MAP);
      GenericRefresh(&NavValue.Pointer, LCDIndGrid);

    } else if(MainState.CurrentMode == M_DESTINATION) {
      NavValue.Destination.X = MainState.CursorX;
      NavValue.Destination.Y = MainState.CursorY;
      MainState.Destination.XYSet  = true;
      MainState.Destination.LatSet = false;
      MainState.Destination.LonSet = false;
      ComputeNewGrid(M_DESTINATION);
      GenericRefresh(&NavValue.Destination, LCDIndDest);

    } else if(MainState.CurrentMode == M_REFPOINTA) {
      NavValue.RefPointA.X = MainState.CursorX;
      NavValue.RefPointA.Y = MainState.CursorY;
      MainState.RefPointA.XYSet = true;
      ComputeNewXY(M_MAP);
      ComputeNewXY(M_DESTINATION);
      ComputeNewXY(M_POSITION);
      GenericRefresh(&NavValue.RefPointA, LCDIndRefPoint);

    } else if(MainState.CurrentMode == M_REFPOINTB) {
      NavValue.RefPointB.X = MainState.CursorX;
      NavValue.RefPointB.Y = MainState.CursorY;
      MainState.RefPointB.XYSet = true;
      ComputeNewXY(M_MAP);
      ComputeNewXY(M_DESTINATION);
      ComputeNewXY(M_POSITION);
      GenericRefresh(&NavValue.RefPointB, LCDIndRefPoint);
    }

    *Key = KEY_NONE;
    MainState.CursorX = 0;
    MainState.CursorY = 0;    
    MainState.Updating = false;

  } else if( *Key==KEY_CLEAR || *Key == KEY_HEADING || *Key == KEY_MAP || *Key == KEY_DEST || *Key == KEY_POSITION || *Key == KEY_REFPOINT ) {
    MainState.Updating = false;
  }
}

