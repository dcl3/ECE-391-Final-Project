#define ASM

#define INTR_LINK(name, func)    \
.GLOBL name                     ;\
name:                           ;\
    pushal                      ;\
    pushfl                      ;\
    call func                   ;\
    popfl                       ;\
    popal                       ;\
    iret                        ;\

INTR_LINK(keyboard_handler_linkage, keyboard_handler);
