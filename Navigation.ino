
// All GPS related computations are done here

// Distance calculates how many meters there is between 2 coordinates (flat plane for ~<100Km and sphere for longer distances)
// Direction caluclates the angle from a coordinate to an other
// ProcessNMEAData parses the received Serial messages
// ProcessTime is called while parsing to calculate the time with offset from UTC time
// ComputeNewAngleDist is called several time per second an only runs when there's no Serial message to process
//   it then computes many distances and directions (round robin) as well as scaling factors, map ratio 
//   and XY coordinate when needed, it also keep the EEPROM updated
// ComputeRatio pre-computes the ratios used to calcute grid coordinate and XY values when needed
// ComputeNewGrid calculates new grid coordinates from XY values when needed
// LatLonScaling used by the previous to do computations
// ComputeScale calculates the map scale factor from XY plotter known size

long Distance(NavData *Src, NavData *Dest) {

  float DistMeter, Lat1F, Lon1F, Lat2F, Lon2F;
  long Lat1 = AddLatLonSign( Src->Lat,  &Src->NInd,  true);
  long Lon1 = AddLatLonSign( Src->Lon,  &Src->EInd,  false);
  long Lat2 = AddLatLonSign( Dest->Lat, &Dest->NInd, true);
  long Lon2 = AddLatLonSign( Dest->Lon, &Dest->EInd, false);
  float LatDelta = abs(Lat1-Lat2);
  float LonDelta = abs(Lon1-Lon2);

  //flat plane aproximation, it gives better results at short distances
  //  because of floating point precision errors
  LatDelta *= LatToMeter;
  LonDelta *= LatToMeter * cos( (((Lat1+Lat2)/LatLonConvFactorL)/2.0) * DEG_TO_RAD );
  DistMeter = sqrt( pow(LatDelta, 2) + pow(LonDelta, 2) );

  //beyond 100Km, spherical is better, and angles are less sensitive to floating point rounding
  //  this could be pushed to 500km and still be fairly good...
  if( DistMeter >= 100000 ) {

    Lat1F = DEG_TO_RAD * (Lat1 / LatLonConvFactorF);
    Lon1F = DEG_TO_RAD * (Lon1 / LatLonConvFactorF);
    Lat2F = DEG_TO_RAD * (Lat2 / LatLonConvFactorF);
    Lon2F = DEG_TO_RAD * (Lon2 / LatLonConvFactorF);

    DistMeter = EarthDiamInM *
                asin(sqrt( 0.5 -
                           cos((Lat2F-Lat1F)) / 2.0 +
                           cos(Lat1F) * cos(Lat2F) * (1.0-cos((Lon2F-Lon1F))) / 2.0 
                    )    );
    
  }
  return floor(DistMeter);
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

byte Direction(NavData *Src, NavData *Dest) {

  float Lat1 = AddLatLonSign( Src->Lat,  &Src->NInd,  true);
  float Lon1 = AddLatLonSign( Src->Lon,  &Src->EInd,  false);
  float Lat2 = AddLatLonSign( Dest->Lat, &Dest->NInd, true);
  float Lon2 = AddLatLonSign( Dest->Lon, &Dest->EInd, false);
  float LonDelta;
  float Result;

  Lat1     = ( Lat1         / LatLonConvFactorF) * DEG_TO_RAD;
  Lat2     = ( Lat2         / LatLonConvFactorF) * DEG_TO_RAD;
  LonDelta = ((Lon2 - Lon1) / LatLonConvFactorF) * DEG_TO_RAD;

  Result = atan2( sin(LonDelta) * cos(Lat2), 
                  cos(Lat1) * sin(Lat2) -
                  sin(Lat1) * cos(Lat2) * cos(LonDelta) );

  if (Result < 0.0)
    Result += TWO_PI;

  Result = Result * RAD_TO_DEG * DegreeToThirtyTwoTh;
  return round(Result);
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

void ProcessNMEAData(char *NEMAData, byte MsgType) {
  char *MsgTok, MsgElem[12];
  const char *Delim = ",";
  long  MinuteDegConv;
  static bool LocalCurrentValid = false;

  // Skip the first token that contains the message type, at this point it's already known
  MsgTok = strsep(&NEMAData, Delim);
  //Go with the second, Time
  MsgTok = strsep(&NEMAData, Delim);

  if( MsgTok != NULL ) {
    if( MsgType == MsgTypeGPGGA ) {

//       UTC time   Latitude    Longitude    Fix      Altitude M
//	                        IndNS        IndEW          Unit
//$GPGGA,hhmmss.sss,ddmm.mmmm,N,dddmm.mmmm,W,1,01,x.x,x.x,M,,,,0000*xx
//0123456789012345678901234567890123456789012345678901234567890123456789
//0         1         2         3         4         5         6

      // Move to the 3rd token, Latitude
      MsgTok = strsep(&NEMAData, Delim);

      // Only process messages once GPS fix is valid
      if( LocalCurrentValid ) {
        LastMessageTTL = 0;

        // LATITUDE, Deg Min.decimal to Deg.decimal conversion
        strncpy(MsgElem, MsgTok+2, 7);
        MsgElem[7] = 0x00;
        MinuteDegConv = ConvertStringToNum(MsgElem, &DegreMinToDec);

        strncpy(MsgElem, MsgTok, 2);
        MsgElem[2] = 0x00;
        NavValue.Current.Lat = ConvertStringToNum(MsgElem, &LatLonConvFactorF) + MinuteDegConv;
        // DONE

        // N/S indicator
        MsgTok = strsep(&NEMAData, Delim);
        NavValue.Current.NInd = MsgTok[0] == 'N';

        // LONGITUDE, Deg Min.decimal to Deg.decimal conversion
        MsgTok = strsep(&NEMAData, Delim);
        strncpy(MsgElem, MsgTok+3, 7);
        MsgElem[7] = 0x00;
        MinuteDegConv = ConvertStringToNum(MsgElem, &DegreMinToDec);

        strncpy(MsgElem, MsgTok, 3);
        MsgElem[3] = 0x00;
        NavValue.Current.Lon = ConvertStringToNum(MsgElem, &LatLonConvFactorF) + MinuteDegConv;
        // DONE

        // E/W indicator
        MsgTok = strsep(&NEMAData, Delim);
        NavValue.Current.EInd = MsgTok[0] == 'E';

        // FIX valid
        MsgTok = strsep(&NEMAData, Delim);
        NavValue.GPSFix = MsgTok[0] - '0';
        MainState.CurrentValid = NavValue.GPSFix > 0 && NavValue.Current.Lat != 0 && NavValue.Current.Lon != 0;

        // Unused
        MsgTok = strsep(&NEMAData, Delim);
        MsgTok = strsep(&NEMAData, Delim);

        // Altitude
        MsgTok = strsep(&NEMAData, Delim);
        strcpy(MsgElem, MsgTok);
        NavValue.Altitude = atol(MsgElem);

        // Altitude unit (must be Meter)
        MsgTok = strsep(&NEMAData, Delim);
        if( MsgTok[0] != 'M' )
          NavValue.Altitude = 0;
      }

    } else if( MsgType == MsgTypeGPRMC ) {

//       UTC time     Latitude   Longitude     Speed Direction
//	                Status      IndNS        IndEW         Date
//$GPRMC,hhmmss.sss,A,ddmm.mmmm,N,dddmm.mmmm,W,000.0,000.0,ddmmyy,,,A*xx
//0123456789012345678901234567890123456789012345678901234567890123456789
//0         1         2         3         4         5         6

      // Time
      ProcessTime(MsgTok);

      // Valid indicator
      MsgTok = strsep(&NEMAData, Delim);
      LocalCurrentValid = MsgTok[0] == 'A';

      // Only process messages once GPS fix is valid
      if( LocalCurrentValid ) {
        LastMessageTTL = 0;
        LocalCurrentValid = true;

        // Unused
        MsgTok = strsep(&NEMAData, Delim);
        MsgTok = strsep(&NEMAData, Delim);
        MsgTok = strsep(&NEMAData, Delim);
        MsgTok = strsep(&NEMAData, Delim);

        // Speed
        MsgTok = strsep(&NEMAData, Delim);
        NavValue.Speed = ConvertStringToNum(MsgTok, &KnotsToKmH);

        // Bearing
        MsgTok = strsep(&NEMAData, Delim);
        NavValue.Current.Angle = strlen(MsgTok)==0 ? 255 : ConvertStringToNum(MsgTok, &DegreeToThirtyTwoTh);

        // Date
        MsgTok = strsep(&NEMAData, Delim);
        NavValue.Date[0] = MsgTok[4];
        NavValue.Date[1] = MsgTok[5];
        NavValue.Date[2] = MsgTok[2];
        NavValue.Date[3] = MsgTok[3];
        NavValue.Date[4] = MsgTok[0];
        NavValue.Date[5] = MsgTok[1];
      }
    } //else if( MsgType == MsgTypeGPVTG ) {

    //}
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

void ProcessTime(char MsgTime[]) {
  char TimeElem[3];
  byte  Value, Offset = 0;

  TimeElem[0] = MsgTime[2];
  TimeElem[1] = MsgTime[3];
  TimeElem[2] = 0x00;

  Value = atoi(TimeElem);
  Value += NavValue.MinuteOffset;
  if(Value > 59) {
    Value -= 60;
    Offset = 1;
  }
  itoa(Value, TimeElem, 10);
  if(Value < 10) {
    NavValue.Time[2] = '0';
    NavValue.Time[3] = TimeElem[0];
  } else {
    NavValue.Time[2] = TimeElem[0];
    NavValue.Time[3] = TimeElem[1];
  }

  TimeElem[0] = MsgTime[0];
  TimeElem[1] = MsgTime[1];
  TimeElem[2] = 0x00;

  Value = atoi(TimeElem);
  Value += NavValue.HourOffset + Offset;
  if(Value > 23)
    Value -= 24;
  itoa(Value, TimeElem, 10);
  if(Value < 10) {
    NavValue.Time[0] = '0';
    NavValue.Time[1] = TimeElem[0];
  } else {
    NavValue.Time[0] = TimeElem[0];
    NavValue.Time[1] = TimeElem[1];
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

void ComputeNewAngleDist(void *){
  static byte CmpComputeStep = 0;

  if(CmpComputeStep == 1) {
    if (MainState.CurrentValid && MainState.Destination.LatSet && MainState.Destination.LonSet) 
      NavValue.Destination.Angle = Direction(&NavValue.Current, &NavValue.Destination);
    //else
    //  NavValue.Destination.Angle = 255;

  } else if(CmpComputeStep == 2) {
    if (MainState.CurrentValid && MainState.Pointer.LatSet && MainState.Pointer.LonSet)
      NavValue.Pointer.Angle     = Direction(&NavValue.Current, &NavValue.Pointer);
    //else
    //  NavValue.Destination.Angle = 255;

  } else if(CmpComputeStep == 3) {
    if (MainState.CurrentValid && MainState.RefPointB.LatSet && MainState.RefPointB.LonSet)
      NavValue.RefPointB.Angle    = Direction(&NavValue.Current, &NavValue.RefPointB);
    //else
    //  NavValue.Destination.Angle = 255;

  } else if(CmpComputeStep == 4) {
    if (MainState.CurrentValid && MainState.RefPointA.LatSet && MainState.RefPointA.LonSet)
      NavValue.RefPointA.Angle    = Direction(&NavValue.Current, &NavValue.RefPointA);
    //else
    //  NavValue.Destination.Angle = 255;

  } else if(CmpComputeStep == 5) {
    if (MainState.CurrentValid && MainState.WaypointsSet[MainState.CurrentWaypoint] )
      NavValue.Position->Angle     = Direction(&NavValue.Current, NavValue.Position);
    //else
    //  NavValue.Destination.Angle = 255;


  } else if(CmpComputeStep == 6) {
    if (MainState.CurrentValid && MainState.Destination.LatSet && MainState.Destination.LonSet)
      NavValue.Destination.Dist = Distance(&NavValue.Current, &NavValue.Destination);
    //else
    //  NavValue.Destination.Dist = -1;

  } else if(CmpComputeStep == 7) {
    if (MainState.CurrentValid && MainState.Pointer.LatSet && MainState.Pointer.LonSet)
      NavValue.Pointer.Dist     = Distance(&NavValue.Current, &NavValue.Pointer);
    //else
    //  NavValue.Pointer.Dist = -1;

  } else if(CmpComputeStep == 8) {
    if (MainState.CurrentValid && MainState.RefPointB.LatSet && MainState.RefPointB.LonSet)
      NavValue.RefPointB.Dist    = Distance(&NavValue.Current, &NavValue.RefPointB);
    //else
    //  NavValue.RefPointB.Dist = -1;

  } else if(CmpComputeStep == 9) {
    if (MainState.CurrentValid && MainState.RefPointA.LatSet && MainState.RefPointA.LonSet)
      NavValue.RefPointA.Dist    = Distance(&NavValue.Current, &NavValue.RefPointA);
    //else
    //  NavValue.RefPointA.Dist = -1;

  } else if(CmpComputeStep == 10) {
    if (MainState.CurrentValid && MainState.WaypointsSet[MainState.CurrentWaypoint] )
      NavValue.Position->Dist     = Distance(&NavValue.Current, NavValue.Position);
    //else
    //  NavValue.Position->Dist = -1;


  } else if (CmpComputeStep == 11) {
    ComputeRatio(NULL);

  } else if(CmpComputeStep == 12) {
    if(MainState.CursorValid) {
      ComputeScale(NULL);

      if(MainState.CurrentValid)
        ComputeNewXY('H');

      if(!MainState.Destination.XYSet)
        ComputeNewXY('D');
        
      if(!MainState.Pointer.XYSet)
        ComputeNewXY('M');  
    }

    if( MainState.CurrentValid && MainState.CurrentWaypoint == 0 &&
        !MainState.WaypointsSet[MainState.CurrentWaypoint] &&
        NavValue.Current.Lat != 0 && NavValue.Current.Lon != 0 )
      SavePosition(NULL);

  } else if(CmpComputeStep == 13) {
    EEPROM.update(EEPROM_ControlVal, EEPROM_MagicValue); 
    UpdateEEPROM(NULL);

    CmpComputeStep = 0;
  }
  CmpComputeStep++;
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

void ComputeRatio(void *) {
  float LatLonDelta, StepMotorDelta;
  long LatRefA, LonRefA, LatRefB, LonRefB;

  if(MainState.CursorValid) {

    LatRefA = AddLatLonSign( NavValue.RefPointA.Lat, &NavValue.RefPointA.NInd, true);
    LonRefA = AddLatLonSign( NavValue.RefPointA.Lon, &NavValue.RefPointA.EInd, false);
    LatRefB = AddLatLonSign( NavValue.RefPointB.Lat, &NavValue.RefPointB.NInd, true);
    LonRefB = AddLatLonSign( NavValue.RefPointB.Lon, &NavValue.RefPointB.EInd, false);

    LatLonDelta = LatRefA - LatRefB;
    LatLonDelta = abs(LatLonDelta);

    StepMotorDelta = NavValue.RefPointA.Y - NavValue.RefPointB.Y;
    StepMotorDelta = abs(StepMotorDelta);
      
    StepMotorVal.ScalingFactorY   = LatLonDelta/StepMotorDelta;
    StepMotorVal.ScalingFactorLat = StepMotorDelta/LatLonDelta;

    LatLonDelta = LonRefA - LonRefB;
    LatLonDelta = abs(LatLonDelta);

    StepMotorDelta = NavValue.RefPointA.X - NavValue.RefPointB.X;
    StepMotorDelta = abs(StepMotorDelta);

    StepMotorVal.ScalingFactorX   = LatLonDelta/StepMotorDelta;
    StepMotorVal.ScalingFactorLon = StepMotorDelta/LatLonDelta;

    if(!MainState.Destination.LatSet || !MainState.Destination.LonSet)
      ComputeNewGrid('D');

    if(!MainState.Pointer.LatSet || !MainState.Pointer.LonSet)
      ComputeNewGrid('M');
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

void ComputeNewGrid(char* Mode) {

  if (MainState.CursorValid) {
    if(Mode == 'M') {
      if(MainState.Pointer.XYSet) {
        LatLonScaling(&NavValue.Pointer);
        MainState.Pointer.LatSet = true;
        MainState.Pointer.LonSet = true;
      }      
    } else if(Mode == 'D') {
      if(MainState.Destination.XYSet) {
        LatLonScaling(&NavValue.Destination);
        MainState.Destination.LatSet = true;
        MainState.Destination.LonSet = true;
      }
    }
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

void LatLonScaling(NavData *Data) {
  long XYDelta,
       LatRef, LonRef;  
  float Value;

  LatRef = AddLatLonSign( NavValue.RefPointA.Lat, &NavValue.RefPointA.NInd, true);
  LonRef = AddLatLonSign( NavValue.RefPointA.Lon, &NavValue.RefPointA.EInd, false);

  XYDelta = (Data->Y - NavValue.RefPointA.Y) * -1;
  Value = StepMotorVal.ScalingFactorY * XYDelta;
  Data->Lat = LatRef + Value;

  XYDelta = Data->X - NavValue.RefPointA.X;
  Value = StepMotorVal.ScalingFactorX * XYDelta;
  Data->Lon = LonRef + Value;

  Data->NInd = LatLonAbsolute(&Data->Lat);
  Data->EInd = LatLonAbsolute(&Data->Lon);
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####+

void ComputeScale(void *) {
  long LatRefA,  LatRefB,
       //LonRefA,  LonRefB,
       Dist;

  float /*StepMotorDeltaX,*/ StepMotorDeltaY,
        LatDelta, LonDelta;

  LatRefA = AddLatLonSign( NavValue.RefPointA.Lat, &NavValue.RefPointA.NInd, true);
  LatRefB = AddLatLonSign( NavValue.RefPointB.Lat, &NavValue.RefPointB.NInd, true);
  //LonRefA = AddLatLonSign( NavValue.RefPointA.Lon, &NavValue.RefPointA.EInd, false);
  //LonRefB = AddLatLonSign( NavValue.RefPointB.Lon, &NavValue.RefPointB.EInd, false);

  LatDelta = abs(LatRefA - LatRefB) * LatToMeter;
  StepMotorDeltaY = abs(NavValue.RefPointA.Y - NavValue.RefPointB.Y) * StepToCM;

  //LonDelta = abs(LonRefA - LonRefB) * LatToMeter * cos( (((LatRefA+LatRefB)/LatLonConvFactorL)/2.0) * DEG_TO_RAD );
  //StepMotorDeltaX = abs(NavValue.RefPointA.X - NavValue.RefPointB.X) * StepToCM;

  //NavValue.Scale = round(LonDelta / StepMotorDeltaX) * 100;
  NavValue.Scale = round(LatDelta / StepMotorDeltaY) * 100;  // 100 cm in a meter

  //If Destination within 2mm (as messured on the map) of the current position
  Dist = NavValue.Scale / 500; // distance in m = Scale/500  =  ( Scale * 2[mm] ) / 1000[mm per m]

  NavValue.NearZone = ( MainState.Destination.LatSet && MainState.Destination.LonSet  &&
                        MainState.CurrentValid       && NavValue.Destination.Dist > 0 &&
                        NavValue.Destination.Dist <= Dist );
}

