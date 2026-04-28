
// All the code related to the XY plotter is kept here

// GoToXY sets the target for the plotter to move to
// MakeStep called regularly move the XY plotter in a few steps increment and hands back control to avoid locking everyting while moving
// ComputeNewXY calculates new XY values from grid coordinates when needed
// XYScaling used by the previous to do computations
// CalibrateXY is called when turning the unit on to make sure the plotter is at a known XY position

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Set the target for the plotter to move to
void GoToXY(int X, int Y) {
  digitalWrite(StepEnablePin, HIGH);
  StepMotorVal.TargetX = min(max(X, 0), STEP_GridSizeX);
  StepMotorVal.TargetY = min(max(Y, 0), STEP_GridSizeY);
  MainState.MovingCur = true;

  //Serial.print("X:");
  //Serial.print(StepMotorVal.TargetX);
  //Serial.print(" Y:");
  //Serial.println(StepMotorVal.TargetY);
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Called regularly to move the XY plotter in a few steps increment
//   hands back control to avoid locking everyting while moving
void MakeStep(void *) {
  byte StepX = 0, StepY = 0;
  byte DirX = LOW,  DirY = LOW;

  if(MainState.MovingCur) {

    if(StepMotorVal.CurrentX != StepMotorVal.TargetX ) {
      //Serial.println(StepMotorVal.CurrentX);

      if( StepMotorVal.CurrentX < StepMotorVal.TargetX ) {
        DirX=LOW;
        StepX = min(StepMotorVal.TargetX - StepMotorVal.CurrentX, STEP_MaxPerBatch);
        StepMotorVal.CurrentX += StepX;
      } else {
        DirX=HIGH;
        StepX = min(StepMotorVal.CurrentX - StepMotorVal.TargetX, STEP_MaxPerBatch);
        StepMotorVal.CurrentX -= StepX;
      }  
      digitalWrite(DirXPin, DirX);

    }

    if(StepMotorVal.CurrentY != StepMotorVal.TargetY ) {
      //Serial.println(StepMotorVal.CurrentY);

      if( StepMotorVal.CurrentY < StepMotorVal.TargetY ) {
        DirY=HIGH;
        StepY = min(StepMotorVal.TargetY - StepMotorVal.CurrentY, STEP_MaxPerBatch);
        StepMotorVal.CurrentY += StepY;
      } else {
        DirY=LOW;
        StepY = min(StepMotorVal.CurrentY - StepMotorVal.TargetY, STEP_MaxPerBatch);
        StepMotorVal.CurrentY -= StepY;
      }  
      digitalWrite(DirYPin, DirY);

    }

    if( StepX != 0 || StepY != 0 ) {
      delayMicroseconds(STEP_InitDelay); 

      do {
        if ( StepX != 0 )
          digitalWrite(StepXPin, HIGH);
        if ( StepY != 0 )
          digitalWrite(StepYPin, HIGH);

        delayMicroseconds(STEP_MicroSecDelay); 
        digitalWrite(StepXPin, LOW);   //if ( StepX != 0 )
        digitalWrite(StepYPin, LOW);   //if ( StepY != 0 )
        delayMicroseconds(STEP_MicroSecDelay);
        if ( StepX != 0 )
          StepX--;
        if ( StepY != 0 )
          StepY--;
      }
      while(StepX != 0 || StepY != 0 );

      //digitalWrite(DirXPin, LOW);
      //digitalWrite(DirYPin, LOW);
    }

    if(StepMotorVal.CurrentX == StepMotorVal.TargetX && StepMotorVal.CurrentY == StepMotorVal.TargetY )
      MainState.MovingCur = false;

    //digitalWrite(StepEnablePin, LOW);
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Calculate new XY values from grid coordinates when needed
void ComputeNewXY(DisplayModes Mode) {
  
  if (MainState.CursorValid) {
    if(Mode == M_HEADING) {
      XYScaling(&NavValue.Current);
    } else if(Mode == M_POSITION && MainState.WaypointsSet[MainState.CurrentWaypoint]) {
      XYScaling(NavValue.Position);
    } else if(Mode == M_MAP) {
      if(MainState.Pointer.LatSet && MainState.Pointer.LonSet) {
        XYScaling(&NavValue.Pointer);
        MainState.Pointer.XYSet = true;
      }
    } else if(Mode == M_DESTINATION) {
      if(MainState.Destination.LatSet && MainState.Destination.LonSet) {
        XYScaling(&NavValue.Destination);
        MainState.Destination.XYSet  = true;
      }
    }
  }
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Used by the previous to do computations
void XYScaling(NavData *Data) {
  long LatRef, LonRef;
  long LatLonDelta, Lat, Lon;
  float Value;

  LatRef = AddLatLonSign( NavValue.RefPointA.Lat, &NavValue.RefPointA.NInd, true);
  LonRef = AddLatLonSign( NavValue.RefPointA.Lon, &NavValue.RefPointA.EInd, false);

  Lat = AddLatLonSign( Data->Lat, &Data->NInd, true);
  Lon = AddLatLonSign( Data->Lon, &Data->EInd, false);

  LatLonDelta = Lat - LatRef;
  Value = StepMotorVal.ScalingFactorLat * LatLonDelta * -1;
  Data->Y = NavValue.RefPointA.Y + Value;
  LatLonDelta = Lon - LonRef;
  Value = StepMotorVal.ScalingFactorLon * LatLonDelta;
  Data->X = NavValue.RefPointA.X + Value;
}

//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####
//     #####     #####     #####     #####     #####

// Make sure the plotter is at a known XY position
//   called when turning the unit on
void CalibrateXY(void *) {
  StepMotorVal.CurrentX = STEP_GridSizeX + STEP_Overshoot;
  StepMotorVal.CurrentY = STEP_GridSizeY + STEP_Overshoot;
  GoToXY(0, 0);
  StepMotorVal.Calibrating = true;
}
