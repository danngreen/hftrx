#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include "hardware.h"
#include "../display/display.h"
#include "display2.h"

#if WITHTOUCHGUI

#if DIM_X == 480 && DIM_Y == 272
	#define WITHGUIMAXX				480						// для разрешения 480х272 используется мини-версия touch GUI
	#define WITHGUIMAXY				272
	#define WITHGUISTYLE_MINI		1
	#define GUI_OLDBUTTONSTYLE		1						// Кнопки без закругления углов
	#define FOOTER_HEIGHT			36
#elif DIM_X >= 800 && DIM_Y >= 480							// при разрешении больше чем 800х480 интерфейс будет сжат до 800х480.
	#define WITHGUIMAXX				800
	#define WITHGUIMAXY				480
	#define WITHGUISTYLE_COMMON		1
	#define GUI_TRANSPARENT_WINDOWS	1						// Прозрачный фон окон
	#define FOOTER_HEIGHT			50
#endif

typedef struct {
	char name[20];
	uint_fast8_t index;
} menu_names_t;

typedef struct {
	char param[20];
	char val[20];
	uint_fast8_t state;
	uint_fast8_t updated;
} enc2_menu_t;

enum {
	memory_cells_count = 20
};

typedef enum {
	BAND_TYPE_HAM,
	BAND_TYPE_BROADCAST
} gui_band_type_t;

typedef struct {
	uint_fast32_t init_freq;
	uint_fast8_t index;
	gui_band_type_t type;
	char name[10];
} band_array_t;

typedef struct {
	char label [10][10];
} bws_t;

/* структура для размещения в конфигурационном ОЗУ */
struct gui_nvram_t {
	uint8_t enc2step_pos;
	uint8_t micprofile;
	uint8_t tune_powerdown_enable;
	uint8_t tune_powerdown_value;
	uint8_t freq_swipe_enable;
	uint8_t freq_swipe_step;
} ATTRPACKED;

uint_fast8_t hamradio_get_multilinemenu_block_groups(menu_names_t * vals);
uint_fast8_t hamradio_get_multilinemenu_block_params(menu_names_t * vals, uint_fast8_t index, uint_fast8_t max_count);
void hamradio_get_multilinemenu_block_vals(menu_names_t * vals, uint_fast8_t index, uint_fast8_t cnt);
const char * hamradio_gui_edit_menu_item(uint_fast8_t index, int_fast8_t rotate);
void hamradio_clean_memory_cells(uint_fast8_t i);
void hamradio_save_memory_cells(uint_fast8_t i);
uint_fast32_t hamradio_load_memory_cells(uint_fast8_t cell, uint_fast8_t set);
uint_fast8_t hamradio_get_submode(void);
const char * hamradio_get_submode_label(uint_fast8_t v);
uint_fast8_t hamradio_load_mic_profile(uint_fast8_t cell, uint_fast8_t set);
void hamradio_save_mic_profile(uint_fast8_t cell);
void hamradio_clean_mic_profile(uint_fast8_t cell);
uint_fast8_t hamradio_get_bands(band_array_t * bands, uint_fast8_t count_only, uint_fast8_t is_bcast_need);
void hamradio_goto_band_by_freq(uint_fast32_t f);
uint_fast8_t hamradio_check_current_freq_by_band(uint_fast8_t band);
void hamradio_load_gui_settings(void * ptr);
void hamradio_save_gui_settings(const void * ptr);
void hamradio_gui_enc2_update(void);
void hamradio_gui_parse_ft8buf(void);
void hamradio_ft8_toggle_state(void);
void hamradio_ft8_start_fill(void);
uint_fast8_t hamradio_get_att_dbs(uint_fast8_t * values, uint_fast8_t limit);
uint_fast8_t hamradio_get_att_db(void);
void hamradio_set_att_db(uint_fast8_t db);
uint_fast8_t hamradio_get_bws(bws_t * bws, uint_fast8_t limit);
void hamradio_set_bw(uint_fast8_t v);
uint_fast16_t hamradio_get_afgain(void);
void hamradio_set_afgain(uint_fast16_t v);

void gui_encoder2_menu(enc2_menu_t * enc2_menu);
void gui_WM_walktrough(uint_fast8_t x, uint_fast8_t y, dctx_t * pctx);
void gui_initialize(void);
void gui_set_encoder2_rotate(int_fast8_t rotate);
void gui_put_keyb_code(uint_fast8_t kbch);
void gui_uif_editmenu(const char * name, uint_fast16_t menupos, uint_fast8_t exitkey);
void gui_open_sys_menu(void);
void gui_update(void);


#endif /* WITHTOUCHGUI */
#endif /* GUI_H_INCLUDED */
