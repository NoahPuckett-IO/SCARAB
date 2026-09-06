unsigned long debounce = 200UL;   // the debounce time, increase if the output flickers

struct Button
{
  /// members
  int  id;             // number of button
  int  inPin;          // button pin
  int  outPin;         // led pin
  int  reading;        // button reading
  bool state;          // HIGH/LOW state of button
  bool previous;       // previous state
  unsigned long ttime; // time, for debouncing
};

void initializeButton(Button& b, int inPin, int outPin, int id)
{
  b.id     = id;
  b.inPin  = inPin;
  b.outPin = outPin;

  pinMode(b.inPin, INPUT_PULLUP);    // (pin#, Mode)
  pinMode(b.outPin, OUTPUT);

  Serial.printf("initializing Button %d input %d ledPin %d\n", b.id, inPin, outPin);
  b.state = LOW;
}

// this function was from example:
// https://docs.arduino.cc/built-in-examples/digital/Debounce/
void readDataAndSetLED(Button& b)
{
  b.reading = digitalRead(b.inPin);

  // if the input just went from LOW to HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the time
  if (b.reading == HIGH && b.previous == LOW && millis() - b.ttime > debounce)
  {
    if (b.state == HIGH)
      b.state = LOW;
    else
      b.state = HIGH;

    b.ttime = millis();
    digitalWrite(b.outPin, b.state);   //(pin#, STATE)  STATE = HIGH or LOW
    Serial.printf("button %d was pressed state = %d\n", b.id, b.state);
  }
  b.previous = b.reading;
}

void turnLEDoff(Button& b)
{
  digitalWrite(b.outPin, LOW);
  b.state = LOW;
}

void turnLEDoffAll(Button& b0, Button& b1, Button& b2, Button& b3, Button& b4, Button& b5)
{
  turnLEDoff(b0);
  turnLEDoff(b1);
  turnLEDoff(b2);
  turnLEDoff(b3);
  turnLEDoff(b4);
  turnLEDoff(b5);
}