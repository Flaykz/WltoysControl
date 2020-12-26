# WltoysControl
Control Wltoys cars from your PC with an esp32 and nrf24l01. 

This is a full implementation of wltoys v202 protocol,specified for rc cars A959, A969, A979 using nrf24l01 and esp32 (or esp8266).
The idea of this project come from (https://github.com/nobotro/wltoys-v202-protocol). I prefered to use the arduino framework with platformIO and the serial interface.
Project don't need any modification of car and you don't need a controller.


## Usage
- Compile the main.cpp and upload it to your esp
- Connect nrf pins correctly(we use software spi) 'miso': 32, 'mosi': 33, 'sck': 25, 'csn': 26, 'ce': 27
- Run "remoteSerial.py" in your pc with your esp connected on your serial interface. You may change the COM port number.
- Keymap is mapped for AZERTY keyboard
- Key 'z' -> go forward
- Key 's' -> go backward
- Key 'q' -> steer left
- Key 'd' -> steer right
- Key 'a' -> decrease time to get full throttle
- Key 'e' -> increase time to get full throttle
- Key.up -> increase gear
- Key.down -> decrease gear
- Key.left -> steer trim
- Key.right -> steer trim
- By default, you begin with gear 6 and can downgrade to 1 or upgrade to 13, time to get full throttle is 1600 ms


## Used resurces 
 - https://github.com/Flaykz/Wltoys-v202
 - https://github.com/nobotro/wltoys-v202-protocol
 - https://github.com/execuc/v202-receiver/blob/master/v202_protocol.cpp
 - https://github.com/goebish/nrf24_multipro/blob/master/nRF24_multipro/V2x2.ino
 - https://bitbucket.org/rivig/v202/src/default/
