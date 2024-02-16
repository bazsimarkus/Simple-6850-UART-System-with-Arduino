/**
 * @file 6850.ino
 * @author Balazs Markus
 * @brief Arduino code for interacting with the 6850 ACIA chip and transmitting a test string with 9600 baud, 8-bit data, no parity, and 1 stop bit, once every second.
 * @details This code provides a simple test environment for a 6850 Asynchronous Communications Interface Adapter (ACIA) chip. It is specifically tested with a MC68B50-P chip but is forward compatible with all serial ICs based on the 6850 architecture.
 *
 * Pinout of the 6850 ACIA chip (DIP-24):
 *
 *         ┌───────○───────┐
 *     Vss │1            24│ CTSN
 *  RxData │2            23│ DCDN
 *   RxCLK │3            22│ D0   
 *   TxCLK │4            21│ D1   
 *    RTSN │5            20│ D2   
 *  TxData │6    6850    19│ D3   
 *    IRQN │7    ACIA    18│ D4   
 *     CS0 │8            17│ D5   
 *    CS2N │9            16│ D6   
 *     CS1 │10           15│ D7   
 *      RS │11           14│ EN    
 *     Vcc │12           13│ RWN
 *         └───────────────┘
 *
 * Pin descriptions of the 6850 ACIA chip:
 *
 * -----------------------------------------------------------------------
 * | Pin Number | Name       | Type    | Description                     |
 * |------------|------------|---------|---------------------------------|
 * | 1          | Vss        | -       | Ground                          |
 * | 2          | RxData     | In      | Receive Data                    |
 * | 3          | RxCLK      | In      | Receive Clock                   |
 * | 4          | TxCLK      | In      | Transmit Clock                  |
 * | 5          | RTSN       | Out     | Request to Send (Active Low)    |
 * | 6          | TxData     | Out     | Transmit Data                   |
 * | 7          | IRQN       | Out     | Interrupt Request (Active Low)  |
 * | 8          | CS0        | In      | Chip Select 0                   |
 * | 9          | CS2N       | In      | Chip Select 2 (Active Low)      |
 * | 10         | CS1        | In      | Chip Select 1                   |
 * | 11         | RS         | In      | Register Select                 |
 * | 12         | Vcc        | -       | Positive Supply                 |
 * | 13         | RWN        | In      | Read/Write (Active Low)         |
 * | 14         | EN         | In      | Enable (Active Low)             |
 * | 15         | D7         | In/Out  | Data Bit 7                      |
 * | 16         | D6         | In/Out  | Data Bit 6                      |
 * | 17         | D5         | In/Out  | Data Bit 5                      |
 * | 18         | D4         | In/Out  | Data Bit 4                      |
 * | 19         | D3         | In/Out  | Data Bit 3                      |
 * | 20         | D2         | In/Out  | Data Bit 2                      |
 * | 21         | D1         | In/Out  | Data Bit 1                      |
 * | 22         | D0         | In/Out  | Data Bit 0                      |
 * | 23         | DCDN       | In      | Data Carrier Detect (Active Low)|
 * | 24         | CTSN       | In      | Clear To Send (Active Low)      |
 * -----------------------------------------------------------------------
 *
 * Components needed for the test circuit:
 * - 6850 ACIA chip
 * - Arduino board (Uno, Nano, both works, the important thing is that it should operate on 5V voltage)
 * - USB-TTL Serial converter for testing (FTDI for example)
 *
 * The circuit works as follows:
 * The Arduino sends 8-bit ASCII characters to the ACIA chip, then the chip converts it to serial data (9600 baud, 8-bit data, no parity, and 1 stop bit).
 * The parameters like the baud rate, parity bit, etc., are initialized by the Arduino at the beginning of the procedure.
 * The text to send is stored in the PROGMEM section of the microcontroller memory, allowing the Arduino to send long messages without any stack problems.
 * No external crystal is needed, as the program utilizes Timer2 of the ATmega328P microcontroller on the Arduino UNO board to generate the clock signal, and achieve the desired standard 9600 Baud rate.
 *
 * How to use:
 * 1. Connect your Arduino board to the 6850 ACIA chip based on the included schematics.
 * 2. Upload the 6850.ino source code to the Arduino board.
 * 3. Connect the USB-TTL Serial converter to the PC and to the TxD/RxD pins of the 6850
 * 4. Open a serial terminal program (for example, RealTerm), select the COM port of your USB-Serial converter, and set up the terminal to use 9600 baud, 8-bit data, no parity, and 1 stop bit.
 * 5. You should be receiving the string "TESTACIA" once every second.
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <string.h>

// Pin Definitions
#define BAUDCLK 11   // Baud Rate Clock
#define ENABLE A0    // Enable pin for 
#define RWN A1       // Read/Write Not (Active Low)
#define RS A2        // Register Select
#define CS2N A3       // Chip Select 2 Not (Active Low)

#define D0 2         // Data bus bit 0
#define D1 3         // Data bus bit 1
#define D2 4         // Data bus bit 2
#define D3 5         // Data bus bit 3
#define D4 6         // Data bus bit 4
#define D5 7         // Data bus bit 5
#define D6 8         // Data bus bit 6
#define D7 9         // Data bus bit 7

#define LED_OUTPUT        13   // Arduino onboard LED to indicate status

// Timing constants
#define DELAY_SHORT   1
#define DELAY_MEDIUM  50
#define DELAY_LONG    200

// Test string definition
const char testString[] PROGMEM = {"TESTACIA"}; // We store the string in the PROGMEM, this enables us to store very long strings

/**
 * @brief Arduino setup function.
 *
 * @details This function is called once when the microcontroller is powered on or reset. It is used to initialize
 * the microcontroller's hardware and peripherals, such as pins, timers, serial communication, etc.
 *
 * @note The setup function runs only once after power-up or reset. It should be used to perform
 * initialization tasks and configuration settings.
 */
void setup() {
	pinMode(LED_OUTPUT, OUTPUT);
    setUartPinsToOutput();
    initClocks();
    initUart();  // Initializing 6850 ACIA
}

/**
 * @brief Arduino main loop function.
 *
 * @details The loop function is called repeatedly as long as the microcontroller is powered on.
 * It is used to implement the main program logic, perform tasks, and handle events.
 *
 * @note The loop function runs in an infinite loop and should be used to execute the main
 * program logic or perform tasks that need to be repeated continuously.
 */
void loop() {
    // Print the string with the 6850 ACIA
    printStringToUart(testString);

    // Blink onboard LED
    digitalWrite(LED_OUTPUT, HIGH);
    delay(DELAY_MEDIUM);
    digitalWrite(LED_OUTPUT, LOW);

    // Wait one second
    delay(1000);
}

/**
 * @brief Sets all data bus pins and control pins to output mode.
 *
 * @details This function configures all the data bus pins and control pins to be used as outputs.
 * It sets the direction of the pins accordingly to enable data transmission and control signal handling.
 */
void setUartPinsToOutput() {
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
    pinMode(CS2N, OUTPUT);
}

/**
 * @brief Set data bus pins to high impedance (not used).
 *
 * @details This function sets the data bus pins to high impedance,
 * but it is not used in the current implementation.
 */
void setDataBusToHighZ() {
    pinMode(D0, INPUT);  // Data Bus D0 - Input (High-Z)
    pinMode(D1, INPUT);  // Data Bus D1 - Input (High-Z)
    pinMode(D2, INPUT);  // Data Bus D2 - Input (High-Z)
    pinMode(D3, INPUT);  // Data Bus D3 - Input (High-Z)
    pinMode(D4, INPUT);  // Data Bus D4 - Input (High-Z)
    pinMode(D5, INPUT);  // Data Bus D5 - Input (High-Z)
    pinMode(D6, INPUT);  // Data Bus D6 - Input (High-Z)
    pinMode(D7, INPUT);  // Data Bus D7 - Input (High-Z)
}

/**
 * @brief Write data to the data bus.
 *
 * @details This function sets the data bus pins in bulk. It uses the bitRead
 * function to retrieve each bit of the input parameter.
 *
 * @param data The data to be written to the data bus.
 */
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

/**
 * @brief Write a character to UART.
 *
 * @details This function writes an 8-bit character to the UART for transmission. This function can only be called after initialization!
 *
 * @param data The character to be transmitted.
 *
 * This function begins by preparing the UART for transmission:
 *
 * 1. Set the ENABLE pin LOW to enable transmission.
 *    - This step indicates that the UART is ready to receive data.
 *
 * 2. Set the RWN (Read/Write Not) pin LOW to indicate a write operation.
 *    - By setting RWN LOW, the function specifies that the following transmission is a write operation.
 *
 * 3. Set the RS (Register Select) pin HIGH to select data register.
 *    - Bringing RS HIGH indicates that the following data will be written to the data register.
 *
 * 4. Set the CS2N (Chip Select 2 Not) pin HIGH to enable communication with UART.
 *    - Bringing CS2N HIGH selects the UART for communication.
 *
 * 5. Provide a short delay (DELAY_SHORT) for signal stabilization.
 *    - This delay ensures that signals settle before transmitting data.
 *
 * 6. Write the character to the data bus using the writeDataBus() function.
 *    - The character is written onto the data bus for communication with UART.
 *
 * 7. Set the CS2N pin LOW to complete data transmission.
 *    - Bringing CS2N LOW indicates the end of data transmission to UART.
 *
 * 8. Provide a short delay (DELAY_SHORT) for signal stability.
 *    - This delay ensures proper signal timing.
 *
 * 9. Set the ENABLE pin HIGH to complete the transmission process.
 *    - Bringing ENABLE HIGH disables further transmission.
 *
 * Each step in this function is essential for proper serial communication, ensuring that data is correctly transmitted and received.
 */
void writeCharToUart(int data) {
	// Write data register
    digitalWrite(ENABLE, LOW);
    delay(DELAY_SHORT);
    digitalWrite(RWN, LOW);
    delay(DELAY_SHORT);
    digitalWrite(RS, HIGH);
    delay(DELAY_SHORT);
    digitalWrite(CS2N, HIGH);
    delay(DELAY_SHORT);

    writeDataBus(data);

    digitalWrite(CS2N, LOW);
    delay(DELAY_SHORT);
    digitalWrite(ENABLE, HIGH);
    delay(DELAY_SHORT);
}

/**
 * @brief Print a string to UART.
 * 
 * @details This function prints a string to the UART for transmission.
 * 
 * @param str The string to be transmitted. The string has to be zero terminated!
 */
void printStringToUart(const char* str) {
    char myChar;
    for (int j = 0; j < strlen(str); j++) {
        myChar = pgm_read_byte_near(str + j);
        writeCharToUart(myChar);
    }
}

/**
 * @brief Blink the onboard LED to indicate status.
 * 
 * @details This function blinks the onboard LED to indicate status.
 * It is used for visual indication in the circuit.
 */
void blinkLed() {
    for (int i = 0; i < 4; i++) {
        digitalWrite(LED_OUTPUT, HIGH);
        delay(DELAY_LONG);
        digitalWrite(LED_OUTPUT, LOW);
        delay(DELAY_LONG);
    }
}

/**
 * @brief Initializes the baud clock for UART communication.
 *
 * @details This function sets up the Timer2 registers to generate the UART baud clock.
 * It sets the timer in Fast PWM mode with a prescaler of 1, and configures the compare match
 * to toggle pin 11 at the desired baud rate.
 */
void initClocks() {
    // Timer2 register setup
    // Bits 7:6 toggle pin 11 on a compare match
    // Bits 1:0 and bit 3 in TCCR2B select Fast PWM mode, with OCRA for the TOP value
    TCCR2A = 0b01000011;
    // Bits 2:0 are the prescaler setting, this is a 1 prescaler
    TCCR2B = 0b00001001;
    // This is the value that, when the timer reaches it, will toggle the output pin and restart.
    // A value of 51 in this case gives a 153.85kHz output
    OCR2A = 51;
}

/**
 * @brief Initializes the MC6850 (ACIA) chip for UART communication.
 *
 * @details This function configures the control lines and registers of the MC6850 chip for proper operation.
 *          It sets the control register to establish the baud rate divisor and other parameters required for communication.
 *          The MC6850 Asynchronous Communications Interface Adaptor (ACIA) serves as a critical intermediary between a computer
 *          and external systems, facilitating the conversion of data between serial and parallel forms. The ACIA's hardware 
 *          consists of three main sections: the CPU side, the transmitter side, and the receiver side.
 *
 * The CPU side of the 6850 ACIA interface involves addressing and interrupt handling. It can be interfaced with 
 * either the lower-order byte or the upper-order byte of the processor, with a single register select line (RS) 
 * determining the addressed register. While lacking a hardware reset input, the ACIA features an internal power-on-reset 
 * circuit and can undergo a secondary reset via software.
 *
 * On the transmitter side, the ACIA features pins for transmitting clock (TxCLK), transmit data (TxD), request-to-send 
 * (RTS*), and clear-to-send (CTS*). The transmitter clock input synchronizes transmitted signals, while RTS* signals 
 * readiness to transmit and can be controlled by software. CTS* indicates readiness of the external transmitting device.
 *
 * The receiver side comprises pins for receiving clock (RxCLK), receive data (RxD), and data carrier detect (DCD*). 
 * The receiver clock aligns with incoming data, while RxD receives serial data. DCD* indicates the validity of incoming data.
 *
 * Operationally, the 6850 ACIA includes four user-accessible registers: Transmit Data Register (TDR), Receive Data 
 * Register (RDR), Control Register (CR), and Status Register (SR). The control register configures operational characteristics, 
 * including baud rate, data format, and interrupt settings. The status register provides insights into the status of 
 * both the transmitter and receiver portions of the ACIA.
 *
 * This function initializes the ACIA by configuring the control register to set the clock divisor, data format, 
 * and interrupt control. It sets the clock divisor to 16x, selects a 8 data bits, no parity, 1 stop bit (8n1) format, and 
 * disables transmitter and receiver interrupts. Additionally, it configures the ACIA's control lines for proper 
 * operation by setting appropriate digital signals.
 *
 * The MC6850 Control Register (W) is structured as follows:
 * - Bits 0-1: Baudrate selection
 *   - 0: CLK/64
 *   - 1: CLK/16
 *   - 2: CLK/1
 *   - 3: RESET
 * - Bits 2-4: Mode selection (data/stop bits, parity)
 *   - 0: 7 data bits, even parity, 2 stop bits (7e2)
 *   - 1: 7 data bits, odd parity, 2 stop bits (7o2)
 *   - 2: 7 data bits, even parity, 1 stop bit (7e1)
 *   - 3: 7 data bits, odd parity, 1 stop bit (7o1)
 *   - 4: 8 data bits, no parity, 2 stop bits (8n2)
 *   - 5: 8 data bits, no parity, 1 stop bit (8n1)
 *   - 6: 8 data bits, even parity, 1 stop bit (8e1)
 *   - 7: 8 data bits, odd parity, 1 stop bit (8o1)
 * - Bits 5-6: Transmit Interrupt/RTS/Break control
 *   - 0: Output /RTS=low and disable Tx Interrupt
 *   - 1: Output /RTS=low and enable Tx Interrupt
 *   - 2: Output /RTS=high and disable Tx Interrupt
 *   - 3: Output /RTS=low and disable Tx Interrupt, and send a Break
 * - Bit 7: Receive Interrupt (1=Enable on buffer full/buffer overrun)
 *
 * The MC6850 Status Register (R) is structured as follows:
 * - Bit 0: Receive Data (0=No data, 1=Data can be read)
 * - Bit 1: Transmit Data (0=Busy, 1=Ready/Empty, Data can be written)
 * - Bit 2: /DCD level
 * - Bit 3: /CTS level
 * - Bit 4: Receive Framing Error (1=Error)
 * - Bit 5: Receive Overrun Error (1=Error)
 * - Bit 6: Receive Parity Error (1=Error)
 * - Bit 7: Interrupt Flag (see Control Bits 5-7) (IRQ pin is not connected)
 *
 * The MC6850 Data Register (R/W) is structured as follows:
 * - Bits 0-7: Data
 *   - Data can be read when Status.Bit0=1, and written when Status.Bit1=1.
 *   - Rx has a 1-stage fifo, plus 1 shift register (2 stages in total)
 *   - Tx has a 1-stage fifo, plus 1 shift register (2 stages in total)
 */
void initUart() {
    // Init Control Register
    delay(DELAY_SHORT);         // Delay for stability after power-up
    digitalWrite(ENABLE, LOW);  // Enable the ACIA chip
    delay(DELAY_SHORT);         // Delay after enabling chip
    digitalWrite(RWN, LOW);     // Set R/W line to write mode
    delay(DELAY_SHORT);         // Delay after setting write mode
    digitalWrite(RS, LOW);      // Set RS line to select control register
    delay(DELAY_SHORT);         // Delay after selecting control register
    digitalWrite(CS2N, HIGH);    // Deselect the chip for initial setup
    delay(DELAY_SHORT);         // Delay after deselecting chip

    // Control Register Settings
    // D0-D1: Clock divisor setting: CR0 = 1, CR1 = 0 - 16x DIVISOR
    // D2-D4: Format control: 8 data bits, no parity, 1 stop bit (8n1)
    // D5-D6: Transmitter Interrupt Control Bits: disabled
    // D7: Receive Interrupt Enable Bit: disabled
    writeDataBus(0b00010101);   // Write configuration data to control register

    digitalWrite(CS2N, LOW);     // Select the chip for normal operation
    delay(DELAY_SHORT);         // Delay after selecting chip
    digitalWrite(ENABLE, HIGH); // Disable chip
    delay(DELAY_SHORT);         // Delay after disabling

    // Indicate the end of initialization by blinking the onboard LED
    blinkLed();
}
