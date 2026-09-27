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
// LCD stub for headless builds (NUKED_HEADLESS): the MCU still drives the
// LCD controller, but nothing is rendered.
#include "lcd.h"

int lcd_width = 741;
int lcd_height = 268;

uint32_t lcd_col1 = 0x000000;
uint32_t lcd_col2 = 0x0050c8;

void LCD_SetBackPath(const std::string &/*path*/)
{
}

void LCD_Init(void)
{
}

void LCD_UnInit(void)
{
}

void LCD_Write(uint32_t /*address*/, uint8_t /*data*/)
{
}

void LCD_Enable(uint32_t /*enable*/)
{
}

bool LCD_QuitRequested()
{
    return false;
}

void LCD_Sync(void)
{
}

void LCD_Update(void)
{
}
