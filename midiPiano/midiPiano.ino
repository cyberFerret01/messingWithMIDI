
#define NUMBER_OF_SHIFT_CHIPS   2
#define DATA_WIDTH   NUMBER_OF_SHIFT_CHIPS * 8
#define PULSE_WIDTH_USEC   5
#define POLL_DELAY_MSEC   1
#define BYTES_VAL_T unsigned int
#define NUM_BUTTONS 16

#include "MIDIUSB.h"
#include "PitchToNote.h"


int ploadPin        = 8;  // Connects to Parallel load pin the 165
int clockEnablePin  = 9;  // Connects to Clock Enable pin the 165
int dataPin         = 11; // Connects to the Q7 pin the 165
int clockPin        = 12; // Connects to the Clock pin the 165


uint8_t pressedButtons = 0x00;
uint8_t previousButtons = 0x00;

BYTES_VAL_T pinValues;
BYTES_VAL_T oldPinValues;


const uint8_t  button1, button2, button3, button4, button5, button6, button7, button8, button9, button10, button11, button12, button13, button14, button15, button16;

const uint8_t buttons[NUM_BUTTONS] = {button1, button2, button3, button4, button5, button6, button7, button8, button9, button10, button11, button12, button13, button14, button15, button16};

const byte notePitches[NUM_BUTTONS] = {pitchC1, pitchD1b, pitchD1, pitchE1b, pitchE1, pitchF1, pitchG1b, pitchG1, pitchA1b, pitchA1,
                    pitchB1b,  pitchB1, pitchC2, pitchD2b, pitchD2, pitchE2b};

//, pitchE2, pitchF2, pitchG2b, pitchG2, pitchA2b, pitchA2};




BYTES_VAL_T read_shift_regs(){
    long bitVal;
    BYTES_VAL_T bytesVal = 0;

    /* Trigger a parallel Load to latch the state of the data lines,
    */
    digitalWrite(clockEnablePin, HIGH);
    digitalWrite(ploadPin, LOW);
    delayMicroseconds(PULSE_WIDTH_USEC);
    digitalWrite(ploadPin, HIGH);
    digitalWrite(clockEnablePin, LOW);

    /* Loop to read each bit value from the serial out line
     * of the SN74HC165N.
    */
    for(int i = 0; i < DATA_WIDTH; i++)
    {
        bitVal = digitalRead(dataPin);

        /* Set the corresponding bit in bytesVal.
        */
        bytesVal |= (bitVal << ((DATA_WIDTH-1) - i));

        /* Pulse the Clock (rising edge shifts the next bit).
        */
        digitalWrite(clockPin, HIGH);
        delayMicroseconds(PULSE_WIDTH_USEC);
        digitalWrite(clockPin, LOW);
    }
        //Serial.println(bytesVal, BIN);
        return(bytesVal);
}

void sendingMIDI (){

 pressedButtons = read_shift_regs();
  
  for(int i =0; i < NUM_BUTTONS; i++){
      if(bitRead(pressedButtons, i) != bitRead(previousButtons,i)){
          if(bitRead(pressedButtons,i)){
              bitWrite(previousButtons,i,1);
              noteOn(0, notePitches[i], 1);
              MidiUSB.flush();
            }
            else{
              bitWrite(previousButtons,i,0);
              noteOff(0, notePitches[i],0);
              MidiUSB.flush();  
            }
        }
    }  
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void setup()
{

    for(int z = 0; z < NUM_BUTTONS; z++){
      pinMode (buttons[z], INPUT_PULLUP);   
      }
    
    Serial.begin(9600);

    /* Initialize our digital pins...
    */
    pinMode(ploadPin, OUTPUT);
    pinMode(clockEnablePin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, INPUT);

    digitalWrite(clockPin, LOW);
    digitalWrite(ploadPin, HIGH);

    /* Read in and display the pin states at startup.
    */
    pinValues = read_shift_regs();
//    display_pin_values();
    oldPinValues = pinValues;
}

void loop()
{
    /* Read the state of all zones.
    */
 BYTES_VAL_T readReg = read_shift_regs();
 

 for(int j = 0; j <= NUM_BUTTONS; j++){
  if(bitRead(readReg,j)){
    bitWrite(readReg,j,0);
    }else{
    bitWrite(readReg,j,1);      
      }
  
  }

  

    /* If there was a chage in state, display which ones changed.
    */
    if(readReg != oldPinValues)
    {
        //Serial.print("*Pin value change detected*\r\n");
       Serial.println(readReg, BIN);
        oldPinValues = readReg;
          
    }

    delay(POLL_DELAY_MSEC);
}