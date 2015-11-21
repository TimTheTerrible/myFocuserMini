#include "myStepper.h"

#define BUZZER_PIN 10

//
//  Serial Control
//

#define MAX_BUFZISE 32
char inBuf[MAX_BUFZISE];
int inBufIdx = 0;
boolean serialCmdRdy = false;

// ASCOM Commands
#define NUM_CMDS 40
#define CMD_NONE              0 // Command Parsing Error
#define CMD_GET_POSITION      1 // GP
#define CMD_GET_ISMOVING      2 // GI
#define CMD_MOVE_NOW          3 // FG
#define CMD_STOP_NOW          4 // FQ
#define CMD_GO_HOME           5 // PH
#define CMD_GET_NEW           6 // GN
#define CMD_SET_FULLSTEP      7 // SF
#define CMD_SET_HALFSTEP      8 // SH
#define CMD_GET_HALFSTEP      9 // GH
#define CMD_GET_STEPMODE     10 // GS
#define CMD_SET_STEPMODE     11 // SS
#define CMD_SET_POSITION     12 // SP
#define CMD_SET_NEW          13 // SN
#define CMD_GET_TEMP1        14 // GT
#define CMD_GET_TEMP2        15 // GZ
#define CMD_GET_FW1          16 // GV
#define CMD_GET_FW2          17 // GF
#define CMD_GET_STEPDELAY    18 // GD
#define CMD_SET_STEPDELAY    19 // SD
#define CMD_SET_TEMPCOEF     20 // SC
#define CMD_GET_TEMPCOEF     21 // GC
#define CMD_SET_TEMP_OFFSET  22 // PO
#define CMD_SET_MAX_POS      23 // SM
#define CMD_GET_MAX_POS      24 // GM
#define CMD_SET_MAX_INCR     25 // SY
#define CMD_GET_MAX_INCR     26 // GY
#define CMD_GET_COILPWR      27 // GO
#define CMD_SET_COILPWR      28 // SO
#define CMD_GET_REVERSE      29 // GR
#define CMD_SET_REVERSE      30 // SR
#define CMD_SET_SPEED        31 // MS
#define CMD_GET_BACKLIGHT    32 // GB
#define CMD_SET_DISPLAY      33 // DS
#define CMD_GET_DISPLAY      34 // DG
#define CMD_SET_UPDATE       35 // SU
#define CMD_SET_TEMPCOMP_ON  36 // +
#define CMD_SET_TEMPCOMP_OFF 37 // -
#define CMD_DUMP_DEBUG       38 // DD
#define CMD_MAKE_REVOLUTION  39 // MR

const char * ASCOM[NUM_CMDS] = {
  "XX", "GP", "GI", "FG", "FQ", "PH", "GN", "SF", "SH", "GH",
  "GS", "SS", "SP", "SN", "GT", "GZ", "GV", "GF", "GD", "SD",
  "SC", "GC", "PO", "SM", "GM", "SY", "GY", "GO", "SO", "GR",
  "SR", "MS", "GB", "DS", "DG", "SU",
  "+", "-", "DD", "MR",
};

// Handle ASCOM commands from the serial port
void checkSerial() {
  char cmd[MAX_BUFZISE];
  char param[MAX_BUFZISE];
  char outBuf[MAX_BUFZISE];  
  int intArg;
  boolean hasArg = false;
  
  if ( ! serialCmdRdy )
    return;
  
  memset(cmd, 0, MAX_BUFZISE);
  memset(param, 0, MAX_BUFZISE);
  
  int len = strlen(inBuf);
  
  // Get the first two chars as the command...
  if (len >= 2) {
    strncpy(cmd, inBuf, 2);
  }
  
  // get anything after as a parameter...
  if (len > 2) {
    strncpy(param, inBuf + 2, len - 2);
    hasArg = true;
  }

  // assume the parameter is an integer...
  intArg = hexstr2int(param);
  
  // convert the command string to an integer...
  int command = CMD_NONE;
  
  for ( int i = 0; i < NUM_CMDS; i++ ) {
    if ( !strcasecmp(cmd, ASCOM[i]) )
      command = i;
  }  

#ifdef _DEBUGGING_
  sprintf(outBuf, "DEBUG: cmd = %s", cmd);
  Serial.println(outBuf);
  sprintf(outBuf, "DEBUG: command = %d", command);
  Serial.println(outBuf);
  sprintf(outBuf, "DEBUG: intArg = %d", intArg);
  Serial.println(outBuf);
#endif

  // switch on the value of command...
  switch ( command  ) {
    case CMD_GET_POSITION: // GP
      sprintf(outBuf, "%04X#", theMotor.getCurPos());
      Serial.println(outBuf);
      break;
    case CMD_GET_ISMOVING: // GI
      sprintf(outBuf, "%02X#", theMotor.getIsMoving());
      Serial.println(outBuf);
      break;
    case CMD_MOVE_NOW: // FG
      theMotor.moveNow();
      break;
    case CMD_STOP_NOW: // FQ
      theMotor.stopNow();
      break;
    case CMD_GO_HOME: // PH
      theMotor.setTgtPos(theMotor.getHomePos());
      theMotor.moveNow();
      break;
    case CMD_GET_NEW: // GN
      sprintf(outBuf, "%02X#", theMotor.getTgtPos());
      Serial.println(outBuf);
      break;
    case CMD_SET_FULLSTEP: // SF
      theMotor.setStepType(MC_STEP_FULL);
      break;
    case CMD_SET_HALFSTEP: // SH
      theMotor.setStepType(MC_STEP_HALF);
      break;
    case CMD_GET_HALFSTEP: // GH
      sprintf(outBuf, "%02X#", theMotor.getStepType() == MC_STEP_HALF ? true : false );
      Serial.println(outBuf);
      break;
    case CMD_GET_STEPMODE: // GS
      sprintf(outBuf, "%02X#", stepTypeNickNames[theMotor.getStepType()]);
      Serial.println(outBuf);
      break;
    case CMD_SET_STEPMODE: // SS
      if ( ! hasArg ) {
        complain();
        break;
      }      
      for ( int i = 0; i < NUM_STEPTYPES; i++ ) {
        if ( intArg == stepTypeNickNames[i] )
          theMotor.setStepType(i);
      }
      break;
    case CMD_SET_POSITION: // SP
      if ( ! hasArg ) {
        complain();
        break;
      }      
      theMotor.setCurPos(intArg);
      theMotor.setTgtPos(intArg);
      break;
    case CMD_SET_NEW: // SN
      if ( ! hasArg ) {
        complain();
        break;
      }      
      theMotor.setTgtPos(intArg);
      break;
    case CMD_GET_TEMP1: // GT
      Serial.println("20#");
      break;
    case CMD_GET_TEMP2: // GZ
      Serial.println("20#");
      break;
    case CMD_GET_FW1: // GV
    case CMD_GET_FW2: // GF
      Serial.println("myFocuserMini v1.0 rev 2#");
      break;
    case CMD_GET_STEPDELAY: // GD
      Serial.println("02#");
      break;
    case CMD_SET_STEPDELAY: // SD
      // not implemented; ignore...
      break;
    case CMD_SET_TEMPCOEF: // SC
      // not implemented; ignore...
      break;
    case CMD_GET_TEMPCOEF: // GC
      // not implemented; ignore...
      break;
    case CMD_SET_TEMP_OFFSET: // PO
      // not implemented; ignore...
      break;
    case CMD_SET_MAX_POS: // SM
      if ( ! hasArg ) {
        complain();
        break;
      }      
      theMotor.setMaxPos(intArg);
      break;
    case CMD_GET_MAX_POS: // GM
      sprintf(outBuf, "%04X#", theMotor.getMaxPos());
      Serial.println(outBuf);      
      break;
    case CMD_SET_MAX_INCR: // SY
      // not implemented; ignore...
      break;
    case CMD_GET_MAX_INCR: // GY
      // not implemented; ignore...
      break;
    case CMD_GET_COILPWR: // GO
      sprintf(outBuf, "%02X#", theMotor.getCoilPwr());
      Serial.println(outBuf);
      break;
    case CMD_SET_COILPWR: // SO
      if ( ! hasArg ) {
        complain();
        break;
      }      
      theMotor.setCoilPwr(intArg?true:false);
      break;
    case CMD_GET_REVERSE: // GR
      sprintf(outBuf, "%02X#", theMotor.getReverse());
      Serial.println(outBuf);
      break;
    case CMD_SET_REVERSE: // SR
      if ( ! hasArg ) {
        complain();
        break;
      }      
      theMotor.setReverse(intArg?true:false);
      break;
    case CMD_SET_SPEED: // MS
      if ( ! hasArg ) {
        complain();
        break;
      }      
      theMotor.setSpeed(intArg);
      break;
    case CMD_GET_BACKLIGHT: // GB
      // not implemented; ignore...
      break;
    case CMD_SET_DISPLAY: // DS
      // not implemented; ignore...
      break;
    case CMD_GET_DISPLAY: // DG
      Serial.println("02#");
      break;
    case CMD_SET_UPDATE: // SU
      // not implemented; ignore...
      break;
    case CMD_SET_TEMPCOMP_ON: // +
      // not implemented; ignore...
      break;
    case CMD_SET_TEMPCOMP_OFF: // -
      // not implemented; ignore...
      break;
    case CMD_DUMP_DEBUG: // DD
      theMotor.dumpDebug();
      break;
    case CMD_MAKE_REVOLUTION: // MR
      theMotor.moveMotor(MC_DIR_FWD, theMotor.getRevSteps());
      break;
    default:
      // Not supposed to get here, so let the user know something went wrong
      complain();
      break;
  }

  // clear the buffers...
  memset(inBuf, 0, MAX_BUFZISE);
  memset(outBuf, 0, MAX_BUFZISE);
   
  // All done, clear the flag...
  serialCmdRdy = false;
}

void serialEvent() {
  
  // : starts the command, # ends the command, do not store these in the command buffer
  while ( Serial.available() && ! serialCmdRdy ) {
    
    char inChar = Serial.read();
    
    if (inChar == '#') {
      inBuf[inBufIdx++] = 0x0;
      serialCmdRdy = true;
    }
    else if ( inChar == ':') {
      inBufIdx = 0;
    }
    else {      
      inBuf[inBufIdx++] = inChar;
      
      if (inBufIdx >= MAX_BUFZISE) {
        inBufIdx = MAX_BUFZISE - 1;
      }
    }
  }
}

// convert hex string to int...
int hexstr2int(char *line) {
  int ret = 0;

  ret = strtol(line, NULL, 16);
  return (ret);
}

void beep(int duration) {
  pinMode(BUZZER_PIN, OUTPUT);
  analogWrite(BUZZER_PIN, 127);
  delay(duration);
  analogWrite(BUZZER_PIN, 0);
}

void complain() {
  beep(50); delay(50); beep(50); delay(50); beep(50);
}

//
//  Main Program
//

void setup() {
  Serial.begin(9600);
  
  // Allow everybody to wake up...
  delay(2000);
  
  // I'm awake, I'm awake!!!
  beep(100);

  // Set up the motor...
  theMotor.begin();
  theMotor.setStepType(MC_STEP_FULL);
  theMotor.setSpeed(10);

  // Waggle the motor...
  theMotor.moveMotor(MC_DIR_FWD, 50);
  theMotor.moveMotor(MC_DIR_REV, 50);

  // Wait for it to stop...
  theMotor.waitMotor();

  // All done!
  beep(100);
}

void loop() {
  checkSerial();
}

