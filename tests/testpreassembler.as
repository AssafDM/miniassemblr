.entry LENGTH
    .extern W
;rowoflength80-67890123456789012345678901234567890123456789012345678901234567890
;rowoflength81-678901234567890123456789012345678901234567890123456789012345678901
MAIN: mov @r3 ,LENGTH
LOOP: jmp L1
mcro longlegalname456789012345678901
prn -5
endmcro extratext
mcro longillegalname67890123456789012
bne W
endmcro
sub @r1, @r4
    bne L3
L1: inc K
    .entry LOOP
jmp W
END: stop
STR: .string "abcdef"
LENGTH: .data 6,-9,15
K: .data 22
    .extern L3
