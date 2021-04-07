#ifndef TOBD1_H_INCLUDED
#define TOBD1_H_INCLUDED

#include "hardware.h"

#if CTLSTYLE_TOBD1

void tobd1_initialize(void);
void tobd1_main_step(void);
void tobd1_interrupt_handler(void);

#endif /* CTLSTYLE_TOBD1 */

#endif /* TOBD1_H_INCLUDED */
