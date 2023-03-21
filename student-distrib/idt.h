/* idt.h - Defines used in idt
 * vim:ts=4 noexpandtab
 */

#ifndef _idt_H
#define _idt_H

#include "types.h"

#define DPL_KERNEL 0
#define DPL_USER 3

#define DE 0
#define DB 1
#define NMI 2
#define BP 3
#define OF 4
#define BR 5
#define UD 6
#define NM 7
#define DF 8
#define CSO 9
#define TS 10
#define NP 11
#define SFE 12
#define GP 13
#define PF 14
#define MF 16
#define AC 17
#define MC 18
#define XF 19

#define SYS_CALL 0x80

/* Externally-visible functions */

/* Initialize both PICs */
void idt_init(void);

void de(void);

void db(void);

void nmi(void);

void bp(void);

void of(void);

void br(void);

void ud(void);

void nm(void);

void df(void);

void cso(void);

void ts(void);

void np(void);

void sfe(void);

void gp(void);

void pf(void);

void mf(void);

void ac(void);

void mc(void);

void xf(void);

void sys_call(void);

#endif /* _idt_H */
