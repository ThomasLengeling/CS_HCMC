#include <Bounce.h>

#define KEY_01 6
#define KEY_02 7
#define KEY_03 8
#define KEY_04 9

#define LED_01 22
#define LED_02 23


// Instantiate a Bounce object with a 100 millisecond debounce time
#define TIME_DEBOUNCE 20


const int NUM_KEYS = 4;
uint8_t key_pins[] = {KEY_01, KEY_02, KEY_03, KEY_04};

byte serial_buffer[] = {0, 0, 0, 0};
byte serial_bufferP[] =  {0, 0, 0, 0};

Bounce * bouncer[NUM_KEYS];

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  pinMode(KEY_01, INPUT);
  pinMode(KEY_02, INPUT);
  pinMode(KEY_03, INPUT);
  pinMode(KEY_04, INPUT);

  pinMode(LED_01, OUTPUT);
  pinMode(LED_02, OUTPUT);
  pinMode(13, OUTPUT );


  digitalWrite(LED_01, HIGH);
  digitalWrite(LED_02, HIGH);
  digitalWrite(13, HIGH );
  delay(500);

  digitalWrite(LED_01, LOW);
  digitalWrite(LED_02, LOW);
  digitalWrite(13, LOW );


  for (int i = 0; i < NUM_KEYS; i++) {
    bouncer[i] = new Bounce(key_pins[i], TIME_DEBOUNCE);
  }
}

void loop() {
  
  for (int i = 0; i < NUM_KEYS; i++) {
    bouncer[i]->update ( );
    serial_bufferP[i] = serial_buffer[i];
    int keyPressed =  bouncer[i]->read();
    if (keyPressed == HIGH) {
      serial_buffer[i] = 1;
      digitalWrite(LED_02, LOW );
    } else {
      serial_buffer[i] = 0;
      digitalWrite(LED_02, HIGH );
    }

  }

  //Check if therw was a change in the state of the button
  boolean foundChange = false;
  for (int i = 0; i < NUM_KEYS; i++) {
    if ( serial_bufferP[i]  != serial_buffer[i]) {
      foundChange = true;
      digitalWrite(13, HIGH );   
      break;
    }else{
      digitalWrite(13, LOW );
    }
  }

  //send serial
  if (foundChange) {
    Serial.write(serial_buffer, 4);
  }






}
