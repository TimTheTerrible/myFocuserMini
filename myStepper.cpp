#include "myStepper.h"

int stepTypes[NUM_STEPTYPES] = {
  MC_STEP_FULL,
  MC_STEP_HALF,
  MC_STEP_QUARTER,
  MC_STEP_EIGHTH,
  MC_STEP_SIXTEENTH,
};

const char * stepTypeNames[NUM_STEPTYPES] = {
  "FULL",
  "HALF",
  "QUARTER",
  "EIGHTH",
  "SIXTEENTH",
};

int stepTypeNickNames[NUM_STEPTYPES] = { 1, 2, 4, 8, 16 };

const char * dirNames[NUM_DIRS] = {
  "MC_DIR_REV",
  "MC_DIR_FWD",
};

myStepper theMotor;

// Set up the motor driver
void myStepper::begin() {

  // Set the pin modes, and set them all to default values...
  pinMode(MC_PIN_DIR, OUTPUT);    digitalWrite(MC_PIN_DIR, MC_DIR_FWD);
  pinMode(MC_PIN_STEP, OUTPUT);   digitalWrite(MC_PIN_STEP, LOW);
  pinMode(MC_PIN_MS1, OUTPUT);    digitalWrite(MC_PIN_MS1, LOW);
  pinMode(MC_PIN_MS2, OUTPUT);    digitalWrite(MC_PIN_MS2, LOW);
  pinMode(MC_PIN_MS3, OUTPUT);    digitalWrite(MC_PIN_MS3, LOW);
  pinMode(MC_PIN_SLEEP, OUTPUT);  digitalWrite(MC_PIN_SLEEP, HIGH);
  pinMode(MC_PIN_RESET, OUTPUT);  digitalWrite(MC_PIN_RESET, HIGH);
  pinMode(MC_PIN_ENABLE, OUTPUT); digitalWrite(MC_PIN_ENABLE, HIGH);
  
  // Set up the interrupt...
  myTimer.begin(doInterrupt, m_stepInterval);
}

void myStepper::dumpDebug() {
  Serial.println();
  Serial.println("myStepper status:");
  Serial.print("minPos = "); Serial.println(m_minPos);
  Serial.print("maxPos = "); Serial.println(m_maxPos);
  Serial.print("homePos = "); Serial.println(m_homePos);
  Serial.print("curPos = "); Serial.println(m_curPos);
  Serial.print("tgtPos = "); Serial.println(m_tgtPos);
  Serial.print("stepType = "); Serial.println(stepTypeNames[m_stepType]);
  Serial.print("stepInterval = "); Serial.print(m_stepInterval);
    Serial.print(" ("); Serial.print(60000000 / m_revSteps / m_stepInterval ); Serial.println(" RPM)");
  Serial.print("canMove = "); Serial.println(m_canMove?"TRUE":"FALSE");
  Serial.print("isMoving = "); Serial.println(m_isMoving?"TRUE":"FALSE");
  Serial.print("coilPwr = "); Serial.println(m_coilPwr?"TRUE":"FALSE");
  Serial.print("reverse = "); Serial.println(m_reverse?"TRUE":"FALSE");
  Serial.println();
}

// Accessors
int myStepper::getMinPos() {
  return m_minPos;
}

void myStepper::setMinPos(int newMin) {
  waitMotor();
  m_minPos = newMin >= MC_MIN_POS ? newMin : m_minPos;
}

int myStepper::getMaxPos() {
  return m_maxPos;
}

void myStepper::setMaxPos(int newMax) {
  waitMotor();
  m_maxPos = newMax <= MC_MAX_POS ? newMax : m_maxPos;
}

int myStepper::getHomePos() {
  return m_homePos;
}

void myStepper::setHomePos(int newPos) {
  waitMotor();
  m_homePos = newPos >= m_minPos && newPos <= m_maxPos ? newPos : m_homePos;
}

int myStepper::getCurPos() {
  return m_curPos;
}

void myStepper::setCurPos(int newPos) {
  waitMotor();
  m_curPos = newPos >= m_minPos && newPos <= m_maxPos ? newPos : m_curPos;
}

int myStepper::getTgtPos() {
  return m_tgtPos;
}

void myStepper::setTgtPos(int newPos) {
  waitMotor();
  m_tgtPos = newPos >= m_minPos && newPos <= m_maxPos ? newPos : m_tgtPos;
}

int myStepper::getStepType() {
  return m_stepType;
}

void myStepper::setStepType(int newType) {

  waitMotor();

  m_stepType = newType >= 0 && newType < NUM_STEPTYPES ? newType : m_stepType;
  
  switch ( newType ) {
    case MC_STEP_FULL:
      digitalWrite(MC_PIN_MS1, LOW);
      digitalWrite(MC_PIN_MS2, LOW);
      digitalWrite(MC_PIN_MS3, LOW);
      break;
    case MC_STEP_HALF:
      digitalWrite(MC_PIN_MS1, HIGH);
      digitalWrite(MC_PIN_MS2, LOW);
      digitalWrite(MC_PIN_MS3, LOW);
      break;
    case MC_STEP_QUARTER:
      digitalWrite(MC_PIN_MS1, LOW);
      digitalWrite(MC_PIN_MS2, HIGH);
      digitalWrite(MC_PIN_MS3, LOW);
      break;
    case MC_STEP_EIGHTH:
      digitalWrite(MC_PIN_MS1, HIGH);
      digitalWrite(MC_PIN_MS2, HIGH);
      digitalWrite(MC_PIN_MS3, LOW);
      break;
    case MC_STEP_SIXTEENTH:
      digitalWrite(MC_PIN_MS1, HIGH);
      digitalWrite(MC_PIN_MS2, HIGH);
      digitalWrite(MC_PIN_MS3, HIGH);
      break;
    default:
#ifdef _DEBUGGING_
      Serial.print("*** EROR *** Invalid stepType requested: "); Serial.println(newType);
#endif
      break;
  }
}

int myStepper::getStepInterval() {
  return m_stepInterval;
}

void myStepper::setStepInterval(int newInterval) {
  waitMotor();
  m_stepInterval = newInterval >= MC_MIN_STEP_INTERVAL && newInterval <= MC_MAX_STEP_INTERVAL ? newInterval : m_stepInterval;
}

int myStepper::getRevSteps() {
  return m_revSteps;
}

void myStepper::setRevSteps(int newSteps) {
  waitMotor();
  m_revSteps = newSteps >= MC_MIN_STEP_INTERVAL && newSteps <= MC_MAX_STEP_INTERVAL ? newSteps : m_revSteps;
}

boolean myStepper::getIsMoving() {
  return m_isMoving;
}

void myStepper::setIsMoving(boolean flag) {
  m_isMoving = flag;
}

boolean myStepper::getCanMove() {
  return m_canMove;
}

void myStepper::setCanMove(boolean flag) {
  m_canMove = flag;
}

boolean myStepper::getCoilPwr() {
  return m_coilPwr;
}

void myStepper::setCoilPwr(boolean flag) {
  waitMotor();
  m_coilPwr = flag;
}

boolean myStepper::getReverse() {
  return m_reverse;
}

void myStepper::setReverse(boolean flag ) {
  waitMotor();
  m_reverse = flag;
}

//
// myStepper Implementation
//

// Turn on the coil power...
void myStepper::coilsOn() {
  digitalWrite(MC_PIN_ENABLE, LOW);
#ifdef _DEBUGGING_
  Serial.println("coilsOn()");
#endif
}

// Turn off the coil power...
void myStepper::coilsOff() {
  digitalWrite(MC_PIN_ENABLE, HIGH);
#ifdef _DEBUGGING_
  Serial.println("coilsOff()");
#endif
}

// Step the motor once in the given direction...
void myStepper::stepOnce(int dir) {

  m_isMoving = true;

  // Set the direction...
  digitalWrite(MC_PIN_DIR, dir);

  // Step the motor...
  digitalWrite(MC_PIN_STEP, HIGH); // assert STEP
  delayMicroseconds(2);            // wait two microseconds
  digitalWrite(MC_PIN_STEP, LOW);  // clear STEP

  // Update the object...
  if ( dir == MC_DIR_FWD )
    m_reverse ? m_curPos -= 1 : m_curPos += 1;
  else
    m_reverse ? m_curPos += 1 : m_curPos -= 1;

  // Are we done moving?
  if ( m_curPos == m_tgtPos ) {
    m_canMove = false;
    m_isMoving = false;
    if ( m_coilPwr == false )
      coilsOff();
  }
}

// Step the motor toward the target
void myStepper::updateMotor() {

  // Are we allowed to move?
  if ( ! m_canMove )
    return;

  // How many steps do we need to move?
  int steps = m_tgtPos - m_curPos;
  
  // None? Done!
  if ( steps == 0 ) {
    m_canMove = false;
    coilsOff();
    return;
  }

  // Figure out which way we need to go...
  int dir;
  
  if ( steps < 1 )
    dir = m_reverse?MC_DIR_FWD:MC_DIR_REV;
  else
    dir = m_reverse?MC_DIR_REV:MC_DIR_FWD;
  
  // Go!!!
  stepOnce(dir);
}

// Handle the timer interrupt...
void myStepper::doInterrupt() {
  noInterrupts();
  theMotor.updateMotor();
  interrupts();
}

// Move the motor...
void myStepper::moveMotor (int dir, int steps ) {

  // Block until the motor stops...
  waitMotor();
  
#ifdef _DEBUGGING_
  char outBuf[64];
  sprintf(outBuf, "moveMotor(%s, %d)", dirNames[dir], steps);
  Serial.println(outBuf);
#endif

  // Figure out the new target position...
  if ( dir == MC_DIR_FWD ) {
    m_tgtPos += steps;
    if ( m_tgtPos > m_maxPos)
      m_tgtPos = m_maxPos;
  }
  else {
    m_tgtPos -= steps;
    if ( m_tgtPos < m_minPos )
      m_tgtPos = m_minPos;
  }

  // Start moving...
  moveNow();

#ifdef _DEBUGGING_
  dumpDebug();
#endif
}

// Move the motor to a previously-specified position...
void myStepper::moveNow() {
  
  // Make sure the motor needs to move first...
  if ( m_curPos == m_tgtPos )
    return;
    
  // Enable the motor and turn on the coils...
  m_canMove = true;
  coilsOn();
}

// Stop the motor immediately...
void myStepper::stopNow() {
  
  m_tgtPos = m_curPos;
  m_isMoving = false;
  m_canMove = false;
  if ( ! m_coilPwr )
    coilsOff();
}

// Set the motor speed...
void myStepper::setSpeed( int newRPM ) {
  // TODO: based on stepType.

  waitMotor();

  // Calculate the new step interval...
  setStepInterval(60000000 / (newRPM * m_revSteps));

  // Stop the current timer interrupt...
  myTimer.end();
  
  // Start another with the new stepInterval...
  myTimer.begin(doInterrupt, m_stepInterval);  
}

// Wait for the motor to stop...
void myStepper::waitMotor() {
  while ( m_isMoving || m_canMove ) {
    delay(1);
#ifdef _DEBUGGING_
    Serial.print(".");
#endif
  }
}

