/*
 * main.c - System Entry Point & Boot Sequence
 *
 * Project 04: Vehicle Dashboard
 * Microcontroller: ATmega32A
 */

#include <avr/io.h>
#include <avr/interrupt.h>

/* MCAL Layer Headers */
#include "MCAL/dio/dio.h"
#include "MCAL/adc/adc.h"
#include "MCAL/timer/timer.h"

/* HAL Layer Headers */
#include "HAL/lamp595/lamp595.h"
#include "HAL/lcd_i2c/lcd_i2c.h"
#include "HAL/eeprom_spi/eeprom_spi.h"
#include "HAL/bodysw/bodysw.h"
#include "HAL/chime/chime.h"

#include "APP/scheduler/scheduler.h"
#include "APP/display/display.h"

/**
 * @brief Initialize all hardware abstractions and system drivers in order.
 */
static void System_Init(void)
{
    /* 1. Initialize Microcontroller Abstraction Layer (MCAL) */
    DIO_Init();
    ADC_Init();
    Timer_Init();

    /* 2. Initialize Hardware Abstraction Layer (HAL) */
    Lamps595_Init();
    Lcd_Init();
    Eeprom_Init();
    BodySw_Init();
    Chime_Init();

    /* 3. Run Startup / Self-Test Sequences */
    Lamps595_BulbCheck();

    /* TODO: Initialize Application Layer and Scheduler */
    Scheduler_Init();
}

int main(void)
{
    /* Initialize MCAL, HAL, and Application modules */
    System_Init();

    /* Enable Global Interrupts */
    sei();

    /* Application Cooperative Main Loop */
    while (1)
    {
        /* TODO: Call Scheduler_Run() when scheduler module is available */
        Scheduler_Run();

        /* Temporary polling calls for testing before Scheduler integration */
        BodySw_Update();
        Chime_Update();
    }

    return 0;
}