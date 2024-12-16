# A5 - UART Communications

## Reference Materials
- STM32L4xxxx Reference Manual (RM) – USART (L476) or LPUART (L4A6, L496)
- STM32L4x6xx Datasheet (DS) – Pinouts
- NUCLEO-L4x6xG Users Manual (UM) – Pin Diagram (L476RG, L4A6ZG, L496ZG)
- Technical Note - VT100 Serial Terminal

## Debugger virtual COM port
The NUCLEO board routes a serial (RS-232) connection through the debugger and connectes using the same USB cable for loading a program and debugging. This creates a virtual COM port on the host computer used for programming that can be used to establish a serial connection using RS-232. On the NUCLEO-L476, the virtual com port is connected to USART2, using pins PA2 (TX) and PA3 (RX). On the NUCLEO-L4A6 and L496, the debugger is connected to LPUART1 using pins PG7 (TX) and PG8 (RX).

## LPUART1 on STM32L4A6 and STM32L496
The LPUART1 on the L4A6 and L496 uses GPIO port G. The upper 14 pins of port G, PG[15:2] use a separate power supply that is isolated from the other GPIO pins, VDDIO2. Before those pins can be used, a configuration setting must be set in the power control module (PWR). The IOSV bit in the CR2 register of the PWR module must be set to signify a proper voltage exists on VDDIO2. 
PWR->CR2 |= PWR_CR2_IOSV; 

## Instructions
- Calculate the baud rate divisors for communicating via UART at 115.2 kbps with the clock running at the default speed of 4 MHz. Use oversampling if possible (Only available with USART2, not with LPUART).
### Interface the STM32L4 to a Serial Terminal
- Write some functions to utilize the LPUART1 or USART2 and verify they function properly with a terminal application. You can find a list of available options in Technical Note 4 
  - UART_init - initialize USART2 or LPUART1 device for serial communication
  - UART_print - print a string of characters (Hint: You can use a similar function to the LCD_print_string function. Recall all strings of characters in C are terminated with a NULL or 0 value)
- Verify the UART connection is working by repeatedly sending a single character. When the software terminal is properly connected, this character should be printed repeatedly across the screen.
## Use VT100 Escape Codes
- Change the program from printing a single character repeatedly to printing the following. This should only be printed once and in the order listed. (Pro tip: create a USART_ESC_Code function that is similar to the USART_print function but includes sending ESC) 
  - Escape codes to move the cursor down 3 lines and to the right 5 spaces
  - Text “All good students read the”
  - Escape codes to move the cursor down 1 line and to the left 21 spaces
  - Escape code to change the text to blinking mode
  - Text “Reference Manual”
  - Escape code to move cursor back to the top left position
  - Escape code to remove character attributes (disable blinking text)
  - Text “Input: “
### Echo Characters
Write an ISR that echoes whatever is received by transmitting it back to the software terminal. Typing a character in the terminal transmits it rather than causes it to be displayed on the screen in the terminal emulator. If you want to see what you are typing, the STM32L4 will need to echo the character it receives back to the terminal to print. 
If the character typed is “R” instead of echoing “R”, it should change the text color to red. It should do the same with “B” for blue, “G” for green, and “W” for white.
Verify this works by typing in the terminal and text should appear after “Input: “ in the terminal screen

## Deliverables
- Demonstrate your working program to the instructor or lab assistant
- Create a single pdf document (not a full lab report) containing the following:
- Baud Rate calculation
- Properly formatted C source code
