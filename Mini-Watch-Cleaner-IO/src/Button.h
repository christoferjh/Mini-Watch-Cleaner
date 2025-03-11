#ifndef Button_h
#define Button_h
#include <Arduino.h>


#define timeformomentpress 10

class Button {
    protected:
      char pin;
        bool debounce = true;
      int state;
      bool changed=true;
      int sinceChange=0;
      bool reverse=false;
      void (*callback)(char) = 0;
      char selectorChar;
    public:
      Button(char pin) {
        this->pin = pin;
        init();
      }
      Button() {
       
      }

      virtual void restart() {
        state=-1;
      }
      virtual void init() {
        pinMode(pin, INPUT_PULLUP);
        update();
      }
      bool isChanged(){
        return changed;
        }
      Button* nopullup(){
        pinMode(pin, INPUT);
        reverse=true;
        update();
        return this;
      }
      bool isTraceOn(){
        return false;
        //return this->isMomentButton();
      }
      /**
      template <typename F>
      Button* setCallback(F&& lambda){
        
        return this;
      }
      **/
      Button* setSelectorCharCallback(char c, void (*foo)(char)){
        this->callback = foo;
        this->selectorChar = c;
        return this;
        }


      virtual void print(bool verbose){
        Serial.print(this->isPressed()); 
      }
      virtual bool isStateSame(int a,int b){
          return a == b;
        }
      void setNewState(int newstate){
        if (isStateSame(newstate,state)){
          changed=false;
          sinceChange++;
        }else{
          changed=true;
          sinceChange = 0;
         }
        state = newstate;
      }
      virtual void update() {
        // You can handle the debounce of the button directly
        // in the class, so you don't have to think about it
        // elsewhere in your code
        char newReading = digitalRead(pin);
      
        int newstate = (reverse?!newReading:newReading);

        setNewState(newstate);
        
        runCallback();
      }
      virtual void runCallback(){
      //run callback if changed and pressed
        if (callback != 0 && isChanged() && isPressed()){
          callback( selectorChar );
        }  
      }

      int getState() {
        //update();
        return state;
      }
      bool isPressed() {
        return (getState() == LOW);
      }
  };

#endif