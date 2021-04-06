#include "hardware.h"
#include "display.h"
#include "formats.h"
#include "gpio.h"
#include "spi.h"

#include <string.h>

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
	__disable_irq();
	prog_spi_send_frame(target, display_buffer_gray, sizeof display_buffer_gray / sizeof display_buffer_gray [0]);
	__enable_irq();
	spi_unselect(target);
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
