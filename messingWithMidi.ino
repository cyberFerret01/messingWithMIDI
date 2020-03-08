#include "MIDIUSB.h"
#include "PitchToNote.h"

#define NUM_BUTTONS 22


const uint8_t  button1, button2, button3, button4, button5, button6, button7, button8, button9, button10, button11, button12, button13, button14, button15, button16, button17,
                    button18, button19, button20, button21, button22;

const uint8_t buttons[NUM_BUTTONS] = {button1, button2, button3, button4, button5, button6, button7, button8, button9, button10, button11, button12, button13, button14, button15, button16, button17, button18, button19, button20, button21, button22};

const byte notePitches[NUM_BUTTONS] = {pitchC1, pitchD1b, pitchD1, pitchE1b, pitchE1, pitchF1, pitchG1b, pitchG1, pitchA1b, pitchA1,
                    pitchB1b,  pitchB1, pitchC2, pitchD2b, pitchD2, pitchE2b, pitchE2, pitchF2, pitchG2b, pitchG2, pitchA2b, pitchA2};

int ploadPin        = 8;  // Connects to Parallel load pin the 165
int clockEnablePin  = 9;  // Connects to Clock Enable pin the 165
int dataPin         = 11; // Connects to the Q7 pin the 165
int clockPin        = 12; // Connects to the Clock pin the 165




uint8_t pressedButtons = 0x00;
uint8_t previousButtons = 0x00;

void setup() {
  Serial.begin(9600);
  
  for(int i = 0; i < NUM_BUTTONS; i++){
    pinMode(buttons[i], INPUT);
    }

}

void loop() {
  
  //noTone(8);
  readButtons();
  sendingMIDI();

}

void readButtons ()
{

    for(int j = 0; j < NUM_BUTTONS; j++){
    
    if((digitalRead(buttons[j])) == HIGH){
      tone(8, notePitches[j]);
      bitWrite(pressedButtons, j, 1);
      Serial.println(pressedButtons, BIN);
      delay(25);
      }else{
        bitWrite(pressedButtons,j,0);
        noTone(8);
        }
    }
    //Serial.println(pressedButtons,BIN);
}

void sendingMIDI ()
{
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
