//quick example of handling rotary encoders smoothly on the Arduino that has worked well for me:

//takes quadrature encoder pins with PULLUPs and returns 1 (or -1) when a step completes, 0 otherwise
//must be called frequently or inputs will be missed; it tends toward missing input over bouncing input
//for example: if getInput() is not called when the encoder is in case 0 the 'click' will not be counted
int8_t getInput(byte pinA, byte pinB) {
  //the value we will return (-1, 0, 1)
  int8_t count = 0;

  //these track the previous dial states as we turn
  static bool aStart;
  static bool bStart;
  static bool center;

  //read the pins and combine them into one value so there aren't so many if()
  byte pinState = digitalRead(pinA) << 1 + digitalRead(pinB);

  switch (pinState) {
  case 0: //both pulled LOW
    center = true; //both are activated so we are in the middle of the 'click'
    break;
  case 1: //pinA pulled LOW 
    if(center && bStart) { //if we already had pinB alone and center then we're done turning
      count = -1; //return a value
      aStart = false; //reset the previous states for the next 'click'
      bStart = false;
      center = false;
    }
    aStart = true; //if we're just starting then we started with pinA
    break;
  case 2: //pinB pulled LOW
    if(center && aStart) { //same as case 1 but for pinB
      count = 1;
      aStart = false;
      bStart = false;
      center = false;
    }
    bStart = true;
    break;
  case 3: //neither pulled LOW
    aStart = false; //here we are sitting between 'clicks' at rest so keep everything reset
    bStart = false;
    center = false;
    break;
  }

  return count; //returns 0 almost all the time, -1 or 1 when a valid 'click' has been detected.
}

//usage example:

void setup() {
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  static int value = 25;
  static int lastValue = value;

  value += getInput(4, 5);

  if(value != lastValue) {
    Serial.println(value);
    lastValue = value;
  }
}
