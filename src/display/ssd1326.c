#include "hardware.h"
#include "display.h"
#include "formats.h"
#include "gpio.h"
#include "spi.h"

#include <string.h>

#if LCDMODE_SSD1326

#include "src/display/fonts/ssd1326_fonts.h"

#define CTLREG_SPIMODE	SPIC_MODE3

uint8_t display_buffer_gray [4096];

void Write_Data(uint_fast8_t dat)
{
	const spitarget_t target = targetlcd;
	uint8_t buff [1] = { dat };
	SSD1326_CONTROL_PORT_S(SSD1326_DC_PIN);
	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, buff, sizeof buff / sizeof buff [0]);
	spi_unselect(target);
}
// ************************************************************************
void Write_Instruction(uint_fast8_t cmd)
{
	const spitarget_t target = targetlcd;
	uint8_t buff [1] = { cmd };
	SSD1326_CONTROL_PORT_C(SSD1326_DC_PIN);
	spi_select(target, CTLREG_SPIMODE);
	prog_spi_send_frame(target, buff, sizeof buff / sizeof buff [0]);
	spi_unselect(target);
}
// ************************************************************************

void ssd1326_send_framebuffer(void)
{
	const spitarget_t target = targetlcd;

	Write_Instruction(0x15); //Set Column Address Gray Scale mode;
	Write_Instruction(0x00);
	Write_Instruction(0x7F);
	Write_Instruction(0x75); //Set Row Address
	Write_Instruction(0x00);
	Write_Instruction(0x1F);
	SSD1326_CONTROL_PORT_S(SSD1326_DC_PIN);
	spi_select(target, CTLREG_SPIMODE);
	//__disable_irq();
	prog_spi_send_frame(target, display_buffer_gray, sizeof display_buffer_gray / sizeof display_buffer_gray [0]);
	//__enable_irq();
	spi_unselect(target);

	memset(display_buffer_gray, 0x00, 4096);
}

void ssd1326_init(void)
{
	SSD1326_CONTROL_INITIALIZE();

	SSD1326_CONTROL_PORT_C(SSD1326_RS_PIN);
	local_delay_ms(100);
	SSD1326_CONTROL_PORT_S(SSD1326_RS_PIN);
	local_delay_ms(100);

	Write_Instruction(0xAF);
	Write_Instruction(0xFD); //Set Command Lock
	Write_Instruction(0x12);
	Write_Instruction(0xAE); //Set Display ON/OFF

	Write_Instruction(0x15); //Set Column Address Gray Scale mode;
	Write_Instruction(0x00);
	Write_Instruction(0x7F);

	Write_Instruction(0x75); //Set Row Address
	Write_Instruction(0x00);
	Write_Instruction(0x1F);

	Write_Instruction(0x81); //Set Contrast Current
	Write_Instruction(0x60); //Contrast level
	Write_Instruction(0x87); //Set Current Range

	Write_Instruction(0xA0); //Set Re-map and Gray Scale /Mono Mode
	Write_Instruction(0x06); //Gray Mode  //Mono Mode Enable Column Address Re-map, Enable COM Re-map, Enable Bit Re-map, Enable Horizontal Address Increment

	Write_Instruction(0xA1); //Set Display Start Line
	Write_Instruction(0x00);
	Write_Instruction(0xA2); //Set Display Offset
	Write_Instruction(0x00);
	Write_Instruction(0xA8); //Set MUX Ratio
	Write_Instruction(0x1F);
	Write_Instruction(0xB1); //Set Phase Length
	Write_Instruction(0x71);
	Write_Instruction(0xB3); //Set Front Clock Divider / Oscillator
	Write_Instruction(0xF0);
	Write_Instruction(0xB7); //Select Default Linear Gray Scale table
	Write_Instruction(0xBB); //Set Pre-charge Steup
	Write_Instruction(0x35);
	Write_Instruction(0xFF);
	Write_Instruction(0xBC); //Set Pre-charge voltage
	Write_Instruction(0x1F);
	Write_Instruction(0xBE); //Set VCOMH
	Write_Instruction(0x0F);
	Write_Instruction(0xAF); //Set Display ON/OFF

	memset(display_buffer_gray, 0x00, 4096);
	ssd1326_send_framebuffer();
}
// ************************************************************************
void ssd1326_draw_point (uint_fast16_t x, uint_fast8_t y, uint_fast8_t color)
{
	if (x >= DIM_X)
		x = DIM_X - 1;

	if (y >= DIM_Y)
		y = DIM_Y - 1;

	uint_fast16_t offset = y * 128 + x / 2;

	if(x % 2)
		display_buffer_gray [offset] = (display_buffer_gray [offset] & 0xF0) | color;
	else
		display_buffer_gray [offset] = (color << 4) | (display_buffer_gray [offset] & 0xF);
}

void ssd1326_draw_line(uint_fast16_t x1, uint_fast8_t y1, uint_fast16_t x2, uint_fast8_t y2, uint_fast8_t color)
{
	uint_fast16_t t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;

	if (x1 >= DIM_X)
		x1 = DIM_X - 1;

	if (y1 >= DIM_Y)
		y1 = DIM_Y - 1;

	if (x2 >= DIM_X)
		x2 = DIM_X - 1;

	if (y2 >= DIM_Y)
		y2 = DIM_Y - 1;

	delta_x=x2-x1;
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;
	if(delta_x>0)incx=1;
	else if(delta_x==0)incx=0;
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if(delta_y==0)incy=0;
	else{incy=-1;delta_y=-delta_y;}
	if( delta_x>delta_y)distance=delta_x;
	else distance=delta_y;
	for(t=0;t<=distance+1;t++ )
	{
		ssd1326_draw_point(uRow,uCol,color);
		xerr+=delta_x ;
		yerr+=delta_y ;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

void DrawSingleAscii8x16(uint_fast8_t x, uint_fast8_t y, char *pAscii, uint_fast8_t inverse)
{
	uint_fast16_t OffsetSym;
	uint_fast8_t x1, y1, str, front, back;
	if (inverse)
	{
		front = 0;
		back = 15;
	}
	else
	{
		front = 15;
		back = 0;
	}

	OffsetSym = (* pAscii - 32) * 16;

	for (y1 = 0; y1 < 16; y1 ++)
	{
		str = * (ASCII_8x16 + OffsetSym + y1);
		for (x1 = 0; x1 < 8; x1 ++)
		{
			if ((str >> x1) & 1)
				ssd1326_draw_point (x + 8 - x1, y + y1, front);
			else
				ssd1326_draw_point (x + 8 - x1, y + y1, back);
		}
	}
}

void DrawSingleAscii16x32(uint_fast8_t x, uint_fast8_t y, char * pAscii, uint_fast8_t inverse)
{
	uint_fast16_t OffsetSym, str;
	uint_fast8_t x1, y1, front, back;

	if (inverse)
	{
		front = 0;
		back = 15;
	}
	else
	{
		front = 15;
		back = 0;
	}

	OffsetSym = (* pAscii - 32) * 64;

	for (y1 = 0; y1 < 32; y1 ++)
	{
		memcpy (& str, ASCII_16x32 + OffsetSym + (y1 * 2), 2);
		str = ((str << 8) | (str >> 8)) & 0xFFFF;
		for (x1 = 0; x1 < 16; x1 ++)
		{
			if (str >> x1 & 1)
				ssd1326_draw_point (x + 16 - x1, y + y1, front);
			else
				ssd1326_draw_point (x + 16 - x1, y + y1, back);
		}
	}
}

// ************************************************************************
void ssd1326_DrawString_small(uint_fast8_t col, uint_fast8_t row, char * pStr, uint_fast8_t inverse)
{
	ASSERT(col < 32);
	ASSERT(row < 2);
    while(1)
    {
        if (* pStr == 0 || * pStr > 0x7e) return;
        else
        {
            DrawSingleAscii8x16(col * 8, row * 16, pStr, inverse);
            col ++;
			if (col > 32)
				return;
            pStr ++;
        }
    }
}

void ssd1326_DrawString_big(uint_fast8_t col, char * pStr, uint_fast8_t inverse)
{
    while(1)
    {
        if (* pStr == 0 || * pStr > 0x7e) return;
        else
        {
            DrawSingleAscii16x32(col * 16, 0, pStr, inverse);
            col ++;
			if (col > 32)
				return;
            pStr ++;
        }
    }
}

#endif /* LCDMODE_SSD1326 */
