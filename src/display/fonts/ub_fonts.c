#include "hardware.h"

#if WITHALTERNATIVEFONTS
#include "ub_fonts.h"

//--------------------------------------------------------------
// Рисование ASCII символ пропорционального шрифта с позицией X, Y
// Цвет шрифта плана и фона (шрифт = макс 16 пикселей в ширину)
// Шрифт должен быть передан с оператором &
// Возвращает: ширину нарисованного символа
//--------------------------------------------------------------
static uint_fast16_t UB_Font_DrawPChar(PACKEDCOLORMAIN_T * __restrict buffer,
		uint_fast16_t dx, uint_fast16_t dy,
		uint_fast16_t x, uint_fast16_t y,
		uint8_t ascii, const UB_pFont * font,
		COLORMAIN_T vg)
{
	uint_fast16_t xn, yn, start_maske, maske, width;
	const uint16_t * wert;

	// Проверка границы символа
	if(ascii < font->first_char)
		return 0;

	if(ascii > font->last_char)
		return 0;

	ascii -= font->first_char;
	wert = & font->table [ascii * (font->height + 1)];
	width = wert [0];

	start_maske = 0x01;
	start_maske = start_maske << (width - 1);

	for (yn = 0; yn < font->height; yn ++)
	{
		maske = start_maske;

		for (xn = 0; xn < width; xn ++)
		{
			if ((wert [yn + 1] & maske))
				* colmain_mem_at(buffer, dx, dy, x + xn, yn + y) = vg;

			maske = (maske >> 1);
		}
	}

	return width;
}

//--------------------------------------------------------------
// Рисование строки пропорционального шрифта с позицией X, Y
// Цвет шрифта плана и фона (шрифт = макс 16 пикселей в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawPString(PACKEDCOLORMAIN_T * __restrict buffer,
		uint_fast16_t dx, uint_fast16_t dy,
		uint_fast16_t x, uint_fast16_t y,
		const char * ptr, const UB_pFont * font,
		COLORMAIN_T vg)
{
	while (* ptr != '\0')
	{
		uint_fast16_t width = UB_Font_DrawPChar(buffer, dx, dy, x, y, (unsigned char) * ptr, font, vg);
		x += width;
		ptr ++;
	}
}

//--------------------------------------------------------------
// Рисование ASCII символ пропорционального шрифта с позицией X, Y
// Цвет шрифта плана и фона (шрифт = макс 32 пикселя в ширину)
// Шрифт должен быть передан с оператором &
// Возвращает: ширину нарисованного символа
//--------------------------------------------------------------
static uint_fast16_t UB_Font_DrawPChar32(PACKEDCOLORMAIN_T * __restrict buffer,
		uint_fast16_t dx, uint_fast16_t dy,
		uint_fast16_t x, uint_fast16_t y,
		uint8_t ascii, const UB_pFont32 * font,
		COLORMAIN_T vg)
{
	uint_fast16_t xn, yn, width;
	uint_fast32_t start_maske, maske;
	const uint32_t * wert;

	// Проверка границы символа
	if (ascii < font->first_char)
		return 0;

	if (ascii > font->last_char)
		return 0;

	ascii -= font->first_char;
	wert = & font->table [ascii * (font->height + 1)];
	width = wert [0];
	start_maske = 0x01;
	start_maske = start_maske << (width - 1);

	for (yn = 0; yn < font->height; yn ++)
	{
		maske = start_maske;

		for(xn = 0; xn < width; xn++)
		{
			if((wert [yn + 1] & maske))
				* colmain_mem_at(buffer, dx, dy, x + xn, yn + y) = vg;

			maske = (maske >> 1);
		}
	}

	return(width);
}

//--------------------------------------------------------------
// Рисование строку пропорционального шрифта с позицией X, Y
// Цвет шрифта плана и фона (шрифт = макс 32 пикселя в ширину)
// Шрифт должен быть передан с оператором &
//--------------------------------------------------------------
void UB_Font_DrawPString32(PACKEDCOLORMAIN_T * __restrict buffer,
		uint_fast16_t dx, uint_fast16_t dy,
		uint_fast16_t x, uint_fast16_t y,
		const char * ptr, const UB_pFont32 * font,
		COLORMAIN_T vg)
{
	while (* ptr != '\0')
	{
		uint_fast16_t width = UB_Font_DrawPChar32(buffer, dx, dy, x, y, (unsigned char) * ptr, font, vg);
		x += width;
		ptr ++;
	}
}

static uint16_t UB_Font_getPcharw32(uint8_t ascii, const UB_pFont32 * font)
{
	uint16_t width;
	uint32_t start_maske, maske;
	const uint32_t * wert;

	if(ascii < font->first_char)
		return 0;

	if(ascii > font->last_char)
		return 0;

	ascii -= font->first_char;
	wert = & font->table [ascii * (font->height + 1)];
	width = wert [0];

	return width;
}

// Возврат ширины строки в пикселях, пропорциональный шрифт 32 бит
uint_fast16_t getwidth_Pstring32(const char * str, const UB_pFont32 * font)
{
	uint16_t width = 0;
	while (* str != '\0')
	{
		width += UB_Font_getPcharw32((unsigned char) * str, font);
		str ++;
	}

	return width;
}

static uint16_t UB_Font_getPcharw(uint8_t ascii, const UB_pFont * font)
{
	uint16_t width;
	uint32_t start_maske, maske;
	const uint16_t * wert;

	if(ascii < font->first_char)
		return 0;

	if(ascii > font->last_char)
		return 0;

	ascii -= font->first_char;
	wert = & font->table [ascii * (font->height + 1)];
	width = wert [0];

	return(width);
}

// Возврат ширины строки в пикселях, пропорциональный шрифт меньше 32 бит
uint_fast16_t getwidth_Pstring(const char * str, const UB_pFont * font)
{
	uint_fast16_t width = 0;
	while (* str != '\0')
	{
		width += UB_Font_getPcharw((unsigned char) * str, font);
		str ++;
	}

	return width;
}

// *********************************************************************************************************************

//--------------------------------------------------------------
// Font-Daten
// erstellt von UB mit PixelFontGenerator 1.8
// Source-Font :
// Name:MS PGothic  /  Size:13  /  Style:[B]
// First-Ascii : 32
// Last-Ascii  : 126
//--------------------------------------------------------------
const uint16_t gothic_12x16_Table[] = {
0x06,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:32 = [ ]
0x05,0x0000,0x0000,0x0000,0x0018,0x0018,0x0018,0x0018,0x0018,0x0018,0x0018,0x0018,0x0000,0x0000,0x0018,0x0000,0x0000, // Ascii:33 = [!]
0x0A,0x0000,0x00D8,0x01B0,0x0360,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:34 = ["]
0x0A,0x0000,0x0000,0x0000,0x00D8,0x00D8,0x00D8,0x03FC,0x00D8,0x01B0,0x01B0,0x03FC,0x01B0,0x01B0,0x01B0,0x0000,0x0000, // Ascii:35 = [#]
0x0A,0x0000,0x0000,0x00C0,0x01F0,0x03D8,0x03D8,0x03C0,0x01C0,0x00E0,0x00F0,0x03D8,0x03D8,0x03D8,0x01F0,0x00C0,0x0000, // Ascii:36 = [$]
0x0A,0x0000,0x0000,0x0000,0x0198,0x03F0,0x03F0,0x03F0,0x03E0,0x01F8,0x00FC,0x00FC,0x00FC,0x01BC,0x0198,0x0000,0x0000, // Ascii:37 = [%]
0x0B,0x0000,0x0000,0x0000,0x01E0,0x0330,0x0330,0x0360,0x01C0,0x0380,0x06CC,0x066C,0x063C,0x061C,0x03F6,0x0000,0x0000, // Ascii:38 = [&]
0x04,0x0000,0x0000,0x0000,0x0006,0x0006,0x000C,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:39 = [']
0x06,0x0006,0x000C,0x0018,0x0018,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0018,0x0018,0x000C,0x0006,0x0000, // Ascii:40 = [(]
0x06,0x0030,0x0018,0x000C,0x000C,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x000C,0x000C,0x0018,0x0030,0x0000, // Ascii:41 = [)]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0060,0x0060,0x03FC,0x00F0,0x00F0,0x03FC,0x0060,0x0060,0x0000,0x0000,0x0000,0x0000, // Ascii:42 = [*]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0060,0x0060,0x0060,0x03FC,0x0060,0x0060,0x0060,0x0000,0x0000,0x0000,0x0000, // Ascii:43 = [+]
0x04,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0006,0x0006,0x000C, // Ascii:44 = [,]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x03FC,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:45 = [-]
0x04,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0006,0x0000,0x0000, // Ascii:46 = [.]
0x0A,0x0000,0x0000,0x000C,0x0018,0x0018,0x0030,0x0030,0x0030,0x0060,0x0060,0x00C0,0x00C0,0x0180,0x0180,0x0300,0x0000, // Ascii:47 = [/]
0x0A,0x0000,0x0000,0x0000,0x00F0,0x0198,0x030C,0x030C,0x030C,0x030C,0x030C,0x030C,0x030C,0x0198,0x00F0,0x0000,0x0000, // Ascii:48 = [0]
0x0A,0x0000,0x0000,0x0000,0x0060,0x00E0,0x01E0,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0000,0x0000, // Ascii:49 = [1]
0x0A,0x0000,0x0000,0x0000,0x00E0,0x01B0,0x0318,0x0318,0x0018,0x0030,0x0060,0x00C0,0x0180,0x0300,0x03F8,0x0000,0x0000, // Ascii:50 = [2]
0x0A,0x0000,0x0000,0x0000,0x01F0,0x0318,0x0318,0x0018,0x0018,0x0070,0x0018,0x0318,0x0318,0x0318,0x01F0,0x0000,0x0000, // Ascii:51 = [3]
0x0A,0x0000,0x0000,0x0000,0x0018,0x0038,0x0078,0x00D8,0x00D8,0x0198,0x0318,0x03FC,0x0018,0x0018,0x0018,0x0000,0x0000, // Ascii:52 = [4]
0x0A,0x0000,0x0000,0x0000,0x03F8,0x0300,0x0300,0x03F0,0x0318,0x0018,0x0018,0x0018,0x0318,0x0318,0x01F0,0x0000,0x0000, // Ascii:53 = [5]
0x0A,0x0000,0x0000,0x0000,0x00F0,0x0198,0x0318,0x0300,0x03F0,0x0398,0x0318,0x0318,0x0318,0x0198,0x00F0,0x0000,0x0000, // Ascii:54 = [6]
0x0A,0x0000,0x0000,0x0000,0x03F8,0x0018,0x0018,0x0030,0x0030,0x0030,0x0060,0x0060,0x0060,0x00C0,0x00C0,0x0000,0x0000, // Ascii:55 = [7]
0x0A,0x0000,0x0000,0x0000,0x00F0,0x0198,0x030C,0x030C,0x0198,0x00F0,0x0198,0x030C,0x030C,0x0198,0x00F0,0x0000,0x0000, // Ascii:56 = [8]
0x0A,0x0000,0x0000,0x0000,0x01E0,0x0330,0x0318,0x0318,0x0318,0x0338,0x01F8,0x0018,0x0318,0x0330,0x01E0,0x0000,0x0000, // Ascii:57 = [9]
0x04,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0006,0x0000,0x0000,0x0000,0x0000,0x0000,0x0006,0x0000,0x0000, // Ascii:58 = [:]
0x04,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0006,0x0000,0x0000,0x0000,0x0000,0x0000,0x0006,0x0006,0x000C, // Ascii:59 = [;]
0x0A,0x0000,0x0000,0x0000,0x0018,0x0030,0x0060,0x00C0,0x0180,0x0300,0x0180,0x00C0,0x0060,0x0030,0x0018,0x0000,0x0000, // Ascii:60 = [<]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x03FC,0x0000,0x0000,0x0000,0x03FC,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:61 = [=]
0x0A,0x0000,0x0000,0x0000,0x0300,0x0180,0x00C0,0x0060,0x0030,0x0018,0x0030,0x0060,0x00C0,0x0180,0x0300,0x0000,0x0000, // Ascii:62 = [>]
0x09,0x0000,0x0000,0x0000,0x00F8,0x018C,0x018C,0x000C,0x0018,0x0030,0x0060,0x0060,0x0000,0x0000,0x0060,0x0000,0x0000, // Ascii:63 = [?]
0x0C,0x0000,0x0000,0x0000,0x01E0,0x0738,0x06F8,0x0DBC,0x0F3C,0x0F6C,0x0F6C,0x0F6C,0x07F8,0x0700,0x01F0,0x0000,0x0000, // Ascii:64 = [@]
0x0C,0x0000,0x0000,0x0000,0x00E0,0x00E0,0x01B0,0x01B0,0x01B0,0x0318,0x0318,0x07FC,0x060C,0x060C,0x0C06,0x0000,0x0000, // Ascii:65 = [A]
0x0C,0x0000,0x0000,0x0000,0x07F0,0x0618,0x060C,0x060C,0x0618,0x07F0,0x0618,0x060C,0x060C,0x0618,0x07F0,0x0000,0x0000, // Ascii:66 = [B]
0x0C,0x0000,0x0000,0x0000,0x01F0,0x0318,0x060C,0x0C0C,0x0C00,0x0C00,0x0C00,0x0C0C,0x060C,0x0318,0x01F0,0x0000,0x0000, // Ascii:67 = [C]
0x0C,0x0000,0x0000,0x0000,0x07E0,0x0630,0x0618,0x060C,0x060C,0x060C,0x060C,0x060C,0x0618,0x0630,0x07E0,0x0000,0x0000, // Ascii:68 = [D]
0x0B,0x0000,0x0000,0x0000,0x03FC,0x0300,0x0300,0x0300,0x0300,0x03F8,0x0300,0x0300,0x0300,0x0300,0x03FC,0x0000,0x0000, // Ascii:69 = [E]
0x0A,0x0000,0x0000,0x0000,0x01FE,0x0180,0x0180,0x0180,0x0180,0x01FC,0x0180,0x0180,0x0180,0x0180,0x0180,0x0000,0x0000, // Ascii:70 = [F]
0x0D,0x0000,0x0000,0x0000,0x03E0,0x0630,0x0C18,0x1818,0x1800,0x1878,0x1818,0x1818,0x0C18,0x0638,0x03F8,0x0000,0x0000, // Ascii:71 = [G]
0x0C,0x0000,0x0000,0x0000,0x060C,0x060C,0x060C,0x060C,0x060C,0x07FC,0x060C,0x060C,0x060C,0x060C,0x060C,0x0000,0x0000, // Ascii:72 = [H]
0x05,0x0000,0x0000,0x0000,0x001E,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x001E,0x0000,0x0000, // Ascii:73 = [I]
0x0A,0x0000,0x0000,0x0000,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x030C,0x030C,0x0198,0x00F0,0x0000,0x0000, // Ascii:74 = [J]
0x0B,0x0000,0x0000,0x0000,0x030C,0x0318,0x0330,0x0360,0x03C0,0x03E0,0x0330,0x0330,0x0318,0x030C,0x0306,0x0000,0x0000, // Ascii:75 = [K]
0x0A,0x0000,0x0000,0x0000,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x01FE,0x0000,0x0000, // Ascii:76 = [L]
0x0E,0x0000,0x0000,0x0000,0x1806,0x1C0E,0x1C0E,0x1E1E,0x1E1E,0x1E1E,0x1B36,0x1B36,0x19E6,0x19E6,0x18C6,0x0000,0x0000, // Ascii:77 = [M]
0x0C,0x0000,0x0000,0x0000,0x060C,0x070C,0x078C,0x078C,0x06CC,0x06CC,0x066C,0x063C,0x063C,0x061C,0x060C,0x0000,0x0000, // Ascii:78 = [N]
0x0D,0x0000,0x0000,0x0000,0x03C0,0x0E70,0x0C30,0x1818,0x1818,0x1818,0x1818,0x1818,0x0C30,0x0E70,0x03C0,0x0000,0x0000, // Ascii:79 = [O]
0x0B,0x0000,0x0000,0x0000,0x03F8,0x030C,0x0306,0x0306,0x0306,0x030C,0x03F8,0x0300,0x0300,0x0300,0x0300,0x0000,0x0000, // Ascii:80 = [P]
0x0D,0x0000,0x0000,0x0000,0x03C0,0x0E70,0x0C30,0x1818,0x1818,0x1818,0x1818,0x18D8,0x0CF0,0x0E60,0x03F0,0x0000,0x0000, // Ascii:81 = [Q]
0x0C,0x0000,0x0000,0x0000,0x07F0,0x0618,0x060C,0x060C,0x0618,0x07F0,0x0630,0x0618,0x0618,0x0618,0x060C,0x0000,0x0000, // Ascii:82 = [R]
0x0B,0x0000,0x0000,0x0000,0x00F8,0x018C,0x0306,0x0300,0x01C0,0x0078,0x000C,0x0306,0x0306,0x018C,0x00F8,0x0000,0x0000, // Ascii:83 = [S]
0x0B,0x0000,0x0000,0x0000,0x07FE,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0000,0x0000, // Ascii:84 = [T]
0x0C,0x0000,0x0000,0x0000,0x060C,0x060C,0x060C,0x060C,0x060C,0x060C,0x060C,0x060C,0x060C,0x0318,0x01F0,0x0000,0x0000, // Ascii:85 = [U]
0x0C,0x0000,0x0000,0x0000,0x0C06,0x060C,0x060C,0x060C,0x0318,0x0318,0x01B0,0x01B0,0x01B0,0x00E0,0x00E0,0x0000,0x0000, // Ascii:86 = [V]
0x0E,0x0000,0x0000,0x0000,0x31C6,0x31C6,0x31C6,0x1B6C,0x1B6C,0x1B6C,0x1E3C,0x1E3C,0x1E3C,0x0C18,0x0C18,0x0000,0x0000, // Ascii:87 = [W]
0x0B,0x0000,0x0000,0x0000,0x0606,0x030C,0x0198,0x00F0,0x00F0,0x0060,0x00F0,0x00F0,0x0198,0x030C,0x0606,0x0000,0x0000, // Ascii:88 = [X]
0x0B,0x0000,0x0000,0x0000,0x0606,0x030C,0x0198,0x0198,0x00F0,0x0060,0x0060,0x0060,0x0060,0x0060,0x0060,0x0000,0x0000, // Ascii:89 = [Y]
0x0B,0x0000,0x0000,0x0000,0x07F8,0x0018,0x0030,0x0060,0x0060,0x00C0,0x0180,0x0300,0x0300,0x0600,0x07F8,0x0000,0x0000, // Ascii:90 = [Z]
0x07,0x003E,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x0030,0x003E,0x0000, // Ascii:91 = [[]
0x0A,0x0000,0x0000,0x0000,0x030C,0x0198,0x0198,0x00F0,0x00F0,0x01F8,0x0060,0x01F8,0x0060,0x0060,0x0060,0x0000,0x0000, // Ascii:92 = [\]
0x07,0x007C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x007C,0x0000, // Ascii:93 = []]
0x08,0x0000,0x0038,0x006C,0x00C6,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:94 = [^]
0x06,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x003E, // Ascii:95 = [_]
0x08,0x0000,0x0070,0x0038,0x000C,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:96 = [`]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x00F8,0x018C,0x000C,0x00FC,0x018C,0x019C,0x00FC,0x0000,0x0000, // Ascii:97 = [a]
0x09,0x0000,0x0000,0x0000,0x00C0,0x00C0,0x00C0,0x00C0,0x00FC,0x00E6,0x00C6,0x00C6,0x00C6,0x00E6,0x00FC,0x0000,0x0000, // Ascii:98 = [b]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x00F8,0x018C,0x0300,0x0300,0x0300,0x018C,0x00F8,0x0000,0x0000, // Ascii:99 = [c]
0x09,0x0000,0x0000,0x0000,0x000C,0x000C,0x000C,0x000C,0x00FC,0x019C,0x018C,0x018C,0x018C,0x019C,0x00FC,0x0000,0x0000, // Ascii:100 = [d]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x00F0,0x0198,0x030C,0x03FC,0x0300,0x018C,0x00F8,0x0000,0x0000, // Ascii:101 = [e]
0x07,0x0000,0x0000,0x0000,0x000F,0x0018,0x0018,0x0018,0x007E,0x0018,0x0018,0x0018,0x0018,0x0018,0x0018,0x0000,0x0000, // Ascii:102 = [f]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x007E,0x00CC,0x00CC,0x00CC,0x0078,0x00C0,0x00FC,0x0186,0x00FC, // Ascii:103 = [g]
0x0A,0x0000,0x0000,0x0000,0x0180,0x0180,0x0180,0x0180,0x01F8,0x01CC,0x018C,0x018C,0x018C,0x018C,0x018C,0x0000,0x0000, // Ascii:104 = [h]
0x05,0x0000,0x0000,0x0000,0x000C,0x000C,0x0000,0x0000,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x0000,0x0000, // Ascii:105 = [i]
0x05,0x0000,0x0000,0x0000,0x0006,0x0006,0x0000,0x0000,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x001C, // Ascii:106 = [j]
0x09,0x0000,0x0000,0x0000,0x00C0,0x00C0,0x00C0,0x00C0,0x00CC,0x00D8,0x00F0,0x00F8,0x00D8,0x00CC,0x00C6,0x0000,0x0000, // Ascii:107 = [k]
0x05,0x0000,0x0000,0x0000,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x0000,0x0000, // Ascii:108 = [l]
0x0D,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0FB8,0x0EEC,0x0CCC,0x0CCC,0x0CCC,0x0CCC,0x0CCC,0x0000,0x0000, // Ascii:109 = [m]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x01F8,0x01CC,0x018C,0x018C,0x018C,0x018C,0x018C,0x0000,0x0000, // Ascii:110 = [n]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x00F0,0x0198,0x030C,0x030C,0x030C,0x0198,0x00F0,0x0000,0x0000, // Ascii:111 = [o]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x00FC,0x00E6,0x00C6,0x00C6,0x00C6,0x00E6,0x00FC,0x00C0,0x00C0, // Ascii:112 = [p]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x00FC,0x019C,0x018C,0x018C,0x018C,0x019C,0x00FC,0x000C,0x000C, // Ascii:113 = [q]
0x07,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0037,0x003C,0x0030,0x0030,0x0030,0x0030,0x0030,0x0000,0x0000, // Ascii:114 = [r]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x00F8,0x018C,0x0180,0x00F8,0x000C,0x018C,0x00F8,0x0000,0x0000, // Ascii:115 = [s]
0x07,0x0000,0x0000,0x0000,0x0000,0x0000,0x0018,0x0018,0x007E,0x0018,0x0018,0x0018,0x0018,0x0018,0x000E,0x0000,0x0000, // Ascii:116 = [t]
0x0A,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x018C,0x018C,0x018C,0x018C,0x018C,0x019C,0x00FC,0x0000,0x0000, // Ascii:117 = [u]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0186,0x00CC,0x00CC,0x00CC,0x0078,0x0078,0x0030,0x0000,0x0000, // Ascii:118 = [v]
0x0C,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0CE6,0x0CE6,0x06EC,0x07BC,0x07BC,0x0318,0x0318,0x0000,0x0000, // Ascii:119 = [w]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0186,0x00CC,0x0078,0x0030,0x0078,0x00CC,0x0186,0x0000,0x0000, // Ascii:120 = [x]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0186,0x00CC,0x00CC,0x00CC,0x0078,0x0078,0x0030,0x0030,0x00E0, // Ascii:121 = [y]
0x09,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x01FC,0x0018,0x0030,0x0060,0x00C0,0x0180,0x01FC,0x0000,0x0000, // Ascii:122 = [z]
0x05,0x000E,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x0018,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000E,0x0000, // Ascii:123 = [{]
0x05,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C, // Ascii:124 = [|]
0x05,0x001C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x0006,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x001C,0x0000, // Ascii:125 = [}]
0x08,0x0076,0x00DE,0x00DC,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, // Ascii:126 = [~]
};


//--------------------------------------------------------------
// Font-Struktur
//--------------------------------------------------------------
UB_pFont gothic_12x16 = {
  gothic_12x16_Table, // Font-Daten
  16,              // Hoehe eines Zeichens  (in Pixel)
  32,              // erstes Zeichen  (Ascii-Nr)
  126,              // letztes Zeichen (Ascii-Nr)
};

#endif /* WITHALTERNATIVEFONTS */
