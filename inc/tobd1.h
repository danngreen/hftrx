#ifndef TOBD1_H_INCLUDED
#define TOBD1_H_INCLUDED

#include "hardware.h"

#if TOBD1_BK

void tobd1_initialize(void);
void tobd1_main_step(void);
void tobd1_interrupt_handler(void);

#endif /* TOBD1_BK */

#endif /* TOBD1_H_INCLUDED */
