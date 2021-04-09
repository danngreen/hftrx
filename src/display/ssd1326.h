#include "hardware.h"

void ssd1326_init(void);
void ssd1326_send_framebuffer(void);
void ssd1326_draw_point (uint_fast16_t x, uint_fast8_t y, uint_fast8_t color);
void ssd1326_draw_line(uint_fast16_t x1, uint_fast8_t y1, uint_fast16_t x2, uint_fast8_t y2, uint_fast8_t color);
void ssd1326_DrawString_small(uint_fast8_t col, uint_fast8_t row, char *pStr, uint_fast8_t inverse);
void ssd1326_DrawString_big(uint_fast8_t col, char *pStr, uint_fast8_t inverse);
