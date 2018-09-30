#include <avdweb_VirtualDelay.h>
#include <Servo.h>
#define LEFT_DEFAULT {9, 2, 7, false, gate_left}
#define RIGHT_DEFAULT {10, 3, 8, false, gate_right}

//The reference position for the servo
struct Gate_Ref{
  int OPEN;
  int CLOSE;
};
const Gate_Ref gate_left = {130, 50}, gate_right = {90, 170};

enum State {VACANT, OCCUPIED};
struct Device {
  int pin_servo; //the motor controlling the gate.
  int pin_switch;
  int pin_indicator; //one LED, turned on when the gate is opened, turned off otherwise
  bool occupied;
  Gate_Ref ref;
  Servo servo;
};
Device right = RIGHT_DEFAULT;
Device left = LEFT_DEFAULT;

//Exmamine if the button is pressed, which indicates the parking slot is occcupied
//@return TRUE if the button is pressedif the button is pressed, FALSE otherwise
bool check_occupied(const int& pin_switch){
  int first = digitalRead(pin_switch); // reads LOW if the button is pressed
  delay(15);
  int second = digitalRead(pin_switch); // read second time in case of glitches
  if (first != second)
    return check_occupied(pin_switch);
  else 
    return !first; //TRUE if the button is pressed
}
int LeftButtonStatus;
int RightButtonStatus = 0;
VirtualDelay singleDelay;
VirtualDelay secondDelay;

void setup()
{
  pinMode(right.pin_switch, INPUT_PULLUP);
  pinMode(right.pin_servo, OUTPUT);
  pinMode(right.pin_indicator, OUTPUT);
  digitalWrite(right.pin_indicator, LOW);
  right.servo.attach(right.pin_servo);
  right.servo.write(right.ref.CLOSE);
  delay(15);
  
  pinMode(left.pin_switch, INPUT_PULLUP);
  pinMode(left.pin_servo, OUTPUT);
  pinMode(left.pin_indicator, OUTPUT);
  digitalWrite(left.pin_indicator, LOW);
  left.servo.attach(left.pin_servo);
  left.servo.write(left.ref.CLOSE);
  delay(15);
  attachInterrupt(digitalPinToInterrupt(right.pin_switch), right_trigger, FALLING);
  attachInterrupt(digitalPinToInterrupt(left.pin_switch), left_trigger, FALLING);
  Serial.begin(115200);
}

//Open the gate if the button state is swithced from OCCUPIED to VACANT, and close the gate automatically after 2 seconds.
void loop(){
    if (RightButtonStatus == 1){
      digitalWrite(right.pin_indicator, HIGH);
      right.servo.write(right.ref.OPEN);
      singleDelay.start(2000); // calls while running are ignored
      digitalWrite(right.pin_indicator, LOW);
      RightButtonStatus = 0;
    }
    if(singleDelay.elapsed()) {
      right.servo.write(right.ref.CLOSE);    
    }
      
      
    if (LeftButtonStatus == 1){
      digitalWrite(left.pin_indicator, HIGH);
      left.servo.write(left.ref.OPEN);
      secondDelay.start(2000); // calls while running are ignored
      digitalWrite(left.pin_indicator, LOW);
      LeftButtonStatus = 0;
    }
    if(secondDelay.elapsed()) { 
    left.servo.write(left.ref.CLOSE);
    }    
}

void left_trigger(){
  LeftButtonStatus = 1;
  
}

void right_trigger(){
  RightButtonStatus = 1;
}
