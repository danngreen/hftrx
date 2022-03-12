#include "formats.h"
#include "hardware.h"

void board_usb_initialize(void);
void board_usb_activate(void);

int main(void) {
  global_disableIRQ();
  cpu_initialize(); // в случае ARM - инициализация прерываний и контроллеров,
                    // AVR - запрет JTAG

#if WITHDEBUG
  HARDWARE_DEBUG_INITIALIZE();
  HARDWARE_DEBUG_SET_SPEED(DEBUGSPEED);
  dbg_puts_impl_P(
      PSTR("Version " __DATE__ " " __TIME__ " 1 debug session starts.\n"));
#endif /* WITHDEBUG */

  hardware_timer_initialize(TICKS_FREQUENCY);

  board_usb_initialize(); // USB device and host support
  dbg_puts_impl_P(PSTR("Most of hardware initialized.\n"));

  global_enableIRQ();
  board_usb_activate(); // USB device and host start

  for (;;) {
  }
  return 0;
}
