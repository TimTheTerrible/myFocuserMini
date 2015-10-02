
#ifndef _myStepper_h_
#define _myStepper_h_

#include <Arduino.h>

//#define _DEBUGGING_

// Motor Controller Pins
#define MC_PIN_DIR               2
#define MC_PIN_STEP              3
#define MC_PIN_SLEEP             4
#define MC_PIN_RESET             5
#define MC_PIN_MS3               6
#define MC_PIN_MS2               7
#define MC_PIN_MS1               8
#define MC_PIN_ENABLE            9

// Motor Step Types
#define NUM_STEPTYPES            5
#define MC_STEP_FULL             0
#define MC_STEP_HALF             1
#define MC_STEP_QUARTER          2
#define MC_STEP_EIGHTH           3
#define MC_STEP_SIXTEENTH        4

// Step Interval constraints
#define MC_MIN_STEP_INTERVAL     6000 // 6 mS ( 50 rpm )
#define MC_MAX_STEP_INTERVAL   300000 // 300 mS ( 1 rpm )

// Motor Directions
#define NUM_DIRS                 2
#define MC_DIR_REV               0
#define MC_DIR_FWD               1

// Motor position constraints
#define MC_MIN_POS               0
#define MC_MAX_POS           10000

// Text debugging aids
extern int stepTypes[NUM_STEPTYPES];
extern const char * stepTypeNames[NUM_STEPTYPES];
extern int stepTypeNickNames[NUM_STEPTYPES];
extern const char * dirNames[NUM_DIRS];

//
// myStepper Class
//

class myStepper
{
  public:
  
    void begin();
    void dumpDebug();
    void moveMotor(int, int);
    void moveNow();
    void stopNow();
    void waitMotor();
    void setSpeed(int);
    
    // Accessors
    int getMinPos();        void setMinPos(int);
    int getMaxPos();        void setMaxPos(int);
    int getHomePos();       void setHomePos(int);
    int getCurPos();        void setCurPos(int);
    int getTgtPos();        void setTgtPos(int);
    int getStepType();      void setStepType(int);
    int getStepInterval();  void setStepInterval(int);
    int getRevSteps();      void setRevSteps(int);
    boolean getIsMoving();  void setIsMoving(boolean);
    boolean getCanMove();   void setCanMove(boolean);
    boolean getCoilPwr();   void setCoilPwr(boolean);
    boolean getReverse();   void setReverse(boolean);

  private:
  
    int m_minPos = 0;
    int m_maxPos = 1600;
    int m_homePos = 800;
    int m_curPos = m_homePos;
    int m_tgtPos = m_curPos;
    int m_stepType = MC_STEP_FULL;
    int m_stepInterval = MC_MIN_STEP_INTERVAL;
    int m_revSteps = 200;
    boolean m_isMoving = false;
    boolean m_canMove = false;
    boolean m_coilPwr = false;
    boolean m_reverse = false;
    IntervalTimer myTimer;
    
    void updateMotor();
    void coilsOn();
    void coilsOff();
    void stepOnce(int);
    static void doInterrupt();
};

// Global motor object
extern myStepper theMotor;

#endif

