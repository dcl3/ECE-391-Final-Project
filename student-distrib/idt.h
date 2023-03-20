/* idt.h - Defines used in idt
 * vim:ts=4 noexpandtab
 */

#ifndef _idt_H
#define _idt_H

#include "types.h"

#define DPL_KERNEL 0
#define DPL_USER 3

/* Externally-visible functions */

/* Initialize both PICs */
void idt_init(void);

#endif /* _idt_H */
