#include <avr/io.h>

#define BAUDCLK 11
#define ENABLE A0
#define RWN A1
#define RS A2
#define CSN A3

#define D0 2
#define D1 3
#define D2 4
#define D3 5
#define D4 6
#define D5 7
#define D6 8
#define D7 9

void setup() {
    setBusesToOutput();
    initBaudClock();
    initMC6850();
}

void loop() {
    writeUart('T');
    writeUart('E');
    writeUart('S');
    writeUart('T');
    writeUart('A');
    writeUart('C');
    writeUart('I');
    writeUart('A');
    delay(100);
}

void writeUart(int data) {
    delay(1);
    digitalWrite(ENABLE, LOW);
    delay(1);
    digitalWrite(RWN, LOW);
    delay(1);
    digitalWrite(RS, HIGH);
    delay(1);
    digitalWrite(CSN, HIGH);
    delay(1);

    writeDataBus(data);

    digitalWrite(CSN, LOW);
    delay(1);
    digitalWrite(ENABLE, HIGH);
    delay(1);
}


void writeDataBus(int data)
{
    digitalWrite(D0, bitRead(data, 0));
    digitalWrite(D1, bitRead(data, 1));
    digitalWrite(D2, bitRead(data, 2));
    digitalWrite(D3, bitRead(data, 3));
    digitalWrite(D4, bitRead(data, 4));
    digitalWrite(D5, bitRead(data, 5));
    digitalWrite(D6, bitRead(data, 6));
    digitalWrite(D7, bitRead(data, 7));
}

void initBaudClock() {

    //UART BAUD CLOCK on pin 11 (153.6 khz which is baud 9600x16) thats why we need divisor 16
//Timer2 register setup
//Bits 7:6 mean toggle pin 11 on a compare match
//Bits 1:0 and bit 3 in TCCR2B select Fast PWM mode, with OCRA for the TOP value
    TCCR2A = 0b01000011;
//Bits 2:0 are the prescaler setting, this is a 1 prescaler
    TCCR2B = 0b00001001;
//This is the value that, when the timer reaches it, will toggle the
//output pin and restart. A value of 51 in this case gives a 153.85kHz output
    OCR2A = 51;
}

void initMC6850() {
//Init Control Register
    delay(1);
    digitalWrite(ENABLE, LOW);
    delay(1);
    digitalWrite(RWN, LOW);
    delay(1);
    digitalWrite(RS, LOW);
    delay(1);
    digitalWrite(CSN, HIGH);
    delay(1);

//data
//CONTROL REGISTER SETTING
// See MC6850 datasheet for more information

//Clock divisor setting
//CR0 = 1 CR1 = 0 - 16x DIVISOR - see datasheet
    digitalWrite(D0, HIGH);
    digitalWrite(D1, LOW);
//Parity control
    digitalWrite(D2, HIGH);
    digitalWrite(D3, LOW);
    digitalWrite(D4, HIGH);
//Transmitter Interrupt Control Bits
    digitalWrite(D5, LOW);
    digitalWrite(D6, LOW);
//Receive Interrrupt Enable Bit
    digitalWrite(D7, LOW);

    digitalWrite(CSN, LOW);
    delay(1);
    digitalWrite(ENABLE, HIGH);
    delay(1);
}

void setBusesToOutput() {
    pinMode(D0, OUTPUT);
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    pinMode(D4, OUTPUT);
    pinMode(D5, OUTPUT);
    pinMode(D6, OUTPUT);
    pinMode(D7, OUTPUT);
    pinMode(RS, OUTPUT);
    pinMode(RWN, OUTPUT);
    pinMode(ENABLE, OUTPUT);
    pinMode(BAUDCLK, OUTPUT);
    pinMode(CSN, OUTPUT);
}
