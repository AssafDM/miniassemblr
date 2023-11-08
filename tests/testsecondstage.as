.entry LENGTH
    .extern W 
;illegal register and label
MAIN: mov @r-1 ,unknowenlbl
LOOP: jmp L1
prn -5
bne W
;illegal registers
sub @r1, @r8
    bne L3
L1: inc K
    .entry unknowenlbl
jmp W
END: stop
STR: .string "abcdef"
LENGTH: .data 6,-9,15
K: .data 22
    .extern L3
