#include <Arduino.h>
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32_NimBLE.h>
#include "avdweb_Switch.h"
#include <AiEsp32RotaryEncoder.h>
#include <AiEsp32RotaryEncoderNumberSelector.h>

#define MIDI_DEVICE_NAME "BLEFSW"  //name of BT Device

BLEMIDI_CREATE_INSTANCE(MIDI_DEVICE_NAME, MIDI);

int abank = 0;
int nbank = 0;

#define buttonup 33
#define buttondown 25
#define buttonleft 27
#define buttonright 26
#define buttonharmony 2

#define auxin1 18
#define auxin2 19

#define dataenc1 17
#define clkenc1 5
#define dataenc2 4
#define clkenc2 16
#define ROTARY_ENCODER_STEPS 4

#define PIN_VOLUME 32

#define ledmode1 12
#define ledmode2 13
#define ledmode3 14
#define ledconn 23

AiEsp32RotaryEncoder enc1 = AiEsp32RotaryEncoder(clkenc1, dataenc1, -1, -1, ROTARY_ENCODER_STEPS);
AiEsp32RotaryEncoder enc2 = AiEsp32RotaryEncoder(clkenc2, dataenc2, -1, -1, ROTARY_ENCODER_STEPS);

bool isConnect = false;

Switch buttup = Switch(buttonup);
Switch buttdown = Switch(buttondown);
Switch buttright = Switch(buttonright);
Switch buttleft = Switch(buttonleft);
Switch buttharm = Switch(buttonharmony);
Switch aux1 = Switch(auxin1);
Switch aux2 = Switch(auxin2);

void ReadCB(void *parameter);  //Continuos Read function (See FreeRTOS multitasks)

// Constants for volume pedal
const int POT_THRESHOLD = 3;           // Threshold amount to guard against false values
const int HISTORY_BUFFER_LENGTH = 10;  // History buffer length
// (to guard against noise being sent)

// Globals for volume pedal
static int s_history[HISTORY_BUFFER_LENGTH];
int MIDIVolume = -1;

void IRAM_ATTR readEncoderISR() {
  enc1.readEncoder_ISR();
  enc2.readEncoder_ISR();
}

void setup() {

  // Initialize ADC-input for volume pedal
  adcAttachPin(PIN_VOLUME);

  // Initialize the buffer
  for (int i = 0; i < HISTORY_BUFFER_LENGTH; i++) {
    s_history[i] = -1;
  }

  MIDI.begin(MIDI_CHANNEL_OMNI);

  BLEMIDI.setHandleConnected(OnConnected);
  BLEMIDI.setHandleDisconnected(OnDisconnected);

  MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {
    digitalWrite(ledconn, LOW);
  });

  MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
    digitalWrite(ledconn, HIGH);
  });


// #ifdef PIN_VOLUME
//   // Adjust MIDI volume to pedal level
//   if (MIDIVolume >= 0)
//     MIDI.sendControlChange(19, MIDIVolume, 1);
// #endif

  pinMode(ledmode1, OUTPUT);
  pinMode(ledmode2, OUTPUT);
  pinMode(ledmode3, OUTPUT);
  pinMode(ledconn, OUTPUT);

  pinMode(dataenc1, INPUT_PULLUP);
  pinMode(clkenc1, INPUT_PULLUP);
  pinMode(dataenc2, INPUT_PULLUP);
  pinMode(clkenc2, INPUT_PULLUP);


  enc1.begin();
  enc1.setup(readEncoderISR);
  enc1.setBoundaries(0, 127, false);  //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  enc1.setAcceleration(1);

  enc2.begin();
  enc2.setup(readEncoderISR);
  enc2.setBoundaries(0, 127, false);  //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  enc2.setAcceleration(1);

  xTaskCreatePinnedToCore(ReadCB,  //See FreeRTOS for more multitask info
                          "MIDI-READ",
                          3000,
                          NULL,
                          1,
                          NULL,
                          1);  //Core0 or Core1
}


void loop() {

  if (isConnect == true) {
    digitalWrite(ledconn, HIGH);
  } else {
    digitalWrite(ledconn, LOW); //removed blink because it slows down the BT connection
  }

  ReadVolumePedal();

  if (enc1.encoderChanged()) {
    int enc1value = enc1.readEncoder();
    MIDI.sendControlChange(17, enc1value, 1);
  }
  if (enc2.encoderChanged()) {
    int enc2value = enc2.readEncoder();
    MIDI.sendControlChange(18, enc2value, 1);
  }

  aux1.poll();
  if (aux1.pushed()) {  //AUX1 
  MIDI.sendControlChange(85, 127, 1);
    }

  aux2.poll();
  if (aux2.pushed()) {  //AUX2
  MIDI.sendControlChange(86, 127, 1);
  }

  buttup.poll();
  if (buttup.pushed()) {
    nbank = abank + 1;  //BANK UP
  }

  buttdown.poll();
  if (buttdown.singleClick()) {  //BANK DOWN
    MIDI.sendControlChange(16, 127, 1);
  }

  buttharm.poll();
  if (buttharm.pushed()) {
    MIDI.sendControlChange(20, 127, 1);
  }
  if (buttharm.released()) {
    MIDI.sendControlChange(20, 0, 1);
  }

  if (nbank > 4 || nbank < 0) {
    nbank = 0;
  }

  abank = nbank;

  switch (nbank) {

    case 0:
      digitalWrite(ledmode1, HIGH);
      digitalWrite(ledmode2, LOW);
      digitalWrite(ledmode3, LOW);
      bank1();
      break;

    case 1:
      digitalWrite(ledmode2, HIGH);
      digitalWrite(ledmode1, LOW);
      digitalWrite(ledmode3, LOW);
      bank2();
      break;

    case 2:
      digitalWrite(ledmode3, HIGH);
      digitalWrite(ledmode2, LOW);
      digitalWrite(ledmode1, LOW);
      bank3();
      break;

    default:
      digitalWrite(ledmode1, HIGH);
      digitalWrite(ledmode2, LOW);
      digitalWrite(ledmode3, LOW);
      bank1();
      nbank = 0;

      break;
  }
}

void bank1() {  //LOOP STATION BANK

  buttright.poll();
  if (buttright.singleClick()) {  //PLAY-STOP
    MIDI.sendControlChange(21, 127, 1);
  }
  if (buttright.doubleClick()) {  //UNDO
    MIDI.sendControlChange(22, 127, 1);
  }

  buttleft.poll();
  if (buttleft.singleClick()) {  //RECORD
    MIDI.sendControlChange(23, 127, 1);
  }
  if (buttleft.longPress()) {  //CLEAR
    MIDI.sendControlChange(24, 127, 1);
  }
}


void bank2() {

  buttright.poll();
  if (buttright.pushed()) {  //TAP TEMPO
    MIDI.sendControlChange(25, 127, 1);
  }

  buttleft.poll();
  if (buttleft.singleClick()) {  //CYCLE THROUGH HARMONIES
    MIDI.sendControlChange(26, 127, 1);
  }
}


void bank3() {

  buttright.poll();
  if (buttright.singleClick()) {

    MIDI.sendControlChange(27, 127, 1);
  }
  if (buttright.doubleClick()) {  //TUNER
    MIDI.sendControlChange(28, 127, 1);
  }

  buttleft.poll();
  if (buttleft.singleClick()) {
    MIDI.sendControlChange(29, 127, 1);
  }

  if (buttleft.doubleClick()) {  //MUTE
    MIDI.sendControlChange(30, 127, 1);
  }
}

void ReadVolumePedal() {
  // Values of volume pedal
  static int s_nLastPotValue = 0;
  static int s_nLastMappedValue = 0;

  int nCurrentPotValue = analogRead(PIN_VOLUME);

  if (abs(nCurrentPotValue - s_nLastPotValue) < POT_THRESHOLD)
    return;
  s_nLastPotValue = nCurrentPotValue;

  int nMappedValue = map(nCurrentPotValue, 0, 4095, 0, 127);  // Map the value to 0-127

  if (nMappedValue == s_nLastMappedValue)
    return;

  for (int i = 0; i < HISTORY_BUFFER_LENGTH; i++) {
    if (s_history[i] == nMappedValue)
      return;
  }

  memcpy(&s_history[0], &s_history[1], sizeof(int) * (HISTORY_BUFFER_LENGTH - 1));
  s_history[HISTORY_BUFFER_LENGTH - 1] = nMappedValue;
  s_nLastMappedValue = nMappedValue;
  MIDI.sendControlChange(19, nMappedValue, 1);
  MIDIVolume = nMappedValue;
}

/**
   This function is called by xTaskCreatePinnedToCore() to perform a multitask execution.
   In this task, read() is called every millisecond (approx.).
   read() function performs connection, reconnection and scan-BLE functions.
   Call read() method repeatedly to perform a successfull connection with the server
   in case connection is lost.
*/
void ReadCB(void *parameter) {
  //  Serial.print("READ Task is started on core: ");
  //  Serial.println(xPortGetCoreID());
  for (;;) {
    MIDI.read();
    vTaskDelay(1 / portTICK_PERIOD_MS);  //Feed the watchdog of FreeRTOS.
    //Serial.println(uxTaskGetStackHighWaterMark(NULL)); //Only for debug. You can see the watermark of the free resources assigned by the xTaskCreatePinnedToCore() function.
  }
  vTaskDelay(1);
}

void OnConnected() {
  isConnect = true;
}

void OnDisconnected() {
  isConnect = false;
}