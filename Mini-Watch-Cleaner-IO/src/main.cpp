/**
 * Mini Watch Cleaner
 * Using A continous rotation servo and some 3d printing.
 * Running on an Arduino Pro Mini.
 * A 4 digit led tube for display.
 * 2 Buttons.
 * 
 * Hello, and warning. Here are some code. It works, but I have spend 0 time on making it good or pretty.
 * I'm uploading it on the chance that someone perhaps will find it usefull as a starting point.
 * 
 * 
 * 
 */

#include <Arduino.h>
#include <Servo.h>
#include "Button.h"


#include <DIYables_4Digit7Segment_74HC595.h> // DIYables_4Digit7Segment_74HC595 library

#define SCLK  8  // The Arduino pin connected to SCLK
#define RCLK  7  // The Arduino pin connected to RCLK
#define DIO   6  // The Arduino pin connected to DIO

DIYables_4Digit7Segment_74HC595 display(SCLK, RCLK, DIO);

#define PIN_SERVO 5

#define BUTTON_A A0
#define BUTTON_B A1
#define SLOW_SPEED_SERVO 2



enum class MODES {
  STOP,
  CLEANING,
  RINSING,
  SLOW_CLEANING
};

MODES activemode = MODES::STOP;
MODES selectmode = MODES::CLEANING;
long lastupdate=0;
int timer = 0;
boolean rotateClockwise = true;
int debouncetimer = 0;
Button * buttonA;
Button * buttonB;
Servo myservo;  // create Servo object to control a servo

void rotateServo(int speed,boolean clockwise);

void stopServo(){
  myservo.write(90);
}
void startMode(MODES newMode){
  activemode = newMode;
  switch (activemode)
    {
    case MODES::STOP:
      stopServo();
      break;
    case MODES::CLEANING:
      timer = 300;
      rotateClockwise = true;
      rotateServo(90,rotateClockwise);
      break;
    case MODES::RINSING:
      timer = 180;
      rotateClockwise = true;
      rotateServo(90,rotateClockwise);
      break;
    case MODES::SLOW_CLEANING:
      timer = 300;
      rotateClockwise = true;
      rotateServo(SLOW_SPEED_SERVO,rotateClockwise);
      break;
    default:
      break;
    }
}

void callbackButtonA(char c){
  startMode(selectmode);
}

void callbackButtonB(char c){
  if (activemode!=MODES::STOP){
   startMode(MODES::STOP);
   selectmode = MODES::CLEANING;
  }else{
    //next mode
    switch (selectmode)
    {
    case MODES::STOP:
      selectmode = MODES::CLEANING;
      break;
    case MODES::CLEANING:
      selectmode = MODES::RINSING;
      break;
    case MODES::RINSING:
      selectmode = MODES::SLOW_CLEANING;
      break;
    case MODES::SLOW_CLEANING:
      selectmode = MODES::CLEANING;
      break;
    default:
      break;
    }
  }
}
    /**
     *  ___        7
     * |   |     2   6
     *  ---        1
     * |   |     3   5
     *  ---  .     4    0
     * 
     */
/**
 *   
 * B11000000,  // 0
  B11111001,  // 1
  B10100100,  // 2
  B10110000,  // 3
  B10011001,  // 4
  B10010010,  // 5
  B10000010,  // 6
  B11111000,  // 7
  B10000000,  // 8
  B10010000,  // 9
  B10111111,  // - (dash)
  B11110111,  // _ (underscore)
  B11000110,  // C
  B10000110,  // E
  B10001110,  // F
  B10011100   // ° (degree)

  I have modified the 4 digit 7 segment code to allow for raw input, like this:
      void setRaw(int pos, byte value);

      void DIYables_4Digit7Segment_74HC595::setRaw(int pos, byte value) {
        if (pos < 1 || pos > 4)
          return;

        if (value < 0 || pos > 9)
          return;

        _digit_sets[pos - 1] = value;
      }

 */

void showModeAtPos(MODES mode,int pos){
  switch (mode)
  {
  case MODES::STOP:
    display.setNumber(pos, 5); 
    break;
  case MODES::CLEANING:
    display.setChar(pos, SegChars::C);

    break;
  case MODES::RINSING:
    //display.setChar(pos, SegChars::DEGREE);
    display.setRaw(pos, B10001000); // R
    break;
  case MODES::SLOW_CLEANING:
    //selectmode = MODES::CLEANING;
    display.setRaw(pos, B11000111); // L
    break;
  default:
    break;
  }
}
void updateDisp(){

  display.clear();
  switch (activemode)
  {
  case MODES::STOP:
    showModeAtPos(selectmode,2);
    break;
  case MODES::CLEANING:
    display.printInt(timer, false);
    break;
  case MODES::SLOW_CLEANING:
    display.printInt(timer, false);
  break;
  case MODES::RINSING:
    display.printInt(timer, false);
  break;
  
  default:
    break;
  }
  showModeAtPos(activemode,1);
  display.setDot(1);                    // set . at the 1st digit
  display.show();                       // show on the display */

}
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(BUTTON_A, INPUT_PULLUP);
  //pinMode(BUTTON_B, INPUT_PULLUP);
  buttonA = new Button(BUTTON_A);
  buttonB = new Button(BUTTON_B);
  buttonA->init();
  buttonB->init();
  buttonA->setSelectorCharCallback('a',&callbackButtonA);
  buttonB->setSelectorCharCallback('b',&callbackButtonB);



  myservo.attach(PIN_SERVO);  // attaches the servo on pin 9 to the Servo object


    myservo.write(90);
    lastupdate = millis();
}

void smartDelay(long time){

}
/**
 * Speed [1-90]
 */
void rotateServo(int speed,boolean clockwise){
  if (clockwise){
    myservo.write(90-speed);

  }else{
    myservo.write(90+speed);
  }
}
//Run every 1 sec
void updateCleaningProgram(int secperpart,int speed){
  timer--;

  if (timer<=0) {
    startMode(MODES::STOP);
    
    return;
  }

  if (timer%secperpart==0){
    rotateClockwise = !rotateClockwise;
    rotateServo(speed,rotateClockwise);
  }

}

// the loop function runs over and over again forever
void loop() {


  buttonA->update();
  buttonB->update();

  long now = millis();
  if (now - lastupdate>1000 || (lastupdate>now && (lastupdate- now > 1000))){
    lastupdate = now;
    //every 1 sec
    switch (activemode)
    {
    case MODES::CLEANING:
      updateCleaningProgram(5,90);
      break;
    case MODES::SLOW_CLEANING:
      updateCleaningProgram(5,SLOW_SPEED_SERVO);
      break;
    case MODES::RINSING:
      updateCleaningProgram(500,90);
      break;
    default:
      break;
    }
  }
 
  

  updateDisp();

  display.loop(); // MUST call the display.loop() function in loop()



}
