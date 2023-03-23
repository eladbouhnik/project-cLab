MAIN: mov r3 ,r7
LOOP: jmp L1(#-1,#-3)
prn #-5
bne LOOP(r4,r7)
sub L1 , r4
bne END
L1: inc STR
bne LOOP(LENGTH,STR) 
END: stop
STR: .string “abcdef”
LENGTH: .data +6,-9,-15, 5
K: .data 28
