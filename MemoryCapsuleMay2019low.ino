#include <TimerOne.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

//#define PINHand 12 //defining the PWM pin
//#define PINCircle 53
//#define N_LEDSHand 20 //number of LED units on the strip
//#define N_LEDSCircle 12
#define ctsPin 13

// define the pins used
//#define CLK 52      // SPI Clock, shared with SD card
//#define MISO 50      // Input data, from VS1053/SD card
//#define MOSI 51      // Output data, to VS1053/SD card
// Connect CLK, MISO and MOSI to hardware SPI pins.
// See http://arduino.cc/en/Reference/SPI "Connections"

// These are the pins used for the breakout example
#define BREAKOUT_RESET  25     // VS1053 reset pin (output)
#define BREAKOUT_CS     24     // VS1053 chip select pin (output)
#define BREAKOUT_DCS    23      // VS1053 Data/command select pin (output)
// These are the pins used for the music maker shield
#define SHIELD_RESET  -1      // VS1053 reset pin (unused!)
#define SHIELD_CS     22    // VS1053 chip select pin (output) //not used
#define SHIELD_DCS    21      // VS1053 Data/command select pin (output) //not used

// These are common pins between breakout and shield
#define CARDCS 19    // Card chip select pin
// DREQ should be an Int pin, see http://arduino.cc/en/Reference/attachInterrupt
#define DREQ 18       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer =
  // create breakout-example object!
  Adafruit_VS1053_FilePlayer(BREAKOUT_RESET, BREAKOUT_CS, BREAKOUT_DCS, DREQ, CARDCS);
// create shield-example object!
//  Adafruit_VS1053_FilePlayer(SHIELD_RESET, SHIELD_CS, SHIELD_DCS, DREQ, CARDCS);

//Adafruit_NeoPixel stripHand = Adafruit_NeoPixel(N_LEDSHand, PINHand);
//Adafruit_NeoPixel stripCircle = Adafruit_NeoPixel(N_LEDSCircle, PINCircle);

int sensorReading = 0;
bool circleOff = false;
long musicRand;

unsigned char channel_1 = 4;  // Output to Opto Triac pin, channel 1
unsigned char channel_2 = 5;  // Output to Opto Triac pin, channel 2
unsigned char channel_3 = 6;  // Output to Opto Triac pin, channel 3
unsigned char channel_4 = 7;  // Output to Opto Triac pin, channel 4
unsigned char channel_5 = 8;  // Output to Opto Triac pin, channel 5
unsigned char channel_6 = 9;  // Output to Opto Triac pin, channel 6
unsigned char channel_7 = 10; // Output to Opto Triac pin, channel 7
unsigned char channel_8 = 11; // Output to Opto Triac pin, channel 8


unsigned char CH1, CH2, CH3, CH4, CH5, CH6, CH7, CH8;
unsigned char CHANNEL_SELECT1;
unsigned char i = 0;

// overall state  designating which act the sequence is currently in
int act = 0;
//int preRan;

int j1 = 5;
int j2 = 5;
int j3 = 5;
int j4 = 5;
int j5 = 5;
int j6 = 5;
int j7 = 5;
int j8 = 5;

//initialize values for act 3
int ch1Val;
int ch2Val;
int ch3Val;
int ch4Val;
int ch5Val;
int ch6Val;
int ch7Val;
int ch8Val;


int chanArray[] = {5, 128};

volatile unsigned char clock_tick; // variable for Timer1

unsigned char low = 64;
unsigned char high = 5;
unsigned char off = 128;
long preTime;
long ptime;
long ptime1;
long ptime2;
long overallptime;
long endingptime;
long finalptime;
boolean forward = false;
int fsr1Pin = 0; // FSR is connected to analog 0
int fsr2Pin = 1; // FSR is connected to analog 1
int fsr3Pin = 2; // FSR is connected to analog 2
int fsr4Pin = 3; // FSR is connected to analog 3
int fsr5Pin = 4; // FSR is connected to analog 4

int fsr1Reading;
int fsr2Reading;
int fsr3Reading;
int fsr4Reading;
int fsr5Reading;

int fsr1State = 0;
int fsr2State = 0;
int fsr3State = 0;
int fsr4State = 0;
int fsr5State = 0;

int prevfsr1State = 0;
int prevfsr2State = 0;
int prevfsr3State = 0;
int prevfsr4State = 0;
int prevfsr5State = 0;

int fsr1ReadingState = 0;
int fsr2ReadingState = 0;
int fsr3ReadingState = 0;
int fsr4ReadingState = 0;
int fsr5ReadingState = 0;

int prevfsr1ReadingState = 0;
int prevfsr2ReadingState = 0;
int prevfsr3ReadingState = 0;
int prevfsr4ReadingState = 0;
int prevfsr5ReadingState = 0;

boolean bulb1Dim = false;
boolean bulb2Dim = false;
boolean bulb3Dim = false;
boolean bulb4Dim = false;
boolean bulb5Dim = false;
boolean bulb6Dim = false;
boolean bulb7Dim = false;
boolean bulb8Dim = false;

boolean preAnimation = false;
boolean openingAnimation = false;
boolean changeState = false;

int soloState; //Animation state for when there is no user in the vicinity
int state1; //first user involved animation state
int state2; //second user involved animation state
int finalState; //final user involved animation state

int Delay1 = 25;
int Delay2 = 25;
int Delay3 = 25;
int Delay4 = 25;
int Delay5 = 25;
long overallDelay = 100000;
long pre_delay_time = 300;
long finalDelay = 8000;
long delay_time1 = 300;
long delay_time2 = 500;
long endingDelay = 300;


void setup() {
  //  stripHand.begin();
  //  stripHand.setBrightness(50);
  //  stripHand.show();
  // stripCircle.begin();
  //stripCircle.setBrightness(50);
  //stripCircle.show();

  //does randomseeding here mess up the millis
  //it shouldnt just want to make sure


  Serial.begin(9600);
  if (! musicPlayer.begin()) { // initialise the music player
    Serial.println(F("Couldn't find VS1053, do you have the right pins defined?"));
    while (1);
  }
  Serial.println(F("VS1053 found"));

  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }

  // list files
  //  printDirectory(SD.open("/"), 0);

  //audio's volume set
  musicPlayer.setVolume(1, 1);

  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);

  if (! musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT)) {
    Serial.println(F("DREQ pin is not an interrupt pin"));
  }

  pinMode(ctsPin, INPUT);

  pinMode(channel_1, OUTPUT);// Set AC Load pin as output
  pinMode(channel_2, OUTPUT);// Set AC Load pin as output
  pinMode(channel_3, OUTPUT);// Set AC Load pin as output
  pinMode(channel_4, OUTPUT);// Set AC Load pin as output
  pinMode(channel_5, OUTPUT);// Set AC Load pin as output
  pinMode(channel_6, OUTPUT);// Set AC Load pin as output
  pinMode(channel_7, OUTPUT);// Set AC Load pin as output
  pinMode(channel_8, OUTPUT);// Set AC Load pin as output

  attachInterrupt(1, zero_crosss_int, RISING);
  Timer1.initialize(83); // set a timer of length 100 microseconds for 200Hz or 83 microseconds for 60Hz;
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
  ptime = millis();
}

void timerIsr()
{
  clock_tick++;

  if (CH1 == clock_tick)
  {
    digitalWrite(channel_1, HIGH); // triac firing
    delayMicroseconds(8.33); // triac On propogation delay (for 60Hz use 8.33)
    digitalWrite(channel_1, LOW); // triac Off
  }

  if (CH2 == clock_tick)
  {
    digitalWrite(channel_2, HIGH); // triac firing
    delayMicroseconds(8.33); // triac On propogation delay (for 60Hz use 8.33)
    digitalWrite(channel_2, LOW); // triac Off
  }

  if (CH3 == clock_tick )
  {
    digitalWrite(channel_3, HIGH); // triac firing
    delayMicroseconds(8.33); // triac On propogation delay (for 60Hz use 8.33)
    digitalWrite(channel_3, LOW); // triac Off
  }

  if (CH4 == clock_tick)
  {
    digitalWrite(channel_4, HIGH); // triac firing
    delayMicroseconds(8.33); // triac On propogation delay (for 60Hz use 8.33)
    digitalWrite(channel_4, LOW); // triac Off
  }

  if (CH5 == clock_tick)
  {
    digitalWrite(channel_5, HIGH); // triac firing
    delayMicroseconds(8.33); // triac On propogation delay (for 60Hz use 8.33)
    digitalWrite(channel_5, LOW); // triac Off
  }

  if (CH6 == clock_tick)
  {
    digitalWrite(channel_6, HIGH); // triac firing
    delayMicroseconds(8.33); // triac On propogation delay (for 60Hz use 8.33)
    digitalWrite(channel_6, LOW); // triac Off
  }

  if (CH7 == clock_tick)
  {
    digitalWrite(channel_7, HIGH); // triac firing
    delayMicroseconds(8.33); // triac On propogation delay (for 60Hz use 8.33)
    digitalWrite(channel_7, LOW); // triac Off
  }

  if (CH8 == clock_tick)
  {
    digitalWrite(channel_8, HIGH); // triac firing
    delayMicroseconds(8.33); // triac On propogation delay (for 60Hz use 8.33)
    digitalWrite(channel_8, LOW); // triac Off
  }

}



void zero_crosss_int() // function to be fired at the zero crossing to dim the light
{
  // Every zerocrossing interrupt: For 200Hz (1/2 Cycle) => 10ms ; For 60Hz (1/2 Cycle) => 8.33ms
  // 10ms=10000us , 8.33ms=8330us

  clock_tick = 0;
}

void loop() {
  randomSeed(millis());
  sensorReading = digitalRead(ctsPin);
  Serial.println(sensorReading);
  actZeroState();
  preUserState();
  //  actOneState();
  //  actTwoState();
  //  actThreeState();

  //  fsr1StateChanges();
  //  fsr2StateChanges();
  //  fsr3StateChanges();
  //  fsr4StateChanges();
  //  fsr5StateChanges();


}

///////////////////FOR ALL FSR STATE CHANGE FUNCTIONS WE NEED TO TAKE OUT THE LIGHTS THAT ARENT USED ANYMORE.

//BULBS 4 7 9 CONTROLLED

void fsr1StateChanges() {
  //  Serial.println(fsr1State);
  //  Serial.println(fsr1ReadingState);
  //  Serial.println(fsr1Reading);

  if (fsr1Reading >= 550) {
    fsr1ReadingState = 1;
  }

  if (fsr1Reading < 550) {
    fsr1ReadingState = 0;
  }

  if (fsr1ReadingState == 1) {
    if (prevfsr1ReadingState == 1) {}
    if (prevfsr1ReadingState == 0) {
      prevfsr1ReadingState = 1;
    }
  }

  if (fsr1ReadingState == 0) {
    if (prevfsr1ReadingState == 0) {}
    if (prevfsr1ReadingState == 1) {
      fsr1State ++;
      prevfsr1ReadingState = 0;
      //      Serial.println(fsr1State);
    }
  }

  // make sure that the correct state is reached for the first finger's fsr
  if (fsr1State == 0 || fsr1State == 1 ) {

    //if the finger isn't putting maximum pressure then the pulsing is mapped to the fsr



    if (fsr1Reading > 550 && fsr1Reading < 800 && !bulb1Dim ) {
      CH7 = 128 - fsr1Reading / 8;
    }


    //if the finger is putting max pressure then the pulsing becomes automated
    if (fsr1Reading >= 800 && bulb7Dim == false) {
      bulb7Dim = true;
      ptime = millis();
    }

    if (bulb7Dim) {
      if (millis() > ptime + Delay1) {
        CH7 = j7;

        if (j7 <= 5) {
          forward = true;
        }

        if (j7 >= 128) {
          forward = false;
        }

        if (forward) {
          j7++;
        } else {
          j7--;
        }
        ptime = millis();
      }
    }
  }

  if (fsr1State == 1 ) {

    //if the finger isn't putting maximum pressure then the pulsing is mapped to the fsr
    if (fsr1Reading > 550 && fsr1Reading < 800 && !bulb4Dim ) {
      CH4 = 128 - fsr1Reading / 8;
    }
    //if the finger is putting max pressure then the pulsing becomes automated
    if (fsr1Reading >= 800 && bulb4Dim == false) {
      //Serial.println("made it to state 1");

      bulb4Dim = true;
      ptime = millis();
      //      Serial.print("state 1 time");
      //      Serial.println(ptime);
    }

    if (bulb4Dim) {

      if (millis() > ptime + Delay1) {
        changeState = true;
        CH4 = j4;

        if (j4 <= 5) {
          forward = true;
        }

        if (j4 >= 128) {
          forward = false;
        }

        if (forward) {
          j4++;
        } else {
          j4--;
        }
        ptime = millis();
      }
    }
  }


  //if (fsr1State == 2) {

  //if the finger isn't putting maximum pressure then the pulsing is mapped to the fsr
  //    if (fsr1Reading > 200 && fsr1Reading < 700 && !bulb9Dim ) {
  //      CH9 = 128 - fsr1Reading / 8;
  //    }
  //    //if the finger is putting max pressure then the pulsing becomes automated
  //    if (fsr1Reading >= 700 && bulb9Dim == false) {
  //      bulb9Dim = true;
  //      ptime = millis();
  //    }
  //
  //    if (bulb9Dim) {
  //      if (millis() > ptime + Delay1) {
  //        changeState = true;
  //        CH9 = j9;
  //
  //        if (j9 <= 5) {
  //          forward = true;
  //        }
  //
  //        if (j9 >= 128) {
  //          forward = false;
  //        }
  //
  //        if (forward) {
  //          j9++;
  //        } else {
  //          j9--;
  //        }
  //        ptime = millis();
  //      }
  //    }
  //  }


  fsr1State = fsr1State % 2;

}


//-----------------------------------------------------------------------------------------------------------

//CONTROLS BULBS 5 11 14

void fsr2StateChanges() {

  if (fsr2Reading >= 550) {
    fsr2ReadingState = 1;
  }

  if (fsr2Reading < 550) {
    fsr2ReadingState = 0;
  }

  if (fsr2ReadingState == 1) {
    if (prevfsr2ReadingState == 1) {
    }
    if (prevfsr2ReadingState == 0) {
      prevfsr2ReadingState = 1;
    }
  }

  if (fsr2ReadingState == 0) {
    if (prevfsr2ReadingState == 0) {}
    if (prevfsr2ReadingState == 1) {
      fsr2State ++;
      prevfsr2ReadingState = 0;
    }
  }

  // make sure that the correct state is reached for the first finger's fsr
  if (fsr2State == 0 || fsr2State == 1 ) {

    //if the finger isn't putting maximum pressure then the pulsing is mapped to the fsr
    if (fsr2Reading > 550 && fsr2Reading < 800 && !bulb5Dim ) {
      CH5 = 128 - fsr2Reading / 8;
    }
    //if the finger is putting max pressure then the pulsing becomes automated
    if (fsr2Reading >= 800 && bulb5Dim == false) {
      bulb5Dim = true;
      ptime = millis();
    }

    if (bulb5Dim) {
      if (millis() > ptime + Delay2) {
        CH5 = j5;

        if (j5 <= 5) {
          forward = true;
        }

        if (j5 >= 128) {
          forward = false;
        }

        if (forward) {
          j5++;
        } else {
          j5--;
        }
        ptime = millis();
      }
    }
  }

  //  if (fsr2State == 1 || fsr2State == 2) {
  //
  //    //if the finger isn't putting maximum pressure then the pulsing is mapped to the fsr
  //    if (fsr2Reading > 200 && fsr2Reading < 700 && !bulb14Dim ) {
  //      CH14 = 128 - fsr2Reading / 8;
  //    }
  //    //if the finger is putting max pressure then the pulsing becomes automated
  //    if (fsr2Reading >= 700 && bulb14Dim == false) {
  //      //Serial.println("made it to state 1");
  //
  //      bulb14Dim = true;
  //      ptime = millis();
  //      //      Serial.print("state 1 time");
  //      //      Serial.println(ptime);
  //    }
  //
  //    if (bulb14Dim) {
  //
  //      if (millis() > ptime + Delay2) {
  //        changeState = true;
  //        CH14 = j14;
  //
  //        if (j14 <= 5) {
  //          forward = true;
  //        }
  //
  //        if (j14 >= 128) {
  //          forward = false;
  //        }
  //
  //        if (forward) {
  //          j14++;
  //        } else {
  //          j14--;
  //        }
  //        ptime = millis();
  //      }
  //    }
  //  }


  //  if (fsr2State == 2) {
  //
  //    //if the finger isn't putting maximum pressure then the pulsing is mapped to the fsr
  //    if (fsr2Reading > 200 && fsr2Reading < 800 && !bulb5Dim ) {
  //      CH5 = 128 - fsr2Reading / 8;
  //    }
  //    //if the finger is putting max pressure then the pulsing becomes automated
  //    if (fsr2Reading >= 800 && bulb5Dim == false) {
  //      bulb5Dim = true;
  //      ptime = millis();
  //    }
  //
  //    if (bulb5Dim) {
  //      if (millis() > ptime + Delay2) {
  //        changeState = true;
  //        CH5 = j5;
  //
  //        if (j5 <= 5) {
  //          forward = true;
  //        }
  //
  //        if (j5 >= 128) {
  //          forward = false;
  //        }
  //
  //        if (forward) {
  //          j5++;
  //        } else {
  //          j5--;
  //        }
  //        ptime = millis();
  //      }
  //    }
  //  }


  fsr2State = fsr2State % 2;

}

//-----------------------------------------------------------------------------------------------------------------

//CONTROLS BULBS 2 10 15

void fsr3StateChanges() {

  if (fsr3Reading >= 550) {
    fsr3ReadingState = 1;
  }

  if (fsr3Reading < 550) {
    fsr3ReadingState = 0;
  }

  if (fsr3ReadingState == 1) {
    if (prevfsr3ReadingState == 1) {}
    if (prevfsr3ReadingState == 0) {
      prevfsr3ReadingState = 1;
    }
  }

  if (fsr3ReadingState == 0) {
    if (prevfsr3ReadingState == 0) {}
    if (prevfsr3ReadingState == 1) {
      fsr3State ++;
      prevfsr3ReadingState = 0;
    }
  }

  // make sure that the correct state is reached for the first finger's fsr
  if (fsr3State == 0 || fsr3State == 1 ) {

    //if the finger isn't putting maximum pressure then the pulsing is mapped to the fsr
    if (fsr3Reading > 550 && fsr3Reading < 800 && !bulb2Dim ) {
      CH2 = 128 - fsr3Reading / 8;
    }
    //if the finger is putting max pressure then the pulsing becomes automated
    if (fsr3Reading >= 800 && bulb2Dim == false) {
      bulb2Dim = true;
      ptime = millis();
    }

    if (bulb2Dim) {
      if (millis() > ptime + Delay3) {
        CH2 = j2;

        if (j2 <= 5) {
          forward = true;
        }

        if (j2 >= 128) {
          forward = false;
        }

        if (forward) {
          j2++;
        } else {
          j2--;
        }
        ptime = millis();
      }
    }
  }

  //  if (fsr3State == 1 || fsr3State == 2) {
  //
  //    //if the finger isn't putting maximum pressure then the pulsing is mapped to the fsr
  //    if (fsr3Reading > 200 && fsr3Reading < 700 && !bulb10Dim ) {
  //      CH10 = 128 - fsr3Reading / 8;
  //    }
  //    //if the finger is putting max pressure then the pulsing becomes automated
  //    if (fsr3Reading >= 700 && bulb10Dim == false) {
  //      //Serial.println("made it to state 1");
  //
  //      bulb10Dim = true;
  //      ptime = millis();
  //      //      Serial.print("state 1 time");
  //      //      Serial.println(ptime);
  //    }
  //
  //    if (bulb10Dim) {
  //
  //      if (millis() > ptime + Delay3) {
  //        changeState = true;
  //        CH10 = j10;
  //
  //        if (j10 <= 5) {
  //          forward = true;
  //        }
  //
  //        if (j10 >= 128) {
  //          forward = false;
  //        }
  //
  //        if (forward) {
  //          j10++;
  //        } else {
  //          j10--;
  //        }
  //        ptime = millis();
  //      }
  //    }
  //  }


  //  if (fsr3State == 2) {
  //
  //    //if the finger isn't putting maximum pressure then the pulsing is mapped to the fsr
  //    if (fsr3Reading > 200 && fsr3Reading < 700 && !bulb2Dim ) {
  //      CH2 = 128 - fsr3Reading / 8;
  //    }
  //    //if the finger is putting max pressure then the pulsing becomes automated
  //    if (fsr3Reading >= 700 && bulb2Dim == false) {
  //      bulb2Dim = true;
  //      ptime = millis();
  //    }
  //
  //    if (bulb2Dim) {
  //      if (millis() > ptime + Delay3) {
  //        changeState = true;
  //        CH2 = j2;
  //
  //        if (j2 <= 5) {
  //          forward = true;
  //        }
  //
  //        if (j2 >= 128) {
  //          forward = false;
  //        }
  //
  //        if (forward) {
  //          j2++;
  //        } else {
  //          j2--;
  //        }
  //        ptime = millis();
  //      }
  //    }
  //  }


  fsr3State = fsr3State % 2;

}

//-----------------------------------------------------------------------------------------------------------------

//CONTROLS BULBS 1 3 13

void fsr4StateChanges() {

  if (fsr4Reading >= 550) {
    fsr4ReadingState = 1;
  }

  if (fsr4Reading < 550) {
    fsr4ReadingState = 0;
  }

  if (fsr4ReadingState == 1) {
    if (prevfsr4ReadingState == 1) {}
    if (prevfsr4ReadingState == 0) {
      prevfsr4ReadingState = 1;
    }
  }

  if (fsr4ReadingState == 0) {
    if (prevfsr4ReadingState == 0) {}
    if (prevfsr4ReadingState == 1) {
      fsr4State ++;
      prevfsr4ReadingState = 0;
    }
  }

  // make sure that the correct state is reached for the first finger's fsr
  if (fsr4State == 0 || fsr4State == 1 ) {

    //if the finger isn't putting maximum pressure then the pulsing is mapped to the fsr
    if (fsr4Reading > 550 && fsr4Reading < 800 && !bulb1Dim ) {
      CH1 = 128 - fsr4Reading / 8;
    }
    //if the finger is putting max pressure then the pulsing becomes automated
    if (fsr4Reading >= 800 && bulb1Dim == false) {
      bulb1Dim = true;
      ptime = millis();
    }

    if (bulb1Dim) {
      if (millis() > ptime + Delay4) {
        CH1 = j1;

        if (j1 <= 5) {
          forward = true;
        }

        if (j1 >= 128) {
          forward = false;
        }

        if (forward) {
          j1++;
        } else {
          j1--;
        }
        ptime = millis();
      }
    }
  }

  //  if (fsr4State == 1 || fsr4State == 2) {
  //
  //    //if the finger isn't putting maximum pressure then the pulsing is mapped to the fsr
  //    if (fsr4Reading > 200 && fsr4Reading < 700 && !bulb13Dim ) {
  //      CH13 = 128 - fsr4Reading / 8;
  //    }
  //    //if the finger is putting max pressure then the pulsing becomes automated
  //    if (fsr4Reading >= 700 && bulb13Dim == false) {
  //      //Serial.println("made it to state 1");
  //
  //      bulb13Dim = true;
  //      ptime = millis();
  //
  //    }
  //
  //    if (bulb13Dim) {
  //
  //      if (millis() > ptime + Delay4) {
  //        changeState = true;
  //        CH13 = j13;
  //
  //        if (j13 <= 5) {
  //          forward = true;
  //        }
  //
  //        if (j13 >= 128) {
  //          forward = false;
  //        }
  //
  //        if (forward) {
  //          j13++;
  //        } else {
  //          j13--;
  //        }
  //        ptime = millis();
  //      }
  //    }
  //  }


  if (fsr4State == 1) {

    //if the finger isn't putting maximum pressure then the pulsing is mapped to the fsr
    if (fsr4Reading > 550 && fsr4Reading < 800 && !bulb3Dim ) {
      CH3 = 128 - fsr4Reading / 8;
    }
    //if the finger is putting max pressure then the pulsing becomes automated
    if (fsr4Reading >= 800 && bulb3Dim == false) {
      bulb3Dim = true;
      ptime = millis();
    }

    if (bulb3Dim) {
      if (millis() > ptime + Delay4) {
        changeState = true;
        CH3 = j3;

        if (j3 <= 5) {
          forward = true;
        }

        if (j3 >= 128) {
          forward = false;
        }

        if (forward) {
          j3++;
        } else {
          j3--;
        }
        ptime = millis();
      }
    }
  }


  fsr4State = fsr4State % 2;

}

//-------------------------------------------------------------------------------------------------------------------------

//CONTROLS BULBS 6 8 12

void fsr5StateChanges() {

  if (fsr5Reading >= 550) {
    fsr5ReadingState = 1;
  }

  if (fsr5Reading < 550) {
    fsr5ReadingState = 0;
  }

  if (fsr5ReadingState == 1) {
    if (prevfsr5ReadingState == 1) {}
    if (prevfsr5ReadingState == 0) {
      prevfsr5ReadingState = 1;
    }
  }

  if (fsr5ReadingState == 0) {
    if (prevfsr5ReadingState == 0) {}
    if (prevfsr5ReadingState == 1) {
      fsr5State ++;
      prevfsr5ReadingState = 0;
    }
  }

  // make sure that the correct state is reached for the first finger's fsr
  if (fsr5State == 0 || fsr5State == 1 ) {

    //if the finger isn't putting maximum pressure then the pulsing is mapped to the fsr
    if (fsr5Reading > 550 && fsr5Reading < 800 && !bulb6Dim ) {
      CH6 = 128 - fsr5Reading / 8;
    }
    //if the finger is putting max pressure then the pulsing becomes automated
    if (fsr5Reading >= 800 && bulb6Dim == false) {
      bulb6Dim = true;
      ptime = millis();
    }

    if (bulb6Dim) {
      if (millis() > ptime + Delay5) {
        CH6 = j6;

        if (j6 <= 5) {
          forward = true;
        }

        if (j6 >= 128) {
          forward = false;
        }

        if (forward) {
          j6++;
        } else {
          j6--;
        }
        ptime = millis();
      }
    }
  }

  //  if (fsr5State == 1 || fsr5State == 2) {
  //
  //    //if the finger isn't putting maximum pressure then the pulsing is mapped to the fsr
  //    if (fsr5Reading > 200 && fsr5Reading < 700 && !bulb12Dim ) {
  //      CH12 = 128 - fsr5Reading / 8;
  //    }
  //    //if the finger is putting max pressure then the pulsing becomes automated
  //    if (fsr5Reading >= 700 && bulb12Dim == false) {
  //      //Serial.println("made it to state 1");
  //
  //      bulb12Dim = true;
  //      ptime = millis();
  //
  //    }
  //
  //    if (bulb12Dim) {
  //
  //      if (millis() > ptime + Delay5) {
  //        changeState = true;
  //        CH12 = j12;
  //
  //        if (j12 <= 5) {
  //          forward = true;
  //        }
  //
  //        if (j12 >= 128) {
  //          forward = false;
  //        }
  //
  //        if (forward) {
  //          j12++;
  //        } else {
  //          j12--;
  //        }
  //        ptime = millis();
  //      }
  //    }
  //  }


  if (fsr5State == 1) {

    //if the finger isn't putting maximum pressure then the pulsing is mapped to the fsr
    if (fsr5Reading > 550 && fsr5Reading < 800 && !bulb8Dim ) {
      CH8 = 128 - fsr5Reading / 8;
    }
    //if the finger is putting max pressure then the pulsing becomes automated
    if (fsr5Reading >= 800 && bulb8Dim == false) {
      bulb8Dim = true;
      ptime = millis();
    }

    if (bulb8Dim) {
      if (millis() > ptime + Delay5) {
        changeState = true;
        CH8 = j8;

        if (j8 <= 5) {
          forward = true;
        }

        if (j8 >= 128) {
          forward = false;
        }

        if (forward) {
          j8++;
        } else {
          j8--;
        }
        ptime = millis();
      }
    }
  }

  fsr5State = fsr5State % 2;
}

//////////////////////////////////////////////////////////Hand Interaction Code Below//////////////////////////////////////////////////////

void initialHandInteraction() {
  sensorReading = digitalRead(ctsPin);
  //stripHandOn();
  //  Serial.println(sensorReading);
  //  Serial.println("Hand interaction!");
  if (sensorReading == 0) {
    //stripCircleOn();
    //    Serial.println("Sensor read is 0!");
    preTime = millis();
    soloState = 0;
    preAnimation = true;
    act = 1;
    //    Serial.println(act);
  }

  if (sensorReading == 1 ) {
    //    Serial.println(sensorReading);
    //    Serial.println("Sensor read is 1!");
    ptime1 = millis();
    //circleOff = true;
    state1 = 0;
    preAnimation = false;
    act = 2;

    //audio player
    musicRand = random(100);
    //    musicRand = 80;
    if (musicRand <= 50) {
      musicPlayer.startPlayingFile("/track001.mp3");
    }
    else if (musicRand > 50) {
      musicPlayer.startPlayingFile("/track002.mp3");
    }
    Serial.println(musicRand);

    //    // Start playing a file, then we can do stuff while waiting for it to finish
    //    if (! musicPlayer.startPlayingFile("/track001.mp3")) {
    //      Serial.println("Could not open file track001.mp3");
    //      while (1);
    //    }
    Serial.println(F("Started playing"));

    while (musicPlayer.playingMusic) {
      // file is now playing in the 'background' so now's a good time
      // to do something else like handling LEDs or buttons :)
      actOneState();
      actTwoState();
      actThreeState();
      Serial.println(sensorReading);
    }
    Serial.println("Done playing music");
  }
}

void stripHandOn() {
  for (int i = 0; i < 20; i++ ) {
    // stripHand.setPixelColor(i, 255, 150, 50);
    //stripHand.show();
  }
}

void stripHandOff() {
  //  Serial.println("turning off pixels for hand");
  for (int i = 0; i < 20; i++ ) {
    //    stripHand.setPixelColor(i, 0);
    //    stripHand.show();
  }
}

void stripCircleOn() {
  for (int i = 0; i < 12; i++ ) {
    //    stripCircle.setPixelColor(i, 255, 150, 50);
    //  stripCircle.show();
  }
}

void stripCircleOff() {
  for (int i = 0; i < 12; i++ ) {
    // stripCircle.setPixelColor(i, 0);
    //stripCircle.show();
  }
}


void actZeroState() {
  if (act == 0) {
    CH1 = off; CH2 = off; CH3 = off; CH4 = off; CH5 = off; CH6 = off; CH7 = off; CH8 = off;

    //    Serial.println("Act 0!");
    initialHandInteraction();
  }
}

///////THIS IS A NEW FUNCTION FOR MEMORY CAPSULE///////////////
void preUserState() {
  //  if (act == 1) {
  if (preAnimation) {
    if (millis() - preTime > pre_delay_time) {
      soloState++;
      preTime = millis();
    }
    /////////////////////////////////Front to back Wave Pattern/////////////////////////////////
    if (soloState == 0) {
      CH1 = low; CH2 = low; CH3 = low; CH4 = off; CH5 = off; CH6 = off; CH7 = off; CH8 = off;
    }
    if (soloState == 1) {
      CH1 = off; CH2 = off; CH3 = off; CH4 = low; CH5 = low; CH6 = off; CH7 = off; CH8 = off;
    }
    if (soloState == 2) {
      CH1 = off; CH2 = off; CH3 = off; CH4 = off; CH5 = off; CH6 = low; CH7 = low; CH8 = low;
    }
    if (soloState == 3) {
      CH1 = off; CH2 = off; CH3 = off; CH4 = off; CH5 = off; CH6 = off; CH7 = off; CH8 = off;
      if (sensorReading == 1 ) {
        //    Serial.println(sensorReading);
        //    Serial.println("Sensor read is 1!");
        ptime1 = millis();
        //circleOff = true;
        state1 = 0;
        preAnimation = false;
        act = 2;

        //audio player
        musicRand = random(100);
        //    musicRand = 80;
        if (musicRand <= 50) {
          musicPlayer.startPlayingFile("/track001.mp3");
        }
        else if (musicRand > 50) {
          musicPlayer.startPlayingFile("/track002.mp3");
        }
        Serial.println(musicRand);

        //    // Start playing a file, then we can do stuff while waiting for it to finish
        //    if (! musicPlayer.startPlayingFile("/track001.mp3")) {
        //      Serial.println("Could not open file track001.mp3");
        //      while (1);
        //    }
        Serial.println(F("Started playing"));

        while (musicPlayer.playingMusic) {
          // file is now playing in the 'background' so now's a good time
          // to do something else like handling LEDs or buttons :)
          actOneState();
          actTwoState();
          actThreeState();
//          Serial.println(sensorReading);
        }
        Serial.println("Done playing music");
      }
    }

    /////////////////////////////////Back to Front Wave Pattern/////////////////////////////////
    //    if (preRan == 1) {
    if (soloState == 4) {
      CH1 = off; CH2 = off; CH3 = off; CH4 = off; CH5 = off; CH6 = low; CH7 = low; CH8 = low;
    }
    if (soloState == 5) {
      CH1 = off; CH2 = off; CH3 = off; CH4 = low; CH5 = low; CH6 = off; CH7 = off; CH8 = off;
    }
    if (soloState == 6) {
      CH1 = low; CH2 = low; CH3 = low; CH4 = off; CH5 = off; CH6 = off; CH7 = off; CH8 = off;
      if (sensorReading == 1 ) {
        //    Serial.println(sensorReading);
        //    Serial.println("Sensor read is 1!");
        ptime1 = millis();
        //circleOff = true;
        state1 = 0;
        preAnimation = false;
        act = 2;

        //audio player
        musicRand = random(100);
        //    musicRand = 80;
        if (musicRand <= 50) {
          musicPlayer.startPlayingFile("/track001.mp3");
        }
        else if (musicRand > 50) {
          musicPlayer.startPlayingFile("/track002.mp3");
        }
        Serial.println(musicRand);

        //    // Start playing a file, then we can do stuff while waiting for it to finish
        //    if (! musicPlayer.startPlayingFile("/track001.mp3")) {
        //      Serial.println("Could not open file track001.mp3");
        //      while (1);
        //    }
        Serial.println(F("Started playing"));

        while (musicPlayer.playingMusic) {
          // file is now playing in the 'background' so now's a good time
          // to do something else like handling LEDs or buttons :)
          actOneState();
          actTwoState();
          actThreeState();
//          Serial.println(sensorReading);
        }
        Serial.println("Done playing music");
      }
    }
    //    }
    //
    //    /////////////////////////////////Left to Right Wave Pattern/////////////////////////////////
    //
    //    if (preRan == 2) {
    if (soloState == 7) {
      CH1 = off; CH2 = off; CH3 = off; CH4 = off; CH5 = off; CH6 = off; CH7 = off; CH8 = off;
    }
    if (soloState == 8) {
      CH1 = low; CH2 = off; CH3 = off; CH4 = off; CH5 = low; CH6 = low; CH7 = off; CH8 = off;
    }
    if (soloState == 9) {
      CH1 = off; CH2 = low; CH3 = off; CH4 = off; CH5 = off; CH6 = off; CH7 = low; CH8 = off;
    }
    if (soloState == 10) {
      CH1 = off; CH2 = off; CH3 = low; CH4 = low; CH5 = off; CH6 = off; CH7 = off; CH8 = low;
      if (sensorReading == 1 ) {
        //    Serial.println(sensorReading);
        //    Serial.println("Sensor read is 1!");
        ptime1 = millis();
        //circleOff = true;
        state1 = 0;
        preAnimation = false;
        act = 2;

        //audio player
        musicRand = random(100);
        //    musicRand = 80;
        if (musicRand <= 50) {
          musicPlayer.startPlayingFile("/track001.mp3");
        }
        else if (musicRand > 50) {
          musicPlayer.startPlayingFile("/track002.mp3");
        }
        Serial.println(musicRand);

        //    // Start playing a file, then we can do stuff while waiting for it to finish
        //    if (! musicPlayer.startPlayingFile("/track001.mp3")) {
        //      Serial.println("Could not open file track001.mp3");
        //      while (1);
        //    }
        Serial.println(F("Started playing"));

        while (musicPlayer.playingMusic) {
          // file is now playing in the 'background' so now's a good time
          // to do something else like handling LEDs or buttons :)
          actOneState();
          actTwoState();
          actThreeState();
//          Serial.println(sensorReading);
        }
        Serial.println("Done playing music");
      }
    }
    //    }
    //
    //    /////////////////////////////////Right To Left Wave Pattern/////////////////////////////////
    //
    //    if (preRan == 3) {
    if (soloState == 11) {
      CH1 = off; CH2 = off; CH3 = off; CH4 = off; CH5 = off; CH6 = off; CH7 = off; CH8 = off;
    }

    if (soloState == 12) {
      CH1 = off; CH2 = off; CH3 = low; CH4 = low; CH5 = off; CH6 = off; CH7 = off; CH8 = low;
    }
    if (soloState == 13) {
      CH1 = off; CH2 = low; CH3 = off; CH4 = off; CH5 = off; CH6 = off; CH7 = low; CH8 = off;
    }
    if (soloState == 14) {
      CH1 = low; CH2 = off; CH3 = off; CH4 = off; CH5 = low; CH6 = low; CH7 = off; CH8 = off;
      act = 0;
    }
  }
  //
  //    ///////////////////////////////////////Snake Pattern///////////////////////////////////////
  //
  //    if (preRan == 4) {
  //      if (soloState == 15) {
  //        CH1 = high; CH2 = off; CH3 = off; CH4 = off; CH5 = off; CH6 = off; CH7 = off; CH8 = off;
  //      }
  //      if (soloState == 16) {
  //        CH1 = off; CH5 = high;
  //      }
  //      if (soloState == 17) {
  //        CH5 = off; CH6 = high;
  //      }
  //      if (soloState == 18) {
  //        CH6 = off; CH7 = high;
  //      }
  //      if (soloState == 19) {
  //        CH7 = off; CH2 = high;
  //      }
  //      if (soloState == 20) {
  //        CH2 = off; CH3 = high;
  //      }
  //      if (soloState == 21) {
  //        CH3 = off; CH4 = high;
  //      }
  //      if (soloState == 22) {
  //        CH3 = off; CH8 = high;
  //      }
  //      if (soloState == 23) {
  //        CH8 = off;
  //      }
  //      if (soloState == 24) {
  //        CH8 = high;
  //      }
  //      if (soloState == 25) {
  //        CH8 = off; CH4 = high;
  //      }
  //      if (soloState == 26) {
  //        CH4 = off; CH3 = high;
  //      }
  //      if (soloState == 27) {
  //        CH3 = off; CH2 = high;
  //      }
  //      if (soloState == 28) {
  //        CH2 = off; CH7 = high;
  //      }
  //      if (soloState == 29) {
  //        CH7 = off; CH6 = high;
  //      }
  //      if (soloState == 30) {
  //        CH6 = off; CH5 = high;
  //      }
  //      if (soloState == 31) {
  //        CH5 = off; CH1 = high;
  //      }
  //      if (soloState == 32) {
  //        CH1 = off;
  //      }
  //      if (soloState == 33) {
  //        soloState = 0;
  //      }
  //    }
  //  }
}

void actOneState() {
  if (act == 2) {
    //    Serial.println("Got to actOneState!");
    if (millis() - ptime1 > delay_time1) {
      state1++;
      ptime1 = millis();
    }
    //    Serial.println("Act 1!");
    //lights turn on from 1 - 15

    if (state1 == 0) {
      CH1 = high;
    }
    //    delay(25);
    if (state1 == 1) {
      CH2 = high; CH1 = off;
    }
    //    delay(25);
    if (state1 == 2) {
      CH3 = high; CH2 = off;
    }
    //    delay(25);
    if (state1 == 3) {
      CH4 = high; CH3 = off;
    }
    //    delay(25);
    if (state1 == 4) {
      CH5 = high; CH4 = off;
    }
    //    delay(25);
    if (state1 == 5) {
      CH6 = high; CH5 = off;
    }
    //    delay(25);
    if (state1 == 6) {
      CH7 = high; CH6 = off;
    }
    //    delay(25);
    if (state1 == 7) {
      CH8 = high; CH7 = off;
    }
    //    delay(25);
    if (state1 == 8) {
      CH7 = high; CH8 = off;
    }
    //    delay(25);
    if (state1 == 9) {
      CH6 = high; CH7 = off;
    }
    //    delay(25);
    if (state1 == 10) {
      CH5 = high; CH6 = off;
    }
    //    delay(25);
    if (state1 == 11) {
      CH4 = high; CH5 = off;
    }
    //    delay(25);
    if (state1 == 12) {
      CH3 = high; CH4 = off;
    }
    //    delay(25);
    if (state1 == 13) {
      CH2 = high; CH3 = off;
    }
    //    delay(25);
    if (state1 == 14) {
      CH1 = high; CH2 = off;
    }
    //    delay(25);

    if (state1 == 15) {
      CH1 = off;
      ptime2 = millis();
      state2 = 0;
      openingAnimation = true;
    }

    if (openingAnimation) {
      if (millis() - ptime2 > delay_time2) {
        state2++;
        ptime2 = millis();
      }
      if (state2 == 0) {
        CH1 = high; CH2 = off; CH3 = off; CH4 = off; CH5 = off; CH6 = high; CH7 = off; CH8 = off;
      }

      if (state2 == 1) {
        CH1 = off; CH2 = off; CH3 = high; CH4 = off; CH5 = off; CH6 = off; CH7 = off; CH8 = high;
      }

      if (state2 == 2) {
        CH1 = off; CH2 = high; CH3 = off; CH4 = off; CH5 = high; CH6 = off; CH7 = high; CH8 = off;
      }

      if (state2 == 3) {
        CH1 = off; CH2 = off; CH3 = high; CH4 = high; CH5 = off; CH6 = off; CH7 = high; CH8 = off;
      }

      if (state2 == 4) {
        CH1 = high; CH2 = off; CH3 = off; CH4 = off; CH5 = off; CH6 = high; CH7 = off; CH8 = off;
      }

      if (state2 == 5) {
        CH1 = off; CH2 = off; CH3 = off; CH4 = off; CH5 = high; CH6 = off; CH7 = off; CH8 = high;
      }

      if (state2 == 6) {
        CH1 = off; CH2 = high; CH3 = high; CH4 = off; CH5 = high; CH6 = off; CH7 = off; CH8 = off;
      }

      if (state2 == 7) {
        CH1 = high; CH2 = off; CH3 = off; CH4 = high; CH5 = off; CH6 = off; CH7 = high; CH8 = off;
      }

      if (state2 == 8) {
        CH1 = off; CH2 = off; CH3 = off; CH4 = off; CH5 = off; CH6 = high; CH7 = off; CH8 = high;
      }

      if (state2 == 9) {
        CH1 = off; CH2 = high; CH3 = off; CH4 = off; CH5 = high; CH6 = off; CH7 = off; CH8 = high;
      }

      if (state2 == 10) {
        CH1 = high; CH2 = off; CH3 = off; CH4 = high; CH5 = off; CH6 = off; CH7 = high; CH8 = off;
      }

      if (state2 == 11) {
        CH1 = off; CH2 = off; CH3 = high; CH4 = off; CH5 = high; CH6 = high; CH7 = off; CH8 = off;
      }

      if (state2 == 12) {
        CH1 = off; CH2 = high; CH3 = off; CH4 = off; CH5 = high; CH6 = off; CH7 = high; CH8 = off;
      }

      if (state2 == 13) {
        CH1 = high; CH2 = off; CH3 = high; CH4 = off; CH5 = off; CH6 = off; CH7 = off; CH8 = high;
      }

      if (state2 == 14) {
        CH1 = off; CH2 = off; CH3 = off; CH4 = off; CH5 = off; CH6 = off; CH7 = off; CH8 = off;
      }

      if (state2 == 15) {
        overallptime = millis();
        act = 3;
      }
    }
  }
}


void actTwoState() {

  if (act == 3) {
    fsr1Reading = analogRead(fsr1Pin);
    fsr2Reading = analogRead(fsr2Pin);
    fsr3Reading = analogRead(fsr3Pin);
    fsr4Reading = analogRead(fsr4Pin);
    fsr5Reading = analogRead(fsr5Pin);
    Serial.print(fsr1Reading);
    Serial.print(",");
    Serial.print(fsr2Reading);
    Serial.print(",");
    Serial.print(fsr3Reading);
    Serial.print(",");
    Serial.print(fsr4Reading);
    Serial.print(",");
    Serial.println(fsr5Reading);
    fsr1StateChanges();
    fsr2StateChanges();
    fsr3StateChanges();
    fsr4StateChanges();
    fsr5StateChanges();
    if (millis() > overallptime + overallDelay) {
      finalptime = millis();
      endingptime = millis();
      finalState = 0;
      act = 4;
    }
  }
}

void actThreeState() {
  if (act == 4) {

    //    for (int i = 0; i < 10; i++) {
    //    if (millis() - endingptime > endingDelay) {
    //    finalState++;
    //      endingptime = millis();
    //    }


    if (finalState == 0) {

      ch1Val = j1;
      ch2Val = j2;
      ch3Val = j3;
      ch4Val = j4;
      ch5Val = j5;
      ch6Val = j6;
      ch7Val = j7;
      ch8Val = j8;


      for (int i = 0 ; i < 128; i++) {
        CH1 = constrain(ch1Val + i, 5, 128);
        CH2 = constrain(ch2Val + i, 5, 128);
        CH3 = constrain(ch3Val + i, 5, 128);
        CH4 = constrain(ch4Val + i, 5, 128);
        CH5 = constrain(ch5Val + i, 5, 128);
        CH6 = constrain(ch6Val + i, 5, 128);
        CH7 = constrain(ch7Val + i, 5, 128);
        CH8 = constrain(ch8Val + i, 5, 128);
        delay(100);
        //        finalState++;
      }
    }

    if (millis() > finalptime + finalDelay) {
      act = 0;
      circleOff = false;
      //      soloState = 0;
      bulb1Dim = false;
      bulb2Dim = false;
      bulb3Dim = false;
      bulb4Dim = false;
      bulb5Dim = false;
      bulb6Dim = false;
      bulb7Dim = false;
      bulb8Dim = false;


    }
  }
}


/// File listing helper
void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      //Serial.println("**nomorefiles**");
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
