// Aamir Elhaissouni 
// Lab 8.2
// 4/26/2025
/*
 Temperature checking project using atmega328p microcontroller, breadboard and OLED screen
*/

//header files provided by professor mclaulghlin @ umass
#include "my_adc_lib.h"
#include "my_uart_lib.h"
#include "SSD1306.h"
#include "i2c.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#define BUTTON PC1 // pin A1          
#define RED_LED PD7 // pin D7
#define TooHot 80  // Too hot threshold in F

// Functions to convert voltage to temperature


    return (volts - 500.0) / 10.0; // Assuming 10mV/°C sensor
}
double calculate_temperature_fahrenheit(double volts) {
    return (calculate_temperature_celcius(volts) * 9.0 / 5.0) + 32.0;
}

// Main function

int main(void) { 
    // Initialize variables
    unsigned int digitalValue;
    double volts;
    char buffer1[5];
    
    uart_init();
    adc_init();
    OLED_Init();

    DDRD |= (1 << RED_LED);  // Set RED_LED as output
    DDRC &= ~(1 << BUTTON);  // Set BUTTON as input
    PORTC |= (1 << BUTTON);  // Enable pull-up resistor on BUTTON pin

    while (1) {
        // Read ADC value and convert to voltage
        digitalValue = get_adc(); // Read ADC value
        // Convert digital value to voltage (in mV)
        volts = digitalValue * ((V_REF*1000.0) / 1024.0); // converts digital value to analog value (in mV)
        // Convert voltage to temperature in Celsius or Fahrenheit
        // Check if button is pressed, display celcius if pressed
        if ((PINC & (1 << BUTTON)) == 0) {
            dtostrf(calculate_temperature_celcius(volts), 5, 1, buffer1);  
            // displays on OLED screen
            OLED_GoToLine(1); 
            OLED_DisplayString("Temperature:");
            OLED_DisplayString(buffer1);
            OLED_DisplayString("C");
            // displays on serial monitor (PuTTY)
            send_string("Temperature:");
            send_string(buffer1);
            send_string("C");
            uart_send(13);
            uart_send(10); 


        } else {  // Button not pressed, display Fahrenheit
            dtostrf(calculate_temperature_fahrenheit(volts), 5, 1, buffer1);  
            // displays on OLED screen
            OLED_GoToLine(1);
            OLED_DisplayString("Temperature:");
            OLED_DisplayString(buffer1);
            OLED_DisplayString("F");
            // displays on serial monitor (PuTTY)
            send_string("Temperature:");
            send_string(buffer1);
            send_string("F");
            uart_send(13); 
            uart_send(10); 

        }
        // Check if temperature exceeds threshold and turn on RED_LED if it does
        if (calculate_temperature_fahrenheit(volts) > TooHot) {
            PORTD |= (1 << RED_LED);  // Turn on
        } else {
            PORTD &= ~(1 << RED_LED);  // Turn off
        }

        _delay_ms(1000); // Delay for 1 second before next reading

    }
}
