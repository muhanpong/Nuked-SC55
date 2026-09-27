/*
 * Copyright (C) 2021, 2024 nukeykt
 *
 * This file is part of Nuked-SC55.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <stdint.h>
#include <string.h>
#include "mcu.h"
#include "mcu_timer.h"

uint64_t timer_cycles;
uint8_t timer_tempreg;

frt_t frt[3];
mcu_timer_t timer;

enum {
    REG_TCR = 0x00,
    REG_TCSR = 0x01,
    REG_FRCH = 0x02,
    REG_FRCL = 0x03,
    REG_OCRAH = 0x04,
    REG_OCRAL = 0x05,
    REG_OCRBH = 0x06,
    REG_OCRBL = 0x07,
    REG_ICRH = 0x08,
    REG_ICRL = 0x09,
};

void TIMER_Reset(void)
{
    timer_cycles = 0;
    timer_tempreg = 0;
    memset(frt, 0, sizeof(frt));
    memset(&timer, 0, sizeof(timer));
}

void TIMER_Write(uint32_t address, uint8_t data)
{
    uint32_t t = (address >> 4) - 1;
    if (t > 2)
        return;
    address &= 0x0f;
    frt_t *timer = &frt[t];
    switch (address)
    {
    case REG_TCR:
        timer->tcr = data;
        break;
    case REG_TCSR:
        timer->tcsr &= ~0xf;
        timer->tcsr |= data & 0xf;
        if ((data & 0x10) == 0 && (timer->status_rd & 0x10) != 0)
        {
            timer->tcsr &= ~0x10;
            timer->status_rd &= ~0x10;
            MCU_Interrupt_SetRequest(INTERRUPT_SOURCE_FRT0_FOVI + t * 4, 0);
        }
        if ((data & 0x20) == 0 && (timer->status_rd & 0x20) != 0)
        {
            timer->tcsr &= ~0x20;
            timer->status_rd &= ~0x20;
            MCU_Interrupt_SetRequest(INTERRUPT_SOURCE_FRT0_OCIA + t * 4, 0);
        }
        if ((data & 0x40) == 0 && (timer->status_rd & 0x40) != 0)
        {
            timer->tcsr &= ~0x40;
            timer->status_rd &= ~0x40;
            MCU_Interrupt_SetRequest(INTERRUPT_SOURCE_FRT0_OCIB + t * 4, 0);
        }
        break;
    case REG_FRCH:
    case REG_OCRAH:
    case REG_OCRBH:
    case REG_ICRH:
        timer_tempreg = data;
        break;
    case REG_FRCL:
        timer->frc = (timer_tempreg << 8) | data;
        break;
    case REG_OCRAL:
        timer->ocra = (timer_tempreg << 8) | data;
        break;
    case REG_OCRBL:
        timer->ocrb = (timer_tempreg << 8) | data;
        break;
    case REG_ICRL:
        timer->icr = (timer_tempreg << 8) | data;
        break;
    }
}

uint8_t TIMER_Read(uint32_t address)
{
    uint32_t t = (address >> 4) - 1;
    if (t > 2)
        return 0xff;
    address &= 0x0f;
    frt_t *timer = &frt[t];
    switch (address)
    {
    case REG_TCR:
        return timer->tcr;
    case REG_TCSR:
    {
        uint8_t ret = timer->tcsr;
        timer->status_rd |= timer->tcsr & 0xf0;
        //timer->status_rd |= 0xf0;
        return ret;
    }
    case REG_FRCH:
        timer_tempreg = timer->frc & 0xff;
        return timer->frc >> 8;
    case REG_OCRAH:
        timer_tempreg = timer->ocra & 0xff;
        return timer->ocra >> 8;
    case REG_OCRBH:
        timer_tempreg = timer->ocrb & 0xff;
        return timer->ocrb >> 8;
    case REG_ICRH:
        timer_tempreg = timer->icr & 0xff;
        return timer->icr >> 8;
    case REG_FRCL:
    case REG_OCRAL:
    case REG_OCRBL:
    case REG_ICRL:
        return timer_tempreg;
    }
    return 0xff;
}

void TIMER2_Write(uint32_t address, uint8_t data)
{
    switch (address)
    {
    case DEV_TMR_TCR:
        timer.tcr = data;
        break;
    case DEV_TMR_TCSR:
        timer.tcsr &= ~0xf;
        timer.tcsr |= data & 0xf;
        if ((data & 0x20) == 0 && (timer.status_rd & 0x20) != 0)
        {
            timer.tcsr &= ~0x20;
            timer.status_rd &= ~0x20;
            MCU_Interrupt_SetRequest(INTERRUPT_SOURCE_TIMER_OVI, 0);
        }
        if ((data & 0x40) == 0 && (timer.status_rd & 0x40) != 0)
        {
            timer.tcsr &= ~0x40;
            timer.status_rd &= ~0x40;
            MCU_Interrupt_SetRequest(INTERRUPT_SOURCE_TIMER_CMIA, 0);
        }
        if ((data & 0x80) == 0 && (timer.status_rd & 0x80) != 0)
        {
            timer.tcsr &= ~0x80;
            timer.status_rd &= ~0x80;
            MCU_Interrupt_SetRequest(INTERRUPT_SOURCE_TIMER_CMIB, 0);
        }
        break;
    case DEV_TMR_TCORA:
        timer.tcora = data;
        break;
    case DEV_TMR_TCORB:
        timer.tcorb = data;
        break;
    case DEV_TMR_TCNT:
        timer.tcnt = data;
        break;
    }
}
uint8_t TIMER_Read2(uint32_t address)
{
    switch (address)
    {
    case DEV_TMR_TCR:
        return timer.tcr;
    case DEV_TMR_TCSR:
    {
        uint8_t ret = timer.tcsr;
        timer.status_rd |= timer.tcsr & 0xe0;
        return ret;
    }
    case DEV_TMR_TCORA:
        return timer.tcora;
    case DEV_TMR_TCORB:
        return timer.tcorb;
    case DEV_TMR_TCNT:
        return timer.tcnt;
    }
    return 0xff;
}

// Number of k in [from, to) with k % (1 << shift) == 0
static inline uint32_t TIMER_Ticks(uint64_t from, uint64_t to, uint32_t shift)
{
    uint64_t mask = ((uint64_t)1 << shift) - 1;
    return (uint32_t)(((to + mask) >> shift) - ((from + mask) >> shift));
}

static void TIMER_StepFRT(uint32_t i)
{
    frt_t *timer = &frt[i];

    uint32_t value = timer->frc;
    uint32_t matcha = value == timer->ocra;
    uint32_t matchb = value == timer->ocrb;
    if ((timer->tcsr & 1) != 0 && matcha) // CCLRA
        value = 0;
    else
        value++;
    uint32_t of = (value >> 16) & 1;
    value &= 0xffff;
    timer->frc = value;

    // flags
    if (of)
        timer->tcsr |= 0x10;
    if (matcha)
        timer->tcsr |= 0x20;
    if (matchb)
        timer->tcsr |= 0x40;
}

static void TIMER_Step8(void)
{
    uint32_t value = timer.tcnt;
    uint32_t matcha = value == timer.tcora;
    uint32_t matchb = value == timer.tcorb;
    if ((timer.tcr & 24) == 8 && matcha)
        value = 0;
    else if ((timer.tcr & 24) == 16 && matchb)
        value = 0;
    else
        value++;
    uint32_t of = (value >> 8) & 1;
    value &= 0xff;
    timer.tcnt = value;

    // flags
    if (of)
        timer.tcsr |= 0x20;
    if (matcha)
        timer.tcsr |= 0x40;
    if (matchb)
        timer.tcsr |= 0x80;
}

// Advances the timers to cycles/2 in one go. Same result as stepping one
// timer clock at a time: when no compare match or overflow can happen within
// the step the counter is just advanced, otherwise it is stepped tick by
// tick. The interrupt requests are only ever set to 1 here, so asserting them
// once after a step is the same as asserting them on every tick.
void TIMER_Clock(uint64_t cycles)
{
    uint64_t from = timer_cycles;
    uint64_t to = (cycles + 1) / 2; // FIXME
    if (to <= from)
        return;
    timer_cycles = to;

    uint32_t i;
    for (i = 0; i < 3; i++)
    {
        frt_t *timer = &frt[i];
        uint32_t shift = 0;

        switch (timer->tcr & 3)
        {
        case 0: // o / 4
            shift = 2;
            break;
        case 1: // o / 8
            shift = 3;
            break;
        case 2: // o / 32
            shift = 5;
            break;
        case 3: // ext (o / 2)
            shift = mcu_mk1 ? 2 : 1;
            break;
        }

        uint32_t n = TIMER_Ticks(from, to, shift);
        if (n == 0)
            continue;

        uint32_t value = timer->frc;
        uint32_t last = value + n - 1; // last value compared
        if (last < 0xffff
            && (timer->ocra < value || timer->ocra > last)
            && (timer->ocrb < value || timer->ocrb > last))
        {
            timer->frc = value + n;
        }
        else
        {
            for (uint32_t k = 0; k < n; k++)
                TIMER_StepFRT(i);
        }

        if ((timer->tcr & 0x10) != 0 && (timer->tcsr & 0x10) != 0)
            MCU_Interrupt_SetRequest(INTERRUPT_SOURCE_FRT0_FOVI + i * 4, 1);
        if ((timer->tcr & 0x20) != 0 && (timer->tcsr & 0x20) != 0)
            MCU_Interrupt_SetRequest(INTERRUPT_SOURCE_FRT0_OCIA + i * 4, 1);
        if ((timer->tcr & 0x40) != 0 && (timer->tcsr & 0x40) != 0)
            MCU_Interrupt_SetRequest(INTERRUPT_SOURCE_FRT0_OCIB + i * 4, 1);
    }

    uint32_t shift = 0;

    switch (timer.tcr & 7)
    {
    case 0:
    case 4:
        break;
    case 1: // o / 8
        shift = 3;
        break;
    case 2: // o / 64
        shift = 6;
        break;
    case 3: // o / 1024
        shift = 10;
        break;
    case 5:
    case 6:
    case 7: // ext (o / 2)
        shift = mcu_mk1 ? 2 : 1;
        break;
    }

    uint32_t n = shift ? TIMER_Ticks(from, to, shift) : 0;
    if (n)
    {
        uint32_t value = timer.tcnt;
        uint32_t last = value + n - 1; // last value compared
        uint32_t clear = 0x100; // counter is cleared after reaching this value
        if ((timer.tcr & 24) == 8)
            clear = timer.tcora;
        else if ((timer.tcr & 24) == 16)
            clear = timer.tcorb;

        if (last < 0xff
            && (timer.tcora < value || timer.tcora > last)
            && (timer.tcorb < value || timer.tcorb > last))
        {
            timer.tcnt = value + n;
        }
        else if (clear < 0x100 && value <= clear)
        {
            // Periodic: counts value, value + 1, ... clear, 0, 1, ... and
            // never overflows. A compare value is matched if it is reached
            // within the n compared values.
            uint32_t period = clear + 1;
            uint32_t da = timer.tcora >= value ? timer.tcora - value : timer.tcora + period - value;
            uint32_t db = timer.tcorb >= value ? timer.tcorb - value : timer.tcorb + period - value;
            if (timer.tcora <= clear && da < n)
                timer.tcsr |= 0x40;
            if (timer.tcorb <= clear && db < n)
                timer.tcsr |= 0x80;
            uint32_t next = value + n;
            while (next >= period)
                next -= period;
            timer.tcnt = next;
        }
        else
        {
            for (uint32_t k = 0; k < n; k++)
                TIMER_Step8();
        }

        if ((timer.tcr & 0x20) != 0 && (timer.tcsr & 0x20) != 0)
            MCU_Interrupt_SetRequest(INTERRUPT_SOURCE_TIMER_OVI, 1);
        if ((timer.tcr & 0x40) != 0 && (timer.tcsr & 0x40) != 0)
            MCU_Interrupt_SetRequest(INTERRUPT_SOURCE_TIMER_CMIA, 1);
        if ((timer.tcr & 0x80) != 0 && (timer.tcsr & 0x80) != 0)
            MCU_Interrupt_SetRequest(INTERRUPT_SOURCE_TIMER_CMIB, 1);
    }
}
