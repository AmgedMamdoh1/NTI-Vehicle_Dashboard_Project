/*
 * dio.c - MCAL Digital I/O Driver for ATmega32A
 *
 * Project 04: Vehicle Dashboard
 * Microcontroller: ATmega32A
 */

#include "dio.h"
#include <avr/io.h>

/* Parameter Validation Helper */
static uint8_t DIO_IsValidPin(uint8_t port, uint8_t pin)
{
    return (port <= PORT_D) && (pin <= 7U);
}

void DIO_Init(void)
{
    /* Digital I/O initialization / Default hardware state */
}

void DIO_SetPinDirection(uint8_t port, uint8_t pin, uint8_t direction)
{
    if (!DIO_IsValidPin(port, pin))
    {
        return;
    }

    switch (port)
    {
        case PORT_A:
            if (direction == OUTPUT) { DDRA |= (1U << pin); }
            else                     { DDRA &= (uint8_t)(~(1U << pin)); }
            break;

        case PORT_B:
            if (direction == OUTPUT) { DDRB |= (1U << pin); }
            else                     { DDRB &= (uint8_t)(~(1U << pin)); }
            break;

        case PORT_C:
            if (direction == OUTPUT) { DDRC |= (1U << pin); }
            else                     { DDRC &= (uint8_t)(~(1U << pin)); }
            break;

        case PORT_D:
            if (direction == OUTPUT) { DDRD |= (1U << pin); }
            else                     { DDRD &= (uint8_t)(~(1U << pin)); }
            break;

        default:
            break;
    }
}

void DIO_SetPinLevel(uint8_t port, uint8_t pin, uint8_t level)
{
    if (!DIO_IsValidPin(port, pin))
    {
        return;
    }

    switch (port)
    {
        case PORT_A:
            if (level == HIGH) { PORTA |= (1U << pin); }
            else               { PORTA &= (uint8_t)(~(1U << pin)); }
            break;

        case PORT_B:
            if (level == HIGH) { PORTB |= (1U << pin); }
            else               { PORTB &= (uint8_t)(~(1U << pin)); }
            break;

        case PORT_C:
            if (level == HIGH) { PORTC |= (1U << pin); }
            else               { PORTC &= (uint8_t)(~(1U << pin)); }
            break;

        case PORT_D:
            if (level == HIGH) { PORTD |= (1U << pin); }
            else               { PORTD &= (uint8_t)(~(1U << pin)); }
            break;

        default:
            break;
    }
}

uint8_t DIO_GetPinLevel(uint8_t port, uint8_t pin)
{
    uint8_t pinVal = 0U;

    if (!DIO_IsValidPin(port, pin))
    {
        return LOW;
    }

    switch (port)
    {
        case PORT_A:
            pinVal = (PINA & (1U << pin)) ? HIGH : LOW;
            break;

        case PORT_B:
            pinVal = (PINB & (1U << pin)) ? HIGH : LOW;
            break;

        case PORT_C:
            pinVal = (PINC & (1U << pin)) ? HIGH : LOW;
            break;

        case PORT_D:
            pinVal = (PIND & (1U << pin)) ? HIGH : LOW;
            break;

        default:
            break;
    }

    return pinVal;
}

void DIO_TogglePin(uint8_t port, uint8_t pin)
{
    if (!DIO_IsValidPin(port, pin))
    {
        return;
    }

    switch (port)
    {
        case PORT_A:
            PORTA ^= (1U << pin);
            break;

        case PORT_B:
            PORTB ^= (1U << pin);
            break;

        case PORT_C:
            PORTC ^= (1U << pin);
            break;

        case PORT_D:
            PORTD ^= (1U << pin);
            break;

        default:
            break;
    }
}

void DIO_EnablePullUp(uint8_t port, uint8_t pin)
{
    /* Enforce input direction before setting pull-up via PORT register */
    DIO_SetPinDirection(port, pin, INPUT);
    DIO_SetPinLevel(port, pin, HIGH);
}

void DIO_WritePort(uint8_t port, uint8_t value)
{
    switch (port)
    {
        case PORT_A: PORTA = value; break;
        case PORT_B: PORTB = value; break;
        case PORT_C: PORTC = value; break;
        case PORT_D: PORTD = value; break;
        default: break;
    }
}

uint8_t DIO_ReadPort(uint8_t port)
{
    uint8_t portVal = 0U;

    switch (port)
    {
        case PORT_A: portVal = PINA; break;
        case PORT_B: portVal = PINB; break;
        case PORT_C: portVal = PINC; break;
        case PORT_D: portVal = PIND; break;
        default: break;
    }

    return portVal;
}