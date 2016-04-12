attiny45 doing a similar job:
http://austindavid.com/jm3/index.php/hardware/57-lvd-project
http://austindavid.com/jm3/index.php/hardware/4-lvd-kit-a-battery-saver
https://github.com/jaustindavid/LVD

TODO: add trim pot to design for fine-tune adjustment to voltage sensitivity
TODO: need simplistic Hysteresis so that we don't accidentally bounce
it up and down a bunch (surface charge/sleep mode lack of draw may bring it back.. ?)
TODO: have a capacitor onboard and power down voltage regulator since regulator quiescent
cuurent gonna be 150ua all the time whether we draw power or not

using code cues from https://arduinoelectronics.wordpress.com/2014/01/06/ultra-low-power-led-flasher-with-attiny/

interesting info http://web.engr.oregonstate.edu/~traylor/ece473/lectures/reset.pdf

reference voltage stuff may be more convenient than tying up an ADC pin:
http://forum.arduino.cc/index.php?topic=223897.0
http://forum.arduino.cc/index.php?topic=139958.0
