/*
 * A program that uses a Teensyduino 4.0, 2 SN74HC595N Shift registers in a daisychain setup and 2 SMA4105 Seven Segment Displays to roll dice.
 * Supports the following dice: D20, D12, D10, D8, D6, and D4.
 * @author Aram Alexanian
 */

//Pins used
enum PINS {
  DATA_PIN = 17,
  LATCH_PIN = 15,
  CLOCK_PIN = 14,
  PB_PIN = 10,
  PB2_PIN = 9
};
//Outputs for seven segment     0           1           2           3           4           5           6           7           8           9
int sevenSegmentDisplay[10] = { 0b00000011, 0b10011111, 0b00100101, 0b00001101, 0b10011001, 0b01001001, 0b01000001, 0b00011111, 0b00000001, 0b00011001 };

//Different dice that can be rolled
int dice[6] = { 20, 12, 10, 8, 6, 4 };
int currentDie = 0;

//Tens and ones sent to the seven segment
volatile int tens = 2;
volatile int ones = 0;
volatile bool start = HIGH;

volatile int currentTime = 0;

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);

  pinMode(PB_PIN, INPUT_PULLUP);
  attachInterrupt(PB_PIN, rollDie, FALLING);
  pinMode(PB2_PIN, INPUT_PULLUP);
  attachInterrupt(PB2_PIN, changeDie, FALLING);

  Serial.begin(115200);
}

void loop() {
}

//Check for debounce
bool bounceCheck() {
  if (currentTime == 0) {
    currentTime = millis();
  }
  if ((millis() - currentTime) > 5) {
    currentTime = 0;
    return HIGH;
  }
  return LOW;
}

//Changes the number on the seven segment display
void animate() {
  digitalWrite(LATCH_PIN, LOW);

  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, sevenSegmentDisplay[ones]);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, sevenSegmentDisplay[tens]);

  digitalWrite(LATCH_PIN, HIGH);
}

//Rolls the die selected. On first roll sets the seed to millis(). Outputs current die and roll to the serial port
void rollDie() {
  if (bounceCheck()) {
    if (start) {
      randomSeed(millis());
      start = LOW;
    }
    for (int i = 0; i < 5; i++) {
      int number = random(1, dice[currentDie]);
      tens = number / 10;
      ones = number % 10;
      animate();
      delay(100);
    }
    Serial.printf("D%d %d%d\n",dice[currentDie], tens, ones);
  }
}

//Changes the die and updates the display to the current die
void changeDie() {
  if (bounceCheck()) {
    currentDie = (currentDie + 1) % (sizeof(dice)/sizeof(dice[0]));
    tens = dice[currentDie] / 10;
    ones = dice[currentDie] % 10;
    animate();
  }
}
