﻿# Simple 6850 ACIA System

This circuit provides a simple test environment for a **6850 Asynchronous Communications Interface Adapter (ACIA)** chip. In this specific example I used an *MC68B50-P* chip, but this software is forward compatible with all of the serial IC-s based on the 6850 architecture. 

This system was intended to be an introductory circuit to the world of ACIAs/UARTs, so I tried to minimize the complexity of the circuit, with keeping the amount of components as low as possible.

## Breadboard photo

![enter image description here](https://github.com/bazsimarkus/Simple-6850-UART-System-with-Arduino/raw/master/images/6850_breadboard.jpg)

## Components needed

 - 6850 ACIA chip
 - Arduino board (Uno, Nano, both works)
 - USB-TTL Serial converter for testing (FTDI will do as well)
	> The USB-TTL Serial adapter is needed to test if the circuit is working properly. Check out "Testing".

## Working

The circuit works like the following: The Arduino sends 8-bit ASCII characters to the ACIA chip, then the chip converts it to serial data. The parameters like the baudrate, parity bit, etc are also initialized by the Arduino in the beginning of the procedure. 
The text to send is divided into characters, as the function handles ASCII 8-bit chars.

In this circuit the clock pulse is generated by the Arduino itself, so no crystal is needed!

## Using

 1. Wire up the circuit based on schematics
 2. Upload the MC6850.ino source code to the Arduino board
 3. Connect the USB-TTL Serial converter to PC
 4. Open a serial terminal program (for example RealTerm)
 5. Enjoy!

## Schematic

![enter image description here](https://github.com/bazsimarkus/Simple-6850-UART-System-with-Arduino/raw/master/images/6850_sch.png)

## Testing

For testing get a serial terminal program like RealTerm:
https://realterm.sourceforge.io/

Then select the COM port of your USB-TTL Serial converter, and open up the serial port to see the results.

## Future goals

 - It's a good idea to add a MAX232 chip to make the circuit fully compatible with the RS232 standard!

## Development

The project was developed using the Arduino IDE (https://www.arduino.cc/) and the testing was made with RealTerm (https://realterm.sourceforge.io/)

## Notes

Check out my other serial project with the 8250 UART chip, which was developed parallel with this circuit!
