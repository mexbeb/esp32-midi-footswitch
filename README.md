# ESP32 MIDI FOOTSWITCH [BLE & WI-FI]

## An open-source fully programmable MIDI footswitch

03/12/2022 UPDATE -> New version of the code with 6 banks!
22/12/2022 UPDATE #2 -> New version with Wi-Fi connection (rtpMIDI)!

With this project you'll build your own footswitch MIDI controller which is connected to your PC via a Bluetooth Low Energy or Wi-Fi connection!

See its functions and how it works here -> https://youtu.be/WgFxVs6B-E0

![FSWtnail](https://user-images.githubusercontent.com/74735686/204547478-894d30da-caaf-4c3a-ab26-5eadeda12911.png)

### You'll need:

• An ESP32 board with BLE (I used an Az-Delivery ESP32 board)

• N.O. switches (I used two momentary and one latching switch)

• Encoders with switches (just the encoders, not the breakout board version)

• TRS female jack 

• LEDs (3 for the banks, use more if you add planning to add more banks, and one for the connection LED) and their 220 Ohm resistors

• Step-down DC module to power the board if you want to use a DC power supply

• Expression pedal (if you want to use it)

• External 2-way footswitch (with N.O. switches)

• Arduino IDE (ver. 2, I had problems compiling with version 1.8)


## Instructions 

I used Arduino IDE and flashed the board as an ESP32 Dev Kit Module.

All the switches have one end to the board and one end to ground.

The pedal sends MIDI CCs and it has 3 banks, each one of the bank has different values for the right and left switches. More banks can be added mapping the switches to other CCs by changing the code.

The switches can be programmed to respond to single, double and long press clicks thanks to the avdweb_Switch library. No debounce is needed.

The encoders are not the version with the breakout board and the 10K resistors, they are just the encoder itself. Their function is implemented with the AiEsp32Rotary library by using internal pullup resistors and ISRs.

Thanks to this strategy and the implementation using these libraries, the parts count is low.

### The expression pedal TRS jack is defined as:

•TIP -> Vcc (3V3)

•RING -> pin on the board (PIN_VOLUME)

•SLEEVE -> Gnd

### The aux TRS jack is defined as:

•TIP -> aux 1 pin on the board (aux1in)

•RING -> aux 2 pin on the board (aux2in)

•SLEEVE -> Gnd

### The pin mapping can be checked in the Arduino code and is the following:

```
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
  
  ```
You can change it according to the board you're using.

### BLE version

Just connect your device as a simple BT MIDI device.

### Wi-Fi Version Connection Guide on MacOS

Flashing the Wi-Fi version of the code will turn the ESP32 into an access point with SSID __ESP32rtp__ and password __esp_32_midi@@@__.

As long as you turn on the board, the AP will appear in the network list and you have to connect to it.

<img width="289" alt="AP" src="https://user-images.githubusercontent.com/74735686/209116272-a78fc112-53ef-4c92-a306-7d96c7166d0f.png">

Open MIDI-Audio configuration, go to window -> show MIDI Studio and go to the network config (the icon near the bluetooth icon).

Add a new session (1) and call it __Arduino with Host__.

<img width="781" alt="MA" src="https://user-images.githubusercontent.com/74735686/209116754-6a1064e8-14ee-4fe5-838c-fc3b03abdf50.png">

Then in the directory section(2) add a new device, call it ESP32 and put the IP address of the ESP32 (you can find it by using any kind of IP scanner, in my case it was 192.168.4.1). The port is __5004__ by default.

<img width="781" alt="IP" src="https://user-images.githubusercontent.com/74735686/209116815-4e00a5ee-aa05-4bee-bca6-881f8e7b08c2.png">

Select the device you just created and click on connect (3), the connection light on the pedal will turn on and you're ready to go!

<img width="781" alt="MA2" src="https://user-images.githubusercontent.com/74735686/209116865-028dd888-f686-4d3d-a03b-faa7ee701413.png">

## Disclaimer

The code is not perfect and it can be surely improved.

But it works and for me is perfect that way.

## Thanks to 

• MicroMidi for the inspiration -> https://github.com/MicroMidi/NUX-MIDI-Footswitch

• avandalen for the switch library -> https://github.com/avandalen/avdweb_Switch

• igorantolic for the rotary enc library -> https://github.com/igorantolic/ai-esp32-rotary-encoder

• jeffb42 for his work on how to implement an expression pedal with arduino -> https://www.codeproject.com/Articles/38203/Arduino-Based-MIDI-Expression-Pedal

• lathoub for the Arduino AppleMIDI library -> https://github.com/lathoub/Arduino-AppleMIDI-Library
