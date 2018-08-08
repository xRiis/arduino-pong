// ************ PONG ************
// Written by Riis Card for E40 M
// at Stanford University
// ******************************
// Pong, according to Wikipedia,
// is one of the earliest video
// games -- a simple table tennis
// sports game featuring 2D
// graphics.  This is my version
// of it on an Arduino computer,
// using one paddle and ball.
// This setup is designed to be
// used on an Adafruit Mini
// Metro hooked up to an 8x8
// array of LEDs, each of which
// could be controlled by an
// NMOS transistor attached to
// the anode and a resistor
// attached to the cathode. As
// a result, the computer must
// cycle through each row.
// Otherwise, unwanted LEDs will
// turn on. However, this code
// should work with any other
// setup. In E40 M, we used a
// custom PCB to make life a bit
// easier, but a breadboard or a
// matrix from companies like
// Adafruit should work fine.
// The code will need to be
// adapted to your own libraries
// or hardware, though!
// ******************************

// Define arrays for the anode (+) and cathode (-) wire pins.
const byte ANODE_PINS[8] = {6, 7, 8, 9, 10, 11, 12, 13};
const byte CATHODE_PINS[8] = {A3, A2, A1, A0, 2, 3, 4, 5};
const byte INPUT_LEFT = A5;
const byte INPUT_RIGHT = A4;

// We don't want to accidentally register multiple inputs, so this variable will serve as milliseconds for debouncing our buttons
const int DEBOUNCE_DELAY = 100;

// To be safe, I'm going to register each part of the paddle outside of setup
int PADDLE_LEFT = 0;
int PADDLE_MIDDLE = 0;
int PADDLE_RIGHT = 0;

// Same with the ball's position
int BALL_X = 0;
int BALL_Y = 0;

// Part of a lazy solution to delay movement of ball
unsigned long COUNTER = 0;

// Determine ball's direction
signed int BALL_DX = 1;
signed int BALL_DY = -1; 


void setup() {

  // For the purposes of "random," we need to define a randomSeed. Arduino doesn't really produce very random numbers.  However, the higher the magnitude of the seed, the more random it appears.
  randomSeed(30);
  
  // Turn all pins off
  for (byte i = 0; i < 8; i++) {
    pinMode(ANODE_PINS[i], OUTPUT);
    pinMode(CATHODE_PINS[i], OUTPUT);
  }
  
  for (byte j = 0; j < 8; j++) {

    digitalWrite(ANODE_PINS[j], HIGH);
    digitalWrite(CATHODE_PINS[j], HIGH);
    
  }

  // Register both input pins as INPUT_PULLUP
  pinMode(INPUT_LEFT, INPUT_PULLUP);
  pinMode(INPUT_RIGHT, INPUT_PULLUP);

  // Register starting position of paddle
  PADDLE_LEFT = 3;
  PADDLE_MIDDLE = 4;
  PADDLE_RIGHT = 5;

  //Generate a random starting spot for the ball
  BALL_X = random(1,7);
  BALL_Y = random(5,8);

  Serial.begin(115200);
  
}

// I'm writing this function to make it easier to keep track of and light the paddle
void lightPaddle() {

  // Make sure nothing is turned on beforehand
  for (byte a = 0; a < 8; a++) {

    digitalWrite(ANODE_PINS[a], HIGH);

    for (byte b = 0; b < 8; b++) {

      digitalWrite(CATHODE_PINS[b], HIGH);
      
    }
    
  }

  // Since the paddle is only limited to the bottom row of the matrix, we might as well just turn it on
  digitalWrite(ANODE_PINS[0], LOW);

  // Turn on the correct cathodes to actually light the paddle
  digitalWrite(CATHODE_PINS[PADDLE_LEFT], LOW);
  digitalWrite(CATHODE_PINS[PADDLE_MIDDLE], LOW);
  digitalWrite(CATHODE_PINS[PADDLE_RIGHT], LOW);
  
}

// Make sure that the ball is lit as well.  Basically a clone of lightPaddle
void lightBall() {

  for (byte c = 0; c < 8; c++) {

    digitalWrite(ANODE_PINS[c], HIGH);

    for (byte d = 0; d < 8; d++) {

      digitalWrite(CATHODE_PINS[d], HIGH);
      
    }
    
  }

  digitalWrite(ANODE_PINS[BALL_Y], LOW);
  digitalWrite(CATHODE_PINS[BALL_X], LOW);
  
}

void checkForButtonInput() {

  // For the purposes of debouncing our buttons, we need to keep track of time (in milliseconds)
  unsigned long now = millis();

  static byte button_state_left = HIGH;
  static byte last_reading_left = HIGH;

  static byte button_state_right = HIGH;
  static byte last_reading_right = HIGH;

  static long last_reading_change = 0;
  byte reading_left = digitalRead(INPUT_LEFT);
  byte reading_right = digitalRead(INPUT_RIGHT);

  if ((COUNTER % 80) == 0) {
  
    if (now - last_reading_change > DEBOUNCE_DELAY) {
  
      if ((reading_left == LOW) && (button_state_left == HIGH)) {

        if (PADDLE_LEFT > 0) {
        
          PADDLE_LEFT = PADDLE_LEFT - 1;
          PADDLE_MIDDLE = PADDLE_MIDDLE - 1;
          PADDLE_RIGHT = PADDLE_RIGHT - 1;

        }
        
      }
  
      if ((reading_right == LOW) && (button_state_right == HIGH)) {

        if (PADDLE_RIGHT < 7) {
        
          PADDLE_LEFT = PADDLE_LEFT + 1;
          PADDLE_MIDDLE = PADDLE_MIDDLE + 1;
          PADDLE_RIGHT = PADDLE_RIGHT + 1;

        }
        
      }
      
    }
    
  }

}

// When the ball goes somewhere it shouldn't be, the game should end and restart, but only after displaying the ball's incorrect position
void endGame() {

  for (byte c = 0; c < 8; c++) {

    digitalWrite(ANODE_PINS[c], HIGH);

    for (byte d = 0; d < 8; d++) {

      digitalWrite(CATHODE_PINS[d], HIGH);
      
    }
    
  }

  // Pause the game and blink the ball's LED three times
  for (byte e = 0; e < 3; e++) {
  
    digitalWrite(ANODE_PINS[BALL_Y], LOW);
    digitalWrite(CATHODE_PINS[BALL_X], LOW);
    delay(500);
  
    digitalWrite(ANODE_PINS[BALL_Y], HIGH);
    digitalWrite(CATHODE_PINS[BALL_X], HIGH);
    delay(500);

  }

  // Small pause. For aesthetic reasons.
  delay(1000);
  
  // Re-define everything involved as starting values
  PADDLE_LEFT = 3;
  PADDLE_MIDDLE = 4;
  PADDLE_RIGHT = 5;

  BALL_X = random(1,7);
  BALL_Y = random(5,8);

  BALL_DX = 1;
  BALL_DY = -1;
  
}

// The whole point of Pong is moving the ball... so let's do that.
void moveBall(int l, int m, int r) {

  // 135 repetitions of loop have to pass before the ball moves
  if ((COUNTER % 135) == 0) {
  
    // Change the position of the ball
    BALL_X = BALL_X + BALL_DX;
    BALL_Y = BALL_Y + BALL_DY;
  
    // Check for collision with wall or paddle

     if (((BALL_X == 7) && (BALL_Y == (0 || 1))) && (r == 7)) {

      BALL_DX = -1;
      BALL_DY = 1;

    } else if (((BALL_X == 0) && (BALL_Y == (0 || 1))) && (l == 0)) {

      BALL_DX = 1;
      BALL_DY = 1;

    } else if ((BALL_Y == 7) && (BALL_X == 7)) {

      BALL_DX = -1;
      BALL_DY = -1;

    } else if ((BALL_Y == 7) && (BALL_X == 0)) {

      BALL_DX = 1;
      BALL_DY = -1;
      
    } else if ((BALL_Y == (0 || 1)) && ((BALL_X == l) || (BALL_X == m) || (BALL_X == r))) {

      BALL_DX = BALL_DX;
      BALL_DY = -BALL_DY;

    } else if (((BALL_X == (l - 1)) && (BALL_Y == (0 || 1))) || ((BALL_X == r + 1) && (BALL_Y == (0 || 1)))) {

      BALL_DX = -BALL_DX;
      BALL_DY = -BALL_DY;

    } else if (BALL_Y == 0) {
  
      BALL_DX = 0;
      BALL_DY = 0;

      endGame();
      
    } else if ((BALL_X == 0) || (BALL_X == 7)) {
  
      BALL_DX = -BALL_DX;
      
    } else if (BALL_Y == 7) {
  
      BALL_DY = -BALL_DY;
      
    } else if (!(-1 < BALL_X < 8) || !(-1 < BALL_Y < 8)) {

      BALL_DX = -BALL_DX;
      BALL_DY = -BALL_DY;
      
    }

  }
  
}

void loop() {  

  // I implemented this counter method to make the ball and paddle move more slowly while still being able to accept input in loop
  // There is probably a much better method to do this but I don't currently know of one
  COUNTER++;
  
  checkForButtonInput();
  lightPaddle();
  lightBall();
  moveBall(PADDLE_LEFT, PADDLE_MIDDLE, PADDLE_RIGHT);
  
}
