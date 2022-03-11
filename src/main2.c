#include "hardware.h"
#include "formats.h"
#include "board.h"

static void lowinitialize(void)
{
#if WITHDEBUG
	HARDWARE_DEBUG_INITIALIZE();
	HARDWARE_DEBUG_SET_SPEED(DEBUGSPEED);
	dbg_puts_impl_P(PSTR("Version " __DATE__ " " __TIME__ " 1 debug session starts.\n"));
#endif /* WITHDEBUG */

	hardware_timer_initialize(TICKS_FREQUENCY);

	cpu_initdone();			/* секция init (в которой лежит образ для загрузки в FPGA) больше не нужна */

	board_usb_initialize();		// USB device and host support
	dbg_puts_impl_P(PSTR("Most of hardware initialized.\n"));

}



int main(void)
{
	global_disableIRQ();
	cpu_initialize();		// в случае ARM - инициализация прерываний и контроллеров, AVR - запрет JTAG
	lowinitialize();	/* вызывается при запрещённых прерываниях. */
	global_enableIRQ();
	board_usb_activate();		// USB device and host start

	for (;;)
	{
	}
	return 0;
}
